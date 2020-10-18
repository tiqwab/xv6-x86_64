#include "defs.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
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
//   0xfe000000..0: mapped direct (devices such as ioapic and lapic)
//
// The kernel allocates physical memory for its heap and for user memory
// between V2P(end) and the end of physical memory (PHYSTOP)
// (directly addressable from end..P2V(PHYSTOP)).

// This table defines the kernel's mappings, which are present in
// every process's page table.
static struct kmap {
  void *virt;
  uintptr_t phys_start;
  uintptr_t phys_end;
  int perm;
} kmap[4];

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

  kmap[3].virt = DEVSPACE_P2V(DEVSPACE_PHYS);
  kmap[3].phys_start = DEVSPACE_PHYS;
  kmap[3].phys_end = 0x100000000;
  kmap[3].perm = PTE_W;
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
  if (phys_top > DEVSPACE_PHYS) {
    panic("PHYSTOP too high");
  }
  for (k = kmap; k < &kmap[NELEM(kmap)]; k++) {
    cprintf("setupkvm for 0x%p\n", k->virt);
    if (mappages(pgdir, k->virt, k->phys_end - k->phys_start,
                 (uint)k->phys_start, k->perm) < 0) {
      freevm(pgdir, 0);
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

// Switch TSS and h/w page table to correspond to process p.
void switchuvm(struct proc *p) {
  if (p == NULL)
    panic("switchuvm: no process");
  if (p->kstack == NULL)
    panic("switchuvm: no kstack");
  if (p->pgdir == NULL)
    panic("switchuvm: no pgdir");

  pushcli();
  *((struct tssdesc *)(&mycpu()->gdt[SEG_TSS])) =
      TSSDESC64(STS_T64A, &mycpu()->ts, sizeof(mycpu()->ts) - 1, 0);
  mycpu()->ts.rsp0_31_0 =
      (uint32_t)((((uintptr_t)p->kstack) + KSTACKSIZE) & 0xffffffff);
  mycpu()->ts.rsp0_63_32 =
      (uint32_t)(((((uintptr_t)p->kstack) + KSTACKSIZE) >> 32) & 0xffffffff);

  // I'm not sure what value iomb should have if we don't want to allow user
  // processes to use I/O port. It should have 0xffff according to:
  // https://stackoverflow.com/questions/54876039/creating-a-proper-task-state-segment-tss-structure-with-and-without-an-io-bitm
  // But rust-osdev set 0 for it.
  // https://github.com/rust-osdev/x86_64/blob/master/src/structures/tss.rs
  //
  // TODO:
  // For x86, 0 is OK for the purpose, so I think 0 is also OK for x86-64, but
  // should check it later.
  //
  // The below comment comes from the original xv6 source.
  // setting IOPL=0 in eflags *and* iomb beyond the tss segment limit
  // forbids I/O instructions (e.g., inb and outb) from user space
  // mycpu()->ts.iomb = (uint16_t) 0xFFFF;
  mycpu()->ts.iomb = (uint16_t)0;

  ltr(SEG_TSS << 3);
  lcr3(V2P(p->pgdir)); // switch to process's address space
  popcli();
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

// Load a program segment into pgdir.  addr must be page-aligned
// and the pages from addr to addr+sz must already be mapped.
int loaduvm(pte_t *pgdir, char *addr, struct inode *ip, uint offset,
            size_t sz) {
  size_t n;
  uintptr_t pa;
  pte_t *pte;

  if (((uintptr_t)addr) % PGSIZE != 0) {
    panic("loaduvm: addr must be page aligned");
  }
  for (size_t i = 0; i < sz; i += PGSIZE) {
    if ((pte = walkpgdir(pgdir, addr + i, 0)) == 0) {
      panic("loaduvm: address should exist");
    }
    pa = PTE_ADDR(*pte);
    if (sz - i < PGSIZE) {
      n = sz - i;
    } else {
      n = PGSIZE;
    }
    if (readi(ip, P2V(pa), offset + i, n) != (int)n) {
      return -1;
    }
  }
  return 0;
}

// Allocate page tables and physical memory to grow process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
int allocuvm(pte_t *pgdir, size_t oldsz, size_t newsz) {
  char *mem;
  uintptr_t a;

  if (newsz >= KERNBASE)
    return 0;
  if (newsz < oldsz)
    return oldsz;

  a = PGROUNDUP(oldsz);
  for (; a < newsz; a += PGSIZE) {
    mem = kalloc();
    if (mem == 0) {
      cprintf("allocuvm out of memory\n");
      deallocuvm(pgdir, newsz, oldsz);
      return 0;
    }
    memset(mem, 0, PGSIZE);
    if (mappages(pgdir, (char *)a, PGSIZE, V2P(mem), PTE_W | PTE_U) < 0) {
      cprintf("allocuvm out of memory (2)\n");
      deallocuvm(pgdir, newsz, oldsz);
      kfree(mem);
      return 0;
    }
  }
  return newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
//
// FIXME: this doesn't deallocate page tables itself.
int deallocuvm(pte_t *pgdir, size_t oldsz, size_t newsz) {
  pte_t *pte;
  uintptr_t a, pa;

  if (newsz >= oldsz) {
    return oldsz;
  }

  a = PGROUNDUP(newsz);
  for (; a < oldsz; a += PGSIZE) {
    pte = walkpgdir(pgdir, (char *)a, 0);
    if (!pte) {
      // cannot skip whole table for now
      continue;
    }
    if (*pte & PTE_P) {
      pa = PTE_ADDR(*pte);
      if (pa == 0) {
        panic("deallocuvm");
      }
      char *v = P2V(pa);
      kfree(v);
      *pte = 0;
    }
  }

  return newsz;
}

// This only free memory used for page tables because user pages are freed by
// deallocuvm.
void do_freevm(pte_t *table, int level) {
  for (int i = 0; i < NPTENTRIES; i++) {
    pte_t *ent = table + i;
    if (*ent & PTE_P) {
      if (level > 1) {
        pte_t *next_table = (pte_t *)P2V(PTE_ADDR(*ent));
        do_freevm(next_table, level - 1);
      }
    }
  }
  kfree((char *)table);
}

// Free a page table and all the physical memory pages
// in the user part.
void freevm(pte_t *pgdir, uintptr_t utop) {
  if (pgdir == 0) {
    panic("freevm: no pgdir");
  }
  deallocuvm(pgdir, utop, 0);
  do_freevm(pgdir, 4);
}

// Given a parent process's page table, create a copy
// of it for a child.
pte_t *copyuvm(pte_t *pgdir, size_t sz) {
  pte_t *d;
  pte_t *pte;
  uintptr_t pa, i;
  uint flags;
  char *mem;

  if ((d = setupkvm()) == 0) {
    return NULL;
  }
  for (i = 0; i < sz; i += PGSIZE) {
    if ((pte = walkpgdir(pgdir, (void *)i, 0)) == 0) {
      panic("copyuvm: pte should exist");
    }
    if (!(*pte & PTE_P)) {
      panic("copyuvm: page not present");
    }
    pa = PTE_ADDR(*pte);
    flags = PTE_FLAGS(*pte);
    if ((mem = kalloc()) == 0) {
      goto bad;
    }
    memmove(mem, (char *)P2V(pa), PGSIZE);
    if (mappages(d, (void *)i, PGSIZE, V2P(mem), flags) < 0) {
      kfree(mem);
      goto bad;
    }
  }
  return d;

bad:
  freevm(d, sz);
  return NULL;
}

// Clear PTE_U on a page. Used to create an inaccessible
// page beneath the user stack.
void clearpteu(pte_t *pgdir, char *uva) {
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
  if (pte == 0) {
    panic("clearpteu");
  }
  *pte &= ~PTE_U;
}

// Map user virtual address to kernel address.
char *uva2ka(pte_t *pgdir, char *uva) {
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
  if ((*pte & PTE_P) == 0)
    return 0;
  if ((*pte & PTE_U) == 0)
    return 0;
  return (char *)P2V(PTE_ADDR(*pte));
}

// Copy len bytes from p to user address va in page table pgdir.
// Most useful when pgdir is not the current page table.
// uva2ka ensures this only works for PTE_U pages.
int copyout(pte_t *pgdir, uintptr_t va, void *p, size_t len) {
  char *buf, *pa0;
  size_t n;
  uintptr_t va0;

  buf = (char *)p;
  while (len > 0) {
    va0 = (uintptr_t)PGROUNDDOWN(va);
    pa0 = uva2ka(pgdir, (char *)va0);
    if (pa0 == 0)
      return -1;
    n = PGSIZE - (va - va0);
    if (n > len)
      n = len;
    memmove(pa0 + (va - va0), buf, n);
    len -= n;
    buf += n;
    va = va0 + PGSIZE;
  }
  return 0;
}
