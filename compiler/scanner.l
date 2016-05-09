/* djluna: Daniel Luna */

%{

#include "lyutils.h"

#define YY_USER_ACTION  { lexer::advance(); }

#define RETURN(SYMBOL) { \
   yylval = new astree (SYMBOL, lexer::lloc, yytext); \
   lexer::output(SYMBOL); \
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
BAD_IDENT       ({DIGIT}({LETTER}|{DIGIT})*)
INT_CONST       ({DIGIT}+)
CHAR_CONST      ('([^\\'\n]|\\[\\'"0nt])')
STRING_CONST    (\"([^\\"\n]|\\[\\'"0nt])*\")
BAD_CHAR        ('([^'\n]*)['\n])
BAD_STRING      (\"([^\"\n]*)[\n\"])


%%

"#".*           { lexer::include(); }
[ \t]+          { /* Skip whitespace. */ }
\n              { lexer::newline(); }

"("             { RETURN ('('); }
")"             { RETURN (')'); }
{ARRAY}         { RETURN (TOK_ARRAY); }
"["             { RETURN ('['); }
"]"             { RETURN (']'); }
"{"             { RETURN ('{'); }
"}"             { RETURN ('}'); }
";"             { RETURN (';'); }
","             { RETURN (','); }
"."             { RETURN ('.'); }
"=="            { RETURN (TOK_EQ); }
"!="            { RETURN (TOK_NE); }
"<="            { RETURN (TOK_LE); }
">="            { RETURN (TOK_GE); }
"<"             { RETURN (TOK_LT); }
">"             { RETURN (TOK_GT); }
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

{INT_CONST}     { RETURN (TOK_INTCON); }
{CHAR_CONST}    { RETURN (TOK_CHARCON); }
{BAD_CHAR}      { lexer::badtoken(yytext); }
{STRING_CONST}  { RETURN (TOK_STRINGCON); }
{BAD_STRING}    { lexer::badtoken(yytext); }
{IDENT}         { RETURN (TOK_IDENT); }


.               { lexer::badchar (*yytext); }

%%
