#ifndef XV6_X86_64_SYSCALL_H
#define XV6_X86_64_SYSCALL_H

// TODO: duplicated with user/usys.c

#define SYS_fork 1
#define SYS_exit 2
#define SYS_wait 3
#define SYS_read 5
#define SYS_exec 7
#define SYS_chdir 9
#define SYS_dup 10
#define SYS_getpid 11
#define SYS_open 15
#define SYS_write 16
#define SYS_mknod 17
#define SYS_unlink 18
#define SYS_link 19
#define SYS_mkdir 20
#define SYS_close 21

#define SYS_hello 32
#define SYS_putc 34
#define SYS_fstest 35

#endif /* ifndef XV6_X86_64_SYSCALL_H */
