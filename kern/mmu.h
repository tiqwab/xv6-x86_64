#ifndef XV6_X86_64_MMU_H
#define XV6_X86_64_MMU_H

// Eflags register
#define FL_IF 0x00000200 // Interrupt Enable

#define PGSIZE 4096 // bytes mapped by a page

#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

#endif /* XV6_X86_64_MMU_H */
