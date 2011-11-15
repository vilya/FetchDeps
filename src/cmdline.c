#include "cmdline.h"

#include "errors.h"

#include <assert.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>


//
// Constants
//

static const struct {
  char* name;
  action_t action;
} ACTIONS[] = {
  { "help",       ACTION_HELP       },
  { "init",       ACTION_INIT       },
  { "get",        ACTION_GET        },
  { "list",       ACTION_LIST       },
  { "install",    ACTION_INSTALL    },
  { "uninstall",  ACTION_UNINSTALL  },
  { "delete",     ACTION_DELETE     },
  { "vars",       ACTION_VARS       },
  { NULL,         ACTION_UNKNOWN    }
};
static const char* ACTION_HELP_MSG[] = {
  "ACTION_HELP",
  "ACTION_INIT",
  "ACTION_GET",
  "ACTION_LIST",
  "ACTION_INSTALL",
  "ACTION_UNINSTALL",
  "ACTION_DELETE",
  "ACTION_VARS",
  NULL,
};


//
// Public functions
//

void
fetchdeps_cmdline_init(cmdline_t* options, int argc, char** argv)
{
  assert(options != NULL);

  options->argc = argc;
  options->argv = argv;

  options->prog = argv[0];

  options->fname = NULL;
  options->verbose = 0;
  options->no_changes = 0;
  options->action = ACTION_HELP;
}


void
fetchdeps_cmdline_cleanup(cmdline_t* options)
{
  assert(options != NULL);

  if (options->fname)
    free(options->fname);
}


exittype_t
fetchdeps_cmdline_parse(cmdline_t* options)
{
  // Parse the command line.
  char* short_options = "f:t:vnh";
  struct option long_options [] = {
    { "file",       required_argument,  NULL, 'f' },
    { "verbose",    no_argument,        NULL, 'v' },
    { "no-changes", no_argument,        NULL, 'n' },
    { "help",       no_argument,        NULL, 'h' },
    { NULL,         0,                  NULL, 0 }
  };

  char ch;
  exittype_t exit_type = NO_EXIT;

  while ((ch = getopt_long(options->argc, options->argv, short_options, long_options, NULL)) != -1) {
    switch (ch) {
    case 'f':
      if (options->fname) {
        fetchdeps_errors_set_with_msg(ERR_CMDLINE, "Multiple deps files specified, which confuses me");
        exit_type = EXIT_FAIL;
      }
      else {
        options->fname = strdup(optarg);
        if (!options->fname)
          exit_type = EXIT_FAIL;
      }
      break;
    case 'v':
      options->verbose = 1;
      break;
    case 'n':
      options->no_changes = 1;
      break;
    case 'h':
      fetchdeps_cmdline_print_usage(options, stderr);
      exit_type = EXIT_OK;
      break;
    default:
      fetchdeps_errors_set_with_msg(ERR_CMDLINE, "Unknown option '%s'", options->argv[optind]);
      exit_type = EXIT_FAIL;
      break;
    }

    if (exit_type != NO_EXIT)
      break;
  }

  if (exit_type == EXIT_FAIL)
    goto failure;

  options->argc -= optind;
  options->argv += optind;

  // Figure out which action we're doing.
  if (options->argc > 0) {
    options->action = fetchdeps_cmdline_lookup_action(options->argv[0]);
    if (options->action == ACTION_UNKNOWN) {
      fetchdeps_errors_set_with_msg(ERR_CMDLINE, "Unknown action '%s'", options->argv[0]);
      exit_type = EXIT_FAIL;
    }
  }

  return exit_type;

failure:
  fetchdeps_errors_trap_system_error();
  return EXIT_FAIL;
}


void
fetchdeps_cmdline_print_usage(cmdline_t* options, FILE* out)
{
  assert(options != NULL);
  assert(out != NULL);

  fprintf(out,
"Usage: %s [options] <command> [ var=value ... ]\n"
"\n"
"The <command> can be any of 'help', 'init', 'get', 'list', 'install',\n"
"'uninstall' or 'delete'. %s help <command> will provide more detailed help\n"
"on a specific command.\n"
"\n"
"The [options] can be any combination of:\n"
"\n"
"-f, --file         Specify a deps file to use. If not specified we'll \n"
"                   search for a file called 'default.deps' in the current\n"
"                   directory or any of its ancestors and use that if found.\n"
"\n"
"-v, --verbose      Print out all variables before starting to parse.\n"
"\n"
"-n, --no-changes   Don't download anything, or change the disk in any way,\n"
"                   but show what would have been downloaded.\n"
"\n"
"-h, --help   Print this message and exit.\n"
      , options->prog, options->prog);
}


void
fetchdeps_cmdline_print_action_help(action_t action, FILE* out)
{
  fprintf(out, "%s\n", ACTION_HELP_MSG[action]);
}


action_t
fetchdeps_cmdline_lookup_action(char* action_name)
{
  int i;

  assert(action_name != NULL);

  for (i = 0; ACTIONS[i].name != NULL; ++i) {
    if (strcmp(ACTIONS[i].name, action_name) == 0)
      break;
  }

  return ACTIONS[i].action;
}
