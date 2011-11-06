#include "parse.h"

#include "stringset.h"

#include <assert.h>
#include <ctype.h>
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

parser_t*
fetchdeps_parser_new(char* fname)
{
  parser_t* ctx = NULL;

  ctx = (parser_t*)calloc(1, sizeof(parser_t));
  if (!ctx)
    goto failure;

  ctx->vars = fetchdeps_varmap_new();
  if (!ctx->vars)
    goto failure;

  ctx->f = fopen(fname, "r");
  if (!ctx->f)
    goto failure;

  ctx->indent_level = 0;

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
fetchdeps_parser_free(parser_t* ctx)
{
  if (ctx->f)
    fclose(ctx->f);
  if (ctx->vars)
    fetchdeps_varmap_free(ctx->vars);
  free(ctx);
}


bool_t
fetchdeps_parser_initvars(parser_t* ctx)
{
  int osIndex, bitsIndex;

  assert(ctx != NULL);
  assert(ctx->vars != NULL);

  if (!fetchdeps_varmap_set_single(ctx->vars, "os", kOperatingSystem))
    return 0;

  if (!fetchdeps_varmap_set_single(ctx->vars, "bits", "64"))
    return 0;
}

