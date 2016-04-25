/* djluna: Daniel Luna */

%{

#include "lyutils.h"

#define YY_USER_ACTION  { lexer::advance(); }

#define RETURN(SYMBOL) { \
   yylval = new astree (SYMBOL, lexer::lloc, yytext); \
   return SYMBOL; \
}

%}

%option 8bit
%option debug
%option nodefault
%option nounput
%option noyywrap
%option verbose
%option warn

LETTER          [A-Za-z_]
DIGIT           [0-9]
ARRAY           \[\]
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
BADIDENT        ({DIGIT}({LETTER}|{DIGIT})*)
INT_CONST       ({DIGIT}+)
CHAR_CONST      ('([^\\"\n]|\\[\\'"0nt])')
STRING_CONST    (\"([^\\"\n]|\\[\\’"0nt])*\")


%%

"#".*           { lexer::include(); }
[ \t]+          { /* Skip whitespace. */ }
\n              { lexer::newline(); }

"("             { RETURN ('('); }
")"             { RETURN (')'); }
"["             { RETURN ('['); }
"]"             { RETURN (']'); }
{ARRAY}         { RETURN (ARRAY}; }
"{"             { RETURN ('{'); }
"}"             { RETURN ('}'); }
";"             { RETURN (';'); }
","             { RETURN (','); }
"."             { RETURN ('.'); }
"="             { RETURN ('='); }
"+"             { RETURN ('+'); }
"-"             { RETURN ('-'); }
"*"             { RETURN ('*'); }
"/"             { RETURN ('/'); }
"%"             { RETURN ('%'); }
"!"             { RETURN ('!'); }

"void"          { RETURN (TOK_VOID); }
"bool"          { RETURN (TOK_BOOL); }
"char"          { RETURN (TOK_CHAR); }
"int"           { RETURN (TOK_INT); }
"string"        { RETURN (TOK_STRING); }
"if"            { RETURN (TOK_IF); }
"else"          { RETURN (TOK_ELSE); }
"while"         { RETURN (TOK_WHILE); }
"return"        { RETURN (TOK_RETURN); }
"struct"        { RETURN (TOK_STRUCT); }
"false"         { RETURN (TOK_FALSE); }
"true"          { RETURN (TOK_TRUE); }
"null"          { RETURN (TOK_NULL); }
"new"           { RETURN (TOK_NEW); }
"ord"           { RETURN (TOK_ORD); }
"chr"           { RETURN (TOK_CHR); }

{IDENT}         { RETURN (TOK_IDENT); }
{INT_CONST}     { RETURN (TOK_INTCON); }
{CHAR_CONST}    { RETURN (TOK_CHARCON); }
{STRING_CONST}  { RETURN (TOK_STRINGCON); }


.               { lexer::badchar (*yytext); }

%%
