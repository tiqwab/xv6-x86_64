#ifndef XV6_X86_64_MMU_H
#define XV6_X86_64_MMU_H

#include "types.h"

// Eflags register
#define FL_IF 0x00000200 // Interrupt Enable

// various segment selectors.
#define SEG_KCODE 1 // kernel code
#define SEG_KDATA 2 // kernel data+stack
#define SEG_UCODE 3 // user code
#define SEG_UDATA 4 // user data+stack
#define SEG_TSS 5   // this process's task state

// cpu->gdt[NSEGS] holds the above segments.
#define NSEGS 6

// Segment Descriptor
// in x86-64, only type, s, dpl, p, and l is effective (other fields are
// ignored).
struct segdesc {
  uint lim_15_0 : 16;  // Low bits of segment limit
  uint base_15_0 : 16; // Low bits of segment base address
  uint base_23_16 : 8; // Middle bits of segment base address
  uint type : 4;       // Segment type (see STS_ constants)
  uint s : 1;          // 0 = system, 1 = application
  uint dpl : 2;        // Descriptor Privilege Level
  uint p : 1;          // Present
  uint lim_19_16 : 4;  // High bits of segment limit
  uint avl : 1;        // Unused (available for software use)
  uint l : 1;          // 64-bit code segment (IA-32e mode only)
  uint db : 1; // 0 = 16-bit segment, 1 = 32-bit segment <- this must be clear
               // in IA-32e mode
  uint g : 1;  // Granularity: limit scaled by 4K when set
  uint base_31_24 : 8; // High bits of segment base address
};

// Normal segment
#define SEG(type, base, lim, dpl)                                              \
  (struct segdesc) {                                                           \
    ((lim) >> 12) & 0xffff, (uint)(base)&0xffff, ((uint)(base) >> 16) & 0xff,  \
        type, 1, dpl, 1, (uint)(lim) >> 28, 0, 1, 0, 1, (uint)(base) >> 24     \
  }

#define DPL_USER 0x3 // User DPL

// Application segment type bits
#define STA_X 0x8 // Executable segment
#define STA_W 0x2 // Writeable (non-executable segments)
#define STA_R 0x2 // Readable (executable segments)

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
