EXTRA_CFLAGS += -Ipru-firmware-05-31-2011-1423-v3.0/pru/hal/uart/include -Ipru-firmware-05-31-2011-1423-v3.0/pru/hal/common/include

$(MOD)-objs :=  d_uart_new.o \
			    lego_ti_omapl_pru_suart.o \
					pru-firmware-05-31-2011-1423-v3.0/pru/hal/uart/src/suart_api.o \
					pru-firmware-05-31-2011-1423-v3.0/pru/hal/uart/src/suart_utils.o \
					pru-firmware-05-31-2011-1423-v3.0/pru/hal/common/src/pru.o
obj-m += $(MOD).o

##ccflags-$(CONFIG_SUART_DEBUG_DEVICES) := -DDEBUG

clean-files := pru


