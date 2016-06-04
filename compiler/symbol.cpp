#include "symbol.h"
#include "lyutils.h"
#include <exception>

vector<symbol_table*> symbol::symbol_stack = {};
vector<size_t> symbol::block_stack = {0};
symbol_table* symbol::struct_def_table = nullptr;
size_t symbol::next_block = 1;
FILE* symbol::symFile = nullptr;

symbol::symbol(astree* origin) {
   attributes = origin->attributes;
   decloc = origin->lloc;
   block_nr = block_stack.back();
   origin->identSym = this;
}




void symbol::dumpTable(FILE* file, symbol_table symTable) {
   for (symbol_entry entry : symTable) {
      dumpEntry(file, entry);
   }
}




void symbol::dumpEntry(FILE* file, symbol_entry symEntry) {
   if (symFile) {
      //Handle indentation
      for (size_t i = 0; i < symEntry.second->block_nr; ++i) {
         fprintf(file, "   ");
      }
      
      fprintf(file, "%s (%zd.%zd.%zd) {%zd}",
            symEntry.first->c_str(),
            symEntry.second->decloc.filenr,
            symEntry.second->decloc.linenr,
            symEntry.second->decloc.offset,
            symEntry.second->block_nr);
      
      for (size_t i = 0; i < ATTR_size; ++i) {
         if (symEntry.second->attributes[i]) {
            fprintf(file, " %s", attributeStrings.at(i).c_str());
            if ((i == ATTR_struct) || (i == ATTR_typeid)) {
               fprintf(file, " \"%s\"", 
                        symEntry.second->structName->c_str());
            }
         }
      }
      
      fprintf(file, "\n");
   }
}




