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

parser_t* fetchdeps_parser_new(char* fname);
void fetchdeps_parser_free(parser_t* ctx);

bool_t fetchdeps_parser_initvars(parser_t* ctx);
bool_t fetchdeps_parser_parse(parser_t* ctx, stringset_t* results);


#endif // fetchdeps_parse_h

