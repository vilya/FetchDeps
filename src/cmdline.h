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


enum _action {
  ACTION_HELP,
  ACTION_INIT,
  ACTION_GET,
  ACTION_LIST,
  ACTION_INSTALL,
  ACTION_UNINSTALL,
  ACTION_DELETE,
  ACTION_VARS,
  ACTION_UNKNOWN,
};

typedef enum _action action_t;


struct _cmdline {
  int argc;
  char** argv;

  char* prog;

  char* fname;
  bool_t verbose;
  bool_t no_changes;
  action_t action;
};

typedef struct _cmdline cmdline_t;


//
// Functions
//

void fetchdeps_cmdline_init(cmdline_t* options, int argc, char** argv);
void fetchdeps_cmdline_cleanup(cmdline_t* options);
exittype_t fetchdeps_cmdline_parse(cmdline_t* options);

void fetchdeps_cmdline_print_usage(cmdline_t* options, FILE* out);
void fetchdeps_cmdline_print_action_help(action_t action, FILE* out);
action_t fetchdeps_cmdline_lookup_action(char* action_name);

#endif // fetchdeps_cmdline_h

