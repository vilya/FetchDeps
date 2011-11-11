#ifndef fetchdeps_errors_h
#define fetchdeps_errors_h

#include <stdio.h>

//
// Types
//

enum _error {
  ERR_NONE,       // Not an error
  ERR_SYSTEM,     // A system error, with the system error code stored in errno
  ERR_PARSE,      // A parsing error while reading the deps file.
  ERR_CMDLINE,    // An unknown option on the command line.
  ERR_NO_DEPS,    // No deps file specified and couldn't find default deps file.
  ERR_NO_DIR,     // No working directory could be found.
};
typedef enum _error error_t;


//
// Functions
//

void fetchdeps_errors_set(error_t err);
void fetchdeps_errors_set_with_msg(error_t err, char* format, ...);
void fetchdeps_errors_trap_system_error();
void fetchdeps_errors_clear();

error_t fetchdeps_errors_get();
char* fetchdeps_errors_get_msg();
void fetchdeps_errors_print(FILE* out);


#endif // fetchdeps_errors_h

