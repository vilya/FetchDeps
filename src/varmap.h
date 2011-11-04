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

varmap_t*     fetchdeps_varmap_new();
void          fetchdeps_varmap_free(varmap_t* sm);

bool_t        fetchdeps_varmap_set(varmap_t* sm, char* key, stringset_t* value);
stringset_t*  fetchdeps_varmap_get(varmap_t* sm, char* key);
bool_t        fetchdeps_varmap_contains(varmap_t* sm, char* key);

#endif // fetchdeps_varmap_h

