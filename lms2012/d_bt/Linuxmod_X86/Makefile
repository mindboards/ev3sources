ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_bt.o

else

# normal makefile

TARGET = d_bt.ko

CONF = Linuxmod
ARCH = X86

include ../../open_first/rules.mk

endif
