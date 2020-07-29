# ref. stage_3.S in bootloader crate. See `COPYRIGHT` for license information.

.text

.global stage_3
stage_3:

set_up_page_table:
  #for p4
  lea (p4),%edx

  # entry 0 for identity mapping (1 GiB from 0x0)
  lea (p3_1),%eax
  or $0x3,%eax # present, writable
  mov $0,%ecx
  mov %eax,(%edx,%ecx,8)

  # entry 511 for kernel mapping (0xff8000000000)
  lea (p3_2),%eax
  or $0x3,%eax # present, writable
  mov $511,%ecx
  mov %eax,(%edx,%ecx,8)

  # for p3_1
  lea (p3_1),%edx
  xor %eax,%eax
  or $0x83,%eax # present, writable, huge page
  mov %eax,(%edx)

  # for p3_2
  # entry 510 ((0xffff80000000 - 0xff8000000000) / 1 GiB)
  lea (p3_2),%edx
  xor %eax,%eax
  or $0x83,%eax # present, writable, huge page
  mov $510,%ecx
  mov %eax,(%edx,%ecx,8)

# ref. https://wiki.osdev.org/Setting_Up_Long_Mode
enable_paging:
    # Write back cache and add a memory fence. I'm not sure if this is
    # necessary, but better be on the safe side.
    wbinvd
    mfence

    # load P4 to cr3 register (cpu uses this to access the P4 table)
    lea (p4),%eax
    mov %eax,%cr3

    # enable PAE-flag in cr4 (Physical Address Extension)
    mov %cr4,%eax
    or $0x20,%eax
    mov %eax,%cr4

    # set the long mode bit in the EFER MSR (model specific register)
    mov $0xC0000080,%ecx
    rdmsr
    or $0x100,%eax
    wrmsr

    # enable paging in the cr0 register
    mov %cr0,%eax
    or $0x80000000,%eax
    mov %eax,%cr0

load_64bit_gdt:
    lgdt gdt64desc                # Load GDT.Pointer defined below.

jump_to_long_mode:
    push $0x8
    mov kernel_entry,%eax
    push %eax
    retf # Load CS with 64 bit segment and flush the instruction cache

spin64:
  hlt
  jmp spin64

.p2align 2
gdt64:
    .quad 0x0000000000000000      # Null Descriptor - should be present.
    .quad 0x00209A0000000000      # 64-bit code descriptor (exec/read).
    .quad 0x0000920000000000      # 64-bit data descriptor (read/write).

gdt64desc:
    .word gdt64desc - gdt64 - 1    # 16-bit Size (Limit) of GDT.
    .long gdt64                   # 32-bit Base Address of GDT. (CPU will zero extend to 64-bit)
