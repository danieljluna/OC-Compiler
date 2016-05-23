%{
// djluna: Daniel Luna

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
%token TOK_EQ TOK_NE TOK_LE TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE
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
%left   TOK_EQ TOK_NE '<' TOK_LE '>' TOK_GE
%left   '+' '-'
%left   '*' '/' '%'
%right  POS NEG '!' TOK_NEW TOK_ORD TOK_CHR
%left   '[' '.' FN_PAREN
%left   EXPR_PAREN

%%

//program-Rules--------------------------------------------------------

start       : program               { $$ = $1 = nullptr; }
            ;
         
program     : program structdef     { $$ = $1->adopt($2); }
            | program function      { $$ = $1->adopt($2); }
            | program stmt          { $$ = $1->adopt($2); }
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
            
fielddec    : basetype ident        { $$ = $1->adopt($2,TOK_FIELD); }
            | basetype array ident  { $$ = $2->adopt($1,$3,TOK_FIELD);}
            ;
            
            
//function-Rules-------------------------------------------------------
            
function    : fndec block           { $$ = parseFn($1, $2); }
            ;
            
fndec       : identdec params ')'   { $$ = parseFn($1, $2, $3); }
            | identdec '(' ')'      { $$ = parseFn($1, $2, $3); }
            ;
            
params      : '(' identdec          { $$=$1->adopt(TOK_PARAMLIST,$2); }
            | params ',' identdec   { free($2); $$ = $1->adopt($3); }
            ;
            
            
//statement-Rules------------------------------------------------------

stmt        : block                 { $$ = $1; }
            | vardec                { $$ = $1; }
            | whilestmt             { $$ = $1; }
            | ifelse                { $$ = $1; }
            | returnstmt            { $$ = $1; }
            | expr ';'              { free($2); $$ = $1; }
            ;

vardec      : identdec '=' expr ';' { $$ = parseVarDec($1,$2,$3,$4); }
            ;
            
whilestmt   : while '(' expr ')' stmt 
                                    { free($2,$4);$$=$1->adopt($3,$5);}
            ;
            
ifelse      : TOK_IF '(' expr ')' stmt %prec TOK_IF
                                    { $$ = parseIf($1,$3,$5,$2,$4); }
            | TOK_IF '(' expr ')' stmt TOK_ELSE stmt
                                    {$$=parseIf($1,$3,$5,$7,$2,$4,$6);}
            ;
            
returnstmt  : return expr ';'       { free($3); $$ = $1->adopt($2); }
            | return ';'            { $$ = parseRetVoid($1, $2); }
            ;

            
//expression-Rules-----------------------------------------------------
            
expr        : expr '=' expr         { $$ = $2->adopt($1, $3); }
            | expr '*' expr         { $$ = $2->adopt($1, $3); }
            | expr '/' expr         { $$ = $2->adopt($1, $3); }
            | expr '%' expr         { $$ = $2->adopt($1, $3); }
            | expr '+' expr         { $$ = $2->adopt($1, $3); }
            | expr '-' expr         { $$ = $2->adopt($1, $3); }
            | expr TOK_EQ expr      { $$ = $2->adopt($1, $3); }
            | expr TOK_NE expr      { $$ = $2->adopt($1, $3); }
            | expr TOK_LE expr      { $$ = $2->adopt($1, $3); }
            | expr TOK_GE expr      { $$ = $2->adopt($1, $3); }
            | expr '<' expr         { $$ = $2->adopt($1, $3); }
            | expr '>' expr         { $$ = $2->adopt($1, $3); }
            | '!' expr              { $$ = $1->adopt($2); }
            | '+' expr   %prec POS  { $$ = $1->adopt(TOK_POS, $2); }
            | '-' expr   %prec NEG  { $$ = $1->adopt(TOK_NEG, $2); }
            | allocator             { $$ = $1; }
            | '(' expr ')'  %prec EXPR_PAREN
                                    { free($1, $3); $$ = $2; }
            | call                  { $$ = $1; }
            | variable              { $$ = $1; }
            | constant              { $$ = $1; }
            ;

allocator   : new ident '(' ')'     { $$ = parseAlloc($1,$2,$3,$4); }
            | new string '(' expr ')'
                                    { $$ = parseAlloc($1,$2,$4,$3,$5);}
            | new basetype '[' expr ']'
                                    { $$ = parseAlloc($1,$2,$4,$3,$5);}
            ;
            
call        : callargs ')'          { free($2); $$ = $1; }
            | ident '(' ')'         { free($3); $$ = parseCall($1,$2);}
            ;
            
callargs    : callargs ',' expr     { free($2); $$ = $1->adopt($3); }
            | ident '(' expr        { $$ = parseCall($1, $2, $3); }
            ;
            
variable    : ident                 { $$ = $1; }
            | expr '[' expr ']'     { $$ = parseIndex($1,$2,$3,$4); }
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

block       : '{' '}'               { free($2); $$=$1->sym(TOK_BLOCK);}
            | blockstmts '}'        { free($2); $$ = $1; }    
            | ';'                   { $$ = $1; }
            ;
            
blockstmts  : blockstmts stmt       { $$ = $1->adopt($2); }
            | '{' stmt              { $$ = $1->adopt(TOK_BLOCK, $2); }
            ;

identdec    : basetype ident        { $$=$1->adopt($2,TOK_DECLID); }
            | basetype array ident  { $$=$2->adopt($1,$3,TOK_DECLID);}
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
            
while       : TOK_WHILE             { $$ = $1; }
            ;
            
return      : TOK_RETURN            { $$ = $1; }
            ;


%%

const char* parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

