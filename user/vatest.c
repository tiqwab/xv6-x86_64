#include "user.h"

void f(char *fmt, ...) {
  va_list va;
  va_start(va, fmt);

  int a = va_arg(va, int);
  printf("1st: %d\n", a);
  char *b = va_arg(va, char *);
  printf("2nd: %s\n", b);
  int c = va_arg(va, int);
  printf("3rd: %d\n", c);

  printf(fmt, a, b, c);

  va_end(va);
}

int main(int argc, char *argv[]) { f("%d %s %d\n", 1, "hello", 3); }
