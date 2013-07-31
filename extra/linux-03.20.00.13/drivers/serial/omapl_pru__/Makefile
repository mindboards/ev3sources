#ifdef CONFIG_OMAPL_SUART_FW_SOURCE
#$(if $(wildcard drivers/serial/omapl_pru/pru), , \
#		$(shell ln -s $(CONFIG_OMAPL_SUART_FW_SOURCE) drivers/serial/omapl_pru/pru))
#endif
ifdef CONFIG_OMAPL_SUART_FW_SOURCE
$(shell rm -rf drivers/serial/omapl_pru/pru)
$(shell ln -s $(CONFIG_OMAPL_SUART_FW_SOURCE) drivers/serial/omapl_pru/pru)
endif

EXTRA_CFLAGS += -I$(srctree)/drivers/serial/omapl_pru/pru/hal/uart/include -I$(srctree)/drivers/serial/omapl_pru/pru/hal/common/include

suart_emu-objs := 	ti_omapl_pru_suart.o \
					pru/hal/uart/src/suart_api.o \
					pru/hal/uart/src/suart_utils.o \
					pru/hal/common/src/pru.o

obj-$(CONFIG_SERIAL_SUART_OMAPL_PRU)	+= suart_emu.o

ccflags-$(CONFIG_SUART_DEBUG_DEVICES) := -DDEBUG

clean-files := pru
