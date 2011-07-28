#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <strings.h>  // Provides strcasecmp on linux & mac.
#else
// Windows defines _stricmp in <string.h>
#define strcasecmp(a,b) _stricmp((a),(b))
// Note: don't use snprintf/_snprintf - semantics are different enough between
// Linux & Win to cause errors.
#endif


//
// Constants
//

#define kMaxLineLen 4096
#define kMaxIndents 128
#define kMaxVariables 128
#define kMaxVarNameLen 1024
#define kMaxValues 128
#define kMaxValueLen 256

#if defined(_WIN64) || defined(_WIN32) || defined(__CYGWIN__)
  #define kOperatingSystem  "win"
#elif defined(__linux__)
  #define kOperatingSystem  "linux"
#elif defined(__APPLE__)
  #define kOperatingSystem  "mac"
#endif


//
// Typedefs
//

typedef int Bool;
static const Bool False = 0;
static const Bool True = ~0;


//
// Structs
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
// Functions
//

int CopyStr(char* to, const char* from, int numChars)
{
  int i;
  for (i = 0; i < numChars && from[i] != '\0'; ++i) {
    to[i] = from[i];
  }
  to[i] = '\0';
  return i;
}


Bool IdentifierChar(char ch)
{
  return isalnum(ch) || ch == '_' || ch == '-';
}


ParserContext* Open(char* fname)
{
  ParserContext* ctx = NULL;

  FILE* f = fopen(fname, "r");
  if (!f) {
    fprintf(stderr, "Error: unable to open %s\n", fname);
    exit(1);
  }

  ctx = (ParserContext*)calloc(1, sizeof(ParserContext));
  if (!ctx) {
    fprintf(stderr, "Error: unable to create parser for %s. Running low on memory?\n", fname);
    exit(1);
  }

  ctx->f = f;
  ctx->lineNum = 0;
  ctx->start = -1;
  ctx->end = -1;
  ctx->indentLevel = 0;
  ctx->skipLevel = -1;
  ctx->expectingIndent = False;

  ctx->numVars = 0;

  return ctx;
}


void Close(ParserContext* ctx)
{
  if (ctx->f)
    fclose(ctx->f);
  free(ctx);
}


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


int FindVariable(ParserContext* ctx, const char* varName)
{
  int i;
  for (i = 0; i < ctx->numVars; ++i) {
    if (strcasecmp(varName, ctx->varNames[i]) == 0)
      return i;
  }
  return -1;
}


Bool HasValue(ParserContext* ctx, int varIndex, const char* value)
{
  int i;
  for (i = 0; i < ctx->numValues[varIndex]; ++i) {
    if (strcasecmp(value, ctx->varValues[varIndex][i]) == 0)
      return True;
  }
  return False;
}


int AddVariable(ParserContext* ctx, const char* varName)
{
  int index;

  if (ctx->numVars == kMaxVariables)
    Error(ctx, "too many variables (maximum is %d)", kMaxVariables);

  index = ctx->numVars;
  CopyStr(ctx->varNames[index], varName, kMaxVarNameLen - 1);

  ctx->numValues[index] = 0;
  ctx->numVars++;

  return index;
}


void AddValue(ParserContext* ctx, int varIndex, const char* value)
{
  int valIndex;

  if (ctx->numValues[varIndex] == kMaxValues)
    Error(ctx, "too many values for variable %s (maximum is %d)", ctx->varNames[varIndex], kMaxValues);

  valIndex = ctx->numValues[varIndex];
  CopyStr(ctx->varValues[varIndex][valIndex], value, kMaxValueLen - 1);

  ctx->numValues[varIndex]++;
}


void SetVariables(ParserContext* ctx, int argc, char** argv)
{
  int osIndex, bitsIndex;

  osIndex = AddVariable(ctx, "os");
  AddValue(ctx, osIndex, kOperatingSystem);

  bitsIndex = AddVariable(ctx, "bits");
  AddValue(ctx, bitsIndex, "64");
}


Bool ReadLine(ParserContext* ctx)
{
  ++ctx->lineNum;
  if (fgets(ctx->lineBuf, kMaxLineLen, ctx->f)) {
    // Strips trailing whitespace.
    ctx->end = strlen(ctx->lineBuf);
    while (ctx->end > 0 && isspace(ctx->lineBuf[ctx->end - 1]))
      ctx->lineBuf[--ctx->end] = '\0';
    return True;
  }
  else {
    ctx->end = -1;
    return False;
  }
}


