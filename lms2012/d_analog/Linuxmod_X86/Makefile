ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_analog.o

else

# normal makefile

TARGET = d_analog.ko

CONF = Linuxmod
ARCH = X86

include ../../open_first/rules.mk

endif
