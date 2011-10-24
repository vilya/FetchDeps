#ifndef fetchdeps_parse_h
#define fetchdeps_parse_h

#include "common.h"

//
// Types
//

struct SParserContext;
typedef struct SParserContext ParserContext;


//
// Functions
//

ParserContext* Open(char* fname);
void Close(ParserContext* ctx);
int AddVariable(ParserContext* ctx, const char* varName);
void AddValue(ParserContext* ctx, int varIndex, const char* value);
Bool Parse(ParserContext* ctx);


#endif // fetchdeps_parse_h

