// djluna: Daniel Luna

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "stringset.h"
#include "lyutils.h"


astree::astree (int symbol_, const location& lloc_, const char* info) {
   symbol = symbol_;
   lloc = lloc_;
   lexinfo = stringset::intern (info);
   block_nr = 0;
   // vector defaults to empty -- no children
}

astree::~astree() {
   while (not children.empty()) {
      astree* child = children.back();
      children.pop_back();
      delete child;
   }
   if (yydebug) {
      fprintf (stderr, "Deleting astree (");
      astree::dump (stderr, this);
      fprintf (stderr, ")\n");
   }
}

astree* astree::adopt (astree* child1,
                       astree* child2,
                       astree* child3) {
   if (child1 != nullptr) children.push_back (child1);
   if (child2 != nullptr) children.push_back (child2);
   if (child3 != nullptr) children.push_back (child3);
   return this;
}

astree* astree::adopt(int rootSym, astree* child1, astree* child2){
   symbol = rootSym;
   return adopt (child1, child2);
}

astree* astree::adopt(astree* child1, int sym1, astree* child2) {
   child1->symbol = sym1;
   return adopt(child1, child2);
}

astree* astree::adopt(astree* child1, astree* child2, int sym2) {
    child2->symbol = sym2;
    return adopt(child1, child2);
}

astree* astree::adopt(astree* child1,int sym1,astree* child2,int sym2){
    child1->symbol = sym1;
    child2->symbol = sym2;
    return adopt(child1, child2);
}

astree* astree::sym(int symbol_) {
   symbol = symbol_;
   return this;
}


void astree::dump_node (FILE* outfile) {
   fprintf (outfile, "%p->{%s %zd.%zd.%zd \"%s\"}",
            this, parser::get_tname (symbol),
            lloc.filenr, lloc.linenr, lloc.offset,
            lexinfo->c_str());
            
   for (size_t i = 0; i < ATTR_size; ++i) {
      if (attributes[i]) {
         fprintf(outfile, " %s", attributeStrings.at(i).c_str());
      }
   }
            
   if (children.size() > 0)
      fprintf(outfile, ":");
            
   for (size_t child = 0; child < children.size(); ++child) {
      fprintf (outfile, " %p", children.at(child));
   }
}

void astree::dump_tree (FILE* outfile, int depth) {
   fprintf (outfile, "%*s", depth * 3, "");
   dump_node (outfile);
   fprintf (outfile, "\n");
   for (astree* child: children) child->dump_tree (outfile, depth + 1);
   fflush (NULL);
}

void astree::dump (FILE* outfile, astree* tree) {
   if (tree == nullptr) fprintf (outfile, "nullptr");
                   else tree->dump_node (outfile);
}

void astree::print (FILE* outfile, astree* tree, int depth) {
   //Print indentation
   for (int i = 0; i < depth; ++i)
      fprintf (outfile, "|  ");
   
   //Trim TOK_ from symbol name
   const char* name = parser::get_tname (tree->symbol);
   if (name[0] == 'T') {
       name = name + 4;
   }
   
   //Print astree
   fprintf (outfile, "%s \"%s\" (%zd.%zd.%zd)",
            name, tree->lexinfo->c_str(),
            tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
            
   for (size_t i = 0; i < ATTR_size; ++i) {
      if (tree->attributes[i]) {
         fprintf(outfile, " %s", attributeStrings.at(i).c_str());
      }
   }
   
   fprintf (outfile, "\n");
            
   //Print children
   for (astree* child: tree->children) {
      astree::print (outfile, child, depth + 1);
   }
}

void free(astree* tree1, 
          astree* tree2,
          astree* tree3) {
   if (tree1 != nullptr) delete tree1;
   if (tree2 != nullptr) delete tree2;
   if (tree3 != nullptr) delete tree3;
}

void errllocprintf (const location& lloc, const char* format,
                    const char* arg) {
   static char buffer[0x1000];
   assert (sizeof buffer > strlen (format) + strlen (arg));
   snprintf (buffer, sizeof buffer, format, arg);
   errprintf ("%s:%zd.%zd: %s", 
              lexer::filename (lloc.filenr)->c_str(), lloc.linenr,
              lloc.offset, buffer);
}
