#include "filesys.h"

#include "common.h"

#include <libgen.h> // For the dirname() function. TODO: check if this is the right include for Mac as well.
#include <limits.h> // For PATH_MAX
#include <stdio.h>  // For snprintf(), fopen(), etc.
#include <stdlib.h> // For realpath(), malloc(), etc.
#include <string.h> // For strcmp().


//
// Constants
//

static const char* DEFAULT_DEPSFILE = "default.deps";
static const char* ROOT_PATH = "/";


//
// Forward declarations
//

bool_t fetchdeps_filesys_depsfile_exists(char* dirpath);


//
// Functions
//

char*
fetchdeps_filesys_default_depsfile()
{
  char path[PATH_MAX];
  int path_len;
  char* dirpath = NULL;
  char* result = NULL;

  if (!getcwd(path, PATH_MAX))
    goto failure;

  dirpath = path;
  while (!fetchdeps_filesys_depsfile_exists(dirpath)) {
    if (strcmp(dirpath, ROOT_PATH) == 0)
      goto failure;
    dirpath = dirname(dirpath);
  }

  if (dirpath != path) {
    strncpy(path, dirpath, PATH_MAX);
    path[PATH_MAX - 1] = '\0'; // Just in case.
  }

  path_len = strlen(path);
  if (snprintf(path + path_len, PATH_MAX - path_len, "/%s", DEFAULT_DEPSFILE) != strlen(DEFAULT_DEPSFILE) + 1)
    goto failure;

  result = strdup(path);
  if (!result)
    goto failure;

  return result;

failure:
  if (result)
    free(result);
  // Note: we don't free dirpath because dirname docs say not to.
  return NULL;
}


//
// Private functions
//

bool_t fetchdeps_filesys_depsfile_exists(char* dirpath)
{
  char filepath[PATH_MAX + 1];
  FILE* f = NULL;

  if (snprintf(filepath, PATH_MAX, "%s/%s", dirpath, DEFAULT_DEPSFILE) > PATH_MAX)
    return 0;
  
  f = fopen(filepath, "r");
  if (!f)
    return 0;

  fclose(f);
  return 1;
}

