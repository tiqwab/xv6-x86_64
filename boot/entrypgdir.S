.section .entrypgdir, "aw"

# Level4 Page Table
.p2align 12
.global p4
p4:
  .quad 0x0 # entry 0 for identity mapping (1 GiB from 0x0)
  .fill 255,8,0x0
  .quad 0x0 # entry 256 for kernel mapping (1 GiB from 0xffff800000000000)
  .fill 255,8,0x0

# Level3 Page Table for identity mapping
.p2align 12
.global p3_1
p3_1:
  .quad 0x0
  .fill 511,8,0x0

# Level3 Page Table for kernel mapping
.p2align 12
.global p3_2
p3_2:
  .quad 0x0
  .fill 511,8,0x0
