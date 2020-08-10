#include "proc.h"
#include "defs.h"
#include "mmu.h"
#include "param.h"
#include "spinlock.h"
#include "x86.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static struct proc *initproc;

// Must be called with interrupts disabled
int cpuid() { return mycpu() - cpus; }

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

// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc *allocproc(void) {
  panic("allocproc is not implemented yet");
  //   struct proc *p;
  //   char *sp;
  //
  //   acquire(&ptable.lock);
  //
  //   for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  //     if(p->state == UNUSED)
  //       goto found;
  //
  //   release(&ptable.lock);
  //   return NULL;
  //
  // found:
  //   p->state = EMBRYO;
  //   p->pid = nextpid++;
  //
  //   release(&ptable.lock);
  //
  //   // Allocate kernel stack.
  //   if((p->kstack = kalloc()) == 0){
  //     p->state = UNUSED;
  //     return NULL;
  //   }
  //   sp = p->kstack + KSTACKSIZE;
  //
  //   // Leave room for trap frame.
  //   sp -= sizeof *p->tf;
  //   p->tf = (struct trapframe*) sp;
  //
  //   // Set up new context to start executing at forkret,
  //   // which returns to trapret.
  //   sp -= sizeof(uintptr_t);
  //   *((uintptr_t *) sp) = (uintptr_t) trapret;
  //
  //   sp -= sizeof *p->context;
  //   p->context = (struct context*)sp;
  //   memset(p->context, 0, sizeof *p->context);
  //   p->context->eip = (uint)forkret;
  //
  //   return p;
}

// Set up first user process.
void userinit(void) {
  struct proc *p;
  extern char _binary_obj_kern_initcode_start[],
      _binary_obj_kern_initcode_size[];

  p = allocproc();

  initproc = p;
  if ((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_obj_kern_initcode_start,
          (size_t)_binary_obj_kern_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->rflags = FL_IF;
  p->tf->rsp = PGSIZE;
  p->tf->rip = 0; // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  // TODO for fs
  // p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}
