ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_ui.o

else

# normal makefile

TARGET = d_ui.ko

CONF = Linuxmod
ARCH = X86

include ../../open_first/rules.mk

endif
