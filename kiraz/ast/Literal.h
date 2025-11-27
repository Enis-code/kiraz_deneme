#ifndef KIRAZ_AST_LITERAL_H
#define KIRAZ_AST_LITERAL_H

#include <kiraz/Node.h>

namespace ast {

class Id : public Node {
public:
    Id(const std::string &name) : m_name(name) {}
    
    std::string as_string() const override {
        return fmt::format("Id({})", m_name);
    }
    
    const std::string& get_name() const { return m_name; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::SymTabEntry get_symbol(const SymbolTable &st) const override;
    Node::SymTabEntry get_symbol() const override;
    Node::SymTabEntry get_subsymbol(Node::Ptr) const override;
    
private:
    std::string m_name;
};

class Integer : public Node {
public:
    Integer(int base, const std::string &text) 
        : m_base(base), m_text(text) {
        // Convert text to integer value
        m_value = std::stoll(text, nullptr, base);
    }
    
    Integer(int base, int64_t value) 
        : m_base(base), m_value(value) {
        m_text = std::to_string(value);
    }
    
    std::string as_string() const override {
        return fmt::format("Int({})", m_value);
    }
    
    int64_t get_value() const { return m_value; }
    int get_base() const { return m_base; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    int m_base;
    std::string m_text;
    int64_t m_value;
};

class String : public Node {
public:
    String(const std::string &value) : m_value(value) {}
    
    std::string as_string() const override {
        return fmt::format("Str({})", m_value);
    }
    
    const std::string& get_value() const { return m_value; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    std::string m_value;
};

class Signed : public Node {
public:
    Signed(const std::string &op, Node::Ptr operand) 
        : m_op(op), m_operand(operand) {}
    
    std::string as_string() const override {
        return fmt::format("Signed({}, {})", m_op, m_operand->as_string());
    }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    
private:
    std::string m_op;
    Node::Ptr m_operand;
};

class BuiltinType : public Node {
public:
    BuiltinType(const std::string &name) : m_name(name) {}
    
    std::string as_string() const override {
        return fmt::format("Builtin({})", m_name);
    }
    
    const std::string& get_name() const { return m_name; }
    
    Node::Ptr compute_stmt_type(SymbolTable &st) override;
    Node::SymTabEntry get_symbol(const SymbolTable &st) const override;
    Node::SymTabEntry get_symbol() const override;
    
private:
    std::string m_name;
};

}

#endif
