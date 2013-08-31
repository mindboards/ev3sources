ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
d_uart-y := d_uart_mod.o \
	lego_ti_omapl_pru_suart.o \
	pru-firmware-05-31-2011-1423-v3.0/pru/hal/uart/src/suart_api.o \
	pru-firmware-05-31-2011-1423-v3.0/pru/hal/uart/src/suart_utils.o \
	pru-firmware-05-31-2011-1423-v3.0/pru/hal/common/src/pru.o
obj-m := d_uart.o

EXTRA_CFLAGS += -Ipru-firmware-05-31-2011-1423-v3.0/pru/hal/uart/include \
		-Ipru-firmware-05-31-2011-1423-v3.0/pru/hal/common/include

else

# normal makefile

TARGET = d_uart.ko

CONF = Linuxmod
ARCH = AM1808

include ../../open_first/rules.mk

endif
