#ifndef XV6_X86_64_TRAP_H
#define XV6_X86_64_TRAP_H

// These are arbitrarily chosen, but with care not to overlap
// processor defined exceptions or interrupt vectors.
#define T_SYSCALL 0x30 // system call

#define T_IRQ0 32 // IRQ 0 corresponds to int T_IRQ

#define IRQ_TIMER 0
#define IRQ_KBD 1
#define IRQ_COM1 4
#define IRQ_ERROR 19
#define IRQ_SPURIOUS 31

#endif /* ifndef XV6_X86_64_DEFS_H */
