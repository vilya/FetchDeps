#include "varmap.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

//
// Constants
//

static const size_t kInitialCapacity = 10;


//
// Types
//

struct _varmap {
  char** keys;
  stringset_t** values;
  size_t size;
  size_t capacity;
};


struct _variter {
  varmap_t* vm;
  size_t index;
};


//
// varmap_t functions
//

varmap_t*
fetchdeps_varmap_new()
{
  varmap_t* vm = (varmap_t*)malloc(sizeof(varmap_t));
  if (!vm)
    return NULL;

  vm->keys = NULL;
  vm->values = NULL;
  vm->size = 0;
  vm->capacity = kInitialCapacity;

  size_t keyBytes = sizeof(char*) * kInitialCapacity;
  size_t valueBytes = sizeof(stringset_t*) * kInitialCapacity;

  vm->keys = (char**)malloc(keyBytes);
  if (!vm->keys)
    goto failure;

  vm->values = (stringset_t**)malloc(valueBytes);
  if (!vm->values)
    goto failure;

  memset(vm->keys, 0, keyBytes);
  memset(vm->values, 0, valueBytes);

  return vm;

failure:
  if (vm) {
    if (vm->keys)
      free(vm->keys);
    if (vm->values)
      free(vm->values);
    free(vm);
  }
  return NULL;
}


void
fetchdeps_varmap_free(varmap_t* vm)
{
  size_t i;

  assert(vm != NULL);
  assert(vm->size <= vm->capacity);
  assert(vm->capacity > 0);
  assert(vm->keys != NULL);
  assert(vm->values != NULL);

  for (i = 0; i < vm->size; ++i) {
    free(vm->keys[i]);
    fetchdeps_stringset_free(vm->values[i]);
  }
  if (vm->keys)
    free(vm->keys);
  if (vm->values)
    free(vm->values);

  free(vm);
}


bool_t
fetchdeps_varmap_set(varmap_t* vm, char* key, stringset_t* value)
{
  size_t i;

  assert(vm != NULL);
  assert(vm->size <= vm->capacity);
  assert(vm->capacity > 0);
  assert(vm->keys != NULL);
  assert(vm->values != NULL);
  assert(key != NULL);
  assert(value != NULL);

  // See if we're replacing an existing value.
  for (i = 0; i < vm->size; ++i) {
    if (strcmp(vm->keys[i], key) == 0) {
      if (vm->values[i])
        fetchdeps_stringset_free(vm->values[i]);
      vm->values[i] = value;
      return 1;
    }
  }

  // If we get here, it's a new key. Make sure we've got room for it.
  if (vm->size == vm->capacity) {
    size_t new_capacity = vm->capacity * 2;
    
    char** new_keys = realloc(vm->keys, new_capacity * sizeof(char*));
    if (!new_keys)
      return 0;
    vm->keys = new_keys;

    stringset_t** new_values = realloc(vm->values, new_capacity * sizeof(stringset_t*));
    if (!new_values)
      return 0;
    vm->values = new_values;

    vm->capacity = new_capacity;
  }

  // Add the new key.
  vm->keys[vm->size] = strdup(key);
  if (!vm->keys[vm->size])
    return 0;
  vm->values[vm->size] = value;
  ++vm->size;

  return 1;
}


bool_t
fetchdeps_varmap_set_single(varmap_t* vm, char* key, char* value)
{
  stringset_t* ss;

  assert(vm != NULL);
  assert(vm->size <= vm->capacity);
  assert(vm->capacity > 0);
  assert(vm->keys != NULL);
  assert(vm->values != NULL);
  assert(key != NULL);
  assert(value != NULL);

  ss = fetchdeps_stringset_new_single(value);
  if (!ss)
    return 0;

  return fetchdeps_varmap_set(vm, key, ss);
}


stringset_t*
fetchdeps_varmap_get(varmap_t* vm, char* key)
{
  size_t i;

  assert(vm != NULL);
  assert(vm->size <= vm->capacity);
  assert(vm->capacity > 0);
  assert(vm->keys != NULL);
  assert(vm->values != NULL);
  assert(key != NULL);

  // See if we're replacing an existing value.
  for (i = 0; i < vm->size; ++i) {
    if (strcmp(vm->keys[i], key) == 0)
      return vm->values[i];
  }

  return NULL;
}


bool_t
fetchdeps_varmap_contains(varmap_t* vm, char* key)
{
  size_t i;

  assert(vm != NULL);
  assert(vm->size <= vm->capacity);
  assert(vm->capacity > 0);
  assert(vm->keys != NULL);
  assert(vm->values != NULL);
  assert(key != NULL);

  // See if we're replacing an existing value.
  for (i = 0; i < vm->size; ++i) {
    if (strcmp(vm->keys[i], key) == 0)
      return 1;
  }

  return 0;
}


bool_t
fetchdeps_varmap_add_value(varmap_t* vm, char* key, char* value)
{
  size_t i;
  stringset_t* ss;

  assert(vm != NULL);
  assert(vm->size <= vm->capacity);
  assert(vm->capacity > 0);
  assert(vm->keys != NULL);
  assert(vm->values != NULL);
  assert(key != NULL);

  // Find the variable we'll be adding to.
  for (i = 0; i < vm->size; ++i) {
    if (strcmp(vm->keys[i], key) == 0)
      break;
  }
  if (i == vm->size)
    return 0;

  ss = vm->values[i];
  assert(ss != NULL);

  return fetchdeps_stringset_add(ss, value);
}


//
// Iterator functions
//

variter_t*
fetchdeps_variter_new(varmap_t* vm)
{
  variter_t* iter = NULL;

  assert(vm != NULL);
  assert(vm->size <= vm->capacity);
  assert(vm->capacity > 0);
  assert(vm->keys != NULL);
  assert(vm->values != NULL);

  iter = (variter_t*)malloc(sizeof(variter_t));
  if (!iter)
    return NULL;

  iter->vm = vm;
  iter->index = 0;

  return iter;
}


void
fetchdeps_variter_free(variter_t* iter)
{
  assert(iter != NULL);

  free(iter);
}


varentry_t
fetchdeps_variter_next(variter_t* iter)
{
  varentry_t result = { NULL, 0 };

  assert(iter != NULL);
  assert(iter->vm != NULL);

  if (iter->index >= iter->vm->size)
    return result;

  result.name = iter->vm->keys[iter->index];
  result.value = iter->vm->values[iter->index];
  ++iter->index;

  return result;
}

