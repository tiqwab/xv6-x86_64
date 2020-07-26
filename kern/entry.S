#include "memlayout.h"
#include "param.h"

# The xv6 kernel starts executing in this file. This file is linked with
# the kernel C code, so it can refer to kernel symbols such as main().
# The boot block jumps to entry below.

# By convention, the _start symbol specifies the ELF entry point.
.globl _start
_start = V2P_WO(entry)

# entrypoint just for jmp to high address
entry:
  # Currently run at around 0x100000.
  # Set stack and Jmp to high addresses by indirect call.
  movq $KERNBASE,%rdx

  leaq stack(%rip),%rax
  add $KSTACKSIZE,%rax
  addq %rdx,%rax
  mov %rax,%rsp

  leaq entry_high_address(%rip),%rax
  addq %rdx,%rax
  jmp *%rax

# actual entrypoint
entry_high_address:
  hlt
  jmp entry_high_address

# .comm is to prepare data in BSS
.comm stack, KSTACKSIZE
