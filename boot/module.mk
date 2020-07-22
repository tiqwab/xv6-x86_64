BOOT_DIR := boot
BOOT_OBJS := \
	$(OBJDIR)/$(BOOT_DIR)/bootasm.o \
	$(OBJDIR)/$(BOOT_DIR)/bootmain.o \

BOOT_CFLAGS := $(CFLAGS) -m32
BOOT_LDFLAGS := $(LDFLAGS) -m elf_i386

BOOT_LINKER_SCRIPT := $(BOOT_DIR)/boot.ld

BOOT_BLOCK := $(BOOT_DIR)/bootblock

-include $(OBJDIR)/$(BOOT_DIR)/*.d

$(OBJDIR)/$(BOOT_BLOCK): $(BOOT_OBJS) $(BOOT_LINKER_SCRIPT)
	$(LD) $(BOOT_LDFLAGS) -N -T $(BOOT_LINKER_SCRIPT) -o $@.o $(BOOT_OBJS)
	$(OBJDUMP) -S $@.o > $@.asm
	$(OBJCOPY) -S -O binary -j .bootloader $@.o $@

$(OBJDIR)/$(BOOT_DIR)/%.o: $(BOOT_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(BOOT_CFLAGS) -fno-pic -nostdinc -I. -c $<

$(OBJDIR)/$(BOOT_DIR)/%.o: $(BOOT_DIR)/%.S
	@mkdir -p $(@D)
	$(CC) -o $@ $(BOOT_CFLAGS) -fno-pic -nostdinc -I. -c $<
