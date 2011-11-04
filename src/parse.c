#include "parse.h"

#include "stringset.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// Platform specific includes.
#ifndef _WIN32
#include <strings.h>  // Provides strcasecmp on linux & mac.
#else
// Windows defines _stricmp in <string.h>
#define strcasecmp(a,b) _stricmp((a),(b))
// Note: don't use snprintf/_snprintf - semantics are different enough between
// Linux & Win to cause errors.
#endif


//
// Public functions
//

ParserContext* Open(char* fname)
{
  ParserContext* ctx = NULL;

  ctx = (ParserContext*)calloc(1, sizeof(ParserContext));
  if (!ctx)
    goto failure;

  ctx->vars = fetchdeps_varmap_new();
  if (!ctx->vars)
    goto failure;

  ctx->f = fopen(fname, "r");
  if (!ctx->f)
    goto failure;

  ctx->lineNum = 0;
  ctx->start = -1;
  ctx->end = -1;
  ctx->indentLevel = 0;
  ctx->skipLevel = -1;

  return ctx;

failure:
  if (ctx) {
    if (ctx->f)
      fclose(ctx->f);
    if (ctx->vars)
      fetchdeps_varmap_free(ctx->vars);
    free(ctx);
  }
  return NULL;
}


void Close(ParserContext* ctx)
{
  if (ctx->f)
    fclose(ctx->f);
  if (ctx->vars)
    fetchdeps_varmap_free(ctx->vars);
  free(ctx);
}


void SetBuiltinVariables(ParserContext* ctx)
{
  int osIndex, bitsIndex;

  assert(ctx != NULL);
  assert(ctx->vars != NULL);

  fetchdeps_varmap_set_single(ctx->vars, "os", kOperatingSystem);
  fetchdeps_varmap_set_single(ctx->vars, "bits", "64");
}


//
// Private functions
//

void Error(ParserContext* ctx, char* format, ...)
{
  va_list args;

  fprintf(stderr, "[line %d] Error: ", ctx->lineNum);

  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  fprintf(stderr, "\n");
  exit(1);
}


void Info(ParserContext* ctx, char* format, ...)
{
  va_list args;

  printf("[line %d] ", ctx->lineNum);

  va_start(args, format);
  vprintf(format, args);
  va_end(args);

  printf("\n");
}


/*
static bool_t HandleCondition(ParserContext* ctx, int next)
{
  char varName[kMaxVarNameLen];
  char value[kMaxValueLen];
  int valStart, varIndex, varNameLen, valLen;

  bool_t result = False;

  varNameLen = next - ctx->start;
  if (varNameLen >= kMaxVarNameLen)
    Error(ctx, "%d chars is too long for a variable name (maximum is %d)", varNameLen, kMaxVarNameLen);

  memset(varName, 0, sizeof(varName));
  CopyStr(varName, &ctx->lineBuf[ctx->start], next - ctx->start);

  varIndex = FindVariable(ctx, varName);
  if (varIndex == -1)
    Error(ctx, "unknown variable \"%s\"", varName);

  while (True) {
    // Skip optional leading whitespace
    while (isspace(ctx->lineBuf[next]))
      ++next;

    // We should be at the start of a value string.
    if (ctx->lineBuf[next] != '"')
      Error(ctx, "expecting the start of a value string but found '%c' instead", ctx->lineBuf[next]);

    // Find the start and end of the string's contents.
    ++next;
    valStart = next;
    while (ctx->lineBuf[next] != '\0' && ctx->lineBuf[next] != '"')
      ++next;
    if (ctx->lineBuf[next] != '"')
      Error(ctx, "unclosed string");

    // Make sure the value isn't over the length limit.
    valLen = next - valStart;
    if (valLen >= kMaxValueLen)
      Error(ctx, "%d chars is too long for a value (maximum is %d)", valLen, kMaxValueLen);

    // Check the string against the list of values for the variable.
    memset(value, 0, sizeof(value));
    CopyStr(value, &ctx->lineBuf[valStart], valLen);
    if (HasValue(ctx, varIndex, value)) {
      result = True;
      // Note: no early exit, to force syntax checking for the remainder of the string.
    }

    // Skip over the closing double-quote.
    ++next;

    // Skip optional trailing whitespace.
    while (isspace(ctx->lineBuf[next]))
      ++next;

    // We should be at either a comma or a colon. If a comma, check the next
    // value; if a colon we've reached the end of the line.
    if (ctx->lineBuf[next] == ',')
      ++next;
    else if (ctx->lineBuf[next] == ':')
      break;
    else if (ctx->lineBuf[next] == '\0')
      Error(ctx, "unexpected end of line; missing a colon?");
    else
      Error(ctx, "unexpected char '%c', expected ',' or ':'", ctx->lineBuf[next]);
  }

  if (!result)
    ctx->skipLevel = ctx->indentLevel;

  //Info(ctx, "condition --> %s --> is %s", &ctx->lineBuf[ctx->start], result ? "True" : "False");

  ctx->expectingIndent = True;
  return result;
}
*/
