#include "Compiler.h"
#include <cassert>
#include <cstring> 
#include <fmt/format.h>
#include <resource/FILE_io_ki.h>

Node::Ptr SymbolTable::s_module_ki;
Node::Ptr SymbolTable::s_module_io;

SymbolTable::~SymbolTable() {}

Compiler *Compiler::s_current;

extern Token::Ptr curtoken;

Compiler::Compiler() {
    assert(! s_current);
    s_current = this;
    Node::reset_root();
}

Compiler::~Compiler() {
    if (buffer) {
        yy_delete_buffer(buffer);
        buffer = nullptr;
    }
    reset_parser();
    s_current = nullptr;
}

int Compiler::compile_file(const std::string &file_name) {
    yyin = fopen(file_name.data(), "rb");
    if (! yyin) {
        perror(file_name.data());
        return 2;
    }

    yyparse();
    auto root = Node::get_root();
    reset();

    return compile(root);
}

int Compiler::compile_string(const std::string &code) {
    buffer = yy_scan_string(code.data());
    yyparse();
    auto root = Node::get_root();
    reset();

    return compile(root);
}

Node::Ptr Compiler::compile_module(const std::string &str) {
    buffer = yy_scan_string(str.data());
    yyparse();
    auto retval = Node::pop_root();
    reset();
    assert(retval);
    return retval;
}

void Compiler::reset_parser() {
    curtoken.reset();
    Node::reset_root();
    Token::colno = 0;
    yylex_destroy();
}

void Compiler::reset() {
    if (buffer) {
        yy_delete_buffer(buffer);
        buffer = nullptr;
    }
    reset_parser();
}

int Compiler::compile(Node::Ptr root) {
    if (! root) {
        return 1;
    }

    SymbolTable st(ScopeType::Module);

    if (auto ret = root->compute_stmt_type(st)) {
        set_error(FF("Error at {}:{}: {}\n", ret->get_line(), ret->get_col(), ret->get_error()));
        Node::reset_root();
        return 1;
    }
    
    m_ctx.body() << "(module\n";
    m_ctx.body() << "  (import \"io\" \"print_i\" (func $io_print_i (param i64)))\n";
    m_ctx.body() << "  (import \"io\" \"print_s\" (func $io_print_s (param i32 i32)))\n";
    m_ctx.body() << "  (import \"io\" \"print_b\" (func $io_print_b (param i32)))\n";
    m_ctx.body() << "  (memory (export \"memory\") 1)\n";

    if (auto ret = root->gen_wat(m_ctx)) {
        return 2;
    }

    if (!m_ctx.get_memory_view().empty()) {
        m_ctx.body() << "  (data (i32.const 0) \"";
        for (unsigned char c : m_ctx.get_memory()) {
            if (isalnum(c)) {
                m_ctx.body() << (char)c;
            } else {
                m_ctx.body() << fmt::format("\\{:02x}", c);
            }
        }
        m_ctx.body() << "\")\n";
    }

    m_ctx.body() << ")\n";

    return 0;
}

SymbolTable::SymbolTable()
        : m_symbols({
                  std::make_shared<Scope>(Scope::SymTab{}, ScopeType::Module, nullptr),
          }) {
    if (! s_module_io) {
        s_module_io = Compiler::current()->compile_module(FILE_io_ki);
    }
}

SymbolTable::SymbolTable(ScopeType scope_type) : SymbolTable() {
    m_symbols.back()->scope_type = scope_type;
}

WasmContext::Coords WasmContext::add_to_memory(const std::string &s) {
    uint32_t offset = m_memory.size();
    uint32_t length = s.length();
    m_memory.insert(m_memory.end(), s.begin(), s.end());
    return {offset, length};
}

WasmContext::Coords WasmContext::add_to_memory(uint32_t s) {
    uint32_t offset = m_memory.size();
    m_memory.push_back(s & 0xFF);
    m_memory.push_back((s >> 8) & 0xFF);
    m_memory.push_back((s >> 16) & 0xFF);
    m_memory.push_back((s >> 24) & 0xFF);
    return {offset, 4}; 
}