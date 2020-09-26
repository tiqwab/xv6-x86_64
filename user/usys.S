# TODO: duplicated with kern/syscall.h
#define SYS_fork 1
#define SYS_exit    2
#define SYS_wait    3
#define SYS_exec 7
#define SYS_getpid 11
#define SYS_open 15
#define SYS_close 21

#define SYS_hello 32
#define SYS_putc 34
#define SYS_fstest 35

#define T_SYSCALL 0x30

#define SYSCALL(name) \
  .globl name; \
  name: \
    mov $SYS_ ## name, %rax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(exec)
SYSCALL(getpid)
SYSCALL(open)
SYSCALL(close)

SYSCALL(hello)
SYSCALL(putc)
SYSCALL(fstest)
