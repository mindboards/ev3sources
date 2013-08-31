ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_usbhost.o

else

# normal makefile

TARGET = d_usbhost.ko

CONF = Linuxmod
ARCH = AM1808

include ../../open_first/rules.mk

endif
