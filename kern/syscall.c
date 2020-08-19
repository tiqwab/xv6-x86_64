#include "syscall.h"
#include "defs.h"
#include "proc.h"

int64_t sys_hello(void) {
  cprintf("hello from syscall\n");
  return 0;
}

static int64_t (*syscalls[])(void) = {
    [SYS_hello] sys_hello,
};

void syscall(void) {
  size_t num;
  struct proc *curproc = myproc();

  num = curproc->tf->rax;
  if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    curproc->tf->rax = syscalls[num]();
  } else {
    cprintf("%d %s: unknown sys call %d\n", curproc->pid, curproc->name, num);
    curproc->tf->rax = -1;
  }
}
