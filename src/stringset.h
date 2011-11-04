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

stringset_t*  fetchdeps_stringset_new();
void          fetchdeps_stringset_free(stringset_t* ss);

void          fetchdeps_stringset_add(stringset_t* ss, char* str);
void          fetchdeps_stringset_add_all(stringset_t* dst, stringset_t* src);

bool_t        fetchdeps_stringset_contains(stringset_t* ss, char* str);
bool_t        fetchdeps_stringset_contains_any(stringset_t* haystack, stringset_t* needles);


//
// Iterator functions
//

stringiter_t  fetchdeps_stringiter_new(stringset_t* ss);
char*         fetchdeps_stringiter_next(stringiter_t* iter);

#endif // fetchdeps_stringset_h

