%{
#include "lexer.hpp"

#include <kiraz/ast/Operator.h>
#include <kiraz/ast/Literal.h>

#include <kiraz/token/Literal.h>

int yyerror(const char *msg);
extern std::shared_ptr<Token> curtoken;
extern int yylineno;
%}

%token    REJECTED

/* Keywords */
%token    KW_IMPORT
%token    KW_FUNC
%token    KW_IF
%token    KW_ELSE
%token    KW_WHILE
%token    KW_CLASS
%token    KW_LET
%token    KW_RETURN

/* Literals and Identifiers */
%token    L_INTEGER
%token    L_STRING
%token    IDENTIFIER

/* Operators and Punctuation */
%token    OP_LBRACE      /* { */
%token    OP_RBRACE      /* } */
%token    OP_LPAREN      /* ( */
%token    OP_RPAREN      /* ) */
%token    OP_PLUS        /* + */
%token    OP_MINUS       /* - */
%token    OP_MULT        /* * */
%token    OP_DIV         /* / */
%token    OP_LT          /* < */
%token    OP_GT          /* > */
%token    OP_EQ          /* == */
%token    OP_NE          /* != */
%token    OP_LE          /* <= */
%token    OP_GE          /* >= */
%token    OP_NOT         /* ! */
%token    OP_ASSIGN      /* = */
%token    OP_COLON       /* : */
%token    OP_SCOLON      /* ; */
%token    OP_COMMA       /* , */
%token    OP_DOT         /* . */

/* Operator precedence */
%left OP_EQ OP_NE OP_LT OP_GT OP_LE OP_GE
%left OP_PLUS OP_MINUS
%left OP_MULT OP_DIV
%left OP_DOT
%nonassoc UMINUS

%%

/* Program entry point - a single statement */
stmt: func_stmt                 { Node::add<ast::StmtList>(std::vector{$1}); }
    | func_stmt OP_SCOLON       { Node::add<ast::StmtList>(std::vector{$1}); }
    | let_stmt OP_SCOLON        { Node::add<ast::StmtList>(std::vector{$1}); }
    | assignment_stmt OP_SCOLON { Node::add<ast::StmtList>(std::vector{$1}); }
    | class_stmt OP_SCOLON      { Node::add<ast::StmtList>(std::vector{$1}); }
    | if_stmt OP_SCOLON         { Node::add<ast::StmtList>(std::vector{$1}); }
    | while_stmt OP_SCOLON      { Node::add<ast::StmtList>(std::vector{$1}); }
    | import_stmt               { Node::add<ast::StmtList>(std::vector{$1}); }
    | return_stmt               { Node::add<ast::StmtList>(std::vector{$1}); }
    | expr OP_SCOLON            { Node::add<ast::StmtList>(std::vector{$1}); }
    | REJECTED                  { yyerror("Rejected token"); }
    ;

/* Function definition */
func_stmt: KW_FUNC IDENTIFIER OP_LPAREN func_args OP_RPAREN OP_COLON type_annotation OP_LBRACE stmt_list OP_RBRACE {
        $$ = Node::add<ast::Func>($2, $4, $7, $9);
      }
    ;

/* Function arguments */
func_args: /* empty */          { $$ = std::make_shared<ast::FuncArgs>(std::vector<Node::Ptr>{}); }
    | func_arg_list             { $$ = $1; }
    ;

func_arg_list: func_arg                         { 
        $$ = std::make_shared<ast::FuncArgs>(std::vector{$1}); 
      }
    | func_arg_list OP_COMMA func_arg           { 
        auto list = std::dynamic_pointer_cast<ast::FuncArgs>($1);
        list->add($3);
        $$ = $1;
      }
    ;

func_arg: IDENTIFIER OP_COLON type_annotation   {
        $$ = std::make_shared<ast::FArg>($1, $3);
      }
    ;

/* Type annotation */
type_annotation: IDENTIFIER     {
        $$ = $1;
      }
    ;

/* Let statement */
let_stmt: KW_LET IDENTIFIER OP_ASSIGN expr {
        $$ = std::make_shared<ast::Let>($2, nullptr, $4);
      }
    | KW_LET IDENTIFIER OP_COLON type_annotation {
        $$ = std::make_shared<ast::Let>($2, $4, nullptr);
      }
    | KW_LET IDENTIFIER OP_COLON type_annotation OP_ASSIGN expr {
        $$ = std::make_shared<ast::Let>($2, $4, $6);
      }
    ;

/* Assignment statement */
assignment_stmt: IDENTIFIER OP_ASSIGN expr {
        $$ = std::make_shared<ast::Assignment>($1, $3);
      }
    ;

/* Class definition */
class_stmt: KW_CLASS IDENTIFIER OP_LBRACE stmt_list OP_RBRACE {
        $$ = Node::add<ast::Class>($2, $4);
      }
    ;

/* If statement */
if_stmt: KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list OP_RBRACE {
        $$ = Node::add<ast::If>($3, $6, std::make_shared<ast::StmtList>(std::vector<Node::Ptr>{}));
      }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list OP_RBRACE KW_ELSE OP_LBRACE stmt_list OP_RBRACE {
        $$ = Node::add<ast::If>($3, $6, $10);
      }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list OP_RBRACE KW_ELSE if_stmt {
        $$ = Node::add<ast::If>($3, $6, $9);
      }
    ;

