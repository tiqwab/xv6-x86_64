#include "defs.h"
#include "memlayout.h"
#include "mmu.h"

extern char end[]; // first address after kernel loaded from ELF file

char *foo;

// Bootstrap processor starts running C code here.
// Allocate a real stack and switch to it, first
// doing some setup required for memory allocator to work.
int main(void) {
  kinit1(end, P2V(4 * 1024 * 1024)); // phys page allocator
  for (;;) {
  }
}
