// TODO: This file should be moved from kern

#include "types.h"
#include "x86.h"

int memcmp(const void *v1, const void *v2, size_t n) {
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  while (n-- > 0) {
    if (*s1 != *s2)
      return *s1 - *s2;
    s1++, s2++;
  }

  return 0;
}

void *memset(void *dst, int c, size_t n) {
  stosb(dst, c, n);
  return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
  const char *s;
  char *d;

  s = src;
  d = dst;
  if (s < d && s + n > d) {
    s += n;
    d += n;
    while (n-- > 0)
      *--d = *--s;
  } else
    while (n-- > 0)
      *d++ = *s++;

  return dst;
}

// Like strncpy but guaranteed to NUL-terminate.
char *safestrcpy(char *s, const char *t, int n) {
  char *os;

  os = s;
  if (n <= 0)
    return os;
  while (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  return os;
}
