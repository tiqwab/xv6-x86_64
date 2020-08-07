#ifndef XV6_X86_64_X86_H
#define XV6_X86_64_X86_H

#include "types.h"

static inline uchar inb(ushort port) {
  uchar data;

  __asm__ volatile("in %1,%0" : "=a"(data) : "d"(port));
  return data;
}

static inline void outb(ushort port, uchar data) {
  __asm__ volatile("out %0,%1" : : "a"(data), "d"(port));
}

static inline void stosb(void *addr, int data, int cnt) {
  __asm__ volatile("cld; rep stosb"
                   : "=D"(addr), "=c"(cnt)
                   : "0"(addr), "1"(cnt), "a"(data)
                   : "memory", "cc");
}

static inline void cli(void) { __asm__ volatile("cli"); }

static inline void sti(void) { __asm__ volatile("sti"); }

static inline void lcr3(uintptr_t val) {
  __asm__ volatile("movq %0,%%cr3" : : "r"(val));
}

static inline uint xchg(volatile uint *addr, uint newval) {
  uint result;

  // The + in "+m" denotes a read-modify-write operand.
  __asm__ volatile("lock; xchgl %0, %1"
                   : "+m"(*addr), "=a"(result)
                   : "1"(newval)
                   : "cc");
  return result;
}

static inline uint readeflags(void) {
  ulong eflags;
  __asm__ volatile("pushfq; pop %0" : "=r"(eflags));
  return (uint)eflags;
}

#endif /* ifndef XV6_X86_64_X86_H */
