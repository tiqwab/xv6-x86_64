#ifndef XV6_X86_64_DEFS_H
#define XV6_X86_64_DEFS_H

#include "types.h"

struct spinlock;

// console.c
void cprintf(char *, ...);
void panic(char *) __attribute__((noreturn));

// kalloc.c
void kinit1(void *, void *);
void kfree(char *);

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

#endif /* ifndef XV6_X86_64_DEFS_H */
