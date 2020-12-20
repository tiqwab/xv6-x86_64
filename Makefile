# Try to infer the correct QEMU
ifndef QEMU
	QEMU = qemu-system-x86_64
	# QEMU = qemu-system-i386
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
AR = ar

CFLAGS := ${XV6_CFLAGS}
CFLAGS += -fno-pic -static -fno-builtin -fno-strict-aliasing -MD -ggdb -fno-omit-frame-pointer
# CFLAGS += -O2 -std=c11 -Wall -Wextra -Wno-format -Wno-unused -Wno-address-of-packed-member -Werror
CFLAGS += -O1 -std=c11 -Wall -Wextra -Wno-format -Wno-unused -Wno-address-of-packed-member -Werror
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
# Prevent gcc from generating MMX and SSE instructions
CFLAGS += -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-ssse3 -mno-sse4.1 -mno-sse4.2 -mfpmath=387
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

XV6_IMG := $(OBJDIR)/xv6.img
FS_IMG := $(OBJDIR)/fs.img
IMAGES := $(XV6_IMG) $(FS_IMG)
UOBJS :=

default: $(IMAGES)

.PHONY: clean default format

include boot/module.mk
include lib/module.mk
include user/module.mk
include net/module.mk
include kern/module.mk

# Disc sector start no where kernel image is loaded
# TODO: duplicated with KERNEL_START_SECTOR in boot/stage_2.c
KERNEL_START_SECTOR := 32

$(XV6_IMG): format $(OBJDIR)/$(BOOT_BLOCK) $(OBJDIR)/$(KERNEL)
	dd if=/dev/zero of=$@ count=10000
	dd if=$(OBJDIR)/$(BOOT_BLOCK) of=$@ conv=notrunc
	dd if=$(OBJDIR)/$(KERNEL) of=$@ seek=$(KERNEL_START_SECTOR) conv=notrunc

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: %.o

# Enter QEMU monitor by 'Ctrl+a then c' if -serial mon:stdio is specified
# ref. https://kashyapc.wordpress.com/2016/02/11/qemu-command-line-behavior-of-serial-stdio-vs-serial-monstdio/
QEMUOPTS := $(QEMUOPTS)
QEMUOPTS += -drive file=$(XV6_IMG),index=0,media=disk,format=raw \
			-drive file=$(FS_IMG),index=1,media=disk,format=raw \
			-serial mon:stdio -gdb tcp::$(GDBPORT) -smp $(CPUS)
QEMUOPTS += $(shell if $(QEMU) -nographic -help | grep -q '^-D '; then echo '-D qemu.log'; fi)

.gdbinit: .gdbinit.tmpl
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

gdb:
	$(GDB) -n -x .gdbinit

# qemu: $(IMAGES) pre-qemu
qemu: $(IMAGES)
	$(QEMU) $(QEMUOPTS)

qemu-gdb: $(IMAGES) .gdbinit
	$(QEMU) $(QEMUOPTS) -S

format:
	./format.sh

clean:
	rm -f *.o *.d *.asm *.bin .gdbinit qemu.log
	rm -rf $(OBJDIR)
