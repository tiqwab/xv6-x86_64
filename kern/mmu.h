#ifndef XV6_X86_64_MMU_H
#define XV6_X86_64_MMU_H

// Eflags register
#define FL_IF 0x00000200 // Interrupt Enable

// A virtual address 'la' has a five-part structure as follows:
//
// +--------10------+-------10--------+--------10------+-------10-------+---------12----------+
// |   Page Table   |   Page Table    |   Page Table   |   Page Table   | Offset
// within Page  | |    L4 Index    |    L3 Index     |    L2 Index    |    L1
// Index    |                     |
// +----------------+-----------------+----------------+----------------+---------------------+
//  \--- PTX4(va) --/ \--- PTX3(va) --\--- PTX2(va) --/ \--- PTX1(va) --/

#define PTX4(va) ((((uintptr_t)va) >> 12 >> 9 >> 9 >> 9) & 0777)
#define PTX3(va) ((((uintptr_t)va) >> 12 >> 9 >> 9) & 0777)
#define PTX2(va) ((((uintptr_t)va) >> 12 >> 9) & 0777)
#define PTX1(va) ((((uintptr_t)va) >> 12) & 0777)

#define PGSIZE 4096 // bytes mapped by a page

#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

// Page table/directory entry flags.
#define PTE_P 0x001  // Present
#define PTE_W 0x002  // Writeable
#define PTE_U 0x004  // User
#define PTE_PS 0x080 // Page Size

// The page alighned physical address of the frame or the next page table
#define PTE_ADDR(pte) (((((uintptr_t)(pte)) >> 12) & 0xffffffffffff) << 12)

#endif /* XV6_X86_64_MMU_H */
