ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_usbhost.o

else

# normal makefile

TARGET = d_usbhost.ko

CONF = Linuxmod
ARCH = X86

include ../../open_first/rules.mk

endif
