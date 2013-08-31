ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_bt.o

else

# normal makefile

TARGET = d_bt.ko

CONF = Linuxmod
ARCH = AM1808

include ../../open_first/rules.mk

endif
