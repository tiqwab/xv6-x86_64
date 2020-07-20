# Try to infer the correct QEMU
ifndef QEMU
	QEMU = qemu-system-x86_64
endif

OBJDIR := obj

CC = gcc
AS = gas
LD = ld
OBJCOPY = objcopy
OBJDUMP = objdump
CP = cp
DD = dd
MKDIR = mkdir
GDB = gdb

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -MD -ggdb -fno-omit-frame-pointer
CFLAGS += -O2 -std=c11 -Wall -Wextra -Wno-format -Wno-unused -Wno-address-of-packed-member -Werror
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
# ASFLAGS = -m32 -gdwarf-2 -Wa,-divide
# LDFLAGS += -m elf_x86_64
# LDFLAGS += -m elf_i386
LDFLAGS :=

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
	CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
	CFLAGS += -fno-pie -nopie
endif

GDBPORT	:= 12345
CPUS ?= 1
IMAGE := xv6.img

default: $(OBJDIR)/$(IMAGE)

-include *.d

include boot/module.mk
include kern/module.mk

$(OBJDIR)/$(IMAGE): $(OBJDIR)/$(BOOT_BLOCK) $(OBJDIR)/$(KERNEL)
	dd if=/dev/zero of=$@ count=10000
	dd if=$(OBJDIR)/$(BOOT_BLOCK) of=$@ conv=notrunc
	dd if=$(OBJDIR)/$(KERNEL) of=$@ seek=8 conv=notrunc

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: %.o

# Enter QEMU monitor by 'Ctrl+a then c' if -serial mon:stdio is specified
# ref. https://kashyapc.wordpress.com/2016/02/11/qemu-command-line-behavior-of-serial-stdio-vs-serial-monstdio/
QEMUOPTS := $(QEMUOPTS)
QEMUOPTS += -drive file=$(OBJDIR)/$(IMAGE),index=0,media=disk,format=raw \
			-serial mon:stdio -gdb tcp::$(GDBPORT) -smp $(CPUS)
QEMUOPTS += $(shell if $(QEMU) -nographic -help | grep -q '^-D '; then echo '-D qemu.log'; fi)

.gdbinit: .gdbinit.tmpl
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

gdb:
	$(GDB) -n -x .gdbinit

# qemu: $(IMAGES) pre-qemu
qemu: $(OBJDIR)/$(IMAGE)
	$(QEMU) $(QEMUOPTS)

qemu-gdb: $(OBJDIR)/$(IMAGE) .gdbinit
	$(QEMU) $(QEMUOPTS) -S

clean:
	rm -f *.o *.d *.asm *.bin .gdbinit qemu.log
	rm -rf $(OBJDIR)
