#include "Operator.h"
#include "Literal.h"
#include <kiraz/Compiler.h>

namespace ast {

// Helper to get name from Id node
static std::string get_id_name(Node::Ptr node) {
    auto id = std::dynamic_pointer_cast<Id>(node);
    return id ? id->get_name() : "";
}

// Helper to check if a name is a builtin
static bool is_builtin_name(const std::string& name) {
    return name == "and" || name == "or" || name == "not" || 
           name == "Boolean" || name == "String" || name == "Integer64" ||
           name == "Null" || name == "null" || name == "true" || name == "false" ||
           name == "Function" || name == "Class" || name == "Module";
}

// BinaryOp base implementation
Node::Ptr BinaryOp::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    // Compute types of operands
    if (auto err = m_left->compute_stmt_type(st)) return err;
    if (auto err = m_right->compute_stmt_type(st)) return err;
    
    auto left_type = m_left->get_stmt_type();
    auto right_type = m_right->get_stmt_type();
    
    if (!left_type || !right_type) {
        return set_error("Missing type information for binary operation");
    }
    
    // Get type names
    auto left_id = std::dynamic_pointer_cast<const Id>(left_type);
    auto right_id = std::dynamic_pointer_cast<const Id>(right_type);
    auto left_builtin = std::dynamic_pointer_cast<const BuiltinType>(left_type);
    auto right_builtin = std::dynamic_pointer_cast<const BuiltinType>(right_type);
    
    std::string left_name = left_id ? left_id->get_name() : (left_builtin ? left_builtin->get_name() : "");
    std::string right_name = right_id ? right_id->get_name() : (right_builtin ? right_builtin->get_name() : "");
    
    // Check type compatibility - must be same type
    if (left_name != right_name) {
        return set_error(FF("Operator '{}' not defined for types '{}' and '{}'", 
                          get_op_symbol(), left_name, right_name));
    }
    
    // Determine result type
    if (is_comparison()) {
        // Comparison operators return Boolean
        auto bool_sym = st.get_symbol("Boolean");
        if (bool_sym) {
            set_stmt_type(bool_sym.stmt);
        }
    } else {
        // Arithmetic operators return same type as operands
        set_stmt_type(left_type);
    }
    
    return nullptr;
}

std::string Add::get_op_symbol() const { return "+"; }
std::string Sub::get_op_symbol() const { return "-"; }
std::string Mult::get_op_symbol() const { return "*"; }
std::string Div::get_op_symbol() const { return "/"; }
std::string OpEq::get_op_symbol() const { return "=="; }
std::string OpNe::get_op_symbol() const { return "!="; }
std::string OpLt::get_op_symbol() const { return "<"; }
std::string OpGt::get_op_symbol() const { return ">"; }
std::string OpLe::get_op_symbol() const { return "<="; }
std::string OpGe::get_op_symbol() const { return ">="; }

// Let statement
Node::Ptr Let::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    return nullptr; // Type checking done in add_to_symtab_ordered
}

Node::Ptr Let::add_to_symtab_ordered(SymbolTable &st) {
    std::string var_name = get_id_name(m_name);
    
    // Check if already exists in current scope
    auto existing = st.get_symbol(var_name);
    if (existing && existing.stmt->get_cur_symtab() == st.get_cur_symtab()) {
        return set_error(FF("Identifier '{}' is already in symtab", var_name));
    }
    
    // Check builtin override
    if (is_builtin_name(var_name)) {
        return set_error(FF("Overriding builtin '{}' is not allowed", var_name));
    }
    
    Node::Ptr type_node = m_type;
    Node::Ptr init_type = nullptr;
    
    // Compute initializer type if present
    if (m_init) {
        if (auto err = m_init->compute_stmt_type(st)) return err;
        init_type = m_init->get_stmt_type();
        
        // Check for null in initializer
        std::string init_name = get_id_name(m_init);
        if (init_name == "null") {
            return set_error("Null type can not be used in let initializer");
        }
    }
    
    // Resolve explicit type if present
    if (type_node) {
        if (auto err = type_node->compute_stmt_type(st)) return err;
        auto type_sym = type_node->get_symbol(st);
        if (!type_sym) {
            return set_error(FF("Type '{}' not found", get_id_name(type_node)));
        }
        
        // Check for Null in type
        std::string type_name = get_id_name(type_node);
        if (type_name == "Null") {
            return set_error("Null type can not be used in let type designation");
        }
        
        // If both type and init, they must match
        if (init_type) {
            auto type_id = std::dynamic_pointer_cast<const Id>(type_sym.stmt);
            auto type_builtin = std::dynamic_pointer_cast<const BuiltinType>(type_sym.stmt);
            auto init_id = std::dynamic_pointer_cast<const Id>(init_type);
            auto init_builtin = std::dynamic_pointer_cast<const BuiltinType>(init_type);
            
            std::string type_name_str = type_id ? type_id->get_name() : (type_builtin ? type_builtin->get_name() : "");
            std::string init_name_str = init_id ? init_id->get_name() : (init_builtin ? init_builtin->get_name() : "");
            
            if (type_name_str != init_name_str) {
                return set_error(FF("Initializer type '{}' does not match explicit type '{}'",
                                  init_name_str, type_name_str));
            }
        }
        
        set_stmt_type(type_sym.stmt);
    } else if (init_type) {
        // Auto type from initializer
        set_stmt_type(init_type);
    }
    
    // Add to symbol table
    st.add_symbol(var_name, shared_from_this());
    m_name->set_cur_symtab(st.get_cur_symtab());
    m_name->set_stmt_type(get_stmt_type());
    
    return nullptr;
}

