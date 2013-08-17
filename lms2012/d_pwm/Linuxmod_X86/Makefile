ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += d_pwm.o

else

# normal makefile

TARGET = d_pwm.ko

CONF = Linuxmod
ARCH = X86

include ../../open_first/rules.mk

endif
