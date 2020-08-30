USER_DIR := user

UOBJS += \
		 $(OBJDIR)/$(USER_DIR)/preemptiontest1 \
		 $(OBJDIR)/$(USER_DIR)/preemptiontest2 \
		 $(OBJDIR)/$(USER_DIR)/init \

ULIBS := \
	$(OBJDIR)/$(USER_DIR)/usys.o

USER_CFLAGS := $(CFLAGS) -m64 -fno-pic -nostdinc -I.
USER_LDFLAGS := $(LDFLAGS) -m elf_x86_64 -N -e main -Ttext 0

-include $(OBJDIR)/$(USER_DIR)/*.d

$(OBJDIR)/$(USER_DIR)/preemptiontest1: $(USER_DIR)/preemptiontest1.c $(ULIBS)
	@mkdir -p $(@D)
	$(CC) $(USER_CFLAGS) -c -o $@.o $<
	$(LD) $(USER_LDFLAGS) -o $@ $@.o $(ULIBS)
	$(OBJDUMP) -S $@.o > $@.asm

$(OBJDIR)/$(USER_DIR)/preemptiontest2: $(USER_DIR)/preemptiontest2.c $(ULIBS)
	@mkdir -p $(@D)
	$(CC) $(USER_CFLAGS) -c -o $@.o $<
	$(LD) $(USER_LDFLAGS) -o $@ $@.o $(ULIBS)
	$(OBJDUMP) -S $@.o > $@.asm

$(OBJDIR)/$(USER_DIR)/init: $(USER_DIR)/init.c $(ULIBS)
	@mkdir -p $(@D)
	$(CC) $(USER_CFLAGS) -c -o $@.o $<
	$(LD) $(USER_LDFLAGS) -o $@ $@.o $(ULIBS)
	$(OBJDUMP) -S $@.o > $@.asm

$(OBJDIR)/$(USER_DIR)/%.o: $(USER_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(USER_CFLAGS) -c $<

$(OBJDIR)/$(USER_DIR)/%.o: $(USER_DIR)/%.S
	@mkdir -p $(@D)
	$(CC) -o $@ $(USER_CFLAGS) -c $<
