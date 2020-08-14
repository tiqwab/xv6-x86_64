#include "defs.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"

extern char end[]; // first address after kernel loaded from ELF file

// Bootstrap processor starts running C code here.
// Allocate a real stack and switch to it, first
// doing some setup required for memory allocator to work.
int main(void) {
  kinit1(end); // phys page allocator
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
  kinit2();   // must come after startothers()
  userinit(); // first user process
  // mpmain();        // finish this processor's setup

  cprintf("initialization finished\n");

  while (1) {
    __asm__ volatile("hlt");
  }
}
