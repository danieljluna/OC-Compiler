// djluna: Daniel Luna

#ifndef __LYUTILS_H__
#define __LYUTILS_H__

// Lex and Yacc interface utility.

#include <string>
#include <vector>
using namespace std;

#include <stdio.h>

#include "astree.h"
#include "auxlib.h"
#include <stdio.h>

extern int yy_flex_debug;
extern int yydebug;
extern FILE* yyin;

int yylex();
void yyerror (const char* message);

struct lexer {
   static bool interactive;
   static location lloc;
   static size_t last_yyleng;
   static vector<string> filenames;
   static const string* filename (int filenr);
   static void newfilename (const string& filename);
   static void advance();
   static void newline();
   static void badchar (unsigned char bad);
   static void badtoken (char* lexeme);
   static void include();
   
   //Functions to generate .tok file
   static bool initializeLog(const char* filename);
   static void terminateLog();
   static void output(int symbol);
   static bool logging;
   static ofstream log;
};

struct parser {
   static astree* root;
   static const char* get_tname (int symbol);
   
   //Functions to generate .ast file
   static bool log(const char* filename);
};

//Parsing functions
astree* parseFn(astree* ident, astree* param, astree* toss);
astree* parseFn(astree* fn, astree* block);

astree* parseIf(astree* ifast, astree* expr, astree* stmt,
                 astree* toss1, astree* toss2);
astree* parseIf(astree* ifast, astree* expr, astree* stmt1,
                 astree* stmt2, astree* toss1, astree* toss2,
                 astree* toss3);
                 
astree* parseVarDec(astree* ident, astree* equals, astree* expr,
                    astree* toss);
                    
astree* parseRetVoid(astree* returnast, astree* toss);

astree* parseAlloc(astree* newast, astree* type,
                   astree* toss1, astree* toss2);
astree* parseAlloc(astree* newast, astree* type, astree* expr,
                   astree* toss1, astree* toss2);
                   
astree* parseCall(astree* ident, astree* paren,
                  astree* expr = nullptr);
                  
astree* parseIndex(astree* expr1, astree* bracket, astree* expr2,
                   astree* toss);

#define YYSTYPE astree*
#include "yyparse.h"

#endif
