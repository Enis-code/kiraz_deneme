#ifndef KIRAZ_AST_OPERATOR_H
#define KIRAZ_AST_OPERATOR_H

#include <cassert>

#include <kiraz/Node.h>

namespace ast {

class BinaryOp : public Node {
public:
    BinaryOp(Node::Ptr left, Node::Ptr right) 
        : m_left(left), m_right(right) {}
    
    Node::Ptr get_left() const { return m_left; }
    Node::Ptr get_right() const { return m_right; }
    
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
};

class Sub : public BinaryOp {
public:
    Sub(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("Sub(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class Mult : public BinaryOp {
public:
    Mult(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("Mult(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class Div : public BinaryOp {
public:
    Div(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("DivF(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class OpEq : public BinaryOp {
public:
    OpEq(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpEq(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class OpNe : public BinaryOp {
public:
    OpNe(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpNe(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class OpLt : public BinaryOp {
public:
    OpLt(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpLt(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class OpGt : public BinaryOp {
public:
    OpGt(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpGt(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class OpLe : public BinaryOp {
public:
    OpLe(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpLe(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
};

class OpGe : public BinaryOp {
public:
    OpGe(Node::Ptr left, Node::Ptr right) : BinaryOp(left, right) {}
    
    std::string as_string() const override {
        return fmt::format("OpGe(l={}, r={})", m_left->as_string(), m_right->as_string());
    }
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
    const std::vector<Node::Ptr>& get_stmts() const { return m_stmts; }
    
private:
    std::vector<Node::Ptr> m_stmts;
};

class Func : public Node {
public:
    Func(Node::Ptr name, Node::Ptr args, Node::Ptr ret_type, Node::Ptr scope) 
        : m_name(name), m_args(args), m_ret_type(ret_type), m_scope(scope) {}
    
    std::string as_string() const override {
        return fmt::format("Func(n={}, a={}, r={}, s={})", 
                          m_name->as_string(), 
                          m_args->as_string(), 
                          m_ret_type->as_string(),
                          m_scope->as_string());
    }
    
    bool is_func() const override { return true; }
    
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
    
private:
    Node::Ptr m_name;
    Node::Ptr m_value;
};

}

#endif // KIRAZ_AST_OPERATOR_H
