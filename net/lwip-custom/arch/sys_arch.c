#include "lwip/err.h"
#include "lwip/sys.h"

#include "arch/assert.h"
#include "arch/cc.h"
#include "arch/queue.h"
#include "arch/sys_arch.h"

#define NSEM 256
#define NMBOX 128
#define MBOXSLOTS 32
#define NMUTEX 64

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

struct sys_mbox_entry {
  struct spinlock lock;
  int freed;
  int head, nextq;
  void *msg[MBOXSLOTS];
  sys_sem_t queued_msg;
  sys_sem_t free_msg;
  LIST_ENTRY(sys_mbox_entry) link;
};

static struct sys_mbox_entry mboxes[NMBOX];
static LIST_HEAD(mbox_list, sys_mbox_entry) mbox_free;
// Get this lock if we use mboxes or mbox_free
static struct spinlock mboxes_lock;

struct sys_mutex_entry {
  struct spinlock lock;
  int freed;
  LIST_ENTRY(sys_mutex_entry) link;
};

static struct sys_mutex_entry mutexes[NMUTEX];
static LIST_HEAD(mutex_list, sys_mutex_entry) mutex_free;
// Get this lock if we use mutexes or mutex_free
static struct spinlock mutexes_lock;

void sys_init(void) {
  int i = 0;

  initlock(&sems_lock, "sems");
  initlock(&mboxes_lock, "mboxes");
  initlock(&mutexes_lock, "mutexes");

  for (i = 0; i < NSEM; i++) {
    initlock(&sems[i].lock, "sem");
    sems[i].freed = 1;
    LIST_INSERT_HEAD(&sem_free, &sems[i], link);
  }

  for (i = 0; i < NMBOX; i++) {
    initlock(&mboxes[i].lock, "mbox");
    mboxes[i].freed = 1;
    LIST_INSERT_HEAD(&mbox_free, &mboxes[i], link);
  }

  for (i = 0; i < NMUTEX; i++) {
    initlock(&mutexes[i].lock, "mutex");
    mutexes[i].freed = 1;
    LIST_INSERT_HEAD(&mutex_free, &mutexes[i], link);
  }
}

/**************
 * Semaphore
 **************/

