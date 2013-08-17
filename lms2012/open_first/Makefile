ARCH = AM1808
CROSS_COMPILE = arm-none-linux-gnueabi-

BASE = ..

KERNEL = $(BASE)/../extra/linux-03.20.00.13
UBOOT = $(BASE)/../extra/uboot-03.20.00.13

KERNEL_CONF = LEGOBoard.config

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

clean: kernel.clean

.PHONY: kernel kernel.prepare kernel.clean clean
