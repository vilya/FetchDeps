#ifndef fetchdeps_parse_h
#define fetchdeps_parse_h

#include "common.h"
#include "varmap.h"

#include <stdio.h>


//
// Constants
//

#define kMaxLineLen 4096
#define kMaxIndents 128
#define kMaxVariables 128
#define kMaxVarNameLen 1024
#define kMaxValues 128
#define kMaxValueLen 256


//
// Types
//

struct SParserContext {
  FILE* f;

  int lineNum;
  int start;    // Index of first non-space char in the lineBuf
  int end;      // Index of the null char in the lineBuf
  int indentLevel;
  int skipLevel;    // The indent level that we start skipping at. Skipping stops when a de-indent brings us back to this level.

  int indents[kMaxIndents];

  varmap_t* vars;
};
typedef struct SParserContext ParserContext;


//
// Public functions
//

ParserContext* fetchdeps_parser_new(char* fname);
void fetchdeps_parser_free(ParserContext* ctx);

bool_t fetchdeps_parser_initvars(ParserContext* ctx);
bool_t fetchdeps_parser_parse(ParserContext* ctx);


//
// Functions for use inside the parser
//

void fetchdeps_parser_error(ParserContext* ctx, char* format, ...);
void fetchdeps_parser_info(ParserContext* ctx, char* format, ...);

#endif // fetchdeps_parse_h

