// djluna: Daniel Luna

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include "auxlib.h"
#include "cppstrtok.h"

using namespace std;

const string CPP = "/usr/bin/cpp";


int parse_args(int argc, char** argv) {
   int c;
   opterr = 0;
   
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
            //Pass optarg to CPP
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
   
   return optind;
   
}


int main(int argc, char** argv) {
   set_execname(argv[0]);
   
   int optind = parse_args(argc, argv);
   
   if (optind == argc - 1) {
      char* filename = argv[optind];
      string command = CPP + " " + filename;
      printf("command=\"%s\"\n", command.c_str());
      FILE* pipe = popen(command.c_str(), "r");
      if (pipe == NULL) {
         syserrprintf(command.c_str());
      } else {
         cpplines(pipe, filename);
         int pclose_rc = pclose(pipe);
         eprint_status(command.c_str(), pclose_rc);
         if (pclose_rc != 0) set_exitstatus(EXIT_FAILURE);
      }
   } else {
      errprintf("Error: No file provided.\n");
   }
   
   DEBUGF('x', "Exit Status: %i\n", get_exitstatus());
   return get_exitstatus();
}

