#include "user.h"

void *memset(void *s, int c, size_t n) {
  char *t = (char *)s;
  for (size_t i = 0; i < n; i++) {
    *t = (char)c;
    t++;
  }
  return s;
}
