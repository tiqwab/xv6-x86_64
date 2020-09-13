.text
.globl _start
_start:
  # assume that (%rsp + 8) contains argc and (%rsp + 16) contains argv respectively
  # see kern/exec.c
  mov 0x8(%rsp),%rdi # argc
  mov 0x10(%rsp),%rsi # argv
  call main

  # when return from main
  call exit
