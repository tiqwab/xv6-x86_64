#ifndef XV6_X86_64_PROC_H
#define XV6_X86_64_PROC_H

#include "mmu.h"
#include "param.h"
#include "x86.h"

// TODO for multicore
// Per-CPU state
// scheduler contains rsp before swtch.
struct cpu {
  uchar apicid;              // Local APIC ID
  struct context *scheduler; // swtch() here to enter scheduler
  struct taskstate ts;       // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS]; // x86 global descriptor table
  volatile uint started;     // Has the CPU started?
  int ncli;                  // Depth of pushcli nesting.
  int intena;                // Were interrupts enabled before pushcli?
  struct proc *proc;         // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to caller-saved registers such as %rax, %rcx and %rdx,
// because the x86-64 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context {
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rbx;
  uint64_t rbp;
  uint64_t rip;
};

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  size_t sz;            // Size of process memory (bytes)
  pte_t *pgdir;         // Page table
  char *kstack;         // Bottom of kernel stack for this process
  enum procstate state; // Process state
  pid_t pid;            // Process ID
  // struct proc *parent;         // Parent process
  trapframe_t *tf;         // Trap frame for current syscall
  struct context *context; // swtch() here to run process
  // void *chan;                  // If non-zero, sleeping on chan
  int killed; // If non-zero, have been killed
  // struct file *ofile[NOFILE];  // Open files
  // struct inode *cwd;           // Current directory
  char name[16]; // Process name (debugging)
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap

#endif /* ifndef XV6_X86_64_PROC_H */
