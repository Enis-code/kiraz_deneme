#ifndef KIRAZ_AST_OPERATOR_H
#define KIRAZ_AST_OPERATOR_H

#include <cassert>
#include <map>

#include <kiraz/Node.h>

namespace ast {

class BinaryOp : public Node {
public:
    BinaryOp(Node::Ptr left, Node::Ptr right) 
        : m_left(left), m_right(right) {}
    
    Node::Ptr get_left() const { return m_left; }
    Node::Ptr get_right() const { return m_right; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
    virtual std::string get_op_symbol() const = 0;
    virtual bool is_comparison() const { return false; }
    
protected:
    Node::Ptr m_left;
    Node::Ptr m_right;
};

class Add : public BinaryOp {
public:
    Add(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("Add(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
    std::string get_op_symbol() const override;
};

class Sub : public BinaryOp {
public:
    Sub(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("Sub(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
    std::string get_op_symbol() const override;
};

class Mult : public BinaryOp {
public:
    Mult(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("Mult(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
    std::string get_op_symbol() const override;
};

class Div : public BinaryOp {
public:
    Div(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("DivF(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
    std::string get_op_symbol() const override;
};

class OpEq : public BinaryOp {
public:
    OpEq(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpEq(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
    std::string get_op_symbol() const override;
    bool is_comparison() const override { return true; }
};

class OpNe : public BinaryOp {
public:
    OpNe(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpNe(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
    std::string get_op_symbol() const override;
    bool is_comparison() const override { return true; }
};

class OpLt : public BinaryOp {
public:
    OpLt(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpLt(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
    std::string get_op_symbol() const override;
    bool is_comparison() const override { return true; }
};

class OpGt : public BinaryOp {
public:
    OpGt(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpGt(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
    std::string get_op_symbol() const override;
    bool is_comparison() const override { return true; }
};

class OpLe : public BinaryOp {
public:
    OpLe(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpLe(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
    std::string get_op_symbol() const override;
    bool is_comparison() const override { return true; }
};

class OpGe : public BinaryOp {
public:
    OpGe(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpGe(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
    
    std::string get_op_symbol() const override;
    bool is_comparison() const override { return true; }
};

class Let : public Node {
public:
    Let(Node::Ptr name, Node::Ptr type, Node::Ptr init) 
        : m_name(name), m_type(type), m_init(init) {}
    
    std::string as_string() const override {
        std::string result = "Let(n=" + m_name->as_string();
        if (m_type) {
            result += ", t=" + m_type->as_string();
        }
        if (m_init) {
            result += ", i=" + m_init->as_string();
        }
        result += ")";
        return result;
    }
    
    Node::Ptr get_name() const { return m_name; }
    Node::Ptr get_type() const { return m_type; }
    Node::Ptr get_init() const { return m_init; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_ordered(SymbolTable &st) override;

private:
    Node::Ptr m_name;
    Node::Ptr m_type;
    Node::Ptr m_init;
};

class FArg : public Node {
public:
    FArg(Node::Ptr name, Node::Ptr type) 
        : m_name(name), m_type(type) {}
    
    std::string as_string() const override {
        return fmt::format("FArg(n={}, t={})", m_name->as_string(), m_type->as_string());
    }
    
    Node::Ptr get_name() const { return m_name; }
    Node::Ptr get_type() const { return m_type; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;

private:
    Node::Ptr m_name;
    Node::Ptr m_type;
};

class FuncArgs : public Node {
public:
    FuncArgs(std::vector<Node::Ptr> args) : m_args(args) {}
    
    std::string as_string() const override {
        std::string result = "[";
        bool first = true;
        for (const auto &arg : m_args) {
            if (!first) result += ", ";
            first = false;
            result += arg->as_string();
        }
        result += "]";
        return result;
    }
    
    bool is_funcarg_list() const override { return true; }
    
    void add(Node::Ptr arg) { m_args.push_back(arg); }
    const std::vector<Node::Ptr>& get_args() const { return m_args; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    std::vector<Node::Ptr> m_args;
};

class StmtList : public Node {
public:
    StmtList(std::vector<Node::Ptr> stmts) : m_stmts(stmts) {}
    
    std::string as_string() const override {
        std::string result = "Module([";
        bool first = true;
        for (const auto &stmt : m_stmts) {
            if (!first) result += ", ";
            first = false;
            result += stmt->as_string();
        }
        result += "])";
        return result;
    }
    
    bool is_stmt_list() const override { return true; }
    
    void add(Node::Ptr stmt) { m_stmts.push_back(stmt); }
    std::vector<Node::Ptr>& get_stmts() { return m_stmts; }
    const std::vector<Node::Ptr>& get_stmts() const { return m_stmts; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    std::vector<Node::Ptr> m_stmts;
};

class Func : public Node {
public:
    Func(Node::Ptr name, Node::Ptr args, Node::Ptr ret_type, Node::Ptr scope) 
        : m_name(name), m_args(args), m_ret_type(ret_type), m_scope(scope) {}
    
    std::string as_string() const override {
        std::string scope_str = m_scope->as_string();
        if (scope_str.rfind("Module(", 0) == 0) {
            scope_str = scope_str.substr(7, scope_str.length() - 8);
        }
        return fmt::format("Func(n={}, a=FuncArgs({}), r={}, s={})", 
                          m_name->as_string(), 
                          m_args->as_string(), 
                          m_ret_type->as_string(),
                          scope_str);
    }
    
    bool is_func() const override { return true; }
    
    Node::Ptr get_name() const { return m_name; }
    Node::Ptr get_args() const { return m_args; }
    Node::Ptr get_ret_type() const { return m_ret_type; }
    Node::Ptr get_scope() const { return m_scope; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    
private:
    Node::Ptr m_name;
    Node::Ptr m_args;
    Node::Ptr m_ret_type;
    Node::Ptr m_scope;
};

class Assignment : public Node {
public:
    Assignment(Node::Ptr name, Node::Ptr value) 
        : m_name(name), m_value(value) {}
    
    std::string as_string() const override {
        return fmt::format("Assign(l={}, r={})", m_name->as_string(), m_value->as_string());
    }
    
    bool is_assign() const override { return true; }
    
    Node::Ptr get_lhs() const { return m_name; }
    Node::Ptr get_rhs() const { return m_value; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    Node::Ptr m_name;
    Node::Ptr m_value;
};

class Class : public Node {
public:
    Class(Node::Ptr name, Node::Ptr scope) 
        : m_name(name), m_scope(scope) {}
    
    std::string as_string() const override {
        std::string scope_str = m_scope->as_string();
        if (scope_str.rfind("Module(", 0) == 0) {
            scope_str = scope_str.substr(7, scope_str.length() - 8);
        }
        return fmt::format("Class(n={}, s={})", 
                          m_name->as_string(), 
                          scope_str);
    }
    
    bool is_class() const override { return true; }
    
    Node::Ptr get_name() const { return m_name; }
    Node::Ptr get_scope() const { return m_scope; }
    
    void set_subsymbols(const std::map<std::string, Node::Ptr>& syms) { m_subsymbols = syms; }
    Node::SymTabEntry get_subsymbol(Node::Ptr) const override;
    Node::SymTabEntry get_subsymbol_by_name(const std::string& name) const {
        auto it = m_subsymbols.find(name);
        if (it == m_subsymbols.end()) return {name, nullptr};
        return {name, it->second};
    }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::Ptr add_to_symtab_forward(SymbolTable &st) override;
    
private:
    Node::Ptr m_name;
    Node::Ptr m_scope;
    std::map<std::string, Node::Ptr> m_subsymbols;
};

class If : public Node {
public:
    If(Node::Ptr cond, Node::Ptr then_stmts, Node::Ptr else_stmts) 
        : m_cond(cond), m_then(then_stmts), m_else(else_stmts) {}
    
    std::string as_string() const override {
        auto unwrap = [](const std::string& s) {
            if (s.rfind("Module(", 0) == 0) {
                return s.substr(7, s.length() - 8);
            }
            return s;
        };
        
        if (m_else && m_else->is_if()) {
            return fmt::format("If(?={}, then={}, else={})", 
                              m_cond->as_string(), 
                              unwrap(m_then->as_string()),
                              m_else->as_string());
        }
        
        return fmt::format("If(?={}, then={}, else={})", 
                          m_cond->as_string(), 
                          unwrap(m_then->as_string()),
                          unwrap(m_else->as_string()));
    }
    
    bool is_if() const override { return true; }
    
    Node::Ptr get_cond() const { return m_cond; }
    Node::Ptr get_then() const { return m_then; }
    Node::Ptr get_else() const { return m_else; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    Node::Ptr m_cond;
    Node::Ptr m_then;
    Node::Ptr m_else;
};

class While : public Node {
public:
    While(Node::Ptr cond, Node::Ptr repeat_stmts) 
        : m_cond(cond), m_repeat(repeat_stmts) {}
    
    std::string as_string() const override {
        std::string repeat_str = m_repeat->as_string();
        if (repeat_str.rfind("Module(", 0) == 0) {
            repeat_str = repeat_str.substr(7, repeat_str.length() - 8);
        }
        return fmt::format("While(?={}, repeat={})", 
                          m_cond->as_string(), 
                          repeat_str);
    }
    
    bool is_while() const override { return true; }
    
    Node::Ptr get_cond() const { return m_cond; }
    Node::Ptr get_repeat() const { return m_repeat; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    Node::Ptr m_cond;
    Node::Ptr m_repeat;
};

class Import : public Node {
public:
    Import(Node::Ptr name) : m_name(name) {}
    
    std::string as_string() const override {
        return fmt::format("Import({})", m_name->as_string());
    }
    
    bool is_import() const override { return true; }
    
    Node::Ptr get_name() const { return m_name; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    Node::Ptr m_name;
};

class Return : public Node {
public:
    Return(Node::Ptr value) : m_value(value) {}
    
    std::string as_string() const override {
        return fmt::format("Return({})", m_value->as_string());
    }
    
    bool is_return() const override { return true; }
    
    Node::Ptr get_value() const { return m_value; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    Node::Ptr m_value;
};

class Dot : public Node {
public:
    Dot(Node::Ptr lhs, Node::Ptr rhs) : m_lhs(lhs), m_rhs(rhs) {}
    
    std::string as_string() const override {
        return fmt::format("Dot(l={}, r={})", m_lhs->as_string(), m_rhs->as_string());
    }
    
    bool is_dot() const override { return true; }
    
    Node::Ptr get_lhs() const { return m_lhs; }
    Node::Ptr get_rhs() const { return m_rhs; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    Node::Ptr m_lhs;
    Node::Ptr m_rhs;
};

class Call : public Node {
public:
    Call(Node::Ptr name, Node::Ptr args) 
        : m_name(name), m_args(args) {}
    
    std::string as_string() const override {
        return fmt::format("Call(n={}, a=FuncArgs({}))", 
                          m_name->as_string(), 
                          m_args->as_string());
    }
    
    bool is_call() const override { return true; }
    
    Node::Ptr get_name() const { return m_name; }
    Node::Ptr get_args() const { return m_args; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    Node::Ptr m_name;
    Node::Ptr m_args;
};

}

#endif // KIRAZ_AST_OPERATOR_H
