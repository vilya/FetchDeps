#include "parse.h"

#include "errors.h"
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
  if (!ctx->f) {
    fetchdeps_errors_set_with_msg(ERR_SYSTEM, "Unable to open deps file %s", fname);
    goto failure;
  }

  ctx->indent_level = 0;

  return ctx;

failure:
  fetchdeps_errors_trap_system_error();
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

