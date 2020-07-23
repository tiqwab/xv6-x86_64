#include "memlayout.h"

# The xv6 kernel starts executing in this file. This file is linked with
# the kernel C code, so it can refer to kernel symbols such as main().
# The boot block jumps to entry below.

# By convention, the _start symbol specifies the ELF entry point.
# Since we haven't set up virtual memory yet, our entry point is
# the physical address of 'entry'.
.globl _start
_start = V2P_WO(entry)

.globl entry
# entrypoint just for jmp to high address
entry:
  # FIXME: What is the best way to jump to high address? (currently run around 0x100000)
  # linker doesn't allow get the address of entry_high_address...
  mov $0xffff80000010000c,%rax # entry_high_address
  jmp *%rax

# actual entrypoint
entry_high_address:
  hlt
  jmp entry_high_address
