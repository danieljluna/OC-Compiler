// djluna: Daniel Luna

#ifndef __STRINGSET__
#define __STRINGSET__

#include <iostream>
#include <string>
#include <unordered_set>

class stringSet {
public:

   const std::string* intern_stringset (const char*);

   void dump_stringset (std::ostream&);
   
   friend std::ostream& operator<<(std::ostream&, 
                                   const stringSet&);
   
private:

   std::unordered_set<std::string> set;

};

#endif
