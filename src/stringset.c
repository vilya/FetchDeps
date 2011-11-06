#include "stringset.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

//
// Types
//

// Note: the first element in a stringset is a sentinel with a NULL str.
// The list is circular, so iteration ends when we find ourselves back at the
// sentinel. The sentinel is the only element allowed to have a NULL str.
struct _stringset {
  char* str;
  struct _stringset* next;
};


struct _stringiter {
  struct _stringset* pos;
};


//
// stringset functions
//

stringset_t*
fetchdeps_stringset_new()
{
  stringset_t* ss = (stringset_t*)malloc(sizeof(stringset_t));
  if (!ss)
    return NULL;

  ss->str = NULL;
  ss->next = ss;
  return ss;
}


stringset_t*
fetchdeps_stringset_new_single(char* str)
{
  stringset_t* ss = (stringset_t*)malloc(sizeof(stringset_t));
  if (!ss)
    goto failure;

  ss->str = NULL;
  ss->next = ss;

  if (!fetchdeps_stringset_add(ss, str))
    goto failure;

  return ss;

failure:
  if (ss)
    free(ss);
  return NULL;
}


void
fetchdeps_stringset_free(stringset_t* ss)
{
  stringset_t* first = ss;
  stringset_t* next;

  assert(ss != NULL);
  assert(ss->str == NULL);

  do {
    if (ss->str)
      free(ss->str);
    next = ss->next;
    free(ss);
    ss = next;
  } while (ss != first);
}


bool_t
fetchdeps_stringset_add(stringset_t* ss, char* str)
{
  stringset_t* prev;
  stringset_t* curr;

  assert(ss != NULL);
  assert(ss->str == NULL);
  assert(str != NULL);

  prev = ss;
  curr = ss->next;
  while (curr != ss && strcmp(curr->str, str) != 0) {
    prev = curr;
    curr = curr->next;
  }
  if (curr != ss)
    return 1;

  // If we got here, we're adding a new string.
  curr = (stringset_t*)malloc(sizeof(stringset_t));
  if (!curr)
    goto failure;

  curr->str = strdup(str);
  if (!curr->str)
    goto failure;
  
  curr->next = ss;
  prev->next = curr;

  return 1;

failure:
  if (curr) {
    if (curr->str)
      free(curr->str);
    free(curr);
  }
  return 0;
}


bool_t
fetchdeps_stringset_add_all(stringset_t* dst, stringset_t* src)
{
  stringset_t* src_pos;

  assert(dst != NULL);
  assert(dst->str == NULL);
  assert(src != NULL);
  assert(src->str == NULL);

  src_pos = src->next;
  while (src_pos != src) {
    fetchdeps_stringset_add(dst, src_pos->str);
    src_pos = src_pos->next;
  }
}


bool_t
fetchdeps_stringset_contains(stringset_t* ss, char* str)
{
  stringset_t* pos;

  assert(ss != NULL);
  assert(ss->str == NULL);
  assert(str != NULL);

  pos = ss->next;
  while (pos != ss && strcmp(pos->str, str) != 0)
    pos = pos->next;
  return (pos != ss);
}


bool_t
fetchdeps_stringset_contains_any(stringset_t* haystack,
                                 stringset_t* needles)
{
  stringset_t* needle;

  assert(haystack != NULL);
  assert(haystack->str == NULL);
  assert(needles != NULL);
  assert(needles->str == NULL);

  needle = needles->next;
  while (needle != needles) {
    if (fetchdeps_stringset_contains(haystack, needle->str))
      return 1;
    needle = needle->next;
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

  iter = (stringiter_t*)malloc(sizeof(stringiter_t));
  if (!iter)
    return NULL;

  iter->pos = (ss->str) ? ss : ss->next;
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

  result = iter->pos->str;
  if (result)
    iter->pos = iter->pos->next;
  return result;
}

