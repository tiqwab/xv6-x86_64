#ifndef XV6_X86_64_MEMLAYOUT_H
#define XV6_X86_64_MEMLAYOUT_H

/*
 * This file contains definitions for memory management in our OS,
 * which are relevant to both the kernel and user-mode software.
 */

#define EXTMEM 0x100000   // Start of extended memory
#define PHYSTOP 0xE000000 // Top physical memory

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0xffffffff80000000  // First kernel virtual address
#define KERNLINK (KERNBASE + EXTMEM) // Address where kernel is linked

// __ASSEMBLER__ is defined by gcc when processing assembly files
#ifndef __ASSEMBLER__

#include "types.h"

#define V2P(a) (((uintptr_t)(a)) - KERNBASE)
#define P2V(a) ((void *)(((char *)(a)) + KERNBASE))

#endif /* __ASSEMBLER__ */

#define V2P_WO(x) ((x)-KERNBASE)   // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE) // same as P2V, but without casts

/*
 * Virtual memory map:                                Permissions
 *                                                    kernel/user
 *
 *    256 TB --------> +------------------------------+
 *                     |                              | RW/--
 *                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                     :                              : RW/--
 *                     :   remapped physical memory   : RW/--
 *                     :                              : RW/--
 *     KERNLINK ---->  :                              : 0xffffffff80100000
 * (kernel entry point) :                              : RW/-- KERNBASE ---->
 * |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| 0xffffffff80000000 :              . : : . :
 *                     :              .               :
 *  start of    ---->  +------------------------------+ 0xffff800000000000
 *  kernel space
 *
 *
 */

#endif /* XV6_X86_64_MEMLAYOUT_H */
