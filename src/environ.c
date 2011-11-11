#include "environ.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h> // for uname()


//
// Global variables
//

// Provided by the C library, each entry is a string of the form NAME=VALUE.
extern char** environ;


//
// Forward declarations
//

// Determine the bit size of the current operating system. This correctly
// handles the case where this program has been compiled as a 32-bit app and is
// running on a 64-bit OS, for example. The return value will be a number like
// 32 or 64.
unsigned int fetchdeps_environ_bitsize();


//
// Public functions
//

bool_t
fetchdeps_environ_default_vars(varmap_t* vm)
{
  char bits[10];

  assert(vm != NULL);

  if (!fetchdeps_varmap_set_single(vm, "os", kOperatingSystem))
    return 0;

  snprintf(bits, 10, "%u", fetchdeps_environ_bitsize());
  if (!fetchdeps_varmap_set_single(vm, "bits", bits))
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


//
// Private functions
//

unsigned int
fetchdeps_environ_bitsize()
{
  struct utsname buf;
  unsigned int bits;

  // First, try to parse the bit size from the result of a uname() call. This
  // should correctly handle a 32-bit program running on a 64-bit OS.
  if (uname(&buf) == 0) {
    if (strncmp("x86_", buf.machine, 4) == 0)
      bits = atoi(buf.machine + 4);
    else if (strcmp("x86", buf.machine) == 0)
      bits = 32; 
    return bits;
  }

  // If that doesn't work, use the bit width of a pointer. This will incorrectly
  // return 32 if we're a 32-bit program running on a 64-bit OS, but it's the
  // best we can do at this point.
  return sizeof(void*) * 8;
}

