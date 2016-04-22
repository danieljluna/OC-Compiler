// djluna: Daniel Luna

#include <iomanip>
using namespace std;

#include "stringset.h"

using str_set = unordered_set<string>;

const string* stringSet::insert(const char* string) {
   pair<str_set::const_iterator,bool> handle = set.insert(string);
   return &*handle.first;
}

void stringSet::dump (ostream& out) const {
   size_t max_bucket_size = 0;
   for (size_t bucket = 0; bucket < set.bucket_count(); ++bucket) {
      bool need_index = true;
      size_t curr_size = set.bucket_size (bucket);
      if (max_bucket_size < curr_size) {max_bucket_size = curr_size;}
      for (str_set::const_local_iterator itor = set.cbegin(bucket);
           itor != set.cend (bucket); ++itor) {
         if (need_index) out << "hash[" << setw(4) << bucket
                             << "]: ";
                    else out << setw(12) << "";
         need_index = false;
         const string* str = &*itor;
         out << setw(20) << set.hash_function()(*str) << ": "
             << str << "->\"" << *str << "\"" << endl;
      }
   }
   out << "load_factor = " << fixed << setprecision(3)
       << set.load_factor() << endl;
   out << "bucket_count = " << set.bucket_count() << endl;
   out << "max_bucket_size = " << max_bucket_size << endl;
}




std::ostream& operator<<(ostream& out,
                         const stringSet& strSet) {
   strSet.dump(out);
   
   return out;
}
