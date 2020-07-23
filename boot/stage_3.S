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

loop:
  hlt
  jmp loop
