# Try to infer the correct QEMU
ifndef QEMU
	QEMU = qemu-system-x86_64
endif

CC = gcc
AS = gas
LD = ld
OBJCOPY = objcopy
OBJDUMP = objdump
CP = cp
DD = dd
GDB = gdb

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -MD -ggdb -m64 -fno-omit-frame-pointer
CFLAGS += -O2 -std=c11 -Wall -Wextra -Wno-format -Wno-unused -Wno-address-of-packed-member -Werror
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
# ASFLAGS = -m32 -gdwarf-2 -Wa,-divide
LDFLAGS += -m elf_x86_64

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
	CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
	CFLAGS += -fno-pie -nopie
endif

GDBPORT	:= 12345
CPUS ?= 1

default: xv6.img

OBJS := 

xv6.img: bootblock kernel
	dd if=/dev/zero of=$@ count=10000
	dd if=bootblock of=$@ conv=notrunc
	dd if=kernel of=$@ seek=1 conv=notrunc

kernel: $(OBJS) entry.o kernel.ld
	$(LD) $(LDFLAGS) -T kernel.ld -o kernel entry.o $(OBJS)
	$(OBJDUMP) -S kernel > kernel.asm

# boot/base.img is 512 bytes empty file except for last 0x55 0xAA. This is MBR format.
bootblock: bootasm.S bootmain.c base.img
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -I. -c bootmain.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -c bootasm.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o bootblock.o bootasm.o bootmain.o
	$(OBJDUMP) -S bootblock.o > bootblock.asm
	$(OBJCOPY) -S -O binary -j .text bootblock.o bootblock.bin
	$(CP) base.img $@
	$(DD) conv=notrunc if=$@.bin of=$@

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: %.o

-include *.d

# Enter QEMU monitor by 'Ctrl+a then c' if -serial mon:stdio is specified
# ref. https://kashyapc.wordpress.com/2016/02/11/qemu-command-line-behavior-of-serial-stdio-vs-serial-monstdio/
QEMUOPTS := $(QEMUOPTS)
QEMUOPTS += -drive file=bootblock,index=0,media=disk,format=raw -serial mon:stdio -gdb tcp::$(GDBPORT) -smp $(CPUS)
QEMUOPTS += $(shell if $(QEMU) -nographic -help | grep -q '^-D '; then echo '-D qemu.log'; fi)

.gdbinit: .gdbinit.tmpl
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

gdb:
	$(GDB) -n -x .gdbinit

# qemu: $(IMAGES) pre-qemu
qemu: xv6.img
	$(QEMU) $(QEMUOPTS)

qemu-gdb: xv6.img .gdbinit
	$(QEMU) $(QEMUOPTS) -S

clean:
	rm -f *.o *.asm *.bin .gdbinit bootblock qemu.log xv6.img kernel
