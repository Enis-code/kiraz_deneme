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
    
private:
    int m_base;
    std::string m_text;
    int64_t m_value;
};

class String : public Node {
public:
    String(const std::string &value) : m_value(value) {}
    
    std::string as_string() const override {
<<<<<<< HEAD
        return fmt::format("Str({})", m_value);
=======
        // Escape characters
        std::string escaped = m_value;
        size_t pos = 0;
        while ((pos = escaped.find('\n', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\n");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find('\t', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\t");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find('\r', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\r");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\\"");
            pos += 2;
        }
        return fmt::format("Str({})", escaped);
>>>>>>> d58639422717638a95d6b4a7567efe7c9289aee1
    }
    
    const std::string& get_value() const { return m_value; }
    
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
    
private:
    std::string m_op;
    Node::Ptr m_operand;
};

}

#endif
