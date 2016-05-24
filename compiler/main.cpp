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
#include "symtable.h"

using namespace std;

const string CPP = "/usr/bin/cpp";


string parse_args(int argc, char** argv) {
   int c;
   opterr = 0;
   string cppArgs = "";
   yy_flex_debug = 0;
   yydebug = 0;
   
   while ((c = getopt(argc, argv, "@:lyD:")) != -1) {
      switch(c) {
         case '@':
            set_debugflags(optarg);
            break;
         case 'l':
            yy_flex_debug = 1;
            break;
         case 'y':
            yydebug = 1;
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
      
      //Remove file bounds
      
      
      FILE* testForTarget = fopen(filename, "r");
      
      //If the file didn't exist:
      if (!testForTarget) {
         errprintf("ERROR: %s not found!\n", filename);
         exit(get_exitstatus());
      } else {
         fclose(testForTarget);
         string command = CPP + " " + filename + cpp_opts;
         DEBUGF('P', "command=\"%s\"\n", command.c_str());
         FILE* cppFile = popen(command.c_str(), "r");
         
         //If we couldn't pipe into cpp:
         if (cppFile == NULL) {
            syserrprintf(command.c_str());
            exit(get_exitstatus());
         } else {
            //Update filename
            string fname(filename);
            fname = fname.substr(0, fname.find_last_of("."));
            
            //Generate .str file
            cpplines(cppFile, filename);
            pclose(cppFile);
            
            //Generate .tok
            yyin = popen(command.c_str(), "r");
            lexer::initializeLog(fname.c_str());
            yyparse();
            lexer::terminateLog();
            
            //Check if bison / flex failed
            int lclose_rc = pclose(yyin);
            eprint_status(command.c_str(), lclose_rc);
            if (lclose_rc != 0) set_exitstatus(EXIT_FAILURE);
            else {
               //Type Check
               //Generate .sym file
               parser::log(fname.c_str());
               symbol::log(fname.c_str());
               symbol::buildSymTable(parser::root);
               symbol::endLog();
               
               //Generate .ast file
               parser::log(fname.c_str());
            }
         }
      }
      
      
   //Not provided a file
   } else {
      errprintf("Error: No file provided.\n");
   }
   
   DEBUGF('x', "Exit Status: %i\n", get_exitstatus());
   free(parser::root);
   
   return get_exitstatus();
}

