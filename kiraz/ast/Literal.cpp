#include "Literal.h"
#include <kiraz/Compiler.h>
#include <fmt/format.h>

namespace ast {

Node::Ptr Integer::gen_wat(WasmContext &ctx) {
    ctx.body() << fmt::format("    i64.const {}\n", m_value);
    return nullptr;
}

Node::Ptr String::gen_wat(WasmContext &ctx) {
    auto coords = ctx.add_to_memory(m_value);
    ctx.body() << fmt::format("    i32.const {}\n", coords.offset);
    ctx.body() << fmt::format("    i32.const {}\n", coords.length);
    return nullptr;
}

Node::Ptr Boolean::gen_wat(WasmContext &ctx) {
    ctx.body() << fmt::format("    i32.const {}\n", m_value ? 1 : 0);
    return nullptr;
}

Node::Ptr Id::gen_wat(WasmContext &ctx) {
    ctx.body() << fmt::format("    local.get ${}\n", get_id());
    return nullptr;
}

}