// FuncArgs
Node::Ptr FuncArgs::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    for (auto& arg : m_args) {
        if (auto err = arg->compute_stmt_type(st)) return err;
    }
    return nullptr;
}

// FArg
Node::Ptr FArg::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    std::string arg_name = get_id_name(m_name);
    
    // Resolve type
    if (auto err = m_type->compute_stmt_type(st)) return err;
    auto type_sym = m_type->get_symbol(st);
    if (!type_sym) {
        return set_error(FF("Type '{}' not found for argument '{}'", get_id_name(m_type), arg_name));
    }
    
    set_stmt_type(type_sym.stmt);
    m_name->set_stmt_type(type_sym.stmt);
    m_name->set_cur_symtab(st.get_cur_symtab());
    
    return nullptr;
}

// StmtList
Node::Ptr StmtList::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    // Forward pass - add functions and classes
    for (auto& stmt : m_stmts) {
        if (auto err = stmt->add_to_symtab_forward(st)) return err;
    }
    
    // Main pass - type check statements in order
    for (auto& stmt : m_stmts) {
        if (auto err = stmt->add_to_symtab_ordered(st)) return err;
        if (auto err = stmt->compute_stmt_type(st)) return err;
    }
    
    return nullptr;
}

// Func
Node::Ptr Func::add_to_symtab_forward(SymbolTable &st) {
    std::string func_name = get_id_name(m_name);
    
    // Check if already exists
    auto existing = st.get_symbol(func_name);
    if (existing && existing.stmt->get_cur_symtab() == st.get_cur_symtab()) {
        return set_error(FF("Identifier '{}' is already in symtab", func_name));
    }
    
    // Add to symbol table
    st.add_symbol(func_name, shared_from_this());
    set_cur_symtab(st.get_cur_symtab());
    
    // Set function type
    auto func_type_sym = st.get_symbol("Function");
    if (func_type_sym) {
        set_stmt_type(func_type_sym.stmt);
        m_name->set_stmt_type(func_type_sym.stmt);
        m_name->set_cur_symtab(st.get_cur_symtab());
    }
    
    return nullptr;
}

