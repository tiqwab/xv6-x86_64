#ifndef XV6_X86_64_X86_H
#define XV6_X86_64_X86_H

#include "types.h"

// Routines to let C code use special x86 instructions.

inline uchar inb(ushort port) {
  uchar data;

  __asm__ volatile("in %1,%0" : "=a" (data) : "d" (port));
  return data;
}

inline void outb(ushort port, uchar data) {
  __asm__ volatile("out %0,%1" : : "a" (data), "d" (port));
}

inline void insl(int port, void *addr, int cnt) {
  __asm__ volatile("cld; rep insl" :
               "=D" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "memory", "cc");
}

inline void stosb(void *addr, int data, int cnt) {
  __asm__ volatile("cld; rep stosb" :
               "=D" (addr), "=c" (cnt) :
               "0" (addr), "1" (cnt), "a" (data) :
               "memory", "cc");
}

#endif /* XV6_X86_64_X86_H */
