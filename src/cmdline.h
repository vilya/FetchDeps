#ifndef fetchdeps_cmdline_h
#define fetchdeps_cmdline_h

#include "common.h"

#include <stdio.h>

//
// Types
//

enum _exittype {
  NO_EXIT,
  EXIT_OK,
  EXIT_FAIL
};

typedef enum _exittype exittype_t;


struct _cmdline {
  int argc;
  char** argv;

  char* prog;

  char* fname;
  bool_t verbose;
  bool_t no_changes;
};

typedef struct _cmdline cmdline_t;


//
// Functions
//

void fetchdeps_cmdline_init(cmdline_t* options, int argc, char** argv);
void fetchdeps_cmdline_cleanup(cmdline_t* options);
exittype_t fetchdeps_cmdline_parse(cmdline_t* options);

#endif // fetchdeps_cmdline_h

