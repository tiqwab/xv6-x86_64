#include "defs.h"
#include "proc.h"
#include "types.h"

int64_t sys_fork(void) { return fork(); }

int64_t sys_exit(void) {
  exit();
  return 0; // not reached
}

int64_t sys_wait(void) { return wait(); }

int64_t sys_getpid(void) { return (int64_t)myproc()->pid; }
