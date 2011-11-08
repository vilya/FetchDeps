#include "common.h"
#include "download.h"
#include "environ.h"
#include "filesys.h"
#include "parse.h"
#include "stringset.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//
// Functions
//

void
usage(char* prog)
{
  fprintf(stderr,
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
      , prog);
}


void
print_urls(stringset_t* urls)
{
  stringiter_t* url_iter = NULL;
  char* url;

  url_iter = fetchdeps_stringiter_new(urls);
  if (!url_iter)
    goto failure;

  url = fetchdeps_stringiter_next(url_iter);
  while (url) {
    printf("%s\n", url);
    url = fetchdeps_stringiter_next(url_iter);
  }

  fetchdeps_stringiter_free(url_iter);
  return;

failure:
  if (url_iter)
    fetchdeps_stringiter_free(url_iter);
}


void
print_vars(varmap_t* vm)
{
  // TODO
}


int
main(int argc, char** argv)
{
  parser_t* ctx = NULL;
  stringset_t* urls = NULL;

  // Options which can be set on the command line.
  char* fname = NULL;
  char* to_dir = NULL;
  bool_t verbose = 0;
  bool_t no_changes = 0;

  // Parse the command line.
  char* short_options = "f:t:vnh";
  struct option long_options [] = {
    { "file",       required_argument,  NULL, 'f' },
    { "to-dir",     required_argument,  NULL, 't' },
    { "verbose",    no_argument,        NULL, 'v' },
    { "no-changes", no_argument,        NULL, 'n' },
    { "help",       no_argument,        NULL, 'h' },
    { NULL,         0,                  NULL, 0 }
  };
  char ch;
  while ((ch = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    switch (ch) {
    case 'f':
      if (fname) {
        free(fname);
        fprintf(stderr, "Warning: multiple deps files specified; all but the last will be ignored.\n");
      }
      fname = strdup(optarg);
      if (!fname) {
        fprintf(stderr, "Error: too low on memory to proceed.\n");
        goto failure;
      }
      break;
    case 't':
      if (to_dir) {
        free(to_dir);
        fprintf(stderr, "Warning: multiple to-dirs specified; all but the last will be ignored.\n");
      }
      to_dir = strdup(optarg);
      if (!to_dir) {
        fprintf(stderr, "Error: too low on memory to proceed.\n");
        goto failure;
      }
      break;
    case 'v':
      verbose = 1;
      break;
    case 'n':
      no_changes = 1;
      break;
    case 'h':
      usage(argv[0]);
      exit(0);
      break;
    default:
      fprintf(stderr, "Error: unknown option '%s'. %s -h for usage info.\n",
              argv[optind], argv[0]);
      exit(1);
      break;
    }
  }
  argc -= optind;
  argv += optind;

  // If no fname was given try to find the default deps file.
  if (!fname)
    fname = fetchdeps_filesys_default_deps_file();
  if (!fname) {
    fprintf(stderr, "Error: no deps file specified and couldn't find default.deps\n");
    goto failure;
  }

  // If no to_dir was given, use the default location.
  if (!to_dir)
    to_dir = fetchdeps_filesys_default_download_dir(fname);
  if (!to_dir) {
    fprintf(stderr, "Error: no to-dir specified and couldn't determing the default location.\n");
    goto failure;
  }
  if (!no_changes && !fetchdeps_filesys_is_directory(to_dir)) {
    fprintf(stderr, "Error: to-dir %s is not a directory, or is not writable.\n", to_dir);
    goto failure;
  }

  // Set up for parsing.
  ctx = fetchdeps_parser_new(fname);
  if (!ctx) {
    fprintf(stderr, "Error: unable to create parser for %s\n", fname);
    goto failure;
  }
  if (!fetchdeps_environ_default_vars(ctx->vars)) {
    fprintf(stderr, "Error: unable to initialise variables for parsing\n");
    goto failure;
  }
  if (!fetchdeps_environ_get_vars(ctx->vars)) {
    fprintf(stderr, "Error: unable to get variables from the environment\n");
    goto failure;
  }
  if (!fetchdeps_environ_parse_vars(ctx->vars, argv)) {
    fprintf(stderr, "Error: unable to get variables from the command line\n");
    goto failure;
  }
  urls = fetchdeps_stringset_new();
  if (!urls) {
    fprintf(stderr, "Error: unable to allocate memory for results\n");
    goto failure;
  }

  if (verbose)
    print_vars(ctx->vars);

  // Parse away!
  if (!fetchdeps_parser_parse(ctx, urls)) {
    fprintf(stderr, "Error: parsing failed\n");
    goto failure;
  }
  fetchdeps_parser_free(ctx);
  ctx = NULL;

  // Finished parsing, let's do something with the urls.
  if (no_changes) {
    print_urls(urls);
  }
  else {
    if (!fetchdeps_download_fetch_all(urls, to_dir)) {
      fprintf(stderr, "Error: download failed.\n");
      goto failure;
    }
  }

  // Clean up.
  if (fname)
    free(fname);
  if (to_dir)
    free(to_dir);
  fetchdeps_stringset_free(urls);

  return 0;

failure:
  if (fname)
    free(fname);
  if (to_dir)
    free(to_dir);
  if (ctx)
    fetchdeps_parser_free(ctx);
  if (urls)
    fetchdeps_stringset_free(urls);
  return 1;
}
