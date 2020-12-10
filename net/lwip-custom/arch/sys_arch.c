#include "lwip/err.h"
#include "lwip/sys.h"

#include "arch/assert.h"
#include "arch/cc.h"
#include "arch/queue.h"
#include "arch/sys_arch.h"

#define NSEM 256

struct sys_sem_entry {
  struct spinlock lock;
  int freed;
  int gen;
  union {
    uint32_t v;
    struct {
      uint16_t counter;
      uint16_t waiters;
    };
  };
  LIST_ENTRY(sys_sem_entry) link;
};

static struct sys_sem_entry sems[NSEM];
static LIST_HEAD(sem_list, sys_sem_entry) sem_free;
// Get this lock if we use sems or sem_free
static struct spinlock sems_lock;

void sys_init(void) {
  int i = 0;

  initlock(&sems_lock, "sems");

  for (i = 0; i < NSEM; i++) {
    initlock(&sems[i].lock, "sem");
    sems[i].freed = 1;
    LIST_INSERT_HEAD(&sem_free, &sems[i], link);
  }

  // for (i = 0; i < NMBOX; i++) {
  //     mboxes[i].freed = 1;
  //     LIST_INSERT_HEAD(&mbox_free, &mboxes[i], link);
  // }
}

err_t sys_sem_new(sys_sem_t *sem, u8_t count) {
  acquire(&sems_lock);

  struct sys_sem_entry *se = LIST_FIRST(&sem_free);
  if (!se) {
    cprintf("lwip: sys_sem_new: out of semaphores\n");
    release(&sems_lock);
    return ERR_MEM;
  }
  LIST_REMOVE(se, link);
  assert(se->freed);
  se->freed = 0;

  se->counter = count;
  se->gen++;

  *sem = se;

  release(&sems_lock);
  return ERR_OK;
}

void
sys_sem_free(sys_sem_t *sem) {
    acquire(&sems_lock);

    struct sys_sem_entry *se = *sem;

    assert(!se->freed);
    se->freed = 1;
    se->gen++;
    LIST_INSERT_HEAD(&sem_free, se, link);

    release(&sems_lock);
}

void sys_sem_signal(sys_sem_t *sem) {
    struct sys_sem_entry *se = *sem;
    acquire(&se->lock);

    assert(!se->freed);
    se->counter++;
    if (se->waiters) {
        se->waiters = 0;
        wakeup(se);
    }

    release(&se->lock);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t tm_msec) {
  struct sys_sem_entry *se = *sem;
  acquire(&se->lock);

  assert(!se->freed);
  u32_t waited = 0;

  int gen = se->gen;

  while (tm_msec == 0 || waited < tm_msec) {
    if (se->counter > 0) {
      se->counter--;
      release(&se->lock);
      return waited;
    } else if (tm_msec == SYS_ARCH_NOWAIT) {
      release(&se->lock);
      return SYS_ARCH_TIMEOUT;
    } else {
      uint32_t a = time_msec();
      uint32_t sleep_until = tm_msec ? a + (tm_msec - waited) : ~(0U);
      se->waiters = 1;
      uint32_t cur_v = se->v;

      sleep(se, &se->lock);

      if (gen != se->gen) {
        cprintf("sys_arch_sem_wait: sem freed under waiter!\n");
        release(&se->lock);
        return SYS_ARCH_TIMEOUT;
      }
      uint32_t b = time_msec();
      waited += (b - a);
    }
  }

  release(&se->lock);

  return SYS_ARCH_TIMEOUT;
}

void lwip_core_lock(void) {}

void lwip_core_unlock(void) {}
