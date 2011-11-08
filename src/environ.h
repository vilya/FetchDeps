#ifndef fetchdeps_environ_h
#define fetchdeps_environ_h

#include "common.h"
#include "varmap.h"

//
// Functions
//

// Store the default variables into the provided varmap. If the varmap already
// contains entries with the same name as any of the default variables, they
// will be replaced; other entries will be unaffected.
bool_t fetchdeps_environ_default_vars(varmap_t* vm);

// Given an array of string of the form "name=value", add corresponding entries
// to the varmap. The last entry in the var_strings array must be NULL. The
// return value is true if all variables were added successfully to the map. If
// anything went wrong - a memory allocation failed, for example - the return
// value will be false.
//
// If the varmap contains any variables with the same name as a variable in the
// array, the existing value in the varmap will be replaced.
//
// Note that this function is not atomic. If it returns false, some variables
// may have already been added to the map.
bool_t fetchdeps_environ_parse_vars(varmap_t* vm, char** var_strings);

// Fill the varmap with a mapping for each environment variable. The varmap
// must not be null. The return value will be true if all environment variables
// were added to the map; it will be false if anything went wrong, such as an
// out of memory error while allocating space for the new entries.
//
// If the varmap contains any variables with the same name as an environment
// variable, the existing value will be overwritten by this function.
//
// Note that this function is not atomic. If it returns false, some variables
// may have already been added to the map.
bool_t fetchdeps_environ_get_vars(varmap_t* vm);

#endif // fetchdeps_environ_h

