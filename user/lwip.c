// required by lwip

#include "user.h"

void cprintf(char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int res = vdprintf(0, fmt, va);
  va_end(va);
}

void __attribute__((noreturn)) panic(char *s, ...) {
  dprintf(2, "%s\n", s);
  exit();

  // for noreturn
  while (1) {
  }
}
