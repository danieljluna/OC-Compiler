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
%option noinput
%option nounput
%option noyywrap
%option warn
/*%option verbose*/

LETTER          [A-Za-z_]
DIGIT           [0-9]
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)
IDENT           ({LETTER}({LETTER}|{DIGIT})*)

%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

{NUMBER}        { RETURN (NUMBER); }
{IDENT}         { RETURN (IDENT); }
"="             { RETURN ('='); }
"+"             { RETURN ('+'); }
"-"             { RETURN ('-'); }
"*"             { RETURN ('*'); }
"/"             { RETURN ('/'); }
"^"             { RETURN ('^'); }
"("             { RETURN ('('); }
")"             { RETURN (')'); }
";"             { RETURN (';'); }

{NOTNUMBER}     { lexer::badtoken (yytext); RETURN (NUMBER); }
.               { lexer::badchar (*yytext); }

%%
