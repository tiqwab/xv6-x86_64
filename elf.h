#ifndef XV6_X86_64_ELF_H
#define XV6_X86_64_ELF_H

// Format of an ELF executable file

#include "types.h"

#define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian

// File header
struct elfhdr {
  uint magic;  // must equal ELF_MAGIC
  uchar elf[12]; // CLASS, DATA, VERSION, OSABI, ABIVERSION, PAD
  ushort type;
  ushort machine;
  uint version;
  uintptr_t entry;
  ulong phoff;
  ulong shoff;
  uint flags;
  ushort ehsize;
  ushort phentsize;
  ushort phnum;
  ushort shentsize;
  ushort shnum;
  ushort shstrndx;
};

// Program section header
struct proghdr {
  uint type;
  uint flags;
  ulong off;
  uintptr_t vaddr;
  uintptr_t paddr;
  ulong filesz;
  ulong memsz;
  ulong align;
};

// Values for Proghdr type
#define ELF_PROG_LOAD           1

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4

#endif /* XV6_X86_64_ELF_H */