Node::Ptr Func::compute_stmt_type(SymbolTable &st) {
    std::string func_name = get_id_name(m_name);
    
    // Check return type exists
    if (auto err = m_ret_type->compute_stmt_type(st)) return err;
    auto ret_type_sym = m_ret_type->get_symbol(st);
    if (!ret_type_sym) {
        return set_error(FF("Return type '{}' of function '{}' is not found", 
                          get_id_name(m_ret_type), func_name));
    }
    
    // Enter function scope
    auto scope_guard = st.enter_scope(ScopeType::Func, shared_from_this());
    m_scope->set_cur_symtab(st.get_cur_symtab());
    
    // Add arguments to scope
    auto args_list = std::dynamic_pointer_cast<FuncArgs>(m_args);
    if (args_list) {
        for (auto& arg_node : args_list->get_args()) {
            auto farg = std::dynamic_pointer_cast<FArg>(arg_node);
            if (!farg) continue;
            
            if (auto err = farg->compute_stmt_type(st)) return err;
            
            std::string arg_name = get_id_name(farg->get_name());
            
            // Check for duplicate
            auto existing = st.get_symbol(arg_name);
            if (existing && existing.stmt->get_cur_symtab() == st.get_cur_symtab()) {
                return set_error(FF("Identifier '{}' in argument list of function '{}' is already in symtab",
                                  arg_name, func_name));
            }
            
            // Check type exists
            auto arg_type_sym = farg->get_type()->get_symbol(st);
            if (!arg_type_sym) {
                return set_error(FF("Identifier '{}' in type of argument '{}' in function '{}' is not found",
                                  get_id_name(farg->get_type()), arg_name, func_name));
            }
            
            st.add_symbol(arg_name, farg);
        }
    }
    
    // Type check function body
    if (auto err = m_scope->compute_stmt_type(st)) return err;
    
    // Check for return statement if not Null return type
    auto ret_type_id = std::dynamic_pointer_cast<const Id>(ret_type_sym.stmt);
    auto ret_type_builtin = std::dynamic_pointer_cast<const BuiltinType>(ret_type_sym.stmt);
    std::string ret_type_name = ret_type_id ? ret_type_id->get_name() : 
                                (ret_type_builtin ? ret_type_builtin->get_name() : "");
    
    if (ret_type_name != "Null") {
        // Check if function has return statement
        auto scope_stmts = std::dynamic_pointer_cast<StmtList>(m_scope);
        bool has_return = false;
        if (scope_stmts) {
            for (auto& stmt : scope_stmts->get_stmts()) {
                if (stmt->is_return()) {
                    has_return = true;
                    break;
                }
            }
        }
        if (!has_return) {
            return set_error("Function is missing return value");
        }
    }
    
    return nullptr;
}

// Class
Node::Ptr Class::add_to_symtab_forward(SymbolTable &st) {
    std::string class_name = get_id_name(m_name);
    
    // Check first letter is uppercase
    if (!class_name.empty() && class_name[0] >= 'a' && class_name[0] <= 'z') {
        return set_error(FF("Class name '{}' can not start with an lowercase letter", class_name));
    }
    
    // Check if already exists
    auto existing = st.get_symbol(class_name);
    if (existing && existing.stmt->get_cur_symtab() == st.get_cur_symtab()) {
        return set_error(FF("Identifier '{}' is already in symtab", class_name));
    }
    
    // Add to symbol table
    st.add_symbol(class_name, shared_from_this());
    set_cur_symtab(st.get_cur_symtab());
    
    // Set class type
    auto class_type_sym = st.get_symbol("Class");
    if (class_type_sym) {
        set_stmt_type(class_type_sym.stmt);
        m_name->set_stmt_type(class_type_sym.stmt);
        m_name->set_cur_symtab(st.get_cur_symtab());
    }
    
    return nullptr;
}

Node::Ptr Class::compute_stmt_type(SymbolTable &st) {
    std::string class_name = get_id_name(m_name);
    
    // Enter class scope
    auto scope_guard = st.enter_scope(ScopeType::Class, shared_from_this());
    m_scope->set_cur_symtab(st.get_cur_symtab());
    
    // Add 'this' to class scope
    auto this_id = std::make_shared<Id>("this");
    this_id->set_stmt_type(shared_from_this());
    this_id->set_cur_symtab(st.get_cur_symtab());
    st.add_symbol("this", this_id);
    
    // Forward pass for methods
    auto scope_stmts = std::dynamic_pointer_cast<StmtList>(m_scope);
    if (scope_stmts) {
        for (auto& stmt : scope_stmts->get_stmts()) {
            if (stmt->is_func()) {
                // Check method name doesn't conflict with class name
                auto func = std::dynamic_pointer_cast<Func>(stmt);
                if (func && get_id_name(func->get_name()) == class_name) {
                    return set_error(FF("Identifier '{}' is already in symtab", class_name));
                }
            }
            if (auto err = stmt->add_to_symtab_forward(st)) return err;
        }
        
        // Main pass
        for (auto& stmt : scope_stmts->get_stmts()) {
            // Check for invalid statements in class scope
            if (stmt->is_if() || stmt->is_while() || stmt->is_return() ||
                stmt->is_assign() || stmt->is_call()) {
                return set_error("Statement not allowed in class scope");
            }
            
            // Check attribute name doesn't conflict with class name
            auto let_stmt = std::dynamic_pointer_cast<Let>(stmt);
            if (let_stmt && get_id_name(let_stmt->get_name()) == class_name) {
                return set_error(FF("Identifier '{}' is already in symtab", class_name));
            }
            
            if (auto err = stmt->add_to_symtab_ordered(st)) return err;
            if (auto err = stmt->compute_stmt_type(st)) return err;
        }
    }
    
    // Store subsymbols
    m_subsymbols = st.get_symbols();
    
    return nullptr;
}

