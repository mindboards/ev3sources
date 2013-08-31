ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_analog.o

else

# normal makefile

TARGET = d_analog.ko

CONF = Linuxmod
ARCH = AM1808

include ../../open_first/rules.mk

endif
