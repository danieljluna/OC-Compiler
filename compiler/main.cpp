#include <string>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

#include "auxlib.h"

using namespace std;

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;

int main(int argc, char** argv) {
   set_execname(argv[0]);
   for (int argi = 1; argi < argc; ++argi) {
      char* filename = arg[argi];
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