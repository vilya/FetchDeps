#include "environ.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


//
// Global variables
//

// Provided by the C library, each entry is a string of the form NAME=VALUE.
extern char** environ;


//
// Public functions
//

bool_t
fetchdeps_environ_default_vars(varmap_t* vm)
{
  assert(vm != NULL);

  if (!fetchdeps_varmap_set_single(vm, "os", kOperatingSystem))
    return 0;
  // TODO: set the bit size correctly for the current platform.
  if (!fetchdeps_varmap_set_single(vm, "bits", "64"))
    return 0;

  return 1;
}


bool_t
fetchdeps_environ_parse_vars(varmap_t* vm, char** var_strings)
{
  int i;

  assert(vm != NULL);

  for (i = 0; var_strings[i] != NULL; ++i) {
    bool_t ok;
    char* var = NULL;
    char* value = NULL;

    var = strdup(var_strings[i]);
    if (!var)
      goto failure;

    value = var;
    while (*value && *value != '=')
      ++value;

    if (*value == '=') {
      *value = '\0';
      ++value;
    }

    ok = fetchdeps_varmap_set_single(vm, var, value);
    free(var);
    if (!ok)
      goto failure;
  }

  return 1;

failure:
  return 0;
}


bool_t
fetchdeps_environ_get_vars(varmap_t* vm)
{
  return fetchdeps_environ_parse_vars(vm, environ);
}

