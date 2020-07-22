KERN_DIR := kern
KERN_OBJS := $(OBJDIR)/$(KERN_DIR)/entry.o

KERN_CFLAGS := $(CFLAGS) -m64
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
