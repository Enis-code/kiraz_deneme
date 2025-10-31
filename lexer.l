
%option noyywrap
%option yylineno

%{
// https://stackoverflow.com/questions/9611682/flexlexer-support-for-unicode/9617585#9617585
#include "main.h"
#include <kiraz/token/Literal.h>
#include <kiraz/token/Operator.h>
#include <kiraz/ast/Literal.h>
#include <kiraz/ast/Operator.h>
static auto &colno = Token::colno;
using namespace token;
extern std::shared_ptr<Token> curtoken;
%}

DIGIT       [0-9]
LETTER      [a-zA-Z]
IDENTIFIER  [a-zA-Z_][a-zA-Z0-9_]*
INTEGER     {DIGIT}+
WHITESPACE  [ \t\r]+

%%

    /* Whitespace - increment column counter */
{WHITESPACE}    { colno += yyleng; }

    /* Newline - reset column counter */
\n              { colno = 0; }

    /* Keywords */
"import"        { colno += yyleng; curtoken = Token::New<Keyword>(KW_IMPORT, yytext); return KW_IMPORT; }
"func"          { colno += yyleng; curtoken = Token::New<Keyword>(KW_FUNC, yytext); return KW_FUNC; }
"if"            { colno += yyleng; curtoken = Token::New<Keyword>(KW_IF, yytext); return KW_IF; }
"else"          { colno += yyleng; curtoken = Token::New<Keyword>(KW_ELSE, yytext); return KW_ELSE; }
"while"         { colno += yyleng; curtoken = Token::New<Keyword>(KW_WHILE, yytext); return KW_WHILE; }
"class"         { colno += yyleng; curtoken = Token::New<Keyword>(KW_CLASS, yytext); return KW_CLASS; }
"let"           { colno += yyleng; curtoken = Token::New<Keyword>(KW_LET, yytext); return KW_LET; }
"return"        { colno += yyleng; curtoken = Token::New<Keyword>(KW_RETURN, yytext); return KW_RETURN; }

    /* Identifiers */
{IDENTIFIER}    { colno += yyleng; 
                  curtoken = Token::New<Identifier>(IDENTIFIER, yytext);
                  yylval = std::make_shared<ast::Id>(yytext);
                  return IDENTIFIER; }

    /* Integer Literals (base 10) */
{INTEGER}       { colno += yyleng; 
                  curtoken = Token::New<Integer>(L_INTEGER, 10, yytext); 
                  yylval = std::make_shared<ast::Integer>(10, yytext);
                  return L_INTEGER; }

    /* String Literals */
\"[^\"]*\"      { colno += yyleng; 
                  std::string str(yytext + 1, yyleng - 2);
                  size_t pos = 0;
                  while ((pos = str.find("\\n", pos)) != std::string::npos) {
                      str.replace(pos, 2, "\n");
                      pos += 1;
                  }
                  pos = 0;
                  while ((pos = str.find("\\t", pos)) != std::string::npos) {
                      str.replace(pos, 2, "\t");
                      pos += 1;
                  }
                  pos = 0;
                  while ((pos = str.find("\\r", pos)) != std::string::npos) {
                      str.replace(pos, 2, "\r");
                      pos += 1;
                  }
                  pos = 0;
                  while ((pos = str.find("\\\\", pos)) != std::string::npos) {
                      str.replace(pos, 2, "\\");
                      pos += 1;
                  }
                  pos = 0;
                  while ((pos = str.find("\\\"", pos)) != std::string::npos) {
                      str.replace(pos, 2, "\"");
                      pos += 1;
                  }
                  curtoken = Token::New<String>(L_STRING, str); 
                  yylval = std::make_shared<ast::String>(str);
                  return L_STRING; }

    /* Operators */
"--"+           { colno += yyleng; curtoken = Token::New<Rejected>(yytext); return REJECTED; }
"=="            { colno += yyleng; curtoken = Token::New<Operator>(OP_EQ, yytext); return OP_EQ; }
"!="            { colno += yyleng; curtoken = Token::New<Operator>(OP_NE, yytext); return OP_NE; }
"<="            { colno += yyleng; curtoken = Token::New<Operator>(OP_LE, yytext); return OP_LE; }
">="            { colno += yyleng; curtoken = Token::New<Operator>(OP_GE, yytext); return OP_GE; }
"{"             { colno += yyleng; curtoken = Token::New<Operator>(OP_LBRACE, yytext); return OP_LBRACE; }
"}"             { colno += yyleng; curtoken = Token::New<Operator>(OP_RBRACE, yytext); return OP_RBRACE; }
"("             { colno += yyleng; curtoken = Token::New<Operator>(OP_LPAREN, yytext); return OP_LPAREN; }
")"             { colno += yyleng; curtoken = Token::New<Operator>(OP_RPAREN, yytext); return OP_RPAREN; }
"+"             { colno += yyleng; curtoken = Token::New<Operator>(OP_PLUS, yytext); return OP_PLUS; }
"-"             { colno += yyleng; curtoken = Token::New<Operator>(OP_MINUS, yytext); return OP_MINUS; }
"*"             { colno += yyleng; curtoken = Token::New<Operator>(OP_MULT, yytext); return OP_MULT; }
"/"             { colno += yyleng; curtoken = Token::New<Operator>(OP_DIV, yytext); return OP_DIV; }
"<"             { colno += yyleng; curtoken = Token::New<Operator>(OP_LT, yytext); return OP_LT; }
"="             { colno += yyleng; curtoken = Token::New<Operator>(OP_ASSIGN, yytext); return OP_ASSIGN; }
">"             { colno += yyleng; curtoken = Token::New<Operator>(OP_GT, yytext); return OP_GT; }
"!"             { colno += yyleng; curtoken = Token::New<Operator>(OP_NOT, yytext); return OP_NOT; }
":"             { colno += yyleng; curtoken = Token::New<Operator>(OP_COLON, yytext); return OP_COLON; }
";"             { colno += yyleng; curtoken = Token::New<Operator>(OP_SCOLON, yytext); return OP_SCOLON; }
","             { colno += yyleng; curtoken = Token::New<Operator>(OP_COMMA, yytext); return OP_COMMA; }
"."             { colno += yyleng; curtoken = Token::New<Operator>(OP_DOT, yytext); return OP_DOT; }

    /* Reject anything else */
.               { colno += yyleng; curtoken = Token::New<Rejected>(yytext); return REJECTED; }
