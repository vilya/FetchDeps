#include "parse.h"

//
// Functions
//

int main(int argc, char** argv)
{
  char* fname;
  bool_t success;
  ParserContext* ctx = NULL;

  fname = (argc > 1) ? argv[1] : "default.deps";
  ctx = fetchdeps_parser_new(fname);
  if (!ctx)
    goto failure;

  if (!fetchdeps_parser_initvars(ctx))
    goto failure;
  
  if (!fetchdeps_parser_parse(ctx))
    goto failure;

  fetchdeps_parser_free(ctx);
  return 0;

failure:
  if (ctx)
    fetchdeps_parser_free(ctx);
  return 1;
}
