// djluna: Daniel Luna

#ifndef __STRINGSET__
#define __STRINGSET__

#include <iostream>
#include <string>
#include <unordered_set>

class stringSet {
public:

   //Replaces intern_stringset for name simplicity
   const std::string* insert(const char*);

   //Replaces dump_stringset for name simplicity
   void dump (std::ostream&) const;
   
   friend std::ostream& operator<<(std::ostream&, 
                                   const stringSet&);
   
private:

   std::unordered_set<std::string> set;

};

#endif