err_t sys_sem_new(sys_sem_t *sem, u8_t count) {
  acquire(&sems_lock);

  struct sys_sem_entry *se = LIST_FIRST(&sem_free);
  if (!se) {
    cprintf("lwip: sys_sem_new: out of semaphores\n");
    release(&sems_lock);
    return SYS_SEM_NULL;
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

void sys_sem_free(sys_sem_t *sem) {
  struct sys_sem_entry *se = *sem;

  acquire(&se->lock);

  assert(!se->freed);
  se->freed = 1;
  se->gen++;

  release(&se->lock);

  acquire(&sems_lock);

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

/**************
 * Mailbox
 **************/

err_t sys_mbox_new(sys_mbox_t *mbox, int size) {
  assert(size < MBOXSLOTS);

  acquire(&mboxes_lock);

  struct sys_mbox_entry *mbe = LIST_FIRST(&mbox_free);
  if (!mbe) {
    cprintf("lwip: sys_mbox_new: out of mailboxes\n");
    release(&mboxes_lock);
    return SYS_MBOX_NULL;
  }
  LIST_REMOVE(mbe, link);
  assert(mbe->freed);
  mbe->freed = 0;

  release(&mboxes_lock);

  int i = mbe - &mboxes[0];
  mbe->head = -1;
  mbe->nextq = 0;
  sys_sem_new(&mbe->queued_msg, 0);
  sys_sem_new(&mbe->free_msg, MBOXSLOTS);

  if (mbe->queued_msg == (sys_sem_t)SYS_SEM_NULL ||
      mbe->free_msg == (sys_sem_t)SYS_SEM_NULL) {
    sys_mbox_free(&mbe);
    cprintf("lwip: sys_mbox_new: can't get semaphore\n");
    return SYS_MBOX_NULL;
  }

  return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox) {
  struct sys_mbox_entry *mbe = *mbox;

  acquire(&mbe->lock);

  assert(!mbe->freed);
  sys_sem_free(&mbe->queued_msg);
  sys_sem_free(&mbe->free_msg);
  mbe->freed = 1;

  release(&mbe->lock);

  acquire(&mboxes_lock);

  LIST_INSERT_HEAD(&mbox_free, mbe, link);

  release(&mboxes_lock);
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg) {
  assert(sys_mbox_trypost(mbox, msg) == ERR_OK);
}

// no timeout
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg) {
  struct sys_mbox_entry *mbe = *mbox;

  acquire(&mbe->lock);

  assert(!mbe->freed);

  sys_arch_sem_wait(&mbe->free_msg, 0);
  if (mbe->nextq == mbe->head) {
    release(&mbe->lock);
    return ERR_MEM;
  }

  int slot = mbe->nextq;
  mbe->nextq = (slot + 1) % MBOXSLOTS;
  mbe->msg[slot] = msg;

  if (mbe->head == -1) {
    mbe->head = slot;
  }

  sys_sem_signal(&mbe->queued_msg);

  release(&mbe->lock);
  return ERR_OK;
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg) {
  return sys_mbox_trypost(mbox, msg);
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t tm_msec) {
  struct sys_mbox_entry *mbe = *mbox;

  acquire(&mbe->lock);

  assert(!mbe->freed);

  u32_t waited = sys_arch_sem_wait(&mbe->queued_msg, tm_msec);
  if (waited == SYS_ARCH_TIMEOUT) {
    release(&mbe->lock);
    return waited;
  }

  int slot = mbe->head;
  if (slot == -1) {
    panic("lwip: sys_arch_mbox_fetch: no message");
  }
  if (msg) {
    *msg = mbe->msg[slot];
  }

  mbe->head = (slot + 1) % MBOXSLOTS;
  if (mbe->head == mbe->nextq) {
    mbe->head = -1;
  }

  sys_sem_signal(&mbe->free_msg);

  release(&mbe->lock);

  return waited;
}

// Tries to fetch a message.
// FIXME: this should return SYS_MBOX_EMPTY not SYS_ARCH_TIMEOUT?
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg) {
  return sys_arch_mbox_fetch(mbox, msg, SYS_ARCH_NOWAIT);
}

/**************
 * Mailbox
 **************/

err_t sys_mutex_new(sys_mutex_t *mutex) {
  acquire(&mutexes_lock);

  struct sys_mutex_entry *me = LIST_FIRST(&mutex_free);
  if (!me) {
    cprintf("lwip: sys_mutex_new: out of mutexes\n");
    release(&mutexes_lock);
    return ERR_MEM;
  }
  LIST_REMOVE(me, link);
  assert(me->freed);
  me->freed = 0;

  *mutex = me;

  release(&mutexes_lock);
  return ERR_OK;
}

void sys_mutex_free(sys_mutex_t *mutex) {
  struct sys_mutex_entry *me = *mutex;

  acquire(&me->lock);

  assert(!me->freed);
  me->freed = 1;

  release(&me->lock);

  acquire(&mutexes_lock);

  LIST_INSERT_HEAD(&mutex_free, me, link);

  release(&mutexes_lock);
}

void sys_mutex_lock(sys_mutex_t *mutex) {
  struct sys_mutex_entry *me = *mutex;
  acquire(&me->lock);
}

void sys_mutex_unlock(sys_mutex_t *mutex) {
  struct sys_mutex_entry *me = *mutex;
  release(&me->lock);
}

u32_t sys_now(void) { return time_msec(); }

// sys_thread_new is used in lwip/api/tcpip.c
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg,
                            int stacksize, int prio) {
  // do nothing
  return 0;
}
