
#include "Literal.h"
#include <kiraz/Compiler.h>

namespace ast {

// Id
Node::Ptr Id::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    auto sym = st.get_symbol(m_name);
    if (!sym) {
        return set_error(FF("Identifier '{}' is not found", m_name));
    }
    
    set_stmt_type(sym.stmt->get_stmt_type());
    return nullptr;
}

Node::SymTabEntry Id::get_symbol(const SymbolTable &st) const {
    return st.get_symbol(m_name);
}

Node::SymTabEntry Id::get_symbol() const {
    if (!m_cur_symtab) return {m_name, nullptr};
    return m_cur_symtab->get_symbol(m_name);
}

Node::SymTabEntry Id::get_subsymbol(Node::Ptr rhs) const {
    // Id doesn't have subsymbols by default
    return {};
}

// Integer
Node::Ptr Integer::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    auto int_type = st.get_symbol("Integer64");
    if (int_type) {
        set_stmt_type(int_type.stmt);
    }
    
    return nullptr;
}

// String  
Node::Ptr String::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    auto str_type = st.get_symbol("String");
    if (str_type) {
        set_stmt_type(str_type.stmt);
    }
    
    return nullptr;
}

// Signed
Node::Ptr Signed::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    if (auto err = m_operand->compute_stmt_type(st)) return err;
    
    // Result has same type as operand
    set_stmt_type(m_operand->get_stmt_type());
    
    return nullptr;
}

// BuiltinType
Node::Ptr BuiltinType::compute_stmt_type(SymbolTable &st) {
    set_cur_symtab(st.get_cur_symtab());
    
    // Builtin types refer to themselves
    set_stmt_type(shared_from_this());
    
    return nullptr;
}

Node::SymTabEntry BuiltinType::get_symbol(const SymbolTable &st) const {
    return st.get_symbol(m_name);
}

Node::SymTabEntry BuiltinType::get_symbol() const {
    if (!m_cur_symtab) return {m_name, nullptr};
    return m_cur_symtab->get_symbol(m_name);
}

}
