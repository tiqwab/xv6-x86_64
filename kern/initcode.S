# Initial process execs /init.
# This code runs in user space.

#include "syscall.h"
#include "trap.h"

.globl start
start:
# exec("/init", argv);
  lea init(%rip),%rax
  mov %rax,%rdi
  lea argv(%rip),%rax
  mov %rax,%rsi
  mov $SYS_exec, %rax
  int $T_SYSCALL

# for(;;) exit();
exit:
  mov $SYS_exit, %rax
  int $T_SYSCALL
  jmp exit

init:
  .string "/init\0"

.p2align 3
argv:
  .long init
  .long 0
