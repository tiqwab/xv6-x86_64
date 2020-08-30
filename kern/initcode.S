.globl start
start:
  # check for syscall
  mov $1,%rax
  call syscall
  # syscall again
  mov $1,%rax
  call syscall
  # print message
  lea msg(%rip),%rax
  mov %rax,%rdi
  mov $2,%rax
  call syscall
  # syscall getpid
  mov $11,%rax
  call syscall

  lea path(%rip),%rdi
  mov $7,%rax
  call syscall

syscall:
  int $0x30
  ret

msg:
  .asciz "hello by print system call\n"
path:
  .asciz "preemptiontest1"
