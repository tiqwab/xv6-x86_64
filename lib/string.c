#include "inc/types.h"
#include "inc/x86.h"

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

void *memcpy(void *dst, const void *src, size_t n) {
  return memmove(dst, src, n);
}

int strncmp(const char *p, const char *q, size_t n) {
  while (n > 0 && *p && *p == *q) {
    n--;
    p++;
    q++;
  }
  if (n == 0) {
    return 0;
  }
  return (uchar)*p - (uchar)*q;
}

char *strncpy(char *s, const char *t, size_t n) {
  char *os;

  os = s;
  while (n-- > 0 && (*s++ = *t++) != 0) {
    ;
  }
  while (n-- > 0) {
    *s++ = 0;
  }
  return os;
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

size_t strlen(const char *s) {
  size_t len = 0;
  while (*s != 0) {
    len++;
    s++;
  }
  return len;
}

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
