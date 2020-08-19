#ifndef XV6_X86_64_DEFS_H
#define XV6_X86_64_DEFS_H

#include "types.h"

struct context;
struct cpu;
struct proc;
struct spinlock;

// console.c
void cprintf(char *, ...);
void panic(char *) __attribute__((noreturn));

// kalloc.c
char *kalloc(void);
void kfree(char *);
void kinit1(void *vstart);
void kinit2();

// proc.c
struct cpu *mycpu(void);
int cpuid(void);
struct proc *myproc(void);
void scheduler(void) __attribute__((noreturn));
void userinit(void);

// swtch.S
void swtch(struct context **, struct context *);

// spinlock.c
void acquire(struct spinlock *);
// void            getcallerpcs(void*, uint*);
int holding(struct spinlock *);
void initlock(struct spinlock *, char *);
void release(struct spinlock *);
void pushcli(void);
void popcli(void);

// string.c
void *memset(void *, int, size_t);
void *memmove(void *, const void *, size_t);
char *safestrcpy(char *s, const char *t, int n);

// syscall.c
void syscall(void);

// trap.c
void idtinit(void);
void tvinit(void);

// vm.c
void freevm(pte_t *pgdir);
void inituvm(pte_t *pgdir, char *init, size_t sz);
void kvmalloc(void);
void seginit(void);
pte_t *setupkvm(void);
void switchkvm(void);
void switchuvm(struct proc *p);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))

#endif /* ifndef XV6_X86_64_DEFS_H */
