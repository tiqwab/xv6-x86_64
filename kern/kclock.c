#include "kclock.h"
#include "x86.h"

uint mc146818_read(uint reg) {
  outb(IO_RTC, reg);
  return inb(IO_RTC + 1);
}

void mc146818_write(uint reg, uint datum) {
  outb(IO_RTC, reg);
  outb(IO_RTC + 1, datum);
}
