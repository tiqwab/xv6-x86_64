#include "user.h"

static void printchar(int fd, char c, int *print_cnt) {
  write(fd, &c, 1);
  (*print_cnt)++;
}

static void printint(int fd, long xx, int base, int sign, int *print_cnt) {
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  unsigned long x;

  neg = 0;
  if (sign && xx < 0) {
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);
  if (neg)
    buf[i++] = '-';

  while (--i >= 0) {
    printchar(fd, buf[i], print_cnt);
  }
}

int vdprintf(int fd, const char *fmt, va_list va) {
  char *s;
  int c, i, state;
  int print_cnt = 0;

  char *val_s;
  int val_d;
  char val_c;

  state = 0;
  for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (state == 0) {
      if (c == '%') {
        state = '%';
      } else {
        printchar(fd, c, &print_cnt);
      }
    } else if (state == '%') {
      if (c == 'd') {
        val_d = va_arg(va, int);
        printint(fd, val_d, 10, 1, &print_cnt);
      } else if (c == 'x' || c == 'p') {
        // FIXME: handle 64 bit value
        val_d = va_arg(va, int);
        printint(fd, val_d, 16, 0, &print_cnt);
      } else if (c == 's') {
        val_s = va_arg(va, char *);
        if (val_s == 0) {
          val_s = "(null)";
        }
        while (*val_s != 0) {
          printchar(fd, *val_s, &print_cnt);
          val_s++;
        }
      } else if (c == 'c') {
        val_c = (char)va_arg(va, int);
        printchar(fd, val_c, &print_cnt);
      } else if (c == '%') {
        printchar(fd, c, &print_cnt);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        printchar(fd, '%', &print_cnt);
        printchar(fd, c, &print_cnt);
      }
      state = 0;
    }
  }

  return print_cnt;
}

// Print to the stdout. Only understands %c, %d, %x, %p, %s.
int printf(const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int res = vdprintf(0, fmt, va);
  va_end(va);
  return res;
}

// Print to the given fd. Only understands %c, %d, %x, %p, %s.
int dprintf(int fd, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int res = vdprintf(fd, fmt, va);
  va_end(va);
  return res;
}
