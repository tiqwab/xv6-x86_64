#include "inc/syscall.h"
#include "buf.h"
#include "defs.h"
#include "proc.h"

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
}

// Fetch the nth 32-bit system call argument.
int argint(int n, int *ip) {
  uint64_t v;
  if (arg(n, &v) < 0) {
    return -1;
  }
  *ip = (int64_t)v;
  return 0;
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int argptr(int n, char **pp, int size) {
  uintptr_t i;
  struct proc *curproc = myproc();

  if (arg(n, &i) < 0) {
    return -1;
  }
  if (size < 0 || i >= curproc->sz || i + size > curproc->sz) {
    return -1;
  }
  *pp = (char *)i;
  return 0;
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

// TODO: remove later
int64_t sys_hello(void) {
  cprintf("hello from syscall\n");
  return 0;
}

// TODO: remove after fs
int64_t sys_fstest(void) {
  struct buf *b1 = bread(1, 0);
  struct buf *b2 = bread(1, 60);
  for (int i = 0; i < BSIZE; i++) {
    b1->data[i] = b2->data[i];
  }
  bwrite(b1);
  brelse(b2);
  brelse(b1);
  return 0;
}

extern int64_t sys_fork(void);
extern int64_t sys_exit(void);
extern int64_t sys_wait(void);
extern int64_t sys_pipe(void);
extern int64_t sys_read(void);
extern int64_t sys_kill(void);
extern int64_t sys_exec(void);
extern int64_t sys_fstat(void);
extern int64_t sys_chdir(void);
extern int64_t sys_dup(void);
extern int64_t sys_getpid(void);
extern int64_t sys_sbrk(void);
extern int64_t sys_sleep(void);
extern int64_t sys_open(void);
extern int64_t sys_write(void);
extern int64_t sys_mknod(void);
extern int64_t sys_unlink(void);
extern int64_t sys_link(void);
extern int64_t sys_mkdir(void);
extern int64_t sys_close(void);

extern int64_t sys_fstest(void);

static int64_t (*syscalls[])(void) = {
    [SYS_fork] = sys_fork,     [SYS_exit] = sys_exit,
    [SYS_wait] = sys_wait,     [SYS_pipe] = sys_pipe,
    [SYS_read] = sys_read,     [SYS_kill] = sys_kill,
    [SYS_exec] = sys_exec,     [SYS_fstat] = sys_fstat,
    [SYS_chdir] = sys_chdir,   [SYS_dup] = sys_dup,
    [SYS_getpid] = sys_getpid, [SYS_sbrk] = sys_sbrk,
    [SYS_sleep] = sys_sleep,   [SYS_hello] = sys_hello,
    [SYS_fstest] = sys_fstest, [SYS_open] = sys_open,
    [SYS_write] = sys_write,   [SYS_mknod] = sys_mknod,
    [SYS_unlink] = sys_unlink, [SYS_link] = sys_link,
    [SYS_mkdir] = sys_mkdir,   [SYS_close] = sys_close,
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
