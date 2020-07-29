/*
 * This file contains definitions for memory management in our OS,
 * which are relevant to both the kernel and user-mode software.
 *
 * The base file is kern/memlayout.h, so refer to it for more information.
 */

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0xffff800000000000 // First kernel virtual address

#define V2P_WO(x) ((x)-KERNBASE)   // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE) // same as P2V, but without casts
