
SYSROOT ?= ./sysroot
ARCH ?= armv8-m
INSTALL_DIR = $(SYSROOT)/bin
ETC_INSTALL_DIR = $(SYSROOT)/etc

-include $(SYSROOT)/lib/libc.mk

CFLAGS += -Os
LDFLAGS += -Os

$(CC) = $(TOOLCHAIN)-gcc

init.o: init.c
	$(TOOLCHAIN)-gcc $(CFLAGS) -c init.c

init: init.o
	$(TOOLCHAIN)-gcc init.o $(LDFLAGS) -o init.elf
	$(TOOLCHAIN)-objcopy -O binary init.elf init

all: clean install

install: init
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(ETC_INSTALL_DIR)
	mv init $(INSTALL_DIR)/init
	cp initrc $(ETC_INSTALL_DIR)/initrc

clean:
	rm -f *.o *.elf

