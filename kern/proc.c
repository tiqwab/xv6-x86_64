#include "proc.h"
#include "defs.h"
#include "mmu.h"
#include "x86.h"

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu *mycpu(void) {
  if (readeflags() & FL_IF)
    panic("mycpu called with interrupts enabled\n");
  return &cpus[0];

  // TODO for multicore
  // int apicid, i;
  //
  // apicid = lapicid();
  // // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // // a reverse map, or reserve a register to store &cpus[i].
  // for (i = 0; i < ncpu; ++i) {
  //   if (cpus[i].apicid == apicid)
  //     return &cpus[i];
  // }
  // panic("unknown apicid\n");
}
