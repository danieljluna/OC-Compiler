#include "attribute.h"

bool isPrimitive(const attr_bitset& attribute) {
   return ((attribute[ATTR_bool]) ||
           (attribute[ATTR_char]) ||
           (attribute[ATTR_int]));
}




bool isReference(const attr_bitset& attribute) {
   return ((attribute[ATTR_null]) ||
           (attribute[ATTR_string]) ||
           (attribute[ATTR_struct]) ||
           (attribute[ATTR_array]));
}




bool isBase(const attr_bitset& attribute) {
   return (((attribute[ATTR_bool]) ||
            (attribute[ATTR_char]) ||
            (attribute[ATTR_int]) ||
            (attribute[ATTR_null]) ||
            (attribute[ATTR_string]) ||
            (attribute[ATTR_struct])) && 
           !(attribute[ATTR_array]));
}




bool isCompatible(const attr_bitset& attr1, 
                  const attr_bitset& attr2) {
   bool result = false;
   //Compatible if one is ref and other is null
   if ((isReference(attr1) && attr2[ATTR_null]) || 
       (isReference(attr2) && attr1[ATTR_null])) {
      result = true;
   } else {
      //Check that types match
      result = true;
      for (size_t i = 0; i < ATTR_array; ++i) {
         //Break if types don't match
         if (attr1[i] != attr2[i]) {
            result = false;
            break;
         }
      }
      
      //Check array type
      if (result) {
         result = attr1[ATTR_array] == attr2[ATTR_array];
      }
   }
   
   return result;
}




void copyType(attr_bitset& target, const attr_bitset& source) {
   for (size_t i = 0; i < ATTR_fn; ++i) {
      target[i] = source[i];
   }
}
