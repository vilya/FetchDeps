#include "parse.h"

#include "stringset.h"

#include <assert.h>
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

ParserContext*
fetchdeps_parser_new(char* fname)
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


void 
fetchdeps_parser_free(ParserContext* ctx)
{
  if (ctx->f)
    fclose(ctx->f);
  if (ctx->vars)
    fetchdeps_varmap_free(ctx->vars);
  free(ctx);
}


bool_t
fetchdeps_parser_initvars(ParserContext* ctx)
{
  int osIndex, bitsIndex;

  assert(ctx != NULL);
  assert(ctx->vars != NULL);

  if (!fetchdeps_varmap_set_single(ctx->vars, "os", kOperatingSystem))
    return 0;

  if (!fetchdeps_varmap_set_single(ctx->vars, "bits", "64"))
    return 0;
}


void
fetchdeps_parser_error(ParserContext* ctx, char* format, ...)
{
  va_list args;

  fprintf(stderr, "[line %d] Error: ", ctx->lineNum);

  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  fprintf(stderr, "\n");
  exit(1);
}


void
fetchdeps_parser_info(ParserContext* ctx, char* format, ...)
{
  va_list args;

  printf("[line %d] ", ctx->lineNum);

  va_start(args, format);
  vprintf(format, args);
  va_end(args);

  printf("\n");
}

