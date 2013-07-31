#ifdef CONFIG_OMAPL_CAN_FW_SOURCE
#$(if $(wildcard drivers/net/can/omapl_pru/pru), , \
#        $(shell ln -s $(CONFIG_OMAPL_CAN_FW_SOURCE) drivers/net/can/omapl_pru/pru))
#endif

ifdef CONFIG_OMAPL_CAN_FW_SOURCE
$(shell rm -rf drivers/net/can/omapl_pru/pru)
$(shell ln -s $(CONFIG_OMAPL_CAN_FW_SOURCE) drivers/net/can/omapl_pru/pru)
endif

EXTRA_CFLAGS += -I$(srctree)/drivers/net/can/omapl_pru/pru/hal/can/include -I$(srctree)/drivers/net/can/omapl_pru/pru/hal/common/include

ccflags-$(CONFIG_CAN_DEBUG_DEVICES) := -DDEBUG

can_emu-objs :=   ti_omapl_pru_can.o \
				  pru/hal/common/src/pru.o \
                  pru/hal/can/src/pru_can_emulation_api.o

obj-$(CONFIG_CAN_TI_OMAPL_PRU)    += can_emu.o

ccflags-$(CONFIG_CAN_DEBUG_DEVICES) := -DDEBUG

clean-files := pru
