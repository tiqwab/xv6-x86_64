// TODO: This file should be moved from kern

#include "types.h"
#include "x86.h"

void *memset(void *dst, int c, size_t n) {
  stosb(dst, c, n);
  return dst;
}
