// djluna: Daniel Luna

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
#include <map>
using namespace std;

#include "attribute.h"
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
   attr_bitset attributes;   // attributes
   size_t block_nr;          // block number

   // Ctor / Dtor
   astree(int symbol, const location&, const char* lexinfo);
   ~astree();
   
   //Adopting Fns (sym always immediately follows astree it modifies)
   astree* adopt(astree* child1, 
                 astree* child2 = nullptr, 
                 astree* child3 = nullptr);
   astree* adopt(int rootSym, astree* child1, astree* child2=nullptr);
   astree* adopt(astree* child1, int sym1, astree* child2 = nullptr);
   astree* adopt(astree* child1, astree* child2, int sym2);
   astree* adopt(astree* child1, int sym1, astree* child2, int sym2);
   astree* sym(int symbol);

   
   //Printing Fns
   void dump_node(FILE*);
   void dump_tree(FILE*, int depth = 0);
   
   //Static fns
   static void dump(FILE* outfile, astree* tree);
   static void print(FILE* outfile, astree* tree, int depth = 0);
};

void free (astree* tree1, 
           astree* tree2 = nullptr,
           astree* tree3 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif
