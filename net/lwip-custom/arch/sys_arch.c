#include "arch/sys_arch.h"

// sys_sem_t
// sys_sem_new(u8_t count)
// {
//     struct sys_sem_entry *se = LIST_FIRST(&sem_free);
//     if (!se) {
// 	cprintf("lwip: sys_sem_new: out of semaphores\n");
// 	return SYS_SEM_NULL;
//     }
//     LIST_REMOVE(se, link);
//     assert(se->freed);
//     se->freed = 0;
// 
//     se->counter = count;
//     se->gen++;
//     return se - &sems[0];
// }
// 
// void
// sys_sem_free(sys_sem_t sem)
// {
//     assert(!sems[sem].freed);
//     sems[sem].freed = 1;
//     sems[sem].gen++;
//     LIST_INSERT_HEAD(&sem_free, &sems[sem], link);
// }
// 
// void
// sys_sem_signal(sys_sem_t sem)
// {
//     assert(!sems[sem].freed);
//     sems[sem].counter++;
//     if (sems[sem].waiters) {
// 	sems[sem].waiters = 0;
// 	thread_wakeup(&sems[sem].v);
//     }
// }
// 
// u32_t
// sys_arch_sem_wait(sys_sem_t sem, u32_t tm_msec)
// {
//     assert(!sems[sem].freed);
//     u32_t waited = 0;
// 
//     int gen = sems[sem].gen;
// 
//     while (tm_msec == 0 || waited < tm_msec) {
// 	if (sems[sem].counter > 0) {
// 	    sems[sem].counter--;
// 	    return waited;
//  	} else if (tm_msec == SYS_ARCH_NOWAIT) {
// 	    return SYS_ARCH_TIMEOUT;
// 	} else {
// 	    uint32_t a = sys_time_msec();
// 	    uint32_t sleep_until = tm_msec ? a + (tm_msec - waited) : ~0;
// 	    sems[sem].waiters = 1;
// 	    uint32_t cur_v = sems[sem].v;
// 	    lwip_core_unlock();
// 	    thread_wait(&sems[sem].v, cur_v, sleep_until);
// 	    lwip_core_lock();
// 	    if (gen != sems[sem].gen) {
// 		cprintf("sys_arch_sem_wait: sem freed under waiter!\n");
// 		return SYS_ARCH_TIMEOUT;
// 	    }
// 	    uint32_t b = sys_time_msec();
// 	    waited += (b - a);
// 	}
//     }
// 
//     return SYS_ARCH_TIMEOUT;
// }
