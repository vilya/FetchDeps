#ifndef fetchdeps_stringset_h
#define fetchdeps_stringset_h

#include "common.h"


//
// Types
//

struct _stringset;
typedef struct _stringset stringset_t;

struct _stringiter;
typedef struct _stringiter stringiter_t;


//
// Functions
//

// Allocate a new empty stringset. This must eventually be freed with
// fetchdeps_stringset_free.
stringset_t* fetchdeps_stringset_new();

// Allocate a new stringset populated with a single string. The set will take
// its own copy of the string. The set must eventually be freed via
// fetchdeps_stringset_free, which will also free up the copy of the string.
stringset_t* fetchdeps_stringset_new_single(char* str);

// Deallocate a string set. This frees the memory for the stringset and for all
// of the strings contained in it.
void fetchdeps_stringset_free(stringset_t* ss);

// Add a string to the stringset. If the string already exists in the set,
// nothing will change. If it's a new value, the set will store a copy of the
// string.
//
// The function returns true if, on completion, the set contains the string;
// it doesn't matter whether the string was there already or it was added by
// this call. The function returns false if it was unable to add the string to
// the set because it couldn't allocate enough memory.
bool_t fetchdeps_stringset_add(stringset_t* ss, char* str);

// Add all strings from the src stringset to the dst stringset. This has the
// semantics of a set union operation and is equivalent to calling
// fetchdeps_stringset_add once for every string in the src stringset. After
// this call the dst stringset will have copied any new strings from the src.
// The src is not altered in any way.
//
// The function returns true if the dst stringset contains all of the strings
// in src on completion. It doesn't matter whether these were added by this
// call, or were part of the set already. The function returns false if it was
// not possible to add one or more of the src strings due to failed memory
// allocations.
//
// Note that this function isn't atomic. If it fails, it's likely that some but
// all of the strings from src have been added to dst.
bool_t fetchdeps_stringset_add_all(stringset_t* dst, stringset_t* src);

// Check whether a string is in a set. The return value is true if the string
// was found, false if it isn't. Both parameters must be non-NULL.
bool_t fetchdeps_stringset_contains(stringset_t* ss, char* str);

// Check whether any of the strings in the 'needles' set are also in the
// 'haystack' set. This is checking whether the intersection of the two sets is
// non-empty & the return value indicates this. Both sets must be non-NULL.
bool_t fetchdeps_stringset_contains_any(stringset_t* haystack, stringset_t* needles);


//
// Iterator functions
//

// Allocate a new iterator over a stringset. Changes to the stringset during
// iteration will invalidate the iterator. The iterator must be freed
// eventually using fetchdeps_stringiter_free.
stringiter_t* fetchdeps_stringiter_new(stringset_t* ss);

// Deallocate a stringset iterator.
void fetchdeps_stringiter_free(stringiter_t* ss);

// Return the next string from the set. Call this repeatedly to iterate over
// the entire set. The return value will be NULL when we reach the end of the
// set; the rest of the time it is the next string.
//
// Note that the pointers returned by iteration are pointers to the strings
// held by the set; we don't copy them before returning them. As such, you
// shouldn't try to free them yourself; nor should you store them beyond
// the duration of the iteration.
char* fetchdeps_stringiter_next(stringiter_t* iter);

#endif // fetchdeps_stringset_h

