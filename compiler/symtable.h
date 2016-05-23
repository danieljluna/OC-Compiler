#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <vector>
#include <unordered_map>

#include "auxlib.h"
#include "attribute.h"

using namespace std;

struct location {
   size_t filenr;
   size_t linenr;
   size_t offset;
};

struct symbol;

using symbol_table = unordered_map<const string*, symbol*>;
using symbol_entry = symbol_table::value_type;

struct astree;

struct symbol {
   
   attr_bitset attributes;
   location decloc;
   size_t block_nr;
   symbol_table* fields;
   vector<symbol*>* parameters;
   
   //Ctor
   symbol(astree* origin);
   
   //Insert this symbol to static tables
   void insert_symbol(const string* lexinfo);
   
   //Static Functionality
   static vector<symbol_table*> symbol_stack;
   static vector<size_t> block_stack;
   static symbol_table* struct_def_table;
   static size_t next_block;
   
   static void enter_block();
   static void exit_block();
   static symbol* find_ident(string* ident);
   
   static int recurseSymTable(astree* subTree);
   static int buildSymTable(astree* root);
   
};

#endif
