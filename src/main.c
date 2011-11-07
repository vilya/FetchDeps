#include "filesys.h"
#include "parse.h"
#include "stringset.h"

#include <stdio.h>
#include <stdlib.h>

//
// Functions
//

int main(int argc, char** argv)
{
  char* fname = NULL;
  bool_t success;
  parser_t* ctx = NULL;
  stringset_t* results = NULL;
  stringiter_t* result_iter = NULL;
  char* result_str = NULL;

  if (argc <= 1) {
    fname = fetchdeps_filesys_default_depsfile();
    if (!fname) {
      fprintf(stderr, "Error: no deps file specified and couldn't find default.deps\n");
      goto failure;
    }
  }
  else {
    fname = argv[1];
  }

  ctx = fetchdeps_parser_new(fname);
  if (!ctx) {
    fprintf(stderr, "Error: unable to create parser for %s\n", fname);
    goto failure;
  }

  if (!fetchdeps_parser_initvars(ctx)) {
    fprintf(stderr, "Error: unable to initialise variables for parsing\n");
    goto failure;
  }
  
  results = fetchdeps_stringset_new();
  if (!results) {
    fprintf(stderr, "Error: unable to allocate memory for results\n");
    goto failure;
  }

  if (!fetchdeps_parser_parse(ctx, results)) {
    fprintf(stderr, "Error: parsing failed\n");
    goto failure;
  }

  fetchdeps_parser_free(ctx);

  // Finished parsing, let's do something with the results.
  result_iter = fetchdeps_stringiter_new(results);
  result_str = fetchdeps_stringiter_next(result_iter);
  while (result_str) {
    printf("%s\n", result_str);
    result_str = fetchdeps_stringiter_next(result_iter);
  }

  if (fname != argv[1])
    free(fname);
  fetchdeps_stringiter_free(result_iter);
  fetchdeps_stringset_free(results);
  return 0;

failure:
  if (fname && fname != argv[1])
    free(fname);
  if (ctx)
    fetchdeps_parser_free(ctx);
  if (results)
    fetchdeps_stringset_free(results);
  return 1;
}
