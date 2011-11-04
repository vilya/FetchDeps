#ifndef fetchdeps_parse_h
#define fetchdeps_parse_h

#include "common.h"
#include "varmap.h"

#include <stdio.h>


//
// Types
//

struct _parser {
  FILE* f;

  int lineNum;
  int start;    // Index of first non-space char in the lineBuf
  int end;      // Index of the null char in the lineBuf
  int indentLevel;
  int skipLevel;    // The indent level we start skipping at. Skipping stops when a de-indent brings us back to this level.

  int indents[100]; // Ought to be enough for anybody...

  varmap_t* vars;
};
typedef struct _parser parser_t;


//
// Public functions
//

parser_t* fetchdeps_parser_new(char* fname);
void fetchdeps_parser_free(parser_t* ctx);

bool_t fetchdeps_parser_initvars(parser_t* ctx);
bool_t fetchdeps_parser_parse(parser_t* ctx);


#endif // fetchdeps_parse_h

