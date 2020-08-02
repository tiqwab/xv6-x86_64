#ifndef XV6_X86_64_PROC_H
#define XV6_X86_64_PROC_H

#include "param.h"

// TODO for multicore
// Per-CPU state
struct cpu {
  // uchar apicid;                // Local APIC ID
  // struct context *scheduler;   // swtch() here to enter scheduler
  // struct taskstate ts;         // Used by x86 to find stack for interrupt
  // struct segdesc gdt[NSEGS];   // x86 global descriptor table
  // volatile uint started;       // Has the CPU started?
  int ncli;   // Depth of pushcli nesting.
  int intena; // Were interrupts enabled before pushcli?
  // struct proc *proc;           // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];

#endif /* ifndef XV6_X86_64_PROC_H */
