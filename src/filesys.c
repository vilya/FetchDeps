#include "filesys.h"

#include "common.h"
#include "errors.h"

#include <assert.h>
#include <dirent.h> // For opendir() and closedir().
#include <libgen.h> // For the dirname() function. TODO: check if this is the right include for Mac as well.
#include <limits.h> // For PATH_MAX
#include <stdio.h>  // For snprintf(), fopen(), etc.
#include <stdlib.h> // For realpath(), malloc(), etc.
#include <string.h> // For strcmp().
#include <sys/stat.h> // for mkdir()
#include <unistd.h> // for getcwd().


//
// Constants
//

static const char* DEFAULT_DEPSFILE = "default.deps";
static const char* DEPS_DIR = ".deps";
static const char* DOWNLOADS_DIR = "downloads";
static const char* DOWNLOADS_LIST = "urls.txt";
static const char* ROOT_PATH = "/";


//
// Forward declarations
//

// Given the path to a directory, check whether it contains a default.deps
// file and, if so, whether it's readable by the current user. Returns true
// if those conditions are met, false otherwise.
bool_t fetchdeps_filesys_depsfile_exists(const char* dirpath);

// Combine a directory path with a filename to make a new path string. The
// result will be a null-terminated string, or NULL if the function failed. It's
// up to the caller to free() the returned string.
char* fetchdeps_filesys_make_filepath(const char* dirpath, const char* filename);


//
// Public functions
//

bool_t
fetchdeps_filesys_init(char* deps_file)
{
  char* file_path = NULL;
  char* parent_path = NULL;
  char* dir_path = NULL;
  char* download_path = NULL;

  file_path = realpath(deps_file, NULL);
  if (!file_path)
    goto failure;

  parent_path = dirname(file_path);
  if (!parent_path)
    goto failure;
  
  dir_path = fetchdeps_filesys_make_filepath(parent_path, DEPS_DIR);
  if (!dir_path)
    goto failure;

  download_path = fetchdeps_filesys_make_filepath(dir_path, DOWNLOADS_DIR);
  if (!download_path)
    goto failure;

  if (fetchdeps_filesys_is_directory(download_path))
    goto failure;

  if (!fetchdeps_filesys_make_directory(dir_path))
    goto failure;

  if (!fetchdeps_filesys_make_directory(download_path))
    goto failure;

  free(file_path);
  free(dir_path);
  free(download_path);

  return 1;

failure:
  fetchdeps_errors_trap_system_error();
  if (file_path)
    free(file_path);
  if (dir_path)
    free(dir_path);
  if (download_path)
    free(download_path);
  return 0;
}


bool_t
fetchdeps_filesys_is_directory(char* path)
{
  DIR* dir = NULL;

  assert(path != NULL);

  dir = opendir(path);
  if (!dir)
    return 0;

  closedir(dir);
  return 1;
}


char*
fetchdeps_filesys_default_deps_file()
{
  char path[PATH_MAX];
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

  result = fetchdeps_filesys_make_filepath(dirpath, DEFAULT_DEPSFILE);
  if (!result)
    goto failure;

  return result;

failure:
  fetchdeps_errors_trap_system_error();
  if (result)
    free(result);
  // Note: we don't free dirpath because dirname docs say not to.
  return NULL;
}


char*
fetchdeps_filesys_download_dir(char* deps_file)
{
  char* file_path = NULL;
  char* parent_path = NULL;
  char* dir_path = NULL;
  char* download_path = NULL;

  file_path = realpath(deps_file, NULL);
  if (!file_path)
    goto failure;

  parent_path = dirname(file_path);
  if (!parent_path)
    goto failure;
  
  dir_path = fetchdeps_filesys_make_filepath(parent_path, DEPS_DIR);
  if (!dir_path)
    goto failure;

  download_path = fetchdeps_filesys_make_filepath(dir_path, DOWNLOADS_DIR);
  if (!download_path)
    goto failure;

  free(file_path);
  free(dir_path);

  return download_path;

failure:
  fetchdeps_errors_trap_system_error();
  if (file_path)
    free(file_path);
  if (dir_path)
    free(dir_path);
  if (download_path)
    free(download_path);
  return NULL;
}


bool_t
fetchdeps_filesys_make_directory(char* path)
{
  // Create a directory with read, write and execute permission for the current
  // user only.
  if (mkdir(path, 0700) == 0)
    return 1;

  fetchdeps_errors_trap_system_error();
  return 0;
}


//
// Private functions
//

bool_t
fetchdeps_filesys_depsfile_exists(const char* dirpath)
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


char*
fetchdeps_filesys_make_filepath(const char* dirpath, const char* filename)
{
  char* filepath = NULL;
  int filepath_len;

  assert(dirpath != NULL);
  assert(filename != NULL);

  filepath_len = strlen(dirpath) + strlen(filename) + 2;
  filepath = malloc(filepath_len * sizeof(char));
  if (!filepath)
    goto failure;

  if (snprintf(filepath, filepath_len, "%s/%s", dirpath, filename) != filepath_len - 1)
    goto failure;

  return filepath;

failure:
  fetchdeps_errors_trap_system_error();
  if (filepath)
    free(filepath);
  return NULL;
}

