// djluna: Daniel Luna

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include "auxlib.h"
#include "lyutils.h"
#include "cppstrtok.h"

using namespace std;

const string CPP = "/usr/bin/cpp";


string parse_args(int argc, char** argv) {
   int c;
   opterr = 0;
   string cppArgs = "";
   
   while ((c = getopt(argc, argv, "@:lyD:")) != -1) {
      switch(c) {
         case '@':
            set_debugflags(optarg);
            break;
         case 'l':
            yydebug = 1;
            break;
         case 'y':
            yy_flex_debug = 1;
            break;
         case 'D':
            cppArgs += " -D" + string(optarg);
            break;
         case '?':
            string string_optopt(char(optopt));
            //If we are short a req argument
            if ((optopt == 'D') || (optopt == '@')) {
               fprintf (stderr, "Option -%c requires an argument.\n", 
                        optopt);
            //If we can't handle an arg
            } else { 
               //If it is a readily printable char
               if (isprint(optopt)) {
                  fprintf(stderr, "Unknown option `-%c'.\n", optopt);
               } else {
                  fprintf(stderr,"Unknown option character `\\x%x'.\n",
                        optopt);
               }
            }
            break;
      }
   }
   
   //Debug flag values
   DEBUGF('x', "yydebug = %i\n", yydebug);
   DEBUGF('x', "yy_flex_debug = %i\n", yy_flex_debug);
   
   return cppArgs;
   
}


int main(int argc, char** argv) {
   set_execname(argv[0]);
   string cpp_opts = parse_args(argc, argv);
   
   //If we were given a file
   if (optind == argc - 1) {
      char* filename = argv[optind];
      string command = CPP + " " + filename + cpp_opts;
      DEBUGF('P', "command=\"%s\"\n", command.c_str());
      FILE* cppFile = popen(command.c_str(), "r");
      if (cppFile == NULL) {
         syserrprintf(command.c_str());
         exit(get_exitstatus());
      } else {
         cpplines(cppFile, filename);
         pclose(cppFile);
         yyin = popen(command.c_str(), "r");
         lexer::newfilename(command);
         int parse_rc = yyparse();
         DEBUGF('l', "%i", parse_rc);
         int pclose_rc = pclose(yyin);
         eprint_status(command.c_str(), pclose_rc);
         if (pclose_rc != 0) set_exitstatus(EXIT_FAILURE);
      }
      
      //If we aren't failing, print out files
      if (get_exitstatus() == 0) {
         FILE* file;
         string outputName(filename);
         outputName = outputName.substr(0, outputName.find("."));
         
         //Print .str
         DEBUGF('o', "Generating .str...");
         string strName = outputName + ".str";
         file = fopen(strName.c_str(), "w");
         stringset::dump(file);
         fclose(file);
         
         //Print .tok
         DEBUGF('o', "Generating .tok...");
         string tokName = outputName + ".tok";
         file = fopen(tokName.c_str(), "w");
         astree::print(file, parser::root);
         fclose(file);
         
      }
      
      
   //Not provided a file
   } else {
      errprintf("Error: No file provided.\n");
   }
   
   DEBUGF('x', "Exit Status: %i\n", get_exitstatus());
   return get_exitstatus();
}

