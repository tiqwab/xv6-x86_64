# TODO: duplicated with kern/syscall.h
#define SYS_hello 1
#define SYS_print 2
#define SYS_getpid 11

#define T_SYSCALL 0x30

#define SYSCALL(name) \
  .globl name; \
  name: \
    mov $SYS_ ## name, %rax; \
    int $T_SYSCALL; \
    ret

SYSCALL(hello)
SYSCALL(print)
SYSCALL(getpid)
