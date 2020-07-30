#ifndef XV6_X86_64_MMU_H
#define XV6_X86_64_MMU_H

#define PGSIZE 4096 // bytes mapped by a page

#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

#endif /* XV6_X86_64_MMU_H */
