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
%token TOK_ORD TOK_CHR TOK_ROOT TOK_DECLID TOK_INDEX 
%token TOK_NEWSTRING TOK_RETURNVOID TOK_VARDECL TOK_FUNCTION
%token TOK_PARAMLIST TOK_PROTOTYPE

%initial-action {
   parser::root = new astree(TOK_ROOT, {0, 0, 0}, "<<ROOT>>");
}

%start start

%right  TOK_IF TOK_ELSE
%right  '='
%left   TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left   '+' '-'
%left   '*' '/' '%'
%right  POS NEG '!' TOK_NEW TOK_ORD TOK_CHR
%left   ']' '.' FN_PAREN
%left   EXPR_PAREN

%%

//program-Rules--------------------------------------------------------

start       : program               { $$ = $1 = nullptr; }
            ;
         
program     : program structdef     { $$ = $1->adopt($2); }
            //| program function      { $$ = $1->adopt($2); }
            | program statement     { $$ = $1->adopt($2); }
            | program error '}'     { $$ = $1; free($3);}
            | program error ';'     { $$ = $1; free($3); }
            |                       { $$ = parser::root; }
            ;

            
//structdef-Rules------------------------------------------------------
            
structdec   : TOK_STRUCT ident      { $$ = $1->adopt($2, TOK_TYPEID);}
            ;
         
structdef   : fielddecs '}'         { $$ = $1; free($2); }
            | structdec '{' '}'     { $$ = $1; free($2, $3); }
            ;
            
fielddecs   : fielddecs fielddec ';'   
                                    { free($3); $$ = $1->adopt($2);}
            | structdec '{' fielddec ';' 
                                    { free($2, $4); $$=$1->adopt($3);}
            ;
            
fielddec    : basetype ident        { $$=$1->adopt($2,TOK_FIELD); }
            | basetype array ident  { $$ = $2->adopt($1,$3,TOK_FIELD);}
            ;
            
            
//function-Rules-------------------------------------------------------
            
            
            
            
//statement-Rules------------------------------------------------------

statement   : block                 { $$ = $1; }
            | vardec                { $$ = $1; }
            //| while                 { $$ = $1; }
            //| ifelse                { $$ = $1; }
            //| return                { $$ = $1; }
            | expr ';'              { free($2); $$ = $1; }
            ;

vardec      : identdec '=' expr ';' { free($4); $$ = $2->adopt(TOK_VARDECL,$1,$3);}
            ;
            
identdec    : basetype ident        { $$=$1->adopt($2,TOK_DECLID); }
            | basetype array ident  { $$=$2->adopt($1,$3,TOK_DECLID);}
            ;

            
//expr-Rules-----------------------------------------------------------
            
expr        : expr '=' expr         { $$ = $2->adopt($1, $3); }
            | allocator             { $$ = $1; }
            | '(' expr ')'  %prec EXPR_PAREN
                                    { free($1, $3); $$ = $2; }
            | variable              { $$ = $1; }
            | constant              { $$ = $1; }
            ;

allocator   : new ident '(' ')'     { free($3, $4); $$ = $1->adopt($2, TOK_TYPEID); }
            | new string '(' expr ')'
                                    { free($3, $5); $$ = $1->adopt(TOK_NEWSTRING, $2, $4); }
            | new basetype '[' expr ']'
                                    { free($3, $5); $$ = $1->adopt(TOK_NEWARRAY, $2, $4); }
            ;
            
variable    : ident                 { $$ = $1; }
            | expr '[' expr ']'     { free($4); $$ = $2->adopt(TOK_INDEX,$1,$3);}
            | expr '.' ident        { $$ = $2->adopt($1,$3,TOK_FIELD);}
            ;
            
constant    : TOK_INTCON            { $$ = $1; }
            | TOK_CHARCON           { $$ = $1; }
            | TOK_STRINGCON         { $$ = $1; }
            | TOK_TRUE              { $$ = $1; }
            | TOK_FALSE             { $$ = $1; }
            | TOK_NULL              { $$ = $1; }
            ;
            
            
//Universally-Useful-Rules---------------------------------------------

block       : '{' '}'               { free($2); $$ = $1; }
            | blockstmts '}'        { free($2); $$ = $1; }    
            | ';'                   { $$ = $1; }
            ;
            
blockstmts  : blockstmts statement  { $$ = $1->adopt($2); }
            | '{' statement         { $$ = $1->adopt($2); }
            ;

basetype    : TOK_VOID              { $$ = $1; }
            | TOK_BOOL              { $$ = $1; }
            | TOK_CHAR              { $$ = $1; }
            | TOK_INT               { $$ = $1; }
            | string                { $$ = $1; }
            | ident                 { $$ = $1->sym(TOK_TYPEID); }
            ;
            
            
//Below rules are only for saving space when naming tokens-------------

ident       : TOK_IDENT             { $$ = $1; }
            ;
            
array       : TOK_ARRAY             { $$ = $1; }
            ;
            
new         : TOK_NEW               { $$ = $1; }
            ;
            
string      : TOK_STRING            { $$ = $1; }
            ;
            
            
%%

const char* parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

