# TODO: duplicated with kern/syscall.h
#define SYS_fork 1
#define SYS_exec 7
#define SYS_getpid 11

#define SYS_hello 32
#define SYS_print 33

#define T_SYSCALL 0x30

#define SYSCALL(name) \
  .globl name; \
  name: \
    mov $SYS_ ## name, %rax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exec)
SYSCALL(getpid)
SYSCALL(hello)
SYSCALL(print)
