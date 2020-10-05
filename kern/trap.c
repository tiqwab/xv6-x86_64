#include "trap.h"
#include "defs.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "x86.h"

// Interrut descriptor table (shared by all CPUs).

struct gatedesc idt[256];
extern uintptr_t vectors[]; // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void tvinit(void) {
  int i;

  for (i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE << 3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE << 3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void idtinit(void) { lidt(idt, sizeof(idt)); }

void trap(struct trapframe *tf) {
  if (tf->trapno == T_SYSCALL) {
    if (myproc()->killed) {
      exit();
    }
    myproc()->tf = tf;
    syscall();
    if (myproc()->killed) {
      exit();
    }
    return;
  }

  switch (tf->trapno) {
  case T_IRQ0 + IRQ_TIMER:
    // interval of ticks is 10ms in QEMU.
    // it depends on bus frequency according to https://wiki.osdev.org/APIC_timer.
    // bus frequency is always 1 GHz in QEMU?: https://www.mail-archive.com/qemu-devel@nongnu.org/msg711610.html
    if (cpuid() == 0) {
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;

  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE + 1:
    // Bochs generates spurious IDE1 interrupts.
    break;

  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;

  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;

    // case T_IRQ0 + 7:
    // case T_IRQ0 + IRQ_SPURIOUS:
    //   cprintf("cpu%d: spurious interrupt at %x:%x\n",
    //           cpuid(), tf->cs, tf->eip);
    //   lapiceoi();
    //   break;

  default:
    if (myproc() == NULL || (tf->cs & 3) == 0) {
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d rip %x (cr2=0x%x)\n", tf->trapno,
              cpuid(), tf->rip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d ", myproc()->pid,
            myproc()->name, tf->trapno, tf->err, cpuid());
    cprintf("rip 0x%x addr 0x%x kill proc\n", tf->rip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if (myproc() && myproc()->killed && (tf->cs & 3) == DPL_USER) {
    exit();
  }

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if (myproc() && myproc()->state == RUNNING &&
      tf->trapno == T_IRQ0 + IRQ_TIMER) {
    yield();
  }

  // Check if the process has been killed since we yielded
  if (myproc() && myproc()->killed && (tf->cs & 3) == DPL_USER) {
    exit();
  }
}
