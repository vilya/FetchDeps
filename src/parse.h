#ifndef fetchdeps_parse_h
#define fetchdeps_parse_h

#include "common.h"

#include <stdio.h>


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
  Bool expectingIndent;

  char lineBuf[kMaxLineLen];
  int indents[kMaxIndents];

  int numVars;
  int numValues[kMaxVariables];
  char varNames[kMaxVariables][kMaxVarNameLen];
  char varValues[kMaxVariables][kMaxValues][kMaxValueLen];
};
typedef struct SParserContext ParserContext;


//
// Public functions
//

ParserContext* Open(char* fname);
void Close(ParserContext* ctx);

void SetBuiltinVariables(ParserContext* ctx);
int AddVariable(ParserContext* ctx, const char* varName);
void AddValue(ParserContext* ctx, int varIndex, const char* value);

Bool Parse(ParserContext* ctx);


//
// Functions for use inside the parser
//

void Error(ParserContext* ctx, char* format, ...);
void Info(ParserContext* ctx, char* format, ...);
int FindVariable(ParserContext* ctx, const char* varName);
Bool HasValue(ParserContext* ctx, int varIndex, const char* value);

#endif // fetchdeps_parse_h

