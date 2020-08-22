#include "defs.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void mpmain(void) __attribute__((noreturn));
extern char end[]; // first address after kernel loaded from ELF file

// Bootstrap processor starts running C code here.
// Allocate a real stack and switch to it, first
// doing some setup required for memory allocator to work.
int main(void) {
  kinit1(end); // phys page allocator
  cprintf("cprintf format test: %d, 0x%x, 0x%p, %s\n", 256, 256, main, "hello");
  kvmalloc(); // kernel page table
  mpinit(); // detect other processors
  // TODO for interrupt
  // lapicinit(); // interrupt controller
  seginit(); // segment descriptors
  picinit(); // disable pic
  // TODO for interrupt
  // ioapicinit();    // another interrupt controller

  // consoleinit();   // console hardware
  uartinit(); // serial port
  // pinit();         // process table
  tvinit(); // trap vectors
  // binit();         // buffer cache
  // fileinit();      // file table
  // ideinit();       // disk
  // startothers();   // start other processors
  kinit2();   // must come after startothers()
  userinit(); // first user process
  cprintf("initialization finished\n");

  mpmain(); // finish this processor's setup
}

// Common CPU setup code.
static void mpmain(void) {
  cprintf("cpu%d: starting %d\n", cpuid(), cpuid());
  idtinit();                    // load idt register
  xchg(&(mycpu()->started), 1); // tell startothers() we're up
  scheduler();                  // start running processes
}
