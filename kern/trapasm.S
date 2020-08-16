#include "mmu.h"

  # vectors.S sends all traps here.
.globl alltraps
alltraps:
  # Build trap frame.
  push %r15
  push %r14
  push %r13
  push %r12
  push %r11
  push %r10
  push %r9
  push %r8
  push %rdi
  push %rsi
  push %rbp
  push %rdx
  push %rcx
  push %rbx
  push %rax
  
  # Set up data segments.
  mov $(SEG_KDATA<<3), %ax
  mov %ax, %ds
  mov %ax, %es

  # Call trap(tf), where tf=%esp
  push %rsp
  # call trap # TODO for interrupt
  add $8, %rsp

  # Return falls through to trapret...
.globl trapret
trapret:
  # restore data segments
  mov $(SEG_UDATA<<3), %ax
  mov %ax, %ds
  mov %ax, %es

  # restore general registers
  pop %rax
  pop %rbx
  pop %rcx
  pop %rdx
  pop %rbp
  pop %rsi
  pop %rdi
  pop %r8
  pop %r9
  pop %r10
  pop %r11
  pop %r12
  pop %r13
  pop %r14
  pop %r15

  add $0x10, %rsp  # trapno and errcode
  iretq
