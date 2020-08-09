#include "defs.h"
#include "memlayout.h"
#include "mmu.h"

extern char end[]; // first address after kernel loaded from ELF file

// Bootstrap processor starts running C code here.
// Allocate a real stack and switch to it, first
// doing some setup required for memory allocator to work.
int main(void) {
  kinit1(end, P2V(4 * 1024 * 1024)); // phys page allocator
  cprintf("\nhello from main\n");
  cprintf("cprintf format test: %d, 0x%x, 0x%p, %s\n", 256, 256, main, "hello");
  kvmalloc(); // kernel page table
  // TODO for multicore
  // mpinit(); // detect other processors
  // TODO for interrupt
  // lapicinit(); // interrupt controller
  seginit(); // segment descriptors
  // TODO for interrupt
  // picinit();       // disable pic
  // TODO for interrupt
  // ioapicinit();    // another interrupt controller

  // consoleinit();   // console hardware
  // uartinit();      // serial port
  // pinit();         // process table
  // tvinit();        // trap vectors
  // binit();         // buffer cache
  // fileinit();      // file table
  // ideinit();       // disk
  // startothers();   // start other processors
  kinit2(P2V(4 * 1024 * 1024), P2V(PHYSTOP)); // must come after startothers()
  // userinit();      // first user process
  // mpmain();        // finish this processor's setup

  while (1) {
    __asm__ volatile("hlt");
  }
}
