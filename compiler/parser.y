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
%printer { astree::dump(yyoutput, $$); } <>

%token TOK_VOID TOK_BOOL TOK_CHAR TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_FALSE TOK_TRUE TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token TOK_ORD TOK_CHR TOK_ROOT

%initial-action {
   parser::root = new astree(TOK_ROOT, {0, 0, 0}, "<<ROOT>>");
}

%start start

%%

start       : program                  { $$ = $1 = nullptr; }
            ;
         
program     : program structdef        { $$ = $1->adopt($2); }
            //| program function         { $$ = $1->adopt($2); }
            //| program statement        { $$ = $1->adopt($2); }
            | program error '}'        { $$ = $1; free($3);}
            | program error ';'        { $$ = $1; free($3); }
            |                          { $$ = parser::root; }
            ;
         
structdec   : TOK_STRUCT ident   {$$ = $1->sym_adopt($2, TOK_TYPEID);}
            ;
         
structdef   : fielddecs '}'            { $$ = $1; free($2); }
            | structdec '{' '}'        { $$ = $1; free($2, $3); }
            ;
            
fielddecs   : fielddecs fielddec ';'   { free($3); $$ = $1->adopt($2);}
            | structdec '{' fielddec ';' { free($2, $4); $$ = $1->adopt($3); }
            ;
            
fielddec    : basetype ident           { $$ = $1->sym_adopt($2, TOK_FIELD); }
            | basetype array ident     { $$ = $2->adopt($1, $3); }
            ;
            
basetype    : TOK_VOID                 { $$ = $1; }
            | TOK_BOOL                 { $$ = $1; }
            | TOK_CHAR                 { $$ = $1; }
            | TOK_INT                  { $$ = $1; }
            | TOK_STRING               { $$ = $1; }
            | ident                    { $$ = $1->sym(TOK_TYPEID); }
            ;
            
//Below rules are only for saving space when naming tokens-------------
ident       : TOK_IDENT                { $$ = $1; }
            ;
            
array       : TOK_ARRAY                { $$ = $1; }
            
%%

const char* parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

