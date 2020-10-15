#include "user.h"

char *gets(char *buf, int max) {
  int i = 0, n;
  char c;

  for (i = 0; i + 1 < max;) {
    n = read(0, &c, 1);
    if (n < 1) {
      break;
    }
    buf[i++] = c;
    if (c == '\n' || c == '\r') {
      break;
    }
  }
  buf[i] = '\0';
  return buf;
}
