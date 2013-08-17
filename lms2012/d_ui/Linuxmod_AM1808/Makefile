ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_ui.o

else

# normal makefile

TARGET = d_ui.ko

CONF = Linuxmod
ARCH = AM1808

include ../../open_first/rules.mk

endif
