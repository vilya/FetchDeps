#include "cmdline.h"
#include "common.h"
#include "download.h"
#include "environ.h"
#include "errors.h"
#include "filesys.h"
#include "parse.h"
#include "stringset.h"

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//
// Functions
//

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
  fetchdeps_errors_trap_system_error();
  if (url_iter)
    fetchdeps_stringiter_free(url_iter);
}


void
print_vars(varmap_t* vm)
{
  variter_t* iter = NULL;
  stringiter_t* value_iter = NULL;
  varentry_t entry;

  assert(vm != NULL);

  iter = fetchdeps_variter_new(vm);
  if (!iter)
    goto failure;

  entry = fetchdeps_variter_next(iter);
  while (entry.name) {
    char* value;

    printf("%s = ", entry.name);

    value_iter = fetchdeps_stringiter_new(entry.value);
    if (!value_iter)
      goto failure;

    value = fetchdeps_stringiter_next(value_iter);
    if (value) {
      printf("%s", value);
      value = fetchdeps_stringiter_next(value_iter);
      while (value) {
        printf(", %s", value);
        value = fetchdeps_stringiter_next(value_iter);
      }
    }

    fetchdeps_stringiter_free(value_iter);
    value_iter = NULL;

    printf("\n");
    entry = fetchdeps_variter_next(iter);
  }

  fetchdeps_variter_free(iter);
  return;

failure:
  fetchdeps_errors_trap_system_error();
  if (iter)
    fetchdeps_variter_free(iter);
  if (value_iter)
    fetchdeps_stringiter_free(value_iter);
}


bool_t
help_action(cmdline_t* options)
{
  // TODO
  return 1;
}


bool_t
init_action(cmdline_t* options)
{
  char* to_dir = NULL;

  assert(options != NULL);

  // Locate the downloads directory.
  to_dir = fetchdeps_filesys_download_dir(options->fname);
  if (!to_dir)
    goto failure;

  // Try to create the directories.
  if (!options->no_changes) {
    if (!fetchdeps_filesys_init(options->fname))
      goto failure;
  }

  free(to_dir);

  return 1;

failure:
  fetchdeps_errors_trap_system_error();
  if (to_dir)
    free(to_dir);
  return 0;
}


bool_t
get_action(cmdline_t* options)
{
  char* to_dir = NULL;
  parser_t* ctx = NULL;
  stringset_t* urls = NULL;

  assert(options != NULL);

  // Locate the downloads directory.
  to_dir = fetchdeps_filesys_download_dir(options->fname);
  if (!to_dir)
    goto failure;

  // Check that the downloads directory exists.
  if (!fetchdeps_filesys_is_directory(to_dir)) {
    fetchdeps_errors_set_with_msg(ERR_NO_DIR, "Bad download directory (you may need to run 'deps init')");
    goto failure;
  }

  // Set up for parsing.
  ctx = fetchdeps_parser_new(options->fname);
  if (!ctx)
    goto failure;
  if (!fetchdeps_environ_init_all_vars(ctx->vars, options->argv))
    goto failure;
  urls = fetchdeps_stringset_new();
  if (!urls)
    goto failure;

  // Parse away!
  if (!fetchdeps_parser_parse(ctx, urls))
    goto failure;

  // Finished parsing, let's do something with the urls.
  if (options->no_changes)
    print_urls(urls);
  else if (!fetchdeps_download_fetch_all(urls, to_dir))
    goto failure;

  // Cleanup
  if (to_dir)
    free(to_dir);
  fetchdeps_parser_free(ctx);
  fetchdeps_stringset_free(urls);

  return 1;

failure:
  fetchdeps_errors_trap_system_error();
  if (to_dir)
    free(to_dir);
  if (ctx)
    fetchdeps_parser_free(ctx);
  if (urls)
    fetchdeps_stringset_free(urls);
  return 0;
}


bool_t
list_action(cmdline_t* options)
{
  parser_t* ctx = NULL;
  stringset_t* urls = NULL;

  assert(options != NULL);

  // Set up for parsing.
  ctx = fetchdeps_parser_new(options->fname);
  if (!ctx)
    goto failure;
  if (!fetchdeps_environ_init_all_vars(ctx->vars, options->argv))
    goto failure;
  urls = fetchdeps_stringset_new();
  if (!urls)
    goto failure;

  // Parse away!
  if (!fetchdeps_parser_parse(ctx, urls))
    goto failure;

  // Finished parsing, let's do something with the urls.
  print_urls(urls);

  // Cleanup
  fetchdeps_parser_free(ctx);
  fetchdeps_stringset_free(urls);

  return 1;

failure:
  fetchdeps_errors_trap_system_error();
  if (ctx)
    fetchdeps_parser_free(ctx);
  if (urls)
    fetchdeps_stringset_free(urls);
  return 0;
}


bool_t
install_action(cmdline_t* options)
{
  // TODO
  return 0;
}


bool_t
uninstall_action(cmdline_t* options)
{
  // TODO
  return 0;
}


bool_t
delete_action(cmdline_t* options)
{
  // TODO
  return 0;
}


bool_t
vars_action(cmdline_t* options)
{
  varmap_t* vm = NULL;

  assert(options != NULL);

  vm = fetchdeps_varmap_new();
  if (!vm)
    goto failure;

  if (!fetchdeps_environ_init_all_vars(vm, options->argv + 1))
    goto failure;

  print_vars(vm);

  fetchdeps_varmap_free(vm);
  return 1;

failure:
  fetchdeps_errors_trap_system_error();
  if (vm)
    fetchdeps_varmap_free(vm);
  return 0;
}


int
main(int argc, char** argv)
{
  cmdline_t options;
  exittype_t exit_type;
  parser_t* ctx = NULL;
  bool_t success = 0;

  // Parse the command line.
  fetchdeps_cmdline_init(&options, argc, argv);
  exit_type = fetchdeps_cmdline_parse(&options);
  if (exit_type == EXIT_FAIL)
    goto failure;
  if (exit_type == EXIT_OK) {
    fetchdeps_cmdline_cleanup(&options);
    exit(0);
  }

  // If no fname was given try to find the default deps file.
  if (!options.fname)
    options.fname = fetchdeps_filesys_default_deps_file();
  if (!options.fname) {
    fetchdeps_errors_set(ERR_NO_DEPS);
    goto failure;
  }

  switch (options.action) {
  case ACTION_HELP:
    success = help_action(&options);
    break;
  case ACTION_INIT:
    success = init_action(&options);
    break;
  case ACTION_GET:
    success = get_action(&options);
    break;
  case ACTION_LIST:
    success = list_action(&options);
    break;
  case ACTION_INSTALL:
    success = install_action(&options);
    break;
  case ACTION_UNINSTALL:
    success = uninstall_action(&options);
    break;
  case ACTION_DELETE:
    success = delete_action(&options);
    break;
  case ACTION_VARS:
    success = vars_action(&options);
    break;
  default:
    success = 0;
    break;
  }

  if (!success)
    goto failure;

  // Clean up.
  fetchdeps_cmdline_cleanup(&options);
  
  return 0;

failure:
  fetchdeps_errors_print(stderr);
  fetchdeps_cmdline_cleanup(&options);
  if (ctx)
    fetchdeps_parser_free(ctx);
  return 1;
}
