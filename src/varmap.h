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
void          fetchdeps_varmap_free(varmap_t* vm);

bool_t        fetchdeps_varmap_set(varmap_t* vm, char* key, stringset_t* value);
bool_t        fetchdeps_varmap_set_single(varmap_t* vm, char* key, char* value);
stringset_t*  fetchdeps_varmap_get(varmap_t* vm, char* key);
bool_t        fetchdeps_varmap_contains(varmap_t* vm, char* key);

bool_t        fetchdeps_varmap_add_value(varmap_t* vm, char* key, char* value);

#endif // fetchdeps_varmap_h

