#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__

#include <bitset>
#include <map>

using namespace std;

// Attribute Enums
enum Attributes {
   ATTR_void = 0, ATTR_bool, ATTR_char, ATTR_int, ATTR_null,
   ATTR_string, ATTR_struct, ATTR_array, ATTR_fn,
   ATTR_var, ATTR_field, ATTR_typeid, ATTR_param, ATTR_lval,
   ATTR_const, ATTR_vreg, ATTR_vaddr, ATTR_size
};

using attr_bitset = bitset<ATTR_size>;

const map<int, string> attributeStrings = {
   {ATTR_void,   "void"    },    {ATTR_bool,   "bool"    },
   {ATTR_char,   "char"    },    {ATTR_int,    "int"     },
   {ATTR_null,   "null"    },    {ATTR_string, "string"  },
   {ATTR_struct, "struct"  },    {ATTR_array,  "array"   },
   {ATTR_fn,     "function"},    {ATTR_var,    "variable"},
   {ATTR_field,  "field"   },    {ATTR_typeid, "typeid"  },
   {ATTR_param,  "param"   },    {ATTR_lval,   "lval"    },
   {ATTR_const,  "const"   },    {ATTR_vreg,   "vreg"    },
   {ATTR_vaddr,  "vaddr"   },    {ATTR_size,   "SIZE"    }
};


bool isPrimitive(const attr_bitset& attribute);
bool isReference(const attr_bitset& attribute);
bool isBase(const attr_bitset& attribute);
bool isCompatible(const attr_bitset& attr1, 
                  const attr_bitset& attr2);
void copyType(attr_bitset& target, const attr_bitset& source);

#endif
