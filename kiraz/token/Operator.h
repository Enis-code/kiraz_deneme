#ifndef KIRAZ_TOKEN_OPERATOR_H
#define KIRAZ_TOKEN_OPERATOR_H

#include <kiraz/Token.h>

namespace token {

class Operator : public Token {
public:
    Operator(int id, std::string op) 
        : Token(id), m_op(op) {}
    
    std::string as_string() const override { 
        return m_op; 
    }
    
    const std::string& get_op() const { return m_op; }
    
private:
    std::string m_op;
};

}

#endif
