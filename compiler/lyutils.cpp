// djluna: Daniel Luna

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "auxlib.h"
#include "lyutils.h"
#include "yylex.h"

bool lexer::interactive = true;
bool lexer::logging = false;
ofstream lexer::log;
location lexer::lloc = {0, 1, 0};
size_t lexer::last_yyleng = 0;
vector<string> lexer::filenames;

astree* parser::root = nullptr;

const string* lexer::filename (int filenr) {
   return &lexer::filenames.at(filenr);
}

void lexer::newfilename (const string& filename) {
   auto it = find(filenames.begin(), filenames.end(), filename);
   if (it == filenames.end()) {
      lexer::lloc.filenr = lexer::filenames.size();
      lexer::filenames.push_back (filename);
   } else {
      lexer::lloc.filenr = it - filenames.begin();
   }
}

void lexer::advance() {
   if (not interactive) {
      if (lexer::lloc.offset == 0) {
         printf (";%2zd.%3zd: ",
                 lexer::lloc.filenr, lexer::lloc.linenr);
      }
      printf ("%s", yytext);
   }
   lexer::lloc.offset += last_yyleng;
   last_yyleng = yyleng;
}

void lexer::newline() {
   ++lexer::lloc.linenr;
   lexer::lloc.offset = 0;
}

void lexer::badchar (unsigned char bad) {
   char buffer[16];
   snprintf (buffer, sizeof buffer,
             isgraph (bad) ? "%c" : "\\%03o", bad);
   errllocprintf (lexer::lloc, "invalid source character (%s)\n",
                  buffer);
}

void lexer::badtoken (char* lexeme) {
   errllocprintf (lexer::lloc, "invalid token (%s)\n", lexeme);
}

void lexer::include() {
   size_t linenr;
   static char filename[0x1000];
   assert (sizeof filename > strlen (yytext));
   int scan_rc = sscanf(yytext, "# %zd \"%[^\"]\"", &linenr, filename);
   if (scan_rc != 2) {
      errprintf ("%s: invalid directive, ignored\n", yytext);
   }else {
      if (yy_flex_debug) {
         fprintf (stderr, "--included # %zd \"%s\"\n",
                  linenr, filename);
      }
      lexer::lloc.linenr = linenr - 1;
      lexer::newfilename (filename);
      
      //Output to log stream if we are scanning
      if (logging) {
         log << "#" << setw(3) << lloc.filenr
             << " \"" << setw(0) << filenames.at(lloc.filenr) << "\"" 
             << endl;
      }
   }
}


bool lexer::initializeLog(const char* file) {
   //Generate .tok file
   string outputName(file);
   outputName = outputName.substr(0, outputName.find("."));
   string tokName = outputName + ".tok";
   log.open(tokName);
   //Mark that we are scanning so we get include output
   logging = log;
   
   return logging;
}
   
   
void lexer::terminateLog() {
   //Reset logging flag
   if (logging) {
      logging = false;
      log.close();
   }
}


void lexer::output(int symbol) {
   if ((logging) && (symbol != 0)) {
      //Dump symbol
      log << setw(4) << yylval->lloc.filenr
          << setw(4) << yylval->lloc.linenr << "."
          << setw(3) << setfill('0') << yylval->lloc.offset
          << setw(5) << setfill(' ') << symbol << "  "
          << setw(16) << left << parser::get_tname(symbol) << "("
          << setw(0) << *(yylval->lexinfo) << ")" << right
          << endl;
   }
}


void yyerror (const char* message) {
   assert (not lexer::filenames.empty());
   errllocprintf (lexer::lloc, "%s\n", message);
}




//PARSER-FUNCTIONALITY-------------------------------------------------

bool parser::log(const char* filename) {
    //Generate .ast file
   string outputName(filename);
   outputName = outputName.substr(0, outputName.find("."));
   string tokName = outputName + ".ast";
   FILE* log;
   log = fopen(tokName.c_str(), "w");
   bool result = false;
   
   if (log) {
      root->dump_tree(log);
      fclose(log);
      result = true;
   }
   
   return result;
}

