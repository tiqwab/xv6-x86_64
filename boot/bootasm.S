#include "mmu.h"

# Start the first CPU: switch to 32-bit protected mode, jump into C.
# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

.section .boot-first-stage, "awx"
.code16                       # Assemble for 16-bit mode
.globl start
start:
  cli                         # BIOS enabled interrupts; disable

  # Zero data segment registers DS, ES, and SS.
  xorw    %ax,%ax             # Set %ax to zero
  movw    %ax,%ds             # -> Data Segment
  movw    %ax,%es             # -> Extra Segment
  movw    %ax,%ss             # -> Stack Segment

  # clear the direction flag (e.g. go forward in memory when using
  # instructions like lodsb)
  cld

  # initialize stack
  mov $0x7c00,%esp

  # Physical address line A20 is tied to zero so that the first PCs
  # with 2 MB would run software that assumed 1 MB.  Undo that.
seta20.1:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al
  jnz     seta20.1

  movb    $0xd1,%al               # 0xd1 -> port 0x64
  outb    %al,$0x64

seta20.2:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al
  jnz     seta20.2

  movb    $0xdf,%al               # 0xdf -> port 0x60
  outb    %al,$0x60

# This code comes from rust-osdev/bootloader.
# https://github.com/rust-osdev/bootloader/blob/b263840a631ea8f5cc5e15d965ea295acbfe456a/src/stage_2.s#L22
set_target_operating_mode:
  # Some BIOSs assume the processor will only operate in Legacy Mode. We change the Target
  # Operating Mode to "Long Mode Target Only", so the firmware expects each CPU to enter Long Mode
  # once and then stay in it. This allows the firmware to enable mode-specifc optimizations.
  # We save the flags, because CF is set if the callback is not supported (in which case, this is
  # a NOP)
  #
  # See also: https://forum.osdev.org/viewtopic.php?f=1&t=20445
  pushf
  mov $0xec00,%ax
  mov $0x2,%bl
  int $0x15
  popf

# See http://oswiki.osask.jp/?%28AT%29BIOS or 14-2 in 30 日 OS 自作入門
# configure_video_mode:
  mov $0x0,%ah
  # mov $0x13,%al # 320x200 256 color graphics
  mov $0x03,%al # 80x25 16 color text
  int $0x10

# check_int13h_extensions:
#     mov $0x41,%ah
#     mov $0x55aa,%bx
#     # dl contains drive number
#     int $0x13
#     jc no_int13h_extensions

load_rest_of_bootloader_from_disk:
  lea (_rest_of_bootloader_start_addr),%eax

  # start of memory buffer
  mov %ax,(dap_buffer_addr)

  # number of disk blocks to load
  lea (_rest_of_bootloader_end_addr),%ebx
  sub %eax,%ebx # end - start
  shr $9,%ebx # divide by 512 (block size)
  mov %bx,(dap_blocks)

  # number of start block
  lea (start),%ebx
  sub %ebx,%eax
  shr $9,%eax # divide by 512 (block size)
  mov %eax,(dap_start_lba)

  lea (dap),%si
  mov $0x42, %ah
  # dl should contain drive number when boot.
  # ref. https://stackoverflow.com/questions/11174399/pc-boot-dl-register-and-drive-number
  # mov $0x80, %dl
  int $0x13
  jc rest_of_bootloader_load_failed

print_test:
  mov $0x13,%ah
  mov $0x1,%al
  mov $0x0,%bh
  mov $0x7,%bl
  mov $0x0010,%cx
  mov $0x0,%dl
  mov $0x0,%dh
  lea (str),%bp
  int $0x10

  # Switch from real to protected mode.  Use a bootstrap GDT that makes
  # virtual addresses map directly to physical addresses so that the
  # effective memory map doesn't change during the transition.
  lgdt    gdtdesc
  movl    %cr0,%eax
  orl     $CR0_PE,%eax
  movl    %eax,%cr0

  # Complete the transition to 32-bit protected mode by using a long jmp
  # to reload %cs and %eip.  The segment descriptors are set up with no
  # translation, so that the mapping is still the identity mapping.
  ljmp    $(SEG_KCODE<<3), $start32

rest_of_bootloader_load_failed:
  mov $0x13,%ah
  mov $0x1,%al
  mov $0x0,%bh
  mov $0x7,%bl
  mov $0x0020,%cx
  mov $0x0,%dl
  mov $0x0,%dh
  lea (rest_of_bootloader_load_failed_str),%bp
  int $0x10

spin16:
  hlt
  jmp spin16

.code32  # Tell assembler to generate 32-bit code now.
start32:
  # Set up the protected-mode data segment registers
  movw    $(SEG_KDATA<<3), %ax    # Our data segment selector
  movw    %ax, %ds                # -> DS: Data Segment
  movw    %ax, %es                # -> ES: Extra Segment
  movw    %ax, %ss                # -> SS: Stack Segment
  movw    $0, %ax                 # Zero segments not ready for use
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS

  # Set up the stack pointer and call into C.
  movl    $start, %esp
  call    bootmain

spin32:
  hlt
  jmp     spin32

str:
  .asciz "hello_xv6-x86_64"
rest_of_bootloader_load_failed_str:
  .asciz "rest_of_bootloader_load failed!!"

# Bootstrap GDT
.p2align 2                            # force 4 byte alignment
gdt:
  SEG_NULL                            # null seg
  SEG(STA_X|STA_R, 0x0, 0xffffffff)   # code seg
  SEG(STA_W, 0x0, 0xffffffff)         # data seg

gdtdesc:
  .word   (gdtdesc - gdt - 1)         # sizeof(gdt) - 1
  .long   gdt                         # address gdt

dap: # disk access packet
  .byte 0x10 # size of dap
  .byte 0 # unused
dap_blocks:
  .word 0 # number of sectors
dap_buffer_addr:
  .word 0 # offset to memory buffer
dap_buffer_seg:
  .word 0 # segment of memory buffer
dap_start_lba:
  .quad 0 # start logical block address

.org 510
.word 0xaa55 # magic number for bootable disk
