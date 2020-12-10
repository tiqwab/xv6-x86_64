#ifndef XV6_X86_64_ARCH_SYS_ARCH_H
#define XV6_X86_64_ARCH_SYS_ARCH_H

struct sys_sem_entry;
typedef struct sys_sem_entry *sys_sem_t;

typedef int sys_mbox_t;
typedef int sys_thread_t;
typedef int sys_mutex_t;

#define SYS_MBOX_NULL (-1)
#define SYS_SEM_NULL (-1)

void lwip_core_lock(void);
void lwip_core_unlock(void);
void lwip_core_init(void);

#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev)
#define SYS_ARCH_UNPROTECT(lev)

#define SYS_ARCH_NOWAIT 0xfffffffe

#endif /* ifndef XV6_X86_64_ARCH_SYS_ARCH_H */