Node::SymTabEntry Class::get_subsymbol(Node::Ptr) const {
    // This should not be called - use get_subsymbol with name instead
    return {};
}

// Assignment
Node::Ptr Assignment::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    // Check placement - not allowed in module or class scope
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        return set_error("Assignment not allowed in this scope");
    }
    
    // Compute types
    if (auto err = m_name->compute_stmt_type(st)) return err;
    if (auto err = m_value->compute_stmt_type(st)) return err;
    
    auto lhs_type = m_name->get_stmt_type();
    auto rhs_type = m_value->get_stmt_type();
    
    if (!lhs_type || !rhs_type) {
        return set_error("Missing type information for assignment");
    }
    
    // Check for builtin assignment
    std::string lhs_name = get_id_name(m_name);
    if (lhs_name == "and" || lhs_name == "or" || lhs_name == "not") {
        return set_error(FF("Overriding builtin '{}' is not allowed", lhs_name));
    }
    if (lhs_name == "true" || lhs_name == "false") {
        return set_error("Can not assign to boolean literal");
    }
    
    // Check for module assignment
    auto lhs_id = std::dynamic_pointer_cast<const Id>(lhs_type);
    auto lhs_builtin = std::dynamic_pointer_cast<const BuiltinType>(lhs_type);
    std::string lhs_type_name = lhs_id ? lhs_id->get_name() : (lhs_builtin ? lhs_builtin->get_name() : "");
    
    if (lhs_type_name == "Module") {
        auto module_name = get_id_name(m_name);
        return set_error(FF("Overriding imported module '{}' is not allowed", module_name));
    }
    
    // Check type compatibility
    auto rhs_id = std::dynamic_pointer_cast<const Id>(rhs_type);
    auto rhs_builtin = std::dynamic_pointer_cast<const BuiltinType>(rhs_type);
    std::string rhs_type_name = rhs_id ? rhs_id->get_name() : (rhs_builtin ? rhs_builtin->get_name() : "");
    
    if (lhs_type_name != rhs_type_name) {
        return set_error(FF("Left type '{}' of assignment does not match the right type '{}'",
                          lhs_type_name, rhs_type_name));
    }
    
    set_stmt_type(lhs_type);
    return nullptr;
}

// If
Node::Ptr If::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    // Check placement
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        return set_error("If statement not allowed in this scope");
    }
    
    // Check condition type
    if (auto err = m_cond->compute_stmt_type(st)) return err;
    auto cond_type = m_cond->get_stmt_type();
    
    auto cond_id = std::dynamic_pointer_cast<const Id>(cond_type);
    auto cond_builtin = std::dynamic_pointer_cast<const BuiltinType>(cond_type);
    std::string cond_type_name = cond_id ? cond_id->get_name() : (cond_builtin ? cond_builtin->get_name() : "");
    
    if (cond_type_name != "Boolean") {
        return set_error("If only accepts tests of type 'Boolean'");
    }
    
    // Type check branches
    if (auto err = m_then->compute_stmt_type(st)) return err;
    if (m_else) {
        if (auto err = m_else->compute_stmt_type(st)) return err;
    }
    
    return nullptr;
}

// While
Node::Ptr While::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    // Check placement
    auto scope_type = st.get_scope_type();
    if (scope_type == ScopeType::Module || scope_type == ScopeType::Class) {
        return set_error("While statement not allowed in this scope");
    }
    
    // Check condition type
    if (auto err = m_cond->compute_stmt_type(st)) return err;
    auto cond_type = m_cond->get_stmt_type();
    
    auto cond_id = std::dynamic_pointer_cast<const Id>(cond_type);
    auto cond_builtin = std::dynamic_pointer_cast<const BuiltinType>(cond_type);
    std::string cond_type_name = cond_id ? cond_id->get_name() : (cond_builtin ? cond_builtin->get_name() : "");
    
    if (cond_type_name != "Boolean") {
        return set_error("While only accepts tests of type 'Boolean'");
    }
    
    // Type check body
    if (auto err = m_repeat->compute_stmt_type(st)) return err;
    
    return nullptr;
}

