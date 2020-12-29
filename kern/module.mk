KERN_DIR := kern
KERN_OBJS := \
	$(OBJDIR)/$(KERN_DIR)/entry.o \
	$(OBJDIR)/$(KERN_DIR)/main.o \
	$(OBJDIR)/$(KERN_DIR)/kalloc.o \
	$(OBJDIR)/$(KERN_DIR)/spinlock.o \
	$(OBJDIR)/$(KERN_DIR)/console.o \
	$(OBJDIR)/$(KERN_DIR)/mp.o \
	$(OBJDIR)/$(KERN_DIR)/proc.o \
	$(OBJDIR)/$(KERN_DIR)/vm.o \
	$(OBJDIR)/$(KERN_DIR)/swtch.o \
	$(OBJDIR)/$(KERN_DIR)/trapasm.o \
	$(OBJDIR)/$(KERN_DIR)/kclock.o \
	$(OBJDIR)/$(KERN_DIR)/trap.o \
	$(OBJDIR)/$(KERN_DIR)/syscall.o \
	$(OBJDIR)/$(KERN_DIR)/vectors.o \
	$(OBJDIR)/$(KERN_DIR)/lapic.o \
	$(OBJDIR)/$(KERN_DIR)/uart.o \
	$(OBJDIR)/$(KERN_DIR)/picirq.o \
	$(OBJDIR)/$(KERN_DIR)/ioapic.o \
	$(OBJDIR)/$(KERN_DIR)/kbd.o \
	$(OBJDIR)/$(KERN_DIR)/sysproc.o \
	$(OBJDIR)/$(KERN_DIR)/exec.o \
	$(OBJDIR)/$(KERN_DIR)/sysfile.o \
	$(OBJDIR)/$(KERN_DIR)/bio.o \
	$(OBJDIR)/$(KERN_DIR)/sleeplock.o \
	$(OBJDIR)/$(KERN_DIR)/ide.o \
	$(OBJDIR)/$(KERN_DIR)/fs.o \
	$(OBJDIR)/$(KERN_DIR)/log.o \
	$(OBJDIR)/$(KERN_DIR)/file.o \
	$(OBJDIR)/$(KERN_DIR)/pipe.o \
	$(OBJDIR)/$(KERN_DIR)/time.o \
	$(OBJDIR)/$(KERN_DIR)/socket.o \
	$(OBJDIR)/$(KERN_DIR)/syssocket.o \
	$(OBJDIR)/$(KERN_DIR)/lwip.o \
	$(OBJDIR)/$(KERN_DIR)/pci.o \
	$(OBJDIR)/$(KERN_DIR)/e1000.o \

KERN_BINARY_OBJS := \
	$(OBJDIR)/$(KERN_DIR)/initcode \

KERN_INCLUDES := \
	-I$(LWIP_DIR)/include \
	-I$(LWIP_CUSTOM_DIR)/include \
	-I./inc \

KERN_CFLAGS := $(CFLAGS) -m64 -mcmodel=kernel
KERN_LDFLAGS := $(LDFLAGS) -m elf_x86_64

KERN_LINKER_SCRIPT := $(KERN_DIR)/kernel.ld

KERNEL := $(KERN_DIR)/kernel

-include $(OBJDIR)/$(KERN_DIR)/*.d

$(KERN_DIR)/vectors.S: $(KERN_DIR)/vectors.sh
	./$< > $@

$(OBJDIR)/$(KERN_DIR)/initcode: $(KERN_DIR)/initcode.S
	$(CC) $(CFLAGS) -m64 -fno-pic -nostdinc -I. -c -o $@.o $<
	$(LD) $(LDFLAGS) -m elf_x86_64 -N -e start -Ttext 0 -o $@.out $@.o
	$(OBJCOPY) -S -O binary $@.out $@
	$(OBJDUMP) -S $@.o > $@.asm

$(OBJDIR)/$(KERNEL): $(KERN_OBJS) $(KERN_LINKER_SCRIPT) $(KERN_BINARY_OBJS) $(LIB_ARCHIVE_FILE) $(LWIP_ARCHIVE_FILE)
	$(LD) $(KERN_LDFLAGS) -T $(KERN_LINKER_SCRIPT) -o $@ $(KERN_OBJS) \
		-L $(OBJDIR)/$(NET_DIR) -l$(LWIP_ARCHIVE_NAME) \
		-L $(OBJDIR)/$(LIB_DIR) -l$(LIB_ARCHIVE_NAME) \
		-b binary $(KERN_BINARY_OBJS)
	$(OBJDUMP) -S $@ > $@.asm

$(OBJDIR)/$(KERN_DIR)/%.o: $(KERN_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(KERN_CFLAGS) -fno-pic -nostdinc $(KERN_INCLUDES) -I. -c $<

$(OBJDIR)/$(KERN_DIR)/%.o: $(KERN_DIR)/%.S
	@mkdir -p $(@D)
	$(CC) -o $@ $(KERN_CFLAGS) -fno-pic -nostdinc $(KERN_INCLUDES) -I. -c $<

$(OBJDIR)/$(KERN_DIR)/mkfs: $(KERN_DIR)/mkfs.c
	$(CC) -std=c11 -Wall -Wextra -Wno-format -Wno-unused -Wno-address-of-packed-member -Werror -I. \
		-o $@ $<

$(FS_IMG): $(OBJDIR)/$(KERN_DIR)/mkfs $(UOBJS)
	./$< $@ $(UOBJS)
