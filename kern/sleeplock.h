#ifndef XV6_X86_64_SLEEPLOCK_H
#define XV6_X86_64_SLEEPLOCK_H

#include "spinlock.h"
#include "types.h"

//
// Long-term locks for processes
struct sleeplock {
  uint locked;        // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  char *name; // Name of lock.
  int pid;    // Process holding lock
};

#endif /* ifndef XV6_X86_64_SLEEPLOCK_H */
