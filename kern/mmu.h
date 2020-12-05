#ifndef XV6_X86_64_MMU_H
#define XV6_X86_64_MMU_H

// Eflags register
#define FL_IF 0x00000200 // Interrupt Enable

// various segment selectors.
#define SEG_KCODE 1 // kernel code
#define SEG_KDATA 2 // kernel data+stack
#define SEG_UCODE 3 // user code
#define SEG_UDATA 4 // user data+stack
#define SEG_TSS 5   // this process's task state, this occupies 2 entries

// cpu->gdt[NSEGS] holds the above segments.
// 7 means null, kcode, kdata, ucode, udata and tss (2 entries)
#define NSEGS 7

// __ASSEMBLER__ is defined by gcc when processing assembly files
#ifndef __ASSEMBLER__

#include "inc/types.h"

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

// TSS Descriptor
// in x86-64, the size of TSS descriptor is 16 bytes, not 8.
struct tssdesc {
  // the below is low 8 bytes
  uint32_t lim_15_0 : 16;  // Low bits of segment limit
  uint32_t base_15_0 : 16; // Low bits of segment base address
  uint32_t base_23_16 : 8; // Middle bits of segment base address
  uint32_t type : 4;       // Segment type (see STS_ constants)
  uint32_t s : 1;          // 0 = system, 1 = application
  uint32_t dpl : 2;        // Descriptor Privilege Level
  uint32_t p : 1;          // Present
  uint32_t lim_19_16 : 4;  // High bits of segment limit
  uint32_t avl : 1;        // Unused (available for software use)
  uint32_t l : 1;          // 64-bit code segment (IA-32e mode only)
  uint32_t db : 1; // 0 = 16-bit segment, 1 = 32-bit segment <- this must be
                   // clear in IA-32e mode
  uint32_t g : 1;  // Granularity: limit scaled by 4K when set
  uint32_t base_31_24 : 8; // High bits of segment base address
  // the below is high 8 bytes
  uint32_t base_63_32;
  uint32_t padding1;
};

#define TSSDESC64(type, base, lim, dpl)                                        \
  (struct tssdesc) {                                                           \
    (lim) & 0xffff, (uint32_t)((uint64_t)(base)&0xffff),                       \
        (uint32_t)(((uint64_t)(base) >> 16) & 0xff), type, 0, dpl, 1,          \
        (uint32_t)(lim) >> 16, 0, 0, 0, 0,                                     \
        (uint32_t)(((uint64_t)(base) >> 24) & 0xff),                           \
        (uint32_t)((uint64_t)(base) >> 32), 0                                  \
  }

#endif /* __ASSEMBLER__ */

#define DPL_USER 0x3 // User DPL

// Application segment type bits
#define STA_X 0x8 // Executable segment
#define STA_W 0x2 // Writeable (non-executable segments)
#define STA_R 0x2 // Readable (executable segments)

// System segment type bits
#define STS_T64A 0x9 // Available 64-bit TSS
#define STS_IG64 0xE // 64-bit Interrupt Gate
#define STS_TG64 0xF // 64-bit Trap Gate

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

#define NPTENTRIES 512 // PTES per page table
#define PGSIZE 4096 // bytes mapped by a page

#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

// Page table/directory entry flags.
#define PTE_P 0x001 // Present
#define PTE_W 0x002 // Writeable
#define PTE_U 0x004 // User
#define PTE_PS 0x080 // Page Size

// The page alighned physical address of the frame or the next page table
#define PTE_FLAGS(pte) ((uintptr_t)(pte)&0xFFF)
#define PTE_ADDR(pte) (((((uintptr_t)(pte)) >> 12) & 0xffffffffffff) << 12)

#ifndef __ASSEMBLER__

#include "inc/types.h"

// 64-bit mode task state segment format
// ref. Intel SDM vol.3 Figure 7-11
struct taskstate {
  uint32_t padding1;
  uint32_t rsp0_31_0;
  uint32_t rsp0_63_32;
  uint32_t rsp1_31_0;
  uint32_t rsp1_63_32;
  uint32_t rsp2_31_0;
  uint32_t rsp2_63_32;
  uint32_t padding2;
  uint32_t padding3;
  uint32_t ist1_31_0;
  uint32_t ist1_63_32;
  uint32_t ist2_31_0;
  uint32_t ist2_63_32;
  uint32_t ist3_31_0;
  uint32_t ist3_63_32;
  uint32_t ist4_31_0;
  uint32_t ist4_63_32;
  uint32_t ist5_31_0;
  uint32_t ist5_63_32;
  uint32_t ist6_31_0;
  uint32_t ist6_63_32;
  uint32_t ist7_31_0;
  uint32_t ist7_63_32;
  uint32_t padding4;
  uint32_t padding5;
  uint16_t padding6;
  uint16_t iomb; // I/O map base address
};

// 64-bit IDT gate descriptors for interrupts and traps
// The size is 16 bytes in x86-64, although it was 8 bytes in x86.
// ref. Intel SDM vol.3 Figure 6-7
struct gatedesc {
  uint32_t off_15_0 : 16;  // low 16 bits of offset in segment
  uint32_t cs : 16;        // code segment selector
  uint32_t ist : 3;        // interrupt stack table index
  uint32_t args : 2;       // # args, 0 for interrupt/trap gates
  uint32_t rsv1 : 3;       // reserved(should be zero I guess)
  uint32_t type : 4;       // type(STS_{IG32,TG32})
  uint32_t s : 1;          // must be 0 (system)
  uint32_t dpl : 2;        // descriptor(meaning new) privilege level
  uint32_t p : 1;          // Present
  uint32_t off_31_16 : 16; // middle bits of offset in segment
  uint32_t off_63_32;      // high bits of offset in segment
  uint32_t padding1;
};

// Set up a normal interrupt/trap gate descriptor.
// - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate.
//   interrupt gate clears FL_IF, trap gate leaves FL_IF alone
// - sel: Code segment selector for interrupt/trap handler
// - off: Offset in code segment for interrupt/trap handler
// - dpl: Descriptor Privilege Level -
//        the privilege level required for software to invoke
//        this interrupt/trap gate explicitly using an int instruction.
#define SETGATE(gate, istrap, sel, off, d)                                     \
  {                                                                            \
    (gate).off_15_0 = (uint32_t)((off)&0xffff);                                \
    (gate).cs = (sel);                                                         \
    (gate).ist = 0;                                                            \
    (gate).args = 0;                                                           \
    (gate).rsv1 = 0;                                                           \
    (gate).type = (istrap) ? STS_TG64 : STS_IG64;                              \
    (gate).s = 0;                                                              \
    (gate).dpl = (d);                                                          \
    (gate).p = 1;                                                              \
    (gate).off_31_16 = (uint32_t)((((off) >> 16) & 0xffff));                   \
    (gate).off_63_32 = (uint32_t)(((off) >> 32));                              \
    (gate).padding1 = 0;                                                       \
  }

#endif /* __ASSEMBLER__ */

#endif /* XV6_X86_64_MMU_H */