void symbol::insert_symbol(const string* lexinfo,
                           bool isStruct) {
   symbol_table* table;
   if (isStruct) {
      if (struct_def_table == nullptr) {
         struct_def_table = new symbol_table();
      }
      
      table = struct_def_table;
   } else {
      //if we don't have an existing table
      if (symbol_stack.size() == 0) {
         symbol_stack.push_back(new symbol_table());
      } else if (symbol_stack.back() == nullptr) {
         symbol_stack.back() = new symbol_table();
      }
      
      table = symbol_stack.back();
   }
   
   auto entry = table->emplace(lexinfo, this);
   
   if (symFile) {
      dumpEntry(symFile, *(entry.first));
   }
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




symbol* symbol::find_ident(const string* ident) {
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




symbol* symbol::find_field(const string* structName,
                           const string* fieldName) {
   //If there is a struct matching this name
   if (symbol_stack.front() != nullptr) {
      auto it = symbol_stack.front()->find(structName);
      if (it != symbol_stack.front()->end()) {
         //If there are fields in the struct
         if (it->second->fields != nullptr) {
            auto symbolIt = it->second->fields->find(fieldName);
            //If we found the symbol
            if (symbolIt != (it)->second->fields->end()) {
               return symbolIt->second;
            }
         }
      }
   }
   
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
   
   symbol* createdSymbol = nullptr;
   symbol* tempSymbol = nullptr;
   
   //Attribute switch
   switch (subTree->token) {
   case TOK_BLOCK:
      exit_block();
      break;
   //BINARY-OPS--------------------------------------------------------
   case TOK_EQ:
   case TOK_NE:
      if (isCompatible(subTree->children[0]->attributes,
                       subTree->children[1]->attributes)) {
         subTree->attributes.set(ATTR_bool, 1);
         subTree->attributes.set(ATTR_vreg, 1);
      } else {
         symErrPrint(subTree, 
            "Equality check expects matching types!");
      }
      break;
   case TOK_LE:
   case TOK_GE:
   case '>':
   case '<':
      if (!isCompatible(subTree->children[0]->attributes,
                       subTree->children[1]->attributes)) {
         symErrPrint(subTree,
            "Relational check expects matching types!");
      } else if (!isPrimitive(subTree->children[0]->attributes)) {
         symErrPrint(subTree,
            "Relational check expects primitive operands!");
      } else {
         subTree->attributes.set(ATTR_bool, 1);
         subTree->attributes.set(ATTR_vreg, 1);
      }
      break;
   //BINARY-OPS--------------------------------------------------------
   case '=':
      if (!isCompatible(subTree->children[0]->attributes,
                       subTree->children[1]->attributes)) {
         symErrPrint(subTree,
            "Assignment operator expects matching types!");
      } else if (!subTree->children[0]->attributes[ATTR_lval]) {
         symErrPrint(subTree,
            "Assignment operator expects lefthand operand is lval!");
      } else {
         copyType(subTree->attributes,
                  subTree->children[0]->attributes);
         subTree->attributes.set(ATTR_vreg, 1);
      }
      break;
   case '+':
   case '-':
   case '*':
   case '/':
   case '%':
      if (!(subTree->children[0]->attributes[ATTR_int]) ||
            (subTree->children[0]->attributes[ATTR_array])) {
         symErrPrint(subTree,
            "Righthand side of op must be an int!");
      } else if (!(subTree->children[1]->attributes[ATTR_int]) ||
               (subTree->children[1]->attributes[ATTR_array])) {
         symErrPrint(subTree,
            "Lefthand side of op must be an int!");
      } else {
         subTree->attributes.set(ATTR_int, 1);
         subTree->attributes.set(ATTR_vreg, 1);
      }
      break;
   //UNARY-OPS---------------------------------------------------------
   case TOK_POS:
   case TOK_NEG:
      typeCheck_unary_op(subTree, ATTR_int, ATTR_int);
      break;
   case '!':
      typeCheck_unary_op(subTree, ATTR_bool, ATTR_bool);
      break;
   case TOK_ORD:
      typeCheck_unary_op(subTree, ATTR_char, ATTR_int);
      break;
   case TOK_CHR:
      typeCheck_unary_op(subTree, ATTR_int, ATTR_char);
      break;
   //NEW-OPS-----------------------------------------------------------
   case TOK_NEW:
      if (!subTree->children[0]->attributes[ATTR_typeid]) {
         symErrPrint(subTree,
               "Identifier must be a valid typeid for allocations!");
      } else {
         subTree->attributes.set(ATTR_struct, 1);
         subTree->attributes.set(ATTR_vreg, 1);
      }
      break;
   case TOK_NEWSTRING:
      if (!subTree->children[1]->attributes[ATTR_int] ||
          (subTree->children[1]->attributes[ATTR_array])) {
         symErrPrint(subTree,
               "Allocated string size must be an int!");
      } else {
         subTree->attributes.set(ATTR_string, 1);
         subTree->attributes.set(ATTR_vreg, 1);
      }
      break;
   case TOK_NEWARRAY:
      if (!subTree->children[1]->attributes[ATTR_int] ||
          (subTree->children[1]->attributes[ATTR_array])) {
         symErrPrint(subTree,
               "Allocated array size must be an int!");
      } else {
         copyType(subTree->attributes, 
                  subTree->children[0]->attributes);
         subTree->attributes.set(ATTR_array, 1);
         subTree->attributes.set(ATTR_vreg, 1);
      }
      break;
   //BASETYPES-&-CONSTS------------------------------------------------
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
   //ELEMENT-ACCESS----------------------------------------------------
   case TOK_INDEX:
      if (!subTree->children[1]->attributes[ATTR_int] ||
          (subTree->children[1]->attributes[ATTR_array])) {
         symErrPrint(subTree,
               "Index value must be an int!");
      } else if (subTree->children[0]->attributes[ATTR_string]) {
         subTree->attributes.set(ATTR_char, 1);
         subTree->attributes.set(ATTR_vaddr, 1);
         subTree->attributes.set(ATTR_lval, 1);
      } else if (subTree->children[0]->attributes[ATTR_array]) {
         copyType(subTree->attributes, 
            subTree->children[0]->attributes);
         subTree->attributes.set(ATTR_array, 0);
         subTree->attributes.set(ATTR_vaddr, 1);
         subTree->attributes.set(ATTR_lval, 1);
      } else {
         symErrPrint(subTree,
               "Only strings and arrays can be indexed!");
      }
      break;
   //STRUCTS-&&-FIELDS-------------------------------------------------
   case TOK_STRUCT:
      subTree->attributes.set(ATTR_struct, 1);
      createdSymbol = new symbol(subTree);
      createdSymbol->structName = subTree->children[0]->lexinfo;
      createdSymbol->insert_symbol(createdSymbol->structName, true);
      if (subTree->children.size() > 0) {
         createdSymbol->fields = new symbol_table();
         for (size_t i = 1; i < subTree->children.size(); ++i) {
            astree* child = subTree->children[i];
            astree* ident = child->children.back();
            ident->attributes.set(ATTR_field, 1);
            tempSymbol = generateIdent(child);
            
            if (tempSymbol != nullptr) {
               auto entry = createdSymbol->fields->emplace(
                                    ident->lexinfo, tempSymbol);
               if (symFile) {
                  dumpEntry(symFile, *(entry.first));
               }
            }
         }
      }
      break;
   case '.':
      if ((!subTree->children[0]->attributes[ATTR_struct]) ||
          (subTree->children[0]->attributes[ATTR_array])) {
         symErrPrint(subTree,
               "Identifier must be a struct for field selection!");
      } else {
         subTree->attributes.set(ATTR_vaddr, 1);
         subTree->attributes.set(ATTR_lval, 1);
      }
      break;
   case TOK_TYPEID:
      break;
   //FUNCTIONS-&&-PARAMS-----------------------------------------------
   case TOK_PARAMLIST:
      for (astree* child : subTree->children) {
         child->attributes.set(ATTR_param, 1);
      }
      break;
   //OTHER-IDENTS------------------------------------------------------
   case TOK_VARDECL:
      break;
   //CONTROL-----------------------------------------------------------
   case TOK_IF:
   case TOK_WHILE:
      if ((!subTree->children[0]->attributes[ATTR_bool]) ||
          (subTree->children[0]->attributes[ATTR_array])) {
         symErrPrint(subTree,
               "Conditional should be a boolean!");
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




bool symbol::log(const char* filename) {
   //Generate .ast file
   string outputName(filename);
   string tokName = outputName + ".sym";
   symFile = fopen(tokName.c_str(), "w");
   bool result = false;
   
   return result;
}




void symbol::endLog() {
   if (symFile != nullptr) {
      fclose(symFile);
   }
}




void symbol::symErrPrint(astree* ast, string error) {
   errprintf("(%zd.%zd.%zd) Error: %s\n", 
             ast->lloc.filenr,
             ast->lloc.linenr,
             ast->lloc.offset,
             error.c_str());
}







//Type Check Helpers


void typeCheck_unary_op(astree* ast,
                        Attributes operand, 
                        Attributes result) {
   if (!(ast->children[0]->attributes[operand]) ||
         (ast->children[0]->attributes[ATTR_array])) {
      symbol::symErrPrint(ast,
         "Unary op expects a " + attributeStrings.at(operand) + "!");
   } else {
      ast->attributes.set(result, 1);
      ast->attributes.set(ATTR_vreg, 1);
   }
}




symbol* generateIdent(astree* ast) {
   symbol* result = nullptr;
   
   if (ast->token != TOK_ARRAY) {
      //Handle structs
      if (ast->token == TOK_TYPEID) {
         auto it = symbol::struct_def_table->find(ast->lexinfo);
         if (it == symbol::struct_def_table->end()) {
            symbol::symErrPrint(ast,
                  "No structure found of that name!");
            
            return nullptr;
         } else {
            copyType(ast->attributes, it->second->attributes);
         }
      }
      copyType(ast->children[0]->attributes, ast->attributes);
      result = new symbol(ast->children[0]);
   } else {
      copyType(ast->children[1]->attributes,
               ast->children[0]->attributes);
      ast->children[1]->attributes.set(ATTR_array, 1);
      result = new symbol(ast->children[1]);
   }
   
   return result;
}
