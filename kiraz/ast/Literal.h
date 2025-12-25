#ifndef KIRAZ_AST_LITERAL_H
#define KIRAZ_AST_LITERAL_H

#include <kiraz/Node.h>
#include <kiraz/Compiler.h>

namespace ast {

class Integer : public Node {
public:
    Integer(int64_t v) : m_value(v) {}
    std::string as_string() const override { return FF("Int({})", m_value); }
    Node::Ptr gen_wat(WasmContext &ctx) override;
private:
    int64_t m_value;
};

class String : public Node {
public:
    String(const std::string &v) : m_value(v) {}
    std::string as_string() const override { return FF("Str({})", m_value); }
    Node::Ptr gen_wat(WasmContext &ctx) override;
private:
    std::string m_value;
};

class Boolean : public Node {
public:
    Boolean(bool v) : m_value(v) {}
    std::string as_string() const override { return FF("Bool({})", m_value); }
    Node::Ptr gen_wat(WasmContext &ctx) override;
private:
    bool m_value;
};

class SignedNode : public Node {
public:
};

class Id : public Node {
public:
    Id(const std::string &n) { set_id(n); }
    std::string as_string() const override { return FF("Id({})", get_id()); }
    Node::Ptr gen_wat(WasmContext &ctx) override;
};

}

#endif