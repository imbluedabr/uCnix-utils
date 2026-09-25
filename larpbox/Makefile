
SYSROOT ?= ./sysroot
ARCH ?= armv8-m
INSTALL_DIR =$(SYSROOT)/usr/bin

include $(SYSROOT)/usr/lib/libc.mk

CFLAGS += -Os
LDFLAGS += -Os

CC = $(TOOLCHAIN)-gcc
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)

%.o: %.c
	$(TOOLCHAIN)-gcc $(CFLAGS) -c $<

larpbox: $(OBJS)
	$(TOOLCHAIN)-gcc $(OBJS) $(LDFLAGS) -o larpbox.elf
	$(TOOLCHAIN)-objcopy -O binary larpbox.elf larpbox

all: clean install

install: larpbox
	mkdir -p $(INSTALL_DIR)
	mv larpbox $(INSTALL_DIR)/larpbox

clean:
	rm -f *.o *.elf larpbox

