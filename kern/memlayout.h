#ifndef XV6_X86_64_MEMLAYOUT_H
#define XV6_X86_64_MEMLAYOUT_H

/*
 * This file contains definitions for memory management in our OS,
 * which are relevant to both the kernel and user-mode software.
 */

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0xffff800000000000         // First kernel virtual address

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as P2V, but without casts

/*
 * Virtual memory map:                                Permissions
 *                                                    kernel/user
 *
 *    256 TB --------> +------------------------------+
 *                     |                              | RW/--
 *                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                     :              .               :
 *                     :              .               :
 *                     :              .               :
 *                     |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| RW/--
 *                     |                              | RW/--
 *                     |   Remapped Physical Memory   | RW/--
 *                     |                              | RW/--
 *    KERNBASE  ---->  +------------------------------+ 0xffff800000000000
 *
 */

#endif /* XV6_X86_64_MEMLAYOUT_H */
