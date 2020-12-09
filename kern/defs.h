#ifndef XV6_X86_64_DEFS_H
#define XV6_X86_64_DEFS_H

#include "inc/string.h"
#include "inc/types.h"

struct buf;
struct context;
struct cpu;
struct inode;
struct ioapic;
struct pipe;
struct proc;
struct sleeplock;
struct spinlock;
struct stat;
struct superblock;

// bio.c
void binit(void);
struct buf *bread(uint dev, uint blockno);
void bwrite(struct buf *b);
void brelse(struct buf *b);

// console.c
void consoleinit(void);
void consoleintr(int (*)(void));
void cprintf(char *, ...);
void panic(char *, ...) __attribute__((noreturn));

// exec.c
int exec(char *, char **);

// file.c
struct file *filealloc(void);
void fileclose(struct file *);
struct file *filedup(struct file *);
void fileinit(void);
int fileread(struct file *, char *, int n);
int filestat(struct file *, struct stat *);
int filewrite(struct file *, char *, int n);

// fs.c
void readsb(int dev, struct superblock *sb);
int dirlink(struct inode *, char *, uint);
struct inode *dirlookup(struct inode *, char *, uint *);
struct inode *ialloc(uint, short);
struct inode *idup(struct inode *);
void iinit(int dev);
void ilock(struct inode *);
void iput(struct inode *);
void iunlock(struct inode *);
void iunlockput(struct inode *);
void iupdate(struct inode *);
int namecmp(const char *, const char *);
struct inode *namei(char *);
struct inode *nameiparent(char *, char *);
int readi(struct inode *, char *, uint, uint);
void stati(struct inode *, struct stat *);
int writei(struct inode *, char *, uint, uint);

// ide.c
void ideinit(void);
void ideintr(void);
void iderw(struct buf *b);

// ioapic.c
extern volatile struct ioapic *ioapic;
extern uint8_t ioapicid;
void ioapicenable(int irq, int cpunum);
void ioapicinit(void);

// kbd.c
void kbdintr(void);

// lapic.c
extern volatile uint32_t *lapic;
void lapiceoi(void);
int lapicid(void);
void lapicinit(void);
void microdelay(int us);

// log.c
void initlog(int dev);
void log_write(struct buf *);
void begin_op();
void end_op();

// kalloc.c
char *kalloc(void);
void kfree(char *);
void kinit1(void *vstart);
void kinit2();

// mp.c
void mpinit(void);

// picirq.c
void picinit(void);

// pipe.c
int pipealloc(struct file **, struct file **);
void pipeclose(struct pipe *, int);
int piperead(struct pipe *, char *, int);
int pipewrite(struct pipe *, char *, int);

// proc.c
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
void exit(void);
#pragma GCC diagnostic pop
pid_t fork(void);
int growproc(int n);
struct cpu *mycpu(void);
int cpuid(void);
struct proc *myproc(void);
void sched(void);
void scheduler(void) __attribute__((noreturn));
void sleep(void *chan, struct spinlock *lk);
void userinit(void);
pid_t wait(void);
void wakeup(void *chan);
int kill(pid_t pid);
void yield(void);
void procdump(void);

// spinlock.c
void pushcli(void);
void popcli(void);

// swtch.S
void swtch(struct context **, struct context *);

// syscall.c
int arg(int n, uint64_t *ip);
int argint(int n, int *ip);
int argptr(int n, char **pp, int size);
int argstr(int n, char **pp);
int fetchint(uintptr_t addr, uint64_t *ip);
int fetchstr(uintptr_t addr, char **pp);
void syscall(void);

// trap.c
void idtinit(void);
void tvinit(void);
extern uint ticks;
extern struct spinlock tickslock;

// uart.c
void uartinit(void);
void uartintr(void);
void uartputc(int);

// vm.c
int allocuvm(pte_t *pgdir, size_t oldsz, size_t newsz);
void clearpteu(pte_t *pgdir, char *uva);
int copyout(pte_t *pgdir, uintptr_t va, void *p, size_t len);
pte_t *copyuvm(pte_t *, size_t);
int deallocuvm(pte_t *pgdir, size_t oldsz, size_t newsz);
void freevm(pte_t *pgdir, uintptr_t utop);
void inituvm(pte_t *pgdir, char *init, size_t sz);
void kvmalloc(void);
int loaduvm(pte_t *pgdir, char *addr, struct inode *ip, uint offset, size_t sz);
void seginit(void);
pte_t *setupkvm(void);
void switchkvm(void);
void switchuvm(struct proc *p);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))

#endif /* ifndef XV6_X86_64_DEFS_H */
