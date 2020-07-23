# ref. stage_3.S in bootloader crate. See `COPYRIGHT` for license information.

.text

.global set_up_page_table
set_up_page_table:
  #for p4
  lea (p4),%edx

  # entry 0 for identity mapping (1 GiB from 0x0)
  lea (p3_1),%eax
  or $0x3,%eax # present, writable
  mov $0,%ecx
  mov %eax,(%edx,%ecx,8)

  # entry 256 for kernel mapping (1 GiB from 0xffff800000000000)
  lea (p3_2),%eax
  or $0x3,%eax # present, writable
  mov $256,%ecx
  mov %eax,(%edx,%ecx,8)

  # for p3_1
  lea (p3_1),%edx
  xor %eax,%eax
  or $0x83,%eax # present, writable, huge page
  mov %eax,(%edx)

  # for p3_2
  lea (p3_2),%edx
  xor %eax,%eax
  or $0x83,%eax # present, writable, huge page
  mov %eax,(%edx)

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

loop:
  hlt
  jmp loop
