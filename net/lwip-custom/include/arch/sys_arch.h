#ifndef XV6_X86_64_ARCH_SYS_ARCH_H
#define XV6_X86_64_ARCH_SYS_ARCH_H

#include "kern/spinlock.h"

struct sys_sem_entry;
typedef struct sys_sem_entry *sys_sem_t;
#define sys_sem_valid(sem) (((sem) != NULL) && (*(sem) != NULL))
#define sys_sem_valid_val(sem) ((sem) != NULL)
#define sys_sem_set_invalid(sem)                                               \
  do {                                                                         \
    if ((sem) != NULL) {                                                       \
      *(sem) = NULL;                                                           \
    }                                                                          \
  } while (0)
#define sys_sem_set_invalid_val(sem)                                           \
  do {                                                                         \
    (sem) = NULL;                                                              \
  } while (0)

struct sys_mbox_entry;
typedef struct sys_mbox_entry *sys_mbox_t;
#define sys_mbox_valid(mbox) sys_sem_valid(mbox)
#define sys_mbox_valid_val(mbox) sys_sem_valid_val(mbox)
#define sys_mbox_set_invalid(mbox) sys_sem_set_invalid(mbox)
#define sys_mbox_set_invalid_val(mbox) sys_sem_set_invalid_val(mbox)

struct sys_mutex_entry;
typedef struct sys_mutex_entry *sys_mutex_t;
#define sys_mutex_valid(mutex) sys_sem_valid(mutex)
#define sys_mutex_set_invalid(mutex) sys_sem_set_invalid(mutex)

typedef int sys_thread_t;

#define SYS_MBOX_NULL (-1)
#define SYS_SEM_NULL (-1)

void lwip_core_lock(void);
void lwip_core_unlock(void);
void lwip_core_init(void);

#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev)
#define SYS_ARCH_UNPROTECT(lev)

#define SYS_ARCH_NOWAIT 0xfffffffe

// FIXME: definitions is necessary?
// #define sys_msleep(t)

#endif /* ifndef XV6_X86_64_ARCH_SYS_ARCH_H */
