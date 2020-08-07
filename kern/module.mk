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

KERN_CFLAGS := $(CFLAGS) -m64 -mcmodel=kernel
KERN_LDFLAGS := $(LDFLAGS) -m elf_x86_64

KERN_LINKER_SCRIPT := $(KERN_DIR)/kernel.ld

KERNEL := $(KERN_DIR)/kernel

-include $(OBJDIR)/$(KERN_DIR)/*.d

$(OBJDIR)/$(KERNEL): $(KERN_OBJS) $(KERN_LINKER_SCRIPT)
	$(LD) $(KERN_LDFLAGS) -T $(KERN_LINKER_SCRIPT) -o $@ $(KERN_OBJS)
	$(OBJDUMP) -S $@ > $@.asm

$(OBJDIR)/$(KERN_DIR)/%.o: $(KERN_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(KERN_CFLAGS) -fno-pic -nostdinc -I. -c $<

$(OBJDIR)/$(KERN_DIR)/%.o: $(KERN_DIR)/%.S
	@mkdir -p $(@D)
	$(CC) -o $@ $(KERN_CFLAGS) -fno-pic -nostdinc -I. -c $<
