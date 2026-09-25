
SYSROOT ?= ./sysroot
ARCH ?= armv8-m

install:
	$(MAKE) -C ./init SYSROOT=$(SYSROOT) ARCH=$(ARCH) install
	$(MAKE) -C ./larpbox SYSROOT=$(SYSROOT) ARCH=$(ARCH) install

clean:
	$(MAKE) -C ./init SYSROOT=$(SYSROOT) ARCH=$(ARCH) clean
	$(MAKE) -C ./larpbox SYSROOT=$(SYSROOT) ARCH=$(ARCH) clean

