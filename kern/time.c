#include "defs.h"
#include "spinlock.h"

unsigned int time_msec() {
  unsigned int ticks0;
  acquire(&tickslock);
  ticks0 = ticks * 10;
  release(&tickslock);
  return ticks0;
}
