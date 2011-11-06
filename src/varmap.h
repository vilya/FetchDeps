#ifndef fetchdeps_varmap_h
#define fetchdeps_varmap_h

#include "common.h"
#include "stringset.h"

//
// Types
//

struct _varmap;
typedef struct _varmap varmap_t;


//
// Functions
//

// Allocate a new empty varmap. This must eventually be freed with
// fetchdeps_varmap_free.
varmap_t* fetchdeps_varmap_new();

// Deallocate a varmap. This frees all memory for the varmap, including all keys
// and values.
void fetchdeps_varmap_free(varmap_t* vm);

// Set the value associated with a key in the varmap. If the key already exists
// in the map, the existing value is replaced. If the key isn't already in the
// varmap a new entry is added.
//
// Note that the varmap will make a copy of the key, but will take ownership of
// the value without copying it. Once you've set a stringset as the value for a
// key, you should be sure not to free it yourself. You should also avoid
// holding on to any pointers to the stringset beyond this point, as they will
// become invalid when the varmap is freed or the value gets replaced.
//
// The return value is true if, after this, the varmap contains the new value.
// The return value will be false when adding a new value if any of the
// necessary memory allocations failed.
bool_t fetchdeps_varmap_set(varmap_t* vm, char* key, stringset_t* value);

// Set the value for a key in the varmap to a stringset containing a single
// string. This is a convenience function; it's equivalent to creating a
// stringset with fetchdeps_stringset_new_single and passing that to
// fetchdeps_varmap_set.
bool_t fetchdeps_varmap_set_single(varmap_t* vm, char* key, char* value);

// Get the value for a key in the varmap. The return value is NULL if there's no
// such key in the varmap; otherwise it's a pointer to the stringset associated
// with the key.
//
// Note that all pointers returned by this point to the stringsets held by the
// varmap; we don't copy them before returning them. As such, you shouldn't try
// to free them yourself. You should also avoid storing the pointers any longer
// than necessary, as they will become invalid when the varmap is freed or the
// value gets replaced.
stringset_t* fetchdeps_varmap_get(varmap_t* vm, char* key);

// Check whether the varmap contains a particular key. The return value is true
// if the key is in the varmap; false if not.
bool_t fetchdeps_varmap_contains(varmap_t* vm, char* key);

// Add a new string to the stringset associated with a particular key. If the
// key doesn't exist in the varmap, nothing will be modified. If the key does
// exist, we call fetchdeps_stringset_add to add this value to the associated
// stringset.
//
// The return value will be true if the value was successfully added to the
// stringset associated with the provided key. It will be false the key doesn't
// exist in the varmap, or if the fetchdeps_stringset_add call failed (which
// would indicate a failure to allocate any extra memory needed by the
// stringset).
bool_t fetchdeps_varmap_add_value(varmap_t* vm, char* key, char* value);

#endif // fetchdeps_varmap_h

