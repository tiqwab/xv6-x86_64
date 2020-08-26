#include "syscall.h"
#include "defs.h"
#include "proc.h"
#include "types.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %rsp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int fetchint(uintptr_t addr, uint64_t *ip) {
  struct proc *curproc = myproc();

  if (addr >= curproc->sz || addr + 8 > curproc->sz) {
    cprintf("fetchint failed: sz: %d, addr: 0x%x\n", curproc->sz, addr);
    return -1;
  }
  *ip = *((uint64_t *)addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int fetchstr(uintptr_t addr, char **pp) {
  char *s, *ep;
  struct proc *curproc = myproc();

  if (addr >= curproc->sz)
    return -1;
  *pp = (char *)addr;
  ep = (char *)curproc->sz;
  for (s = *pp; s < ep; s++) {
    if (*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 64-bit system call argument.
int arg(int n, uint64_t *ip) {
  uint64_t v;

  switch (n) {
  case 0:
    v = myproc()->tf->rdi;
    break;
  case 1:
    v = myproc()->tf->rsi;
    break;
  case 2:
    v = myproc()->tf->rdx;
    break;
  case 3:
    v = myproc()->tf->rcx;
    break;
  case 4:
    v = myproc()->tf->r8;
    break;
  default:
    cprintf("args should be equal to or smaller than 5 for now.");
    return -1;
  }

  *ip = v;
  return 0;
  // return fetchint((myproc()->tf->rsp) + 8 + 8 * n, ip);
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int argstr(int n, char **pp) {
  uintptr_t addr;
  if (arg(n, (uint64_t *)&addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

int64_t sys_hello(void) {
  cprintf("hello from syscall\n");
  return 0;
}

int64_t sys_print(void) {
  char *str;
  if (argstr(0, &str) < 0) {
    cprintf("failed to fetch a system call argument\n");
    return 0;
  }
  cprintf("%s", str);
  return 0;
}

extern int64_t sys_getpid(void);

static int64_t (*syscalls[])(void) = {
    [SYS_hello] = sys_hello,
    [SYS_print] = sys_print,
    [SYS_getpid] = sys_getpid,
};

// FIXME: Accept up to 5 arguments for now.
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
