#include "stringset.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


//
// Constants
//

static const size_t INITIAL_CAPACITY = 2;


//
// Types
//

// Note: the first element in a stringset is a sentinel with a NULL str.
// The list is circular, so iteration ends when we find ourselves back at the
// sentinel. The sentinel is the only element allowed to have a NULL str.
struct _stringset {
  char** strings;
  size_t size;
  size_t capacity;
};


struct _stringiter {
  struct _stringset* ss;
  size_t index;
};


//
// stringset functions
//

stringset_t*
fetchdeps_stringset_new()
{
  stringset_t* ss = (stringset_t*)malloc(sizeof(stringset_t));
  if (!ss)
    goto failure;

  ss->strings = (char**)calloc(INITIAL_CAPACITY, sizeof(char*));
  if (!ss->strings)
    goto failure;

  ss->size = 0;
  ss->capacity = INITIAL_CAPACITY;

  return ss;

failure:
  if (ss) {
    if (ss->strings)
      free(ss->strings);
    free(ss);
  }
  return NULL;
}


stringset_t*
fetchdeps_stringset_new_single(char* str)
{
  stringset_t* ss = NULL;

  assert(str != NULL);

  ss = fetchdeps_stringset_new();
  if (!ss)
    goto failure;

  if (!fetchdeps_stringset_add(ss, str))
    goto failure;

  return ss;

failure:
  if (ss)
    fetchdeps_stringset_free(ss);
  return NULL;
}


void
fetchdeps_stringset_free(stringset_t* ss)
{
  size_t i;

  assert(ss != NULL);
  assert(ss->strings != NULL);
  assert(ss->size <= ss->capacity);

  for (i = 0; i < ss->size; ++i)
    free(ss->strings[i]);
  free(ss->strings);
  free(ss);
}

#include <stdio.h>
bool_t
fetchdeps_stringset_add(stringset_t* ss, char* str)
{
  size_t i;

  assert(ss != NULL);
  assert(ss->strings != NULL);
  assert(ss->size <= ss->capacity);
  assert(str != NULL);

  fprintf(stderr, "Adding '%s' to stringset 0x%llx\n", str, (long long int)ss);

  for (i = 0; i < ss->size; ++i) {
    if (strcmp(ss->strings[i], str) == 0)
      return 1;
  }

  // If we got here, we're adding a new string.
  if (ss->size == ss->capacity) {
    size_t new_capacity = ss->capacity * 2;

    char** new_strings = (char**)realloc(ss->strings, new_capacity * sizeof(char));
    if (!new_strings)
      goto failure;
    ss->strings = new_strings;
    ss->capacity = new_capacity;
    memset(ss->strings + ss->size, 0, ss->capacity - ss->size);
  }

  ss->strings[ss->size] = strdup(str);
  if (!ss->strings[ss->size])
    goto failure;
  ++ss->size;

  return 1;

failure:
  return 0;
}


bool_t
fetchdeps_stringset_add_all(stringset_t* dst, stringset_t* src)
{
  size_t i;

  assert(dst != NULL);
  assert(dst->strings != NULL);
  assert(dst->size <= dst->capacity);
  assert(src != NULL);
  assert(src->strings != NULL);
  assert(src->size <= src->capacity);

  for (i = 0; i < src->size; ++i) {
    if (!fetchdeps_stringset_add(dst, src->strings[i]))
      goto failure;
  }

  return 1;

failure:
  return 0;
}


bool_t
fetchdeps_stringset_contains(stringset_t* ss, char* str)
{
  size_t i;

  assert(ss != NULL);
  assert(ss->strings != NULL);
  assert(ss->size <= ss->capacity);
  assert(str != NULL);

  for (i = 0; i < ss->size; ++i) {
    if (strcmp(ss->strings[i], str) == 0)
      return 1;
  }
  return 0;
}


bool_t
fetchdeps_stringset_contains_any(stringset_t* haystack,
                                 stringset_t* needles)
{
  size_t i;

  assert(haystack != NULL);
  assert(haystack->strings != NULL);
  assert(haystack->size <= haystack->capacity);
  assert(needles != NULL);
  assert(needles->strings != NULL);
  assert(needles->size <= haystack->capacity);

  for (i = 0; i < needles->size; ++i) {
    if (fetchdeps_stringset_contains(haystack, needles->strings[i]))
      return 1;
  }
  return 0;
}


//
// stringiter_t functions
//

stringiter_t*
fetchdeps_stringiter_new(stringset_t* ss)
{
  stringiter_t* iter = NULL;

  assert(ss != NULL);
  assert(ss->strings != NULL);
  assert(ss->size <= ss->capacity);

  iter = (stringiter_t*)malloc(sizeof(stringiter_t));
  if (!iter)
    return NULL;

  iter->ss = ss;
  iter->index = 0;

  return iter;
}


void
fetchdeps_stringiter_free(stringiter_t* iter)
{
  assert(iter != NULL);

  free(iter);
}


char*
fetchdeps_stringiter_next(stringiter_t* iter)
{
  char* result;

  assert(iter != NULL);
  assert(iter->ss != NULL);

  if (iter->index >= iter->ss->size)
    return NULL;

  result = iter->ss->strings[iter->index];
  ++iter->index;

  return result;
}

