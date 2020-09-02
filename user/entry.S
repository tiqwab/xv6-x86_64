.text
.globl _start
_start:
  # TODO: handle exec arguments
  mov $0,%rdi # argc
  mov $0,%rsi # argv
  call main

  # when return from main
  call exit
