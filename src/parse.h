#ifndef fetchdeps_parse_h
#define fetchdeps_parse_h

#include "common.h"
#include "stringset.h"
#include "varmap.h"

#include <stdio.h>


//
// Types
//

struct _parser {
  int indent_level; // Index of the current indent level in the indents array.
  int skip_level;   // The indent level we start skipping at. We stop when a de-indent brings us back to this level.
  int indents[100]; // Ought to be enough for anybody...

  varmap_t* vars;
  FILE* f;
};
typedef struct _parser parser_t;


//
// Public functions
//

// Create a new parser, ready to parse the specified file. If the file couldn't
// be opened, or we couldn't allocate the memory we need for the parser, the
// return value will be NULL. Otherwise it's a pointer to an initialised
// parser_t object, which must eventually be freed with fetchdeps_parser_free.
//
// It is safe to create multiple parsers, but only one of them may be parsing at
// a time as the flex- and bison-generated code uses some global variables to
// keep track of the parsing state.
parser_t* fetchdeps_parser_new(char* fname);

// Free an existing parser. As well as deallocating the memory (including the
// memory for the varmap), this also closes the input file.
void fetchdeps_parser_free(parser_t* ctx);

// Initialise the built-in variables for the parser. The built-in variables
// include the current operating system and bit-size.
bool_t fetchdeps_parser_initvars(parser_t* ctx);

// Parse the input file. This parses the entire file, evaluating any conditions
// in the file and fills in the results parameter. The return value is true if
// parsing was successful. If parsing failed for any reason - a syntax error,
// reference to a non-existent variable, failed memory allocation, etc. - then
// the return value will be false and nothing will be added to results.
bool_t fetchdeps_parser_parse(parser_t* ctx, stringset_t* results);


#endif // fetchdeps_parse_h

