ARCH = AM1808
CROSS_COMPILE = arm-none-linux-gnueabi-

BASE = ..

KERNEL = $(BASE)/../extra/linux-03.20.00.13
UBOOT = $(BASE)/../extra/uboot-03.20.00.13

KERNEL_CONF = LEGOBoard.config

default: kernel u-boot

#
# Kernel.
#

kernel: uImage

$(KERNEL)/.config: $(KERNEL_CONF)
	cp $< $@

kernel.prepare: $(KERNEL)/include/generated/bounds.h
$(KERNEL)/include/generated/bounds.h: $(KERNEL)/.config
	$(MAKE) -C $(KERNEL) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) oldconfig modules_prepare

$(KERNEL)/arch/arm/boot/uImage: kernel.prepare
	$(MAKE) -C $(KERNEL) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) uImage

uImage: $(KERNEL)/arch/arm/boot/uImage
	cp $< $@

kernel.clean:
	$(MAKE) -C $(KERNEL) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) clean

.PHONY: kernel kernel.prepare kernel.clean

#
# U-Boot.
#

u-boot: uBoot

$(UBOOT)/u-boot.bin:
	$(MAKE) -C $(UBOOT) CROSS_COMPILE=$(CROSS_COMPILE) da850_omapl138_evm_config
	$(MAKE) -C $(UBOOT) CROSS_COMPILE=$(CROSS_COMPILE) all

uBoot: $(UBOOT)/u-boot.bin
	cp $< $@

u-boot.clean:
	$(MAKE) -C $(UBOOT) CROSS_COMPILE=$(CROSS_COMPILE) distclean

.PHONY: u-boot u-boot.clean

#
# Misc.
#

clean: kernel.clean u-boot.clean

.PHONY: clean
