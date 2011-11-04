#include "parse.h"

//
// Functions
//

int main(int argc, char** argv)
{
  char* fname = (argc > 1) ? argv[1] : "default.deps";
  bool_t success;

  ParserContext* ctx = Open(fname);
  SetBuiltinVariables(ctx);
  success = Parse(ctx);
  Close(ctx);

  if (success)
    return 0;
  else
    return 1;
}
