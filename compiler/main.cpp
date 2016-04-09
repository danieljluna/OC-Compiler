//djluna
#include <string>
#include <iostream>

#include <libgen.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>

#include "auxlib.h"

using namespace std;

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

// Run cpp against the lines of the file.
void cpplines (FILE* pipe, char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      chomp (buffer, '\n');
      printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, filename);
      if (sscanf_rc == 2) {
         printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, filename);
         continue;
      }
      char* savepos = NULL;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if (token == NULL) break;
         printf ("token %d.%d: [%s]\n",
                 linenr, tokenct, token);
      }
      ++linenr;
   }
}


void parse_args(int argc, char** argv) {
   int c;
   opterr = 0;
   
   while (c = getopt(argc, argv, "@lyD:") != -1) {
      switch(c) {
         case '@':
            set_debugflag(optarg);
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
            if (optopt == 'D') { //We didn't get an argument for -D
               cerr << "Option -D requires an argument." << endl;
               //fprintf(stderr, "Option -%D requires an argument.\n", optopt);
            } else if (isprint(optopt)) { //Couldn't handle an option
               cerr << "Unknown option '-" << optopt << "'." << endl;
            } else {
               cerr << "Unknown option character '" << int(optopt) << 
                       "'." << endl;
            }
            break;
      }
   }
}


int main(int argc, char** argv) {
   set_execname(argv[0]);
   
   parse_args(argc, argv);
   
   for (int argi = 1; argi < argc; ++argi) {
      char* filename = argv[argi];
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
   }
   
   return get_exitstatus();
}

