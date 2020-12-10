// declaration defined in kernel

#ifndef XV6_X86_64_ARCH_KERN_H
#define XV6_X86_64_ARCH_KERN_H

#include "kern/sleeplock.h"
#include "kern/spinlock.h"

void cprintf(char *, ...);
void panic(char *, ...) __attribute__((noreturn));

unsigned int time_msec();

void sleep(void *chan, struct spinlock *lk);
void wakeup(void *chan);

#endif /* XV6_X86_64_ARCH_KERN_H */
