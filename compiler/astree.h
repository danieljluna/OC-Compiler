// djluna: Daniel Luna

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
using namespace std;

#include "auxlib.h"
#include "stringset.h"

struct location {
   size_t filenr;
   size_t linenr;
   size_t offset;
};

struct astree {

   // Fields.
   int symbol;               // token code
   location lloc;            // source location
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node

   // Functions.
   astree(int symbol, const location&, const char* lexinfo);
   ~astree();
   //adopt is formated so any symbols present alter the preceding ast
   astree* adopt(astree* child1, 
                 astree* child2 = nullptr, 
                 astree* child3 = nullptr);
   astree* adopt(int rootSym, astree* child1, astree* child2=nullptr);
   astree* adopt(astree* child1, int sym1, astree* child2 = nullptr);
   astree* adopt(astree* child1, astree* child2, int sym2);
   astree* adopt(astree* child1, int sym1, astree* child2, int sym2);
   astree* sym(int symbol);
   void dump_node(FILE*);
   void dump_tree(FILE*, int depth = 0);
   static void dump(FILE* outfile, astree* tree);
   static void print(FILE* outfile, astree* tree, int depth = 0);
};

void free (astree* tree1, 
           astree* tree2 = nullptr,
           astree* tree3 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif
