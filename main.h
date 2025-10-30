
#pragma once

#include <memory>
#include <cstdio>

#include <fmt/format.h>
#include <fmt/ranges.h>

class Node;
extern "C" int yylex(void);
extern "C" int yyparse(void);
extern int yydebug;
extern FILE *yyin;
#define YY_DECL int yylex(void)
#define YYSTYPE std::shared_ptr<Node>
#include "parser.hpp"
