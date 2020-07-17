# Start the first CPU: switch to 32-bit protected mode, jump into C.
# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

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

loop:
  hlt
  jmp loop
