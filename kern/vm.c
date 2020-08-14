#include "defs.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "types.h"
#include "x86.h"

extern char data[]; // defined by kernel.ld
pte_t *kpgdir;      // for use in scheduler()

extern uintptr_t phys_top;

// There is one page table per process, plus one that's used when
// a CPU is not running any process (kpgdir). The kernel uses the
// current process's page table during system calls and interrupts;
// page protection bits prevent user code from using the kernel's
// mappings.
//
// setupkvm() and exec() set up every page table like this:
//
//   0..KERNBASE: user memory (text+data+stack+heap), mapped to
//                phys memory allocated by the kernel
//   KERNBASE..KERNBASE+EXTMEM: mapped to 0..EXTMEM (for I/O space)
//   KERNBASE+EXTMEM..data: mapped to EXTMEM..V2P(data)
//                for the kernel's instructions and r/o data
//   data..KERNBASE+PHYSTOP: mapped to V2P(data)..PHYSTOP,
//                                  rw data + free physical memory
//   0xfe000000..0: mapped direct (devices such as ioapic)
//
// The kernel allocates physical memory for its heap and for user memory
// between V2P(end) and the end of physical memory (PHYSTOP)
// (directly addressable from end..P2V(PHYSTOP)).

// This table defines the kernel's mappings, which are present in
// every process's page table.
// TODO for DEVSPACE (kmap[3] -> kmap[4])
static struct kmap {
  void *virt;
  uint phys_start;
  uint phys_end;
  int perm;
} kmap[3];

// Initialize kmap.
// We couldn't initilize it at load time...:
// kern/memlayout.h:21:16: error: initializer element is not computable at load
// time
//   21 | #define V2P(a) (((uintptr_t)(a)) - KERNBASE)
void init_kmap(void) {

  // I/O space
  kmap[0].virt = (void *)KERNBASE;
  kmap[0].phys_start = 0;
  kmap[0].phys_end = EXTMEM;
  kmap[0].perm = PTE_W;

  // kern text+rodata
  kmap[1].virt = (void *)KERNLINK;
  kmap[1].phys_start = V2P(KERNLINK);
  kmap[1].phys_end = V2P(data);
  kmap[1].perm = 0;

  // kern data+memory
  kmap[2].virt = (void *)data;
  kmap[2].phys_start = V2P(data);
  kmap[2].phys_end = phys_top;
  kmap[2].perm = PTE_W;

  // TODO for DEVSPACE
  // kmap[3] = {(void *)DEVSPACE, DEVSPACE, 0, PTE_W},          // more devices
}

// Set up CPU's kernel segment descriptors.
// Run once on entry on each CPU.
void seginit(void) {
  struct cpu *c;

  // Map "logical" addresses to virtual addresses using identity map.
  // Cannot share a CODE descriptor for both kernel and user
  // because it would have to have DPL_USR, but the CPU forbids
  // an interrupt from CPL=0 to DPL=3.
  c = &cpus[cpuid()];
  c->gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0, 0xffffffff, 0);
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  c->gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0, 0xffffffff, DPL_USER);
  c->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);
  lgdt(c->gdt, sizeof(c->gdt));
}

static uint ptx(const void *va, int level) {
  switch (level) {
  case 4:
    return PTX4(va);
  case 3:
    return PTX3(va);
  case 2:
    return PTX2(va);
  case 1:
    return PTX1(va);
  default:
    return -1;
  }
}

static pte_t *__attribute__((noinline))
do_walk(pte_t *table, const void *va, int alloc, int level) {
  if (level == 1) {
    return &table[ptx(va, level)];
  }

  pte_t *next_table;
  pte_t *pte = &table[ptx(va, level)];
  if (*pte & PTE_P) {
    next_table = (pte_t *)P2V(PTE_ADDR(*pte));
  } else {
    if (!alloc || (next_table = (pte_t *)kalloc()) == 0) {
      return NULL;
    }
    // Make sure all those PTE_P bits are zero.
    memset(next_table, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table
    // entries, if necessary.
    *pte = V2P(next_table) | PTE_P | PTE_W | PTE_U;
  }
  return do_walk(next_table, va, alloc, level - 1);
}

// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
static pte_t *__attribute__((noinline))
walkpgdir(pte_t *pgdir, const void *va, int alloc) {
  return do_walk(pgdir, va, alloc, 4);
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned.
static int __attribute__((noinline))
mappages(pte_t *pgdir, void *va, uint size, uintptr_t pa, int perm) {
  char *a, *last;
  pte_t *pte;

  a = (char *)PGROUNDDOWN((uintptr_t)va);
  last = (char *)PGROUNDDOWN(((uintptr_t)va) + size - 1);
  for (;;) {
    if ((pte = walkpgdir(pgdir, a, 1)) == 0)
      return -1;
    if (*pte & PTE_P)
      panic("remap");
    *pte = pa | perm | PTE_P;
    if (a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

// Set up kernel part of a page table.
pte_t *setupkvm(void) {
  pte_t *pgdir;
  struct kmap *k;

  if ((pgdir = (pte_t *)kalloc()) == 0)
    return 0;
  memset(pgdir, 0, PGSIZE);
  // TODO for DEVSPACE
  // if (P2V(PHYSTOP) > (void *)DEVSPACE)
  //   panic("PHYSTOP too high");
  for (k = kmap; k < &kmap[NELEM(kmap)]; k++) {
    cprintf("setupkvm for 0x%p\n", k->virt);
    if (mappages(pgdir, k->virt, k->phys_end - k->phys_start,
                 (uint)k->phys_start, k->perm) < 0) {
      freevm(pgdir);
      return 0;
    }
  }
  return pgdir;
}

// Allocate one page table for the machine for the kernel address
// space for scheduler processes.
void kvmalloc(void) {
  init_kmap();
  kpgdir = setupkvm();
  switchkvm();
}

// Switch h/w page table register to the kernel-only page table,
// for when no process is running.
void switchkvm(void) {
  lcr3(V2P(kpgdir)); // switch to the kernel page table
}

// Load the initcode into address 0 of pgdir.
// sz must be less than a page.
void inituvm(pte_t *pgdir, char *init, size_t sz) {
  char *mem;

  if (sz >= PGSIZE)
    panic("inituvm: more than a page");
  mem = kalloc();
  memset(mem, 0, PGSIZE);
  mappages(pgdir, 0, PGSIZE, V2P(mem), PTE_W | PTE_U);
  memmove(mem, init, sz);
}

// Free a page table and all the physical memory pages
// in the user part.
void freevm(pte_t *pgdir) { panic("should implement freevm"); }
