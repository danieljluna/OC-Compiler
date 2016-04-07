// $Id: main.cpp,v 1.6 2014-10-09 15:44:18-07 - - $

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#include "stringset.h"

int main (int argc, char** argv) {
   set_execname (argv[0]);
   for (int argi = 1; argi < argc; ++argi) {
      char* filename = argv[argi];
      string command = CPP + " " + filename;
      printf ("command=\"%s\"\n", command.c_str());
      FILE* pipe = popen (command.c_str(), "r");
      if (pipe == NULL) {
         syserrprintf (command.c_str());
      }else {
         cpplines (pipe, filename);
         int pclose_rc = pclose (pipe);
         eprint_status (command.c_str(), pclose_rc);
         if (pclose_rc != 0) set_exitstatus (EXIT_FAILURE);
      }
   }
   
   vector<string> args (&argv[1], &argv[argc]);
   for (const string& arg: args) {
      const string* str = intern_stringset (arg.c_str());
      cout << "intern(" << arg << ") returned " << str
           << "->\"" << *str << "\"" << endl;
   }
   dump_stringset (cout);
   return EXIT_SUCCESS;
}



