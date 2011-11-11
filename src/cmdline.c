#include "cmdline.h"

#include <assert.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>


//
// Forward declarations
//

void fetchdeps_cmdline_print_usage(cmdline_t* options, FILE* out);


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
  struct {
    char* name;
    action_t action;
  } actions[] = {
    { "help",       ACTION_HELP       },
    { "init",       ACTION_INIT       },
    { "get",        ACTION_GET        },
    { "list",       ACTION_LIST       },
    { "install",    ACTION_INSTALL    },
    { "uninstall",  ACTION_UNINSTALL  },
    { "delete",     ACTION_DELETE     },
    { NULL,         ACTION_UNKNOWN    }
  };

  int i;
  char ch;
  exittype_t exit_type = NO_EXIT;

  while ((ch = getopt_long(options->argc, options->argv, short_options, long_options, NULL)) != -1) {
    switch (ch) {
    case 'f':
      if (options->fname) {
        free(options->fname);
        fprintf(stderr, "Warning: multiple deps files specified; "
                        "all but the last will be ignored.\n");
      }
      options->fname = strdup(optarg);
      if (!options->fname) {
        fprintf(stderr, "Error: too low on memory to proceed.\n");
        goto failure;
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
      fprintf(stderr, "Error: unknown option '%s'. %s -h for usage info.\n",
              options->argv[optind], options->prog);
      goto failure;
      break;
    }
  }

  options->argc -= optind;
  options->argv += optind;

  // Figure out which action we're doing.
  if (options->argc > 0) {
    for (i = 0; actions[i].name != NULL; ++i) {
      if (strcmp(actions[i].name, options->argv[0]) == 0)
        break;
    }
    options->action = actions[i].action;
    if (options->action == ACTION_UNKNOWN)
      exit_type = EXIT_FAIL;
  }

  return exit_type;

failure:
  return EXIT_FAIL;
}


//
// Private functions
//

void
fetchdeps_cmdline_print_usage(cmdline_t* options, FILE* out)
{
  assert(options != NULL);
  assert(out != NULL);

  fprintf(out,
"Usage: %s [options]\n"
"\n"
"where [options] can be any combination of:\n"
"\n"
"-f, --file         Specify a deps file to use. If not specified we'll \n"
"                   search for a file called 'default.deps' in the current\n"
"                   directory or any of its ancestors and use that if found.\n"
"\n"
"-t, --to-dir       Specify the directory to store the downloaded files in.\n"
"                   This will default to 'Thirdparty' in the same directory\n"
"                   as the deps file if not specified.\n"
"\n"
"-v, --verbose      Print out all variables before starting to parse.\n"
"\n"
"-n, --no-changes   Don't download anything, or change the disk in any way,\n"
"                   but show what would have been downloaded.\n"
"\n"
"-h, --help   Print this message and exit.\n"
      , options->prog);
}

