#ifndef XV6_X86_64_X86_H
#define XV6_X86_64_X86_H

#include "types.h"

static inline void stosb(void *addr, int data, int cnt) {
  __asm__ volatile("cld; rep stosb"
                   : "=D"(addr), "=c"(cnt)
                   : "0"(addr), "1"(cnt), "a"(data)
                   : "memory", "cc");
}

static inline void cli(void) { __asm__ volatile("cli"); }

static inline void sti(void) { __asm__ volatile("sti"); }

static inline uint xchg(volatile uint *addr, uint newval) {
  uint result;

  // The + in "+m" denotes a read-modify-write operand.
  __asm__ volatile("lock; xchgl %0, %1"
                   : "+m"(*addr), "=a"(result)
                   : "1"(newval)
                   : "cc");
  return result;
}

#endif /* ifndef XV6_X86_64_X86_H */
