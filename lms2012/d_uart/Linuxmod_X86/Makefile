ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_uart.o

else

# normal makefile

TARGET = d_uart.ko

CONF = Linuxmod
ARCH = X86

include ../../open_first/rules.mk

endif
