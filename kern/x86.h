#ifndef XV6_X86_64_X86_H
#define XV6_X86_64_X86_H

#include "types.h"

static inline uchar inb(ushort port) {
  uchar data;

  __asm__ volatile("in %1,%0" : "=a"(data) : "d"(port));
  return data;
}

static inline void insl(int port, void *addr, int cnt) {
  __asm__ volatile("cld; rep insl"
                   : "=D"(addr), "=c"(cnt)
                   : "d"(port), "0"(addr), "1"(cnt)
                   : "memory", "cc");
}

static inline void outb(ushort port, uchar data) {
  __asm__ volatile("out %0,%1" : : "a"(data), "d"(port));
}

static inline void outsl(int port, const void *addr, int cnt) {
  __asm__ volatile("cld; rep outsl"
                   : "=S"(addr), "=c"(cnt)
                   : "d"(port), "0"(addr), "1"(cnt)
                   : "cc");
}

static inline void stosb(void *addr, int data, int cnt) {
  __asm__ volatile("cld; rep stosb"
                   : "=D"(addr), "=c"(cnt)
                   : "0"(addr), "1"(cnt), "a"(data)
                   : "memory", "cc");
}

struct segdesc;

static inline void lgdt(struct segdesc *p, uint16_t size) {
  volatile uint16_t pd[5];

  pd[0] = size - 1;
  pd[1] = ((uintptr_t)p) & 0xffff;
  pd[2] = (((uintptr_t)p) >> 16) & 0xffff;
  pd[3] = (((uintptr_t)p) >> 32) & 0xffff;
  pd[4] = (((uintptr_t)p) >> 48) & 0xffff;

  __asm__ volatile("lgdt (%0)" : : "r"(pd));
}

struct gatedesc;

static inline void lidt(struct gatedesc *p, uint16_t size) {
  volatile uint16_t pd[5];

  pd[0] = size - 1;
  pd[1] = ((uintptr_t)p) & 0xffff;
  pd[2] = (((uintptr_t)p) >> 16) & 0xffff;
  pd[3] = (((uintptr_t)p) >> 32) & 0xffff;
  pd[4] = (((uintptr_t)p) >> 48) & 0xffff;

  __asm__ volatile("lidt (%0)" : : "r"(pd));
}

static inline void ltr(uint16_t sel) {
  __asm__ volatile("ltr %0" : : "r"(sel));
}

static inline void cli(void) { __asm__ volatile("cli"); }

static inline void sti(void) { __asm__ volatile("sti"); }

static inline void lcr3(uintptr_t val) {
  __asm__ volatile("movq %0,%%cr3" : : "r"(val));
}

static inline uintptr_t rcr2(void) {
  uintptr_t val;
  __asm__ volatile("movq %%cr2,%0" : "=r"(val));
  return val;
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

// Layout of the trap frame built on the stack by the
// hardware and by trapasm.S, and passed to trap().
//
// in x86-64, trapframe is:
// - always aligned in 16-bytes
// - always contains ss and rsp
// - err is still optional (some exception push it, but others not)
struct trapframe {
  // registers
  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rbp;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;

  // rest of trap frame
  // remove segment registers because the value is always same (2 for kernel, 4
  // for user) uint16_t gs; uint16_t padding_gs1; uint32_t padding_gs2; uint16_t
  // fs; uint16_t padding_fs1; uint32_t padding_fs2; uint16_t es; uint16_t
  // padding_es1; uint32_t padding_es2; uint16_t ds; uint16_t padding_ds1;
  // uint32_t padding_ds2;

  uint64_t trapno;

  // below here defined by x86-64 hardware
  uint64_t err;
  uint64_t rip;
  uint16_t cs;
  uint16_t padding_cs1;
  uint32_t padding_cs2;
  uint64_t rflags;

  // below here only when crossing rings, such as from user to kernel
  uint64_t rsp;
  uint16_t ss;
  uint16_t padding_ss1;
  uint32_t padding_ss2;
};

typedef struct trapframe trapframe_t __attribute__((aligned(16)));

#endif /* ifndef XV6_X86_64_X86_H */
