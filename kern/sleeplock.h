#ifndef XV6_X86_64_SLEEPLOCK_H
#define XV6_X86_64_SLEEPLOCK_H

#include "inc/types.h"
#include "spinlock.h"

//
// Long-term locks for processes
struct sleeplock {
  uint locked;        // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  char *name; // Name of lock.
  pid_t pid;  // Process holding lock
};

#endif /* ifndef XV6_X86_64_SLEEPLOCK_H */
