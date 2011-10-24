#include "common.h"

int CopyStr(char* to, const char* from, int numChars)
{
  int i;
  for (i = 0; i < numChars && from[i] != '\0'; ++i) {
    to[i] = from[i];
  }
  to[i] = '\0';
  return i;
}


