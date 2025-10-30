#ifndef KIRAZ_TOKEN_LITERAL_H
#define KIRAZ_TOKEN_LITERAL_H

#include <kiraz/Token.h>

namespace token {

class Integer : public Token {
public:
    Integer(int id, int base, std::string text) 
        : Token(id), m_base(base), m_text(text) {}
    
    std::string as_string() const override { 
        return fmt::format("L_INTEGER({}, \"{}\")", m_base, m_text); 
    }
    
    int get_base() const { return m_base; }
    const std::string& get_text() const { return m_text; }
    
private:
    int m_base;
    std::string m_text;
};

class String : public Token {
public:
    String(int id, std::string text) 
        : Token(id), m_text(text) {}
    
    std::string as_string() const override { 
        return fmt::format("L_STRING(\"{}\")", m_text); 
    }
    
    const std::string& get_text() const { return m_text; }
    
private:
    std::string m_text;
};

class Identifier : public Token {
public:
    Identifier(int id, std::string name) 
        : Token(id), m_name(name) {}
    
    std::string as_string() const override { 
        return fmt::format("IDENTIFIER({})", m_name); 
    }
    
    const std::string& get_name() const { return m_name; }
    
private:
    std::string m_name;
};

class Keyword : public Token {
public:
    Keyword(int id, std::string name) 
        : Token(id), m_name(name) {}
    
    std::string as_string() const override { 
        return m_name; 
    }
    
    const std::string& get_name() const { return m_name; }
    
private:
    std::string m_name;
};

}

#endif // KIRAZ_TOKEN_LITERAL_H
