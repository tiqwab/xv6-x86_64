#include "user.h"

char *strchr(const char *s, int c) {
  const char *p = s;
  while (*p != 0) {
    if (*p == (char)c) {
      return (char *)p;
    }
    p++;
  }
  return NULL;
}
