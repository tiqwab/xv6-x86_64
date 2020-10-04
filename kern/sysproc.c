#include "defs.h"
#include "proc.h"
#include "types.h"

int64_t sys_fork(void) { return fork(); }

int64_t sys_exit(void) {
  exit();
  return 0; // not reached
}

int64_t sys_wait(void) { return wait(); }

int64_t sys_kill(void) {
  pid_t pid;
  if (argint(0, &pid) < 0) {
    return -1;
  }
  return kill(pid);
}

int64_t sys_getpid(void) { return (int64_t)myproc()->pid; }

int64_t sys_sleep(void) {
  uint n;
  uint ticks0;

  if (argint(0, (int *)&n) < 0) {
    return -1;
  }

  // interval of ticks is 10ms (in QEMU), so we have to count 100 ticks for 1
  // second.
  n *= 100;

  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (myproc()->killed) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}
