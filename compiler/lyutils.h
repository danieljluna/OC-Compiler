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
   static int scan(const char* filename);
   static bool scanning;
   static ofstream log;
};

struct parser {
   static astree* root;
   static const char* get_tname (int symbol);
};

#define YYSTYPE astree*
#include "yyparse.h"

#endif
