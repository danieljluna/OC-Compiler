%{
// Dummy parser for scanner project.

#include <cassert>

#include "lyutils.h"
#include "astree.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%destructor { delete $$; } <>
%printer { $$->dump_node(yyoutput); } <>

%initial-action {
   parser::root = new astree(TOK_ROOT, {0, 0, 0}, "<<ROOT>>");
}

%token TOK_VOID TOK_BOOL TOK_CHAR TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_FALSE TOK_TRUE TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token TOK_ORD TOK_CHR TOK_ROOT

%right TOK_IF TOK_ELSE
%right 

%start start

%%

start       : program                  { $$ = $1 = nullptr; }
            ;
         
program     : program structdef        { $$ = $1->adopt($2); }
            //| program function         { $$ = $1->adopt($2); }
            //| program statement        { $$ = $1->adopt($2); }
            | program error '}'        { $$ = $1; }
            | program error ';'        { $$ = $1; }
            |                          { $$ = parser::root; }
            ;
         
structdecl  : TOK_STRUCT TOK_IDENT     { $$->adopt($1, $2); }
            ;
         
structdef   : structdecl '{' '}'       { $$ = $1->adopt($2); }
            | structdecl fieldlist '}' { $$ = $}
            ;
%%

const char* parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

