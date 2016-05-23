#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <vector>
#include <unordered_map>

#include "astree.h"
#include "attribute.h"

using namespace std;

struct symbol;

using symbol_table = unordered_map<string*, symbol*>;
using symbol_entry = symbol_table::value_type;

struct symbol {
   attr_bitset attributes;
   symbol_table* fields;
   location lloc;
   string* lexInfo;
   size_t block_nr;
   vector<symbol*>* parameters;
   
   void insert_symbol();
   
   //Static Functionality
   static vector<symbol_table*> symbol_stack;
   static vector<size_t> block_stack;
   static symbol_table* struct_def_table;
   static size_t next_block;
   
   static void enter_block();
   static void exit_block();
   static symbol* find_ident(string* ident);
   
   static int recurseSymTable(astree* subTree, size_t block_nr);
   static int buildSymTable(astree* root);
   
};

#endif
