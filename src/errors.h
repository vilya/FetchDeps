#ifndef fetchdeps_errors_h
#define fetchdeps_errors_h

#include <stdio.h>

//
// Types
//

enum _error {
  ERR_NONE,       // Not an error.
  ERR_NO_MEMORY,  // Unable to allocate memory.
};
typedef enum _error error_t;


//
// Functions
//

void fetchdeps_errors_set(error_t err);
void fetchdeps_errors_set_with_msg(error_t err, char* format, ...);
void fetchdeps_errors_clear();

error_t fetchdeps_errors_get();
char* fetchdeps_errors_get_msg();
void fetchdeps_errors_print(FILE* out);


#endif // fetchdeps_errors_h

