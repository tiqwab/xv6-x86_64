.globl start
start:
  # check for syscall
  mov $0x1,%rax
  int $0x30
  # syscall again
  mov $0x1,%rax
  int $0x30

loop:
  jmp loop
