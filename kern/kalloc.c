#include "defs.h"
#include "kclock.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

static uintptr_t phys_temporary_top = 4 * 1024 * 1024;
uintptr_t phys_top;

static int nvram_read(int r) {
  return mc146818_read(r) | (mc146818_read(r + 1) << 8);
}

// Detect machine's physical memory setup.
static void detect_memory(void) {
  size_t basemem, extmem, ext16mem, totalmem;

  // Use CMOS calls to measure available base & extended memory.
  // (CMOS calls return results in kilobytes.)
  basemem = nvram_read(NVRAM_BASELO);
  extmem = nvram_read(NVRAM_EXTLO);
  ext16mem = nvram_read(NVRAM_EXT16LO) * 64;

  // Calculate the number of physical poages available in both base
  // and extended memory.
  if (ext16mem)
    totalmem = 16 * 1024 + ext16mem;
  else if (extmem)
    totalmem = 1 * 1024 + extmem;
  else
    totalmem = basemem;

  cprintf("Available memory size is %dK\n", totalmem);
  phys_top = ((uintptr_t)totalmem * 1024) / PGSIZE * PGSIZE;
}

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void kinit1(void *vstart) {
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  detect_memory();
  freerange(vstart, (void *)P2V(phys_temporary_top));
}

void kinit2() {
  cprintf("[kinit2] vstart: 0x%p, vend: 0x%p\n", phys_temporary_top, phys_top);
  freerange((void *)P2V(phys_temporary_top), (void *)P2V(phys_top));
  kmem.use_lock = 1;
}

void freerange(void *vstart, void *vend) {
  char *p;
  p = (char *)PGROUNDUP((uintptr_t)vstart);
  for (; p + PGSIZE <= (char *)vend; p += PGSIZE) {
    // cprintf("kfree 0x%p\n", p);
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(char *v) {
  struct run *r;

  if ((uintptr_t)v % PGSIZE || v < end || V2P(v) >= phys_top)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if (kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run *)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  if (kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char *kalloc(void) {
  struct run *r;

  if (kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if (r)
    kmem.freelist = r->next;
  if (kmem.use_lock)
    release(&kmem.lock);
  return (char *)r;
}
