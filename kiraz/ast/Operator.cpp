#include "Operator.h"
#include <kiraz/Compiler.h>
#include <fmt/format.h>
#include "Literal.h"

using kiraz::WasmContext; // Bu dosya içinde WasmContext kullanımını kolaylaştırır

namespace ast {

Node::Ptr Func::gen_wat(WasmContext &ctx) {
    std::string func_name = m_name->as_string();
    if (auto id_node = std::dynamic_pointer_cast<Id>(m_name)) {
        func_name = id_node->get_id();
    } else {
        if (func_name.rfind("Id(", 0) == 0) {
            func_name = func_name.substr(3, func_name.length() - 4);
        }
    }

    ctx.body() << fmt::format("  (func ${}", func_name);

    bool is_main = (func_name == "main");
    bool returns_int64 = false;

    if (m_ret_type) {
        std::string ret_str = m_ret_type->as_string();
        if (ret_str.find("Integer64") != std::string::npos) {
            returns_int64 = true;
        }
    }

    if (returns_int64) {
        ctx.body() << " (result i64)";
    } 

    ctx.body() << "\n";

    if (auto stmts = std::dynamic_pointer_cast<StmtList>(m_scope)) {
        for (auto &stmt : stmts->get_stmts()) {
            if (auto let = std::dynamic_pointer_cast<Let>(stmt)) {
                 std::string var_name = "";
                 if (auto id_node = std::dynamic_pointer_cast<Id>(let->get_name())) {
                     var_name = id_node->get_id();
                 } else {
                     var_name = let->get_name()->as_string();
                     if (var_name.rfind("Id(", 0) == 0) {
                        var_name = var_name.substr(3, var_name.length() - 4);
                     }
                 }
                ctx.body() << fmt::format("    (local ${} i64)\n", var_name);
            }
        }
    }
    
    if (m_scope) {
        m_scope->gen_wat(ctx);
    }
    
    ctx.body() << "  )\n";

    if (is_main) {
        ctx.body() << "  (export \"main\" (func $main))\n";
    }

    return nullptr;
}

Node::Ptr Call::gen_wat(WasmContext &ctx) {
    std::string name_str = m_name->as_string();
    
    if (name_str.find("io") != std::string::npos && name_str.find("print") != std::string::npos) {
        auto args = std::dynamic_pointer_cast<FuncArgs>(m_args);
        if (args && !args->get_args().empty()) {
            auto arg = args->get_args()[0];
            
            arg->gen_wat(ctx);
            
            auto type = arg->get_stmt_type();
            std::string type_name = type ? type->get_id() : "";
            
            if (type_name == "String" || std::dynamic_pointer_cast<String>(arg)) {
                ctx.body() << "    call $io_print_s\n";
            } else if (type_name == "Boolean" || std::dynamic_pointer_cast<Boolean>(arg)) {
                ctx.body() << "    call $io_print_b\n";
            } else {
                ctx.body() << "    call $io_print_i\n";
            }
        }
    }
    return nullptr;
}

Node::Ptr Let::gen_wat(WasmContext &ctx) {
    if (m_init) {
        m_init->gen_wat(ctx);
        std::string var_name = "";
        if (auto id_node = std::dynamic_pointer_cast<Id>(m_name)) {
            var_name = id_node->get_id();
        } else {
            var_name = m_name->as_string();
             if (var_name.rfind("Id(", 0) == 0) {
                var_name = var_name.substr(3, var_name.length() - 4);
             }
        }
        ctx.body() << fmt::format("    local.set ${}\n", var_name);
    }
    return nullptr;
}

Node::Ptr Add::gen_wat(WasmContext &ctx) {
    m_left->gen_wat(ctx);
    m_right->gen_wat(ctx);
    ctx.body() << "    i64.add\n";
    return nullptr;
}

Node::Ptr OpEq::gen_wat(WasmContext &ctx) {
    m_left->gen_wat(ctx);
    m_right->gen_wat(ctx);
    ctx.body() << "    i64.eq\n";
    return nullptr;
}

Node::Ptr If::gen_wat(WasmContext &ctx) {
    m_cond->gen_wat(ctx);
    ctx.body() << "    (if\n";
    ctx.body() << "      (then\n";
    m_then->gen_wat(ctx);
    ctx.body() << "      )\n";
    if (m_else) {
        ctx.body() << "      (else\n";
        m_else->gen_wat(ctx);
        ctx.body() << "      )\n";
    }
    ctx.body() << "    )\n";
    return nullptr;
}

Node::Ptr Return::gen_wat(WasmContext &ctx) {
    if (m_val) {
        m_val->gen_wat(ctx);
    }
    ctx.body() << "    return\n";
    return nullptr;
}

Node::Ptr StmtList::gen_wat(WasmContext &ctx) {
    for (auto &s : m_stmts) {
        s->gen_wat(ctx);
    }
    return nullptr;
}

Node::Ptr Module::gen_wat(WasmContext &ctx) {
    for (auto &s : m_stmts) {
        s->gen_wat(ctx);
    }
    return nullptr;
}

Node::Ptr BinaryOp::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr BinaryOp::gen_wat(WasmContext &ctx) { return nullptr; }
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

Node::Ptr Let::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr Let::add_to_symtab_ordered(SymbolTable &st) { return nullptr; }
Node::Ptr FArg::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr FuncArgs::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr StmtList::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr Func::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr Func::add_to_symtab_forward(SymbolTable &st) { return nullptr; }
Node::Ptr Assignment::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr Assignment::gen_wat(WasmContext &ctx) { return nullptr; }
Node::SymTabEntry Class::get_subsymbol(Node::Ptr) const { return {}; }
Node::Ptr Class::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr Class::add_to_symtab_forward(SymbolTable &st) { return nullptr; }
Node::Ptr If::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr While::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr Import::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr Return::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr Dot::compute_stmt_type(SymbolTable &st) { return nullptr; }
Node::Ptr Call::compute_stmt_type(SymbolTable &st) { return nullptr; }

} // namespace ast
