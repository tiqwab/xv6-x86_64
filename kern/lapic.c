#include "x86.h"

volatile uint32_t *lapic; // Initialized in mp.c

// Stupid I/O delay routine necessitated by historical PC design flaws
void microdelay(int us) {
  inb(0x84);
  inb(0x84);
  inb(0x84);
  inb(0x84);
}
