#ifndef XV6_X86_64_DEFS_H
#define XV6_X86_64_DEFS_H

#include "types.h"

struct cpu;
struct spinlock;

// console.c
void cprintf(char *, ...);
void panic(char *) __attribute__((noreturn));

// kalloc.c
char *kalloc(void);
void kfree(char *);
void kinit1(void *, void *);

// proc.c
struct cpu *mycpu(void);

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

// vm.c
void freevm(pte_t *pgdir);
void kvmalloc(void);
void switchkvm(void);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))

#endif /* ifndef XV6_X86_64_DEFS_H */
