NET_DIR := net
LWIP_DIR := $(NET_DIR)/lwip
LWIP_CUSTOM_DIR := $(NET_DIR)/lwip-custom
# This is used in Filelists.mk
LWIPDIR := $(LWIP_DIR)

include $(LWIP_DIR)/Filelists.mk

LWIP_SRCS := $(LWIPNOAPPSFILES) \
	$(LWIP_CUSTOM_DIR)/arch/sys_arch.c \

LWIP_INCLUDES := \
	-I$(LWIP_DIR)/include \
	-I$(LWIP_CUSTOM_DIR)/include \
	-I./inc \

LWIP_OBJS := $(patsubst %.c, $(OBJDIR)/%.o, $(LWIP_SRCS))
LWIP_OBJS += $(patsubst %.S, $(OBJDIR)/%.o, $(LWIP_SRCS))

LWIP_CFLAGS := $(CFLAGS) -m64 -I.
LWIP_LDFLAGS := $(LDFLAGS) -m elf_x86_64

$(OBJDIR)/$(LWIP_DIR)/%.o: $(LWIP_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(LWIP_CFLAGS) -fno-pic -nostdinc $(LWIP_INCLUDES) -c $<

$(OBJDIR)/$(LWIP_DIR)/%.o: $(LWIP_DIR)/%.S
	@mkdir -p $(@D)
	$(CC) -o $@ $(LWIP_CFLAGS) -fno-pic -nostdinc $(LWIP_INCLUDES) -c $<

$(OBJDIR)/$(LWIP_CUSTOM_DIR)/%.o: $(LWIP_CUSTOM_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -o $@ $(LWIP_CFLAGS) -fno-pic -nostdinc $(LWIP_INCLUDES) -c $<

$(OBJDIR)/$(LWIP_CUSTOM_DIR)/%.o: $(LWIP_CUSTOM_DIR)/%.S
	@mkdir -p $(@D)
	$(CC) -o $@ $(LWIP_CFLAGS) -fno-pic -nostdinc $(LWIP_INCLUDES) -c $<

$(OBJDIR)/$(NET_DIR)/liblwip.a: $(LWIP_OBJS)
	@mkdir -p $(@D)
	$(AR) r $@ $(LWIP_OBJS)
