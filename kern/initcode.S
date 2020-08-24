.globl start
start:
  # check for syscall
  mov $1,%rax
  int $0x30
  # syscall again
  mov $1,%rax
  int $0x30
  # syscall getpid
  mov $11,%rax
  int $0x30

loop:
  jmp loop
