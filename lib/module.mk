LIB_DIR := lib

LIB_OBJS := \
	$(OBJDIR)/$(LIB_DIR)/string.o \
	$(OBJDIR)/$(LIB_DIR)/atoi.o \

LIB_ARCHIVE_NAME := xv6_x86_64
LIB_ARCHIVE_FILE := $(OBJDIR)/$(LIB_DIR)/lib$(LIB_ARCHIVE_NAME).a

LIB_CFLAGS := $(CFLAGS) -m64 -fno-pic -nostdinc -I.
LIB_LDFLAGS := $(LDFLAGS) -m elf_x86_64

-include $(OBJDIR)/$(LIB_DIR)/*.d

$(LIB_ARCHIVE_FILE): $(LIB_OBJS)
	$(AR) r $@ $(LIB_OBJS)

$(OBJDIR)/$(LIB_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(LIB_CFLAGS) -c $<
