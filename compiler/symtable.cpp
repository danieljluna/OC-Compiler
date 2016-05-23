#include "symtable.h"
#include "lyutils.h"
#include <exception>

vector<symbol_table*> symbol::symbol_stack = {};
vector<size_t> symbol::block_stack = {0};
symbol_table* symbol::struct_def_table = nullptr;
size_t symbol::next_block = 1;

symbol::symbol(astree* origin) {
   attributes = origin->attributes;
   decloc = origin->lloc;
   block_nr = block_stack.back();
   origin->identSym = this;
   
   insert_symbol(origin->lexinfo);
}




void symbol::insert_symbol(const string* lexinfo) {
   //if we don't have an existing table
   if (symbol_stack.size() == 0) {
      symbol_stack.push_back(new symbol_table());
   } else if (symbol_stack.back() == nullptr) {
      symbol_stack.back() = new symbol_table();
   }
   
   symbol_stack.back()->emplace(lexinfo, this);
}




void symbol::enter_block() {
   block_stack.push_back(next_block);
   symbol_stack.push_back(nullptr);
   ++next_block;
}




void symbol::exit_block() {
   block_stack.pop_back();
   symbol_stack.pop_back();
}




symbol* symbol::find_ident(string* ident) {
   //For each symTable in the stack
   for (auto it = symbol_stack.rbegin();
        it < symbol_stack.rend();
        ++it) {
      //If there is a valid map
      if ((*it) != nullptr) {
         auto symbolIt = (*it)->find(ident);
         //If we found the symbol
         if (symbolIt != (*it)->end()) {
            return symbolIt->second;
         }
      }
   }
   
   //We did not find it
   return nullptr;
}




int symbol::recurseSymTable(astree* subTree) {
   subTree->block_nr = block_stack.back();
   
   if (subTree->token == TOK_BLOCK) {
      enter_block();
   }
   
   for (astree* child: subTree->children) {
      recurseSymTable(child);
   }
   
   //Attribute switch
   switch (subTree->token) {
   case TOK_BLOCK:
      exit_block();
      break;
   case TOK_VOID:
      subTree->attributes.set(ATTR_void, 1);
      break;
   case TOK_TRUE:
   case TOK_FALSE:
      subTree->attributes.set(ATTR_const, 1);
   case TOK_BOOL:
      subTree->attributes.set(ATTR_bool, 1);
      break;
   case TOK_CHARCON:
      subTree->attributes.set(ATTR_const, 1);
   case TOK_CHAR:
      subTree->attributes.set(ATTR_char, 1);
      break;
   case TOK_INTCON:
      subTree->attributes.set(ATTR_const, 1);
   case TOK_INT:
      subTree->attributes.set(ATTR_int, 1);
      break;
   case TOK_STRINGCON:
      subTree->attributes.set(ATTR_const, 1);
   case TOK_STRING:
      subTree->attributes.set(ATTR_string, 1);
      break;
   case TOK_NULL:
      subTree->attributes.set(ATTR_const, 1);
      subTree->attributes.set(ATTR_null, 1);
      break;
   case TOK_STRUCT:
      subTree->attributes.set(ATTR_struct, 1);
      break;
   case TOK_ARRAY:
      subTree->attributes.set(ATTR_array, 1);
      break;
   case TOK_FIELD:
      subTree->attributes.set(ATTR_field, 1);
      break;
   case TOK_PARAMLIST:
      for (astree* child : subTree->children) {
         child->attributes.set(ATTR_param, 1);
      }
      break;
   default:
      break;
   }
   
   return 0;
}




int symbol::buildSymTable(astree* root) {
   try {
      recurseSymTable(root);
      return 0;
   } catch (exception& e) {
      eprintf("%s\n", e.what());
      return -1;
   }
}
