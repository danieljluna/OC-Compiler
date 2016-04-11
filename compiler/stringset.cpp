// djluna: Daniel Luna

#include <iomanip>
using namespace std;

#include "stringset.h"

using stringset = unordered_set<string>;

stringset set;

const string* stringSet::intern_stringset (const char* string) {
   pair<stringset::const_iterator,bool> handle = set.insert (string);
   return &*handle.first;
}

void stringSet::dump_stringset (ostream& out) {
   size_t max_bucket_size = 0;
   for (size_t bucket = 0; bucket < set.bucket_count(); ++bucket) {
      bool need_index = true;
      size_t curr_size = set.bucket_size (bucket);
      if (max_bucket_size < curr_size) max_bucket_size = curr_size;
      for (stringset::const_local_iterator itor = set.cbegin (bucket);
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




std::ostream& operator<<(const ostream& out,
                         const stringSet& strSet) {
   strSet.dump_stringset(out);
}
