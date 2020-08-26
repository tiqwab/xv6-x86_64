.globl start
start:
  # check for syscall
  mov $1,%rax
  call syscall
  # syscall again
  mov $1,%rax
  call syscall
  # print message
  lea msg(%rip),%rdx
  push %rdx
  mov $2,%rax
  call syscall
  pop %rdx
  # syscall getpid
  mov $11,%rax
  call syscall

loop:
  jmp loop

syscall:
  int $0x30
  ret

msg:
  .asciz "hello by print system call\n"