/* While statement */
while_stmt: KW_WHILE OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list OP_RBRACE {
        $$ = Node::add<ast::While>($3, $6);
      }
    ;

/* Import statement */
import_stmt: KW_IMPORT IDENTIFIER OP_SCOLON {
        $$ = Node::add<ast::Import>($2);
      }
    ;

/* Return statement */
return_stmt: KW_RETURN expr OP_SCOLON {
        $$ = Node::add<ast::Return>($2);
      }
    ;

/* Statement list (for function body) */
stmt_list: /* empty */                    { $$ = std::make_shared<ast::StmtList>(std::vector<Node::Ptr>{}); }
    | stmt_list let_stmt OP_SCOLON        { 
        auto list = std::dynamic_pointer_cast<ast::StmtList>($1);
        list->add($2);
        $$ = $1;
      }
    | stmt_list assignment_stmt OP_SCOLON { 
        auto list = std::dynamic_pointer_cast<ast::StmtList>($1);
        list->add($2);
        $$ = $1;
      }
    | stmt_list expr OP_SCOLON            { 
        auto list = std::dynamic_pointer_cast<ast::StmtList>($1);
        list->add($2);
        $$ = $1;
      }
    | stmt_list func_stmt                 { 
        auto list = std::dynamic_pointer_cast<ast::StmtList>($1);
        list->add($2);
        $$ = $1;
      }
    | stmt_list func_stmt OP_SCOLON      { 
        auto list = std::dynamic_pointer_cast<ast::StmtList>($1);
        list->add($2);
        $$ = $1;
      }
    | stmt_list class_stmt OP_SCOLON     { 
        auto list = std::dynamic_pointer_cast<ast::StmtList>($1);
        list->add($2);
        $$ = $1;
      }
    | stmt_list if_stmt OP_SCOLON        { 
        auto list = std::dynamic_pointer_cast<ast::StmtList>($1);
        list->add($2);
        $$ = $1;
      }
    | stmt_list while_stmt OP_SCOLON     { 
        auto list = std::dynamic_pointer_cast<ast::StmtList>($1);
        list->add($2);
        $$ = $1;
      }
    | stmt_list import_stmt              { 
        auto list = std::dynamic_pointer_cast<ast::StmtList>($1);
        list->add($2);
        $$ = $1;
      }
    | stmt_list return_stmt              { 
        auto list = std::dynamic_pointer_cast<ast::StmtList>($1);
        list->add($2);
        $$ = $1;
      }
    ;

/* Expressions */
expr: L_INTEGER                  { $$ = $1; }
    | L_STRING                   { $$ = $1; }
    | IDENTIFIER                 { $$ = $1; }
    | OP_LPAREN expr OP_RPAREN   { $$ = $2; }
    | OP_MINUS expr %prec UMINUS { $$ = std::make_shared<ast::Signed>("OP_MINUS", $2); }
    | expr OP_PLUS expr          { $$ = std::make_shared<ast::Add>($1, $3); }
    | expr OP_MINUS expr         { $$ = std::make_shared<ast::Sub>($1, $3); }
    | expr OP_MULT expr          { $$ = std::make_shared<ast::Mult>($1, $3); }
    | expr OP_DIV expr           { $$ = std::make_shared<ast::Div>($1, $3); }
    | expr OP_EQ expr            { $$ = std::make_shared<ast::OpEq>($1, $3); }
    | expr OP_NE expr            { $$ = std::make_shared<ast::OpNe>($1, $3); }
    | expr OP_LT expr            { $$ = std::make_shared<ast::OpLt>($1, $3); }
    | expr OP_GT expr            { $$ = std::make_shared<ast::OpGt>($1, $3); }
    | expr OP_LE expr            { $$ = std::make_shared<ast::OpLe>($1, $3); }
    | expr OP_GE expr            { $$ = std::make_shared<ast::OpGe>($1, $3); }
    | expr OP_DOT IDENTIFIER     { $$ = std::make_shared<ast::Dot>($1, $3); }
    | expr OP_LPAREN call_args OP_RPAREN { $$ = std::make_shared<ast::Call>($1, $3); }
    ;

/* Call arguments */
call_args: /* empty */           { $$ = std::make_shared<ast::FuncArgs>(std::vector<Node::Ptr>{}); }
    | call_arg_list              { $$ = $1; }
    ;

call_arg_list: expr              { 
        $$ = std::make_shared<ast::FuncArgs>(std::vector{$1}); 
      }
    | call_arg_list OP_COMMA expr { 
        auto list = std::dynamic_pointer_cast<ast::FuncArgs>($1);
        list->add($3);
        $$ = $1;
      }
    ;

%%

int yyerror(const char *s) {
    if (curtoken) {
        fmt::print("** Parser Error at {}:{} at token: {}\n",
            yylineno, Token::colno, curtoken->as_string());
    }
    else {
        fmt::print("** Parser Error at {}:{}, null token\n",
            yylineno, Token::colno);
    }

    Token::colno = 0;
    Node::reset_root();

    return 1;
}
