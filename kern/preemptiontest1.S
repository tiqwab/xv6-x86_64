.globl start
# print message repeatedly
start:
  lea msg(%rip),%rdx
loop:
  push %rdx
  mov $2,%rax
  call syscall
  pop %rdx
  jmp loop

syscall:
  int $0x30
  ret

msg:
  .asciz "I am preemptiontest1\n"
