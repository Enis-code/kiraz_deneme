#include "Node.h"
#include <kiraz/Compiler.h> // SymbolTable ve WasmContext tanımları için şart

Node::Ptr Node::s_root;
Node::Ptr Node::s_root_before;

Node::Node() {}
Node::~Node() {}

// Semantik Analiz (Base implementasyonlar)
Node::Ptr Node::compute_stmt_type(kiraz::SymbolTable &st) {
    // Varsayılan olarak mevcut scope'u kaydet
    set_cur_symtab(st.get_cur_symtab());
    return nullptr;
}

Node::Ptr Node::add_to_symtab_ordered(kiraz::SymbolTable &st) {
    return nullptr;
}

Node::Ptr Node::add_to_symtab_forward(kiraz::SymbolTable &st) {
    return nullptr;
}

Node::SymTabEntry Node::get_subsymbol(Ptr name) const {
    return {};
}

// WASM Kod Üretimi (Base implementasyon)
Node::Ptr Node::gen_wat(kiraz::WasmContext &ctx) {
    return nullptr;
}

// Root Yönetimi
void Node::set_root(Ptr root) {
    s_root = root;
    s_root_before = root; // Testler için yedeği tut
}

Node::Ptr Node::get_root() {
    return s_root;
}

Node::Ptr Node::pop_root() {
    auto temp = s_root;
    s_root = nullptr;
    return temp;
}

void Node::reset_root() {
    s_root = nullptr;
    // s_root_before sıfırlanmaz, testler verify için kullanır
}

Node::Ptr Node::current_root() {
    return s_root;
}

Node::Ptr Node::get_root_before() {
    return s_root_before;
}
