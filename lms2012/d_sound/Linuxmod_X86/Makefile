ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_sound.o

else

# normal makefile

TARGET = d_sound.ko

CONF = Linuxmod
ARCH = X86

include ../../open_first/rules.mk

endif