void HandleIndentation(ParserContext* ctx)
{
  // Find how far this line is indented.
  ctx->start = 0;
  while (ctx->lineBuf[ctx->start] == ' ')
    ++ctx->start;

  // If the line is blank, or comment-only, we can skip it. Otherwise... we have work to do!
  if (ctx->lineBuf[ctx->start] == '\0' && ctx->lineBuf[ctx->start] == '#')
   ctx->start = -1;

  // If the ctx->start is increasing....
  if (ctx->start > ctx->indents[ctx->indentLevel]) {
    // Check that we're actually expecting an indentation here.
    if (!ctx->expectingIndent)
      Error(ctx, "unexpected indentation");

    // Make sure we don't overflow the indents stack.
    if (ctx->indentLevel == (kMaxIndents - 1))
      Error(ctx, "too many levels of indentation (maximum is %d)", kMaxIndents);

    ctx->expectingIndent = False;
    ++ctx->indentLevel;
    ctx->indents[ctx->indentLevel] = ctx->start;
  }
  // Otherwise if the indent is staying the same
  else if (ctx->start == ctx->indents[ctx->indentLevel]) {
    if (ctx->expectingIndent)
      Error(ctx, "was expecting the line to be indented");
  }
  // Otherwise the indent must be decreasing... but by how much?
  else {
    if (ctx->expectingIndent)
      Error(ctx, "expecting an indent but got an un-indent");

    while (ctx->start < ctx->indents[ctx->indentLevel]) {
      // Make sure we don't underflow the indents stack.
      if (ctx->indentLevel == 0)
        Error(ctx, "too many levels of unindentation. Are you missing a condition line?");

      --ctx->indentLevel;
    }

    // Make sure the unindent matches the previous indentation level
    if (ctx->start != ctx->indents[ctx->indentLevel])
      Error(ctx, "bad unindent, doesn't align with an earlier indentation level");

    // Now check if we've finished skipping stuff.
    if (ctx->indentLevel <= ctx->skipLevel)
      ctx->skipLevel = -1;
  }
}


Bool IgnorableLine(ParserContext* ctx)
{
  Bool emptyLine, commentLine;

  emptyLine = ctx->start < 0 || ctx->end <= ctx->start;
  if (emptyLine)
    return True;

  commentLine = ctx->lineBuf[ctx->start] == '#';
  if (commentLine)
    return True;

  return False;
}


Bool SkippableLine(ParserContext* ctx)
{
  return (ctx->skipLevel >= 0 && ctx->indentLevel >ctx->skipLevel);
}


Bool EndsWith(ParserContext* ctx, char ch)
{
  return (ctx->end > 0) && (ctx->lineBuf[ctx->end - 1] == ch);
}


Bool HandleCondition(ParserContext* ctx, int next)
{
  char varName[kMaxVarNameLen];
  char value[kMaxValueLen];
  int valStart, varIndex, varNameLen, valLen;

  Bool result = False;

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


int main(int argc, char** argv)
{
  int next;

  char* fname = (argc > 1) ? argv[1] : "default.deps";
  ParserContext* ctx = Open(fname);

  SetVariables(ctx, argc, argv);

  while (ReadLine(ctx)) {
    HandleIndentation(ctx);
    if (IgnorableLine(ctx))
      continue;
    if (SkippableLine(ctx)) {
      if (EndsWith(ctx, ':'))
        ctx->expectingIndent = True;
      continue;
    }

    ctx->expectingIndent = False;
    next = ctx->start;
    while (IdentifierChar(ctx->lineBuf[next]))
      ++next;

    if (next > ctx->start && ctx->lineBuf[next] == ':') {
      // It looks like a URL...
      Info(ctx, "%s", &ctx->lineBuf[ctx->start]);
    }
    else if (next > ctx->start && isspace(ctx->lineBuf[next])) {
      // It looks like a variable name...
      HandleCondition(ctx, next);
    }
    else {
      // Looks like an error.
      Error(ctx, "unexpected character '%c'", ctx->lineBuf[next]);
    }
  }

  Close(ctx);
  return 0;
}
