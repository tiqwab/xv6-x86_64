// This file contains definitions for the
// x86 memory management unit (MMU).

// Control Register flags
// See Intel SDM Vol.3 2.5 CONTROL REGISTERS

// CR0 is almost same as i386
#define CR0_PE 0x00000001 // Protection Enable
#define CR0_WP 0x00010000 // Write Protect
#define CR0_PG 0x80000000 // Paging

// various segment selectors.
#define SEG_KCODE 1 // kernel code
#define SEG_KDATA 2 // kernel data+stack

// __ASSEMBLER__ is defined by gcc when processing assembly files
#ifdef __ASSEMBLER__

#define SEG_NULL                                                               \
  .word 0, 0;                                                                  \
  .byte 0, 0, 0, 0

// The 0xC0 means the limit is in 4096-byte units (G bit)
// and (for executable segments) 32-bit mode (D/B bit).
// See Intel SDM Vol.3 3.4.5 Segment Descriptors
#define SEG(type, base, lim)                                                   \
  .word(((lim) >> 12) & 0xffff), ((base)&0xffff);                              \
  .byte(((base) >> 16) & 0xff), (0x90 | (type)),                               \
      (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#endif

// Application segment type bits
#define STA_X 0x8 // Executable segment
#define STA_W 0x2 // Writeable (non-executable segments)
#define STA_R 0x2 // Readable (executable segments)
