ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_power.o

else

# normal makefile

TARGET = d_power.ko

CONF = Linuxmod
ARCH = X86

include ../../open_first/rules.mk

endif
