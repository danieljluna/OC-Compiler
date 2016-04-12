// djuna: Daniel Luna

#include <stdio.h>
#include "stringset.h"
#include "auxlib.h"

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim);


// Run cpp against the lines of the file.
stringSet cpplines (FILE* pipe, char* filename);

