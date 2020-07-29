// Boot loader.
//
// Part of the boot block, along with stage_1.S, which calls stage_2().
// stage_1.S has put the processor into protected 32-bit mode.
// stage_2() loads an ELF kernel image from the disk and then jumps to the
// stage_3.

#include "elf.h"
#include "types.h"
#include "x86.h"

#define SECTSIZE 512
#define KERNEL_START_PHYS_ADDR 0x10000
// TODO: duplicated with KERNEL_START_SECTOR in Makefile
#define KERNEL_START_SECTOR 32

void (*kernel_entry)(void);

void readseg(uchar *, uint, uint);
void stage_3(void);

void stage_2(void) {
  struct elfhdr *elf;
  struct proghdr *ph, *eph;
  uchar *pa;

  elf = (struct elfhdr *)KERNEL_START_PHYS_ADDR; // scratch space

  // Read 1st page off disk
  readseg((uchar *)elf, 4096, 0);

  // Is this an ELF executable?
  if (elf->magic != ELF_MAGIC)
    return; // let stage_1.S handle error

  // Load each program segment (ignores ph flags).
  ph = (struct proghdr *)((uchar *)elf + elf->phoff);
  eph = ph + elf->phnum;
  for (; ph < eph; ph++) {
    pa = (uchar *)((uint32_t)ph->paddr);
    readseg(pa, ph->filesz, ph->off);
    if (ph->memsz > ph->filesz)
      stosb(pa + ph->filesz, 0, ph->memsz - ph->filesz);
  }

  kernel_entry = (void (*)(void))((uint32_t)elf->entry);

  stage_3();
}

void waitdisk(void) {
  // Wait for disk ready.
  while ((inb(0x1F7) & 0xC0) != 0x40)
    ;
}

// Read a single sector at offset into dst.
void readsect(void *dst, uint offset) {
  // Issue command.
  waitdisk();
  outb(0x1F2, 1); // count = 1
  outb(0x1F3, offset);
  outb(0x1F4, offset >> 8);
  outb(0x1F5, offset >> 16);
  outb(0x1F6, (offset >> 24) | 0xE0);
  outb(0x1F7, 0x20); // cmd 0x20 - read sectors

  // Read data.
  waitdisk();
  insl(0x1F0, dst, SECTSIZE / 4);
}

// Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
// Might copy more than asked.
void readseg(uchar *pa, uint count, uint offset) {
  uchar *epa;

  epa = pa + count;

  // Round down to sector boundary.
  pa -= offset % SECTSIZE;

  // Translate from bytes to sectors; kernel starts at sector 1.
  offset = (offset / SECTSIZE) + KERNEL_START_SECTOR;

  // If this is too slow, we could read lots of sectors at a time.
  // We'd write more to memory than asked, but it doesn't matter --
  // we load in increasing order.
  for (; pa < epa; pa += SECTSIZE, offset++)
    readsect(pa, offset);
}