// Import
Node::Ptr Import::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    std::string module_name = get_id_name(m_name);
    
    // Check if already exists
    auto existing = st.get_symbol(module_name);
    if (existing) {
        return nullptr; // Already imported
    }
    
    // Compile the module
    if (module_name == "io") {
        auto io_module = SymbolTable::get_module_io();
        if (io_module) {
            st.add_symbol(module_name, io_module);
            m_name->set_cur_symtab(st.get_cur_symtab());
            
            auto module_type_sym = st.get_symbol("Module");
            if (module_type_sym) {
                set_stmt_type(module_type_sym.stmt);
                m_name->set_stmt_type(module_type_sym.stmt);
            }
            return nullptr;
        }
    }
    
    return set_error(FF("Module '{}' not found", module_name));
}

// Return
Node::Ptr Return::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    // Check placement - only in function/method
    auto scope_type = st.get_scope_type();
    if (scope_type != ScopeType::Func && scope_type != ScopeType::Method) {
        return set_error("Return statement not allowed in this scope");
    }
    
    // Compute value type
    if (auto err = m_value->compute_stmt_type(st)) return err;
    auto value_type = m_value->get_stmt_type();
    
    // Get function's return type
    auto func_stmt = st.get_scope_stmt();
    auto func = std::dynamic_pointer_cast<const Func>(func_stmt);
    if (!func) {
        return set_error("Return statement outside function");
    }
    
    auto ret_type_sym = func->get_ret_type()->get_symbol(st);
    if (!ret_type_sym) {
        return set_error("Function return type not found");
    }
    
    // Check type match
    auto value_id = std::dynamic_pointer_cast<const Id>(value_type);
    auto value_builtin = std::dynamic_pointer_cast<const BuiltinType>(value_type);
    auto ret_id = std::dynamic_pointer_cast<const Id>(ret_type_sym.stmt);
    auto ret_builtin = std::dynamic_pointer_cast<const BuiltinType>(ret_type_sym.stmt);
    
    std::string value_type_name = value_id ? value_id->get_name() : (value_builtin ? value_builtin->get_name() : "");
    std::string ret_type_name = ret_id ? ret_id->get_name() : (ret_builtin ? ret_builtin->get_name() : "");
    
    if (value_type_name != ret_type_name) {
        return set_error(FF("Return statement type '{}' does not match function return type '{}'",
                          value_type_name, ret_type_name));
    }
    
    set_stmt_type(value_type);
    return nullptr;
}

// Dot
Node::Ptr Dot::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    // Compute left side type
    if (auto err = m_lhs->compute_stmt_type(st)) return err;
    auto lhs_type = m_lhs->get_stmt_type();
    
    // Get subsymbol
    std::string rhs_name = get_id_name(m_rhs);
    
    // Check if lhs is a module or class instance
    auto lhs_sym = m_lhs->get_symbol(st);
    if (!lhs_sym) {
        return set_error(FF("Identifier '{}' is not found", get_id_name(m_lhs)));
    }
    
    // Get subsymbol from lhs
    auto subsym = lhs_sym.stmt->get_subsymbol(m_rhs);
    if (!subsym) {
        // Try to get by name from class
        auto class_node = std::dynamic_pointer_cast<const Class>(lhs_type);
        if (class_node) {
            auto sub = class_node->get_subsymbol_by_name(rhs_name);
            if (!sub) {
                return set_error(FF("Identifier '{}' has no subsymbol '{}'", 
                                  get_id_name(m_lhs), rhs_name));
            }
            subsym = sub;
        } else {
            return set_error(FF("Identifier '{}.{}' is not found", get_id_name(m_lhs), rhs_name));
        }
    }
    
    set_stmt_type(subsym.stmt->get_stmt_type());
    m_rhs->set_stmt_type(subsym.stmt->get_stmt_type());
    m_rhs->set_cur_symtab(st.get_cur_symtab());
    
    return nullptr;
}

