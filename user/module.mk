USER_DIR := user

UOBJS += \
		 $(OBJDIR)/$(USER_DIR)/init \
		 $(OBJDIR)/$(USER_DIR)/preemptiontest1 \
		 $(OBJDIR)/$(USER_DIR)/preemptiontest2 \
		 $(OBJDIR)/$(USER_DIR)/fstest \
		 $(OBJDIR)/$(USER_DIR)/sh \
		 $(OBJDIR)/$(USER_DIR)/ls \
		 $(OBJDIR)/$(USER_DIR)/mkdir \
		 $(OBJDIR)/$(USER_DIR)/cat \
		 $(OBJDIR)/$(USER_DIR)/echo \
		 $(OBJDIR)/$(USER_DIR)/ln \
		 $(OBJDIR)/$(USER_DIR)/rm \
		 $(OBJDIR)/$(USER_DIR)/wc \
		 $(OBJDIR)/$(USER_DIR)/vatest \
		 $(OBJDIR)/$(USER_DIR)/socktest \

ULIBS := \
	$(OBJDIR)/$(USER_DIR)/usys.o \
	$(OBJDIR)/$(USER_DIR)/entry.o \
	$(OBJDIR)/$(USER_DIR)/printf.o \
	$(OBJDIR)/$(USER_DIR)/gets.o \
	$(OBJDIR)/$(USER_DIR)/stat.o \
	$(OBJDIR)/$(USER_DIR)/umalloc.o \

USER_LINKER_SCRIPT := $(USER_DIR)/user.ld

USER_INCLUDES := \
	-I$(LWIP_DIR)/include \
	-I$(LWIP_CUSTOM_DIR)/include \
	-I./inc \

USER_CFLAGS := $(CFLAGS) -m64 -fno-pic -nostdinc -I.
USER_LDFLAGS := $(LDFLAGS) -T $(USER_LINKER_SCRIPT)

-include $(OBJDIR)/$(USER_DIR)/*.d

# macro to generate rules for $(UBOJS)
define RULE_UOBJ
$(1): $(USER_DIR)/$(notdir $(1)).c $(ULIBS) $(LIB_ARCHIVE_FILE) $(LWIP_ARCHIVE_FILE)
	@mkdir -p $(dir $(1))
	$(CC) $(USER_CFLAGS) $(USER_INCLUDES) -c -o $(1).o $(USER_DIR)/$(notdir $(1)).c
	$(LD) $(USER_LDFLAGS) -o $(1) $(1).o $(ULIBS) \
		-L $(OBJDIR)/$(NET_DIR) -l$(LWIP_ARCHIVE_NAME) \
		-L $(OBJDIR)/$(LIB_DIR) -l$(LIB_ARCHIVE_NAME)
	$(OBJDUMP) -S $(1) > $(1).asm
endef

$(foreach obj, $(UOBJS), $(eval $(call RULE_UOBJ, $(obj))))

$(OBJDIR)/$(USER_DIR)/%.o: $(USER_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(USER_CFLAGS) $(USER_INCLUDES) -c $<

$(OBJDIR)/$(USER_DIR)/%.o: $(USER_DIR)/%.S
	@mkdir -p $(@D)
	$(CC) -o $@ $(USER_CFLAGS) $(USER_INCLUDES) -c $<
