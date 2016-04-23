/* $Id: lexer.l,v 1.4 2015-11-13 13:50:39-08 - - $ */

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
TOK_IDENT       ({LETTER}({LETTER}|{DIGIT})*)
TOK_INTCON      ({DIGIT}+)
TOK_CHARCON     ('([^\\"\n]|\\[\\'"0nt])')
TOK_STRINGCON   (\"([^\\"\n]|\\[\\’"0nt])*\")
TOK_ARRAY       \[\]


%%

"#".*           { lexer::include(); }
[ \t]+          { /* Skip whitespace. */ }
\n              { lexer::newline(); }

{TOK_INTCON}    { RETURN (TOK_INTCON); }
{TOK_IDENT}     { RETURN (TOK_IDENT); }

"("             { RETURN ('('); }
")"             { RETURN (')'); }
"["             { RETURN ('['); }
"]"             { RETURN (']'); }
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

.               { lexer::badchar (*yytext); }

%%
