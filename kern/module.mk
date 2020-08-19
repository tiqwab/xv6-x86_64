KERN_DIR := kern
KERN_OBJS := \
	$(OBJDIR)/$(KERN_DIR)/entry.o \
	$(OBJDIR)/$(KERN_DIR)/main.o \
	$(OBJDIR)/$(KERN_DIR)/kalloc.o \
	$(OBJDIR)/$(KERN_DIR)/spinlock.o \
	$(OBJDIR)/$(KERN_DIR)/console.o \
	$(OBJDIR)/$(KERN_DIR)/string.o \
	$(OBJDIR)/$(KERN_DIR)/mp.o \
	$(OBJDIR)/$(KERN_DIR)/proc.o \
	$(OBJDIR)/$(KERN_DIR)/vm.o \
	$(OBJDIR)/$(KERN_DIR)/swtch.o \
	$(OBJDIR)/$(KERN_DIR)/trapasm.o \
	$(OBJDIR)/$(KERN_DIR)/kclock.o \
	$(OBJDIR)/$(KERN_DIR)/trap.o \
	$(OBJDIR)/$(KERN_DIR)/syscall.o \
	$(OBJDIR)/$(KERN_DIR)/vectors.o \

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

$(OBJDIR)/$(KERNEL): $(KERN_OBJS) $(KERN_LINKER_SCRIPT) $(OBJDIR)/$(KERN_DIR)/initcode
	$(LD) $(KERN_LDFLAGS) -T $(KERN_LINKER_SCRIPT) -o $@ $(KERN_OBJS) -b binary $(OBJDIR)/$(KERN_DIR)/initcode
	$(OBJDUMP) -S $@ > $@.asm

$(OBJDIR)/$(KERN_DIR)/%.o: $(KERN_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(KERN_CFLAGS) -fno-pic -nostdinc -I. -c $<

$(OBJDIR)/$(KERN_DIR)/%.o: $(KERN_DIR)/%.S
	@mkdir -p $(@D)
	$(CC) -o $@ $(KERN_CFLAGS) -fno-pic -nostdinc -I. -c $<