// Call
Node::Ptr Call::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    // Compute function type
    if (auto err = m_name->compute_stmt_type(st)) return err;
    
    // Get function symbol
    auto func_sym = m_name->get_symbol(st);
    if (!func_sym) {
        return set_error(FF("Identifier '{}' is not found", get_id_name(m_name)));
    }
    
    auto func = std::dynamic_pointer_cast<const Func>(func_sym.stmt);
    if (!func) {
        // Check if it's a builtin function
        std::string func_name = get_id_name(m_name);
        if (func_name == "and" || func_name == "or" || func_name == "not") {
            // Type check arguments
            auto args_list = std::dynamic_pointer_cast<FuncArgs>(m_args);
            if (args_list) {
                for (auto& arg : args_list->get_args()) {
                    if (auto err = arg->compute_stmt_type(st)) return err;
                }
            }
            
            // Return Boolean
            auto bool_sym = st.get_symbol("Boolean");
            if (bool_sym) {
                set_stmt_type(bool_sym.stmt);
            }
            return nullptr;
        }
        
        return set_error(FF("'{}' is not a function", func_name));
    }
    
    // Get function arguments
    auto func_args = std::dynamic_pointer_cast<const FuncArgs>(func->get_args());
    auto call_args = std::dynamic_pointer_cast<FuncArgs>(m_args);
    
    size_t expected_count = func_args ? func_args->get_args().size() : 0;
    size_t actual_count = call_args ? call_args->get_args().size() : 0;
    
    // Check for io.print overload
    std::string full_name = get_id_name(m_name);
    bool is_dot_call = m_name->is_dot();
    if (is_dot_call) {
        auto dot = std::dynamic_pointer_cast<Dot>(m_name);
        if (dot) {
            std::string lhs_name = get_id_name(dot->get_lhs());
            std::string rhs_name = get_id_name(dot->get_rhs());
            if (lhs_name == "io" && rhs_name == "print") {
                // io.print accepts Integer64, String, or Boolean
                if (actual_count != 1) {
                    return set_error(FF("Call to function 'io.print' has wrong number of arguments"));
                }
                
                if (call_args) {
                    auto& arg = call_args->get_args()[0];
                    if (auto err = arg->compute_stmt_type(st)) return err;
                    
                    auto arg_type = arg->get_stmt_type();
                    auto arg_id = std::dynamic_pointer_cast<const Id>(arg_type);
                    auto arg_builtin = std::dynamic_pointer_cast<const BuiltinType>(arg_type);
                    std::string arg_type_name = arg_id ? arg_id->get_name() : 
                                                (arg_builtin ? arg_builtin->get_name() : "");
                    
                    if (arg_type_name != "Integer64" && arg_type_name != "String" && arg_type_name != "Boolean") {
                        return set_error("io.print only accepts Integer64, String, or Boolean");
                    }
                }
                
                // Return Null
                auto null_sym = st.get_symbol("Null");
                if (null_sym) {
                    set_stmt_type(null_sym.stmt);
                }
                return nullptr;
            }
        }
    }
    
    if (expected_count != actual_count) {
        return set_error(FF("Call to function '{}' has wrong number of arguments", full_name));
    }
    
    // Check argument types
    if (call_args && func_args) {
        for (size_t i = 0; i < expected_count; ++i) {
            auto& call_arg = call_args->get_args()[i];
            auto& func_arg_node = func_args->get_args()[i];
            
            if (auto err = call_arg->compute_stmt_type(st)) return err;
            
            auto func_arg = std::dynamic_pointer_cast<const FArg>(func_arg_node);
            if (!func_arg) continue;
            
            auto call_arg_type = call_arg->get_stmt_type();
            auto func_arg_type = func_arg->get_stmt_type();
            
            auto call_id = std::dynamic_pointer_cast<const Id>(call_arg_type);
            auto call_builtin = std::dynamic_pointer_cast<const BuiltinType>(call_arg_type);
            auto func_id = std::dynamic_pointer_cast<const Id>(func_arg_type);
            auto func_builtin = std::dynamic_pointer_cast<const BuiltinType>(func_arg_type);
            
            std::string call_type_name = call_id ? call_id->get_name() : 
                                        (call_builtin ? call_builtin->get_name() : "");
            std::string func_type_name = func_id ? func_id->get_name() : 
                                        (func_builtin ? func_builtin->get_name() : "");
            
            if (call_type_name != func_type_name) {
                return set_error(FF("Argument {} in call to function '{}' has type '{}' which does not match definition type '{}'",
                                  i + 1, full_name, call_type_name, func_type_name));
            }
        }
    }
    
    // Set return type
    auto ret_type_sym = func->get_ret_type()->get_symbol(st);
    if (ret_type_sym) {
        set_stmt_type(ret_type_sym.stmt);
    }
    
    return nullptr;
}

} // namespace ast