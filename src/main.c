#include "parse.h"
#include "stringset.h"


//
// Functions
//

int main(int argc, char** argv)
{
  char* fname;
  bool_t success;
  parser_t* ctx = NULL;
  stringset_t* results = NULL;
  stringiter_t* result_iter = NULL;
  char* result_str = NULL;

  fname = (argc > 1) ? argv[1] : "default.deps";
  ctx = fetchdeps_parser_new(fname);
  if (!ctx)
    goto failure;

  if (!fetchdeps_parser_initvars(ctx))
    goto failure;
  
  results = fetchdeps_stringset_new();
  if (!results)
    goto failure;

  if (!fetchdeps_parser_parse(ctx, results))
    goto failure;

  fetchdeps_parser_free(ctx);

  // Finished parsing, let's do something with the results.
  result_iter = fetchdeps_stringiter_new(results);
  result_str = fetchdeps_stringiter_next(result_iter);
  while (result_str) {
    printf("URL: %s\n", result_str);
    result_str = fetchdeps_stringiter_next(result_iter);
  }

  fetchdeps_stringiter_free(result_iter);
  fetchdeps_stringset_free(results);
  return 0;

failure:
  if (ctx)
    fetchdeps_parser_free(ctx);
  if (results)
    fetchdeps_stringset_free(results);
  return 1;
}
