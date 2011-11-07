#include "common.h"
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
"-f, --file   Specify a deps file to use. If not specified we'll search for\n"
"             a file called default.deps in the current directory or any of\n"
"             its ancestors and use that if found.\n"
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

int
main(int argc, char** argv)
{
  bool_t success;
  parser_t* ctx = NULL;
  stringset_t* urls = NULL;

  // Options which can be set on the command line.
  char* fname = NULL;
  char* to_dir = NULL;
  bool_t no_changes = 0;

  // Parse the command line.
  char* short_options = "f:nh";
  struct option long_options [] = {
    { "file",       required_argument,  NULL, 'f' },
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
  if (!fetchdeps_filesys_is_directory(to_dir)) {
    fprintf(stderr, "Error: to-dir %s is not a directory, or is not writable.\n", to_dir);
    goto failure;
  }

  // Set up for parsing.
  ctx = fetchdeps_parser_new(fname);
  if (!ctx) {
    fprintf(stderr, "Error: unable to create parser for %s\n", fname);
    goto failure;
  }
  if (!fetchdeps_parser_initvars(ctx)) {
    fprintf(stderr, "Error: unable to initialise variables for parsing\n");
    goto failure;
  }
  urls = fetchdeps_stringset_new();
  if (!urls) {
    fprintf(stderr, "Error: unable to allocate memory for results\n");
    goto failure;
  }

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
