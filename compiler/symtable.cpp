#include "symtable.h"

vector<symbol_table*> symbol::symbol_stack = {};
vector<size_t> symbol::block_stack = {0};
symbol_table* symbol::struct_def_table = nullptr;
size_t symbol::next_block = 1;

void symbol::insert_symbol(string* lex) {
   //if we don't have an existing table
   if ((symbol_stack.size() > 0) && (symbol_stack.back() != nullptr)){
      symbol_stack.push_back(new symbol_table());
   }
   
   symbol_stack.back()->emplace(lex, this);
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




static int recurseSymTable(astree* subTree) {
   return 0;
}




int symbol::buildSymTable(astree* root) {
   return 0;
}
