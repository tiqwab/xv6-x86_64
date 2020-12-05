#ifndef XV6_X86_64_ARCH_SYS_ARCH_H
#define XV6_X86_64_ARCH_SYS_ARCH_H

typedef int sys_sem_t;
typedef int sys_mbox_t;
typedef int sys_thread_t;
typedef int sys_mutex_t;

#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev)
#define SYS_ARCH_UNPROTECT(lev)

#endif /* ifndef XV6_X86_64_ARCH_SYS_ARCH_H */
