#include "errors.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>


//
// Constants
//

#define MAX_ERRMSG_LENGTH 1024

// The error messages that correspond to the error codes defined in the error_t
// enum. These must be in the same order as the enum values because we do
// direct lookups in this array using the enum values.
//
// The strings here will be used as part of a longer message when printed out,
// so they should begin with a lower case letter and should not end with any
// punctuation.
static const char* ERRORS[] = {
  "no error",
  "system error"
};


//
//
// Global variables
//

error_t fetchdeps_error = ERR_NONE;
char fetchdeps_errmsg[MAX_ERRMSG_LENGTH];


//
// Functions
//

void
fetchdeps_errors_set(error_t err)
{
  fetchdeps_error = err;
  memset(fetchdeps_errmsg, 0, sizeof(fetchdeps_errmsg));
}


void
fetchdeps_errors_set_with_msg(error_t err, char* format, ...)
{
  va_list args;

  assert(format != NULL);

  fetchdeps_error = err;

  va_start(args, format);
  vsnprintf(fetchdeps_errmsg, MAX_ERRMSG_LENGTH, format, args);
  va_end(args);
}


void
fetchdeps_errors_trap_system_error()
{
  if (fetchdeps_error == ERR_NONE && errno != 0) {
    fetchdeps_error = ERR_SYSTEM;
    memset(fetchdeps_errmsg, 0, sizeof(fetchdeps_errmsg));
  }
}


void
fetchdeps_errors_clear()
{
  fetchdeps_error = ERR_NONE;
  memset(fetchdeps_errmsg, 0, sizeof(fetchdeps_errmsg));
}


error_t
fetchdeps_errors_get()
{
  return fetchdeps_error;
}


char*
fetchdeps_errors_get_msg()
{
  return fetchdeps_errmsg;
}


void
fetchdeps_errors_print(FILE* out)
{
  // Don't print anything when there's no error.
  if (fetchdeps_error == ERR_NONE)
    return;

  char* prefix = (fetchdeps_errmsg[0] != '\0') ? fetchdeps_errmsg : "Error";
  if (fetchdeps_error == ERR_SYSTEM)
    fprintf(out, "%s: %s\n", prefix, strerror(errno));
  else
    fprintf(out, "%s: %s\n", prefix, ERRORS[fetchdeps_error]);
}

