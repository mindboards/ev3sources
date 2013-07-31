/*
 * DA8XX/OMAP L1XX platform device data
 *
 * Copyright (c) 2007-2009, MontaVista Software, Inc. <source@mvista.com>
 * Derived from code that was:
 *	Copyright (C) 2006 Komal Shah <komal_shah802003@yahoo.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/serial_8250.h>
#include <linux/ti_omapl_pru_suart.h>
#include <linux/can/platform/ti_omapl_pru_can.h>

#include <mach/cputype.h>
#include <mach/common.h>
#include <mach/time.h>
#include <mach/da8xx.h>
#include <mach/cpuidle.h>
#include <mach/usb_musb.h>
#include <mach/spi.h>

#include "clock.h"

#define DA8XX_TPCC_BASE			0x01c00000
#define DA850_TPCC1_BASE		0x01e30000
#define DA8XX_TPTC0_BASE		0x01c08000
#define DA8XX_TPTC1_BASE		0x01c08400
#define DA850_TPTC2_BASE		0x01e38000
#define DA8XX_WDOG_BASE			0x01c21000 /* DA8XX_TIMER64P1_BASE */
#define DA8XX_I2C0_BASE			0x01c22000
#define DA8XX_RTC_BASE			0x01C23000
#define DA8XX_EMAC_CPPI_PORT_BASE	0x01e20000
#define DA8XX_EMAC_CPGMACSS_BASE	0x01e22000
#define DA8XX_EMAC_CPGMAC_BASE		0x01e23000
#define DA8XX_EMAC_MDIO_BASE		0x01e24000
#define DA8XX_GPIO_BASE			0x01e26000
#define DA8XX_I2C1_BASE			0x01e28000
#define DA8XX_USB0_BASE			0x01e00000
#define DA8XX_USB1_BASE			0x01e25000

#define DA8XX_EMAC_CTRL_REG_OFFSET	0x3000
#define DA8XX_EMAC_MOD_REG_OFFSET	0x2000
#define DA8XX_EMAC_RAM_OFFSET		0x0000
#define DA8XX_MDIO_REG_OFFSET		0x4000
#define DA8XX_EMAC_CTRL_RAM_SIZE	SZ_8K

void __iomem *da8xx_syscfg0_base;
EXPORT_SYMBOL(da8xx_syscfg0_base);
void __iomem *da8xx_syscfg1_base;
EXPORT_SYMBOL(da8xx_syscfg1_base);


void __iomem *da8xx_psc1_base;			// LEGO BT slow clock
EXPORT_SYMBOL(da8xx_psc1_base);			// LEGO BT slow clock
void __iomem *da8xx_ecap2_base;			// LEGO BT slow clock
EXPORT_SYMBOL(da8xx_ecap2_base);		// LEGO BT slow clock


static struct plat_serial8250_port da8xx_serial_pdata[] = {
	{
		.mapbase	= DA8XX_UART0_BASE,
		.irq		= IRQ_DA8XX_UARTINT0,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST |
					UPF_IOREMAP | UPF_FIXED_TYPE, //mod
		.type		= PORT_AR7,		
		.iotype		= UPIO_MEM,
		.regshift	= 2,
	},
	{
		.mapbase	= DA8XX_UART1_BASE,
		.irq		= IRQ_DA8XX_UARTINT1,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST |
					UPF_IOREMAP | UPF_FIXED_TYPE, //mod
		.type		= PORT_AR7,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
	},
	{
		.mapbase	= DA8XX_UART2_BASE,
		.irq		= IRQ_DA8XX_UARTINT2,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST |
					UPF_IOREMAP | UPF_FIXED_TYPE, //mod
		.type		= PORT_AR7,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
	},
	{
		.flags	= 0,
	},
};

struct platform_device da8xx_serial_device = {
	.name	= "serial8250",
	.id	= PLAT8250_DEV_PLATFORM,
	.dev	= {
		.platform_data	= da8xx_serial_pdata,
	},
};

#define OMAPL138_PRU_MEM_BASE	   0x01C30000

#define OMAPL138_INT_PRU_SUART_1 IRQ_DA8XX_EVTOUT0
#define OMAPL138_INT_PRU_SUART_2 IRQ_DA8XX_EVTOUT1
#define OMAPL138_INT_PRU_SUART_3 IRQ_DA8XX_EVTOUT2
#define OMAPL138_INT_PRU_SUART_4 IRQ_DA8XX_EVTOUT3
#define OMAPL138_INT_PRU_SUART_5 IRQ_DA8XX_EVTOUT4
#define OMAPL138_INT_PRU_SUART_6 IRQ_DA8XX_EVTOUT5
#define OMAPL138_INT_PRU_SUART_7 IRQ_DA8XX_EVTOUT6
#define OMAPL138_INT_PRU_SUART_8 IRQ_DA8XX_EVTOUT7
#ifndef CONFIG_OMAPL_SUART_MCASP
#define CONFIG_OMAPL_SUART_MCASP 0
#endif
static struct resource omapl138_pru_suart_resources[] = {
        {
				.name 	= "omapl_pru_suart",
                .start  = OMAPL138_PRU_MEM_BASE,
                .end    = OMAPL138_PRU_MEM_BASE + 0xFFFF,
                .flags  = IORESOURCE_MEM,
        },
        {
#if (CONFIG_OMAPL_SUART_MCASP == 0)
				.start  = DAVINCI_DA8XX_MCASP0_REG_BASE,
		        .end    = DAVINCI_DA8XX_MCASP0_REG_BASE + (SZ_1K * 12) - 1,
                .flags  = IORESOURCE_MEM,
#elif (CONFIG_OMAPL_SUART_MCASP == 1)
				.start  = DAVINCI_DA830_MCASP1_REG_BASE,
		        .end    = DAVINCI_DA830_MCASP1_REG_BASE + (SZ_1K * 12) - 1,
                .flags  = IORESOURCE_MEM,
#elif (CONFIG_OMAPL_SUART_MCASP == 2)
				.start  = DAVINCI_DA830_MCASP2_REG_BASE,
		        .end    = DAVINCI_DA830_MCASP2_REG_BASE + (SZ_1K * 12) - 1,
                .flags  = IORESOURCE_MEM,
#endif
        },

        {
				.start  = DA8XX_PSC0_BASE,
		        .end    = DA8XX_PSC0_BASE + (SZ_1K * 3) - 1,
                .flags  = IORESOURCE_MEM,
        },
        {
				.start  = DA8XX_PSC1_BASE,
		        .end    = DA8XX_PSC1_BASE + (SZ_1K * 3) - 1,
                .flags  = IORESOURCE_MEM,
        },
        {
				.start  = DA8XX_SHARED_RAM_BASE,
		        .end    = DA8XX_SHARED_RAM_BASE + (SZ_1K * 8) - 1,
                .flags  = IORESOURCE_MEM,
        },
        {
                .start  = OMAPL138_INT_PRU_SUART_1,
                .end    = OMAPL138_INT_PRU_SUART_1,
                .flags  = IORESOURCE_IRQ,
        },
        {
                .start  = OMAPL138_INT_PRU_SUART_2,
                .end    = OMAPL138_INT_PRU_SUART_2,
                .flags  = IORESOURCE_IRQ,
        },
        {
                .start  = OMAPL138_INT_PRU_SUART_3,
                .end    = OMAPL138_INT_PRU_SUART_3,
                .flags  = IORESOURCE_IRQ,
        },
        {
                .start  = OMAPL138_INT_PRU_SUART_4,
                .end    = OMAPL138_INT_PRU_SUART_4,
                .flags  = IORESOURCE_IRQ,
        },
        {
                .start  = OMAPL138_INT_PRU_SUART_5,
                .end    = OMAPL138_INT_PRU_SUART_5,
                .flags  = IORESOURCE_IRQ,
        },
        {
                .start  = OMAPL138_INT_PRU_SUART_6,
                .end    = OMAPL138_INT_PRU_SUART_6,
                .flags  = IORESOURCE_IRQ,
        },
        {
                .start  = OMAPL138_INT_PRU_SUART_7,
                .end    = OMAPL138_INT_PRU_SUART_7,
                .flags  = IORESOURCE_IRQ,
        },
        {
                .start  = OMAPL138_INT_PRU_SUART_8,
                .end    = OMAPL138_INT_PRU_SUART_8,
                .flags  = IORESOURCE_IRQ,
        },
};

struct platform_device omapl_pru_suart_device = {
    .name   = "ti_omapl_pru_suart",
    .id             = 1,
    .num_resources  = ARRAY_SIZE(omapl138_pru_suart_resources),
    .resource       = omapl138_pru_suart_resources,
};

#define OMAPL138_PRU_SUART_VERSION           1

static struct ti_pru_suart_platform_data ti_pru_suart_pdata = {
        .version                = OMAPL138_PRU_SUART_VERSION,
};

int __init da8xx_register_pru_suart(void)
{
        omapl_pru_suart_device.dev.platform_data = &ti_pru_suart_pdata;
        return platform_device_register(&omapl_pru_suart_device);
}

/* Info specific to OMAPL138 */
#define OMAPL138_INT_PRU_CAN IRQ_DA8XX_EVTOUT0
static struct resource omapl138_pru_can_resources[] = {
        {
                .start  = OMAPL138_PRU_MEM_BASE,
                .end    = OMAPL138_PRU_MEM_BASE + 0xFFFF,
                .flags  = IORESOURCE_MEM,
        },
        {
                .start  = OMAPL138_INT_PRU_CAN,
                .end    = OMAPL138_INT_PRU_CAN,
                .flags  = IORESOURCE_IRQ,
        },
};

static struct platform_device omapl138_pru_can_device = {
        .name           = "ti_omapl_pru_can",
        .id             = 1,
        .num_resources  = ARRAY_SIZE(omapl138_pru_can_resources),
        .resource       = omapl138_pru_can_resources,
};

/* Info specific to CAN conroller */
#define OMAPL138_PRU_CAN_VERSION           1

static struct ti_pru_can_platform_data ti_pru_can_pdata = {
        .version                = OMAPL138_PRU_CAN_VERSION,
};

int __init da8xx_register_pru_can(void)
{
    omapl138_pru_can_device.dev.platform_data = &ti_pru_can_pdata;
    return platform_device_register(&omapl138_pru_can_device);
}

static const s8 da8xx_queue_tc_mapping[][2] = {
	/* {event queue no, TC no} */
	{0, 0},
	{1, 1},
	{-1, -1}
};

static const s8 da8xx_queue_priority_mapping[][2] = {
	/* {event queue no, Priority} */
	{0, 3},
	{1, 7},
	{-1, -1}
};

static const s8 da850_queue_tc_mapping[][2] = {
	/* {event queue no, TC no} */
	{0, 0},
	{-1, -1}
};

static const s8 da850_queue_priority_mapping[][2] = {
	/* {event queue no, Priority} */
	{0, 3},
	{-1, -1}
};

static struct edma_soc_info da830_edma_cc0_info = {
	.n_channel		= 32,
	.n_region		= 4,
	.n_slot			= 128,
	.n_tc			= 2,
	.n_cc			= 1,
	.queue_tc_mapping	= da8xx_queue_tc_mapping,
	.queue_priority_mapping	= da8xx_queue_priority_mapping,
	.default_queue		= EVENTQ_0,
};

static struct edma_soc_info *da830_edma_info[EDMA_MAX_CC] = {
	&da830_edma_cc0_info,
};

static struct edma_soc_info da850_edma_cc_info[] = {
	{
		.n_channel		= 32,
		.n_region		= 4,
		.n_slot			= 128,
		.n_tc			= 2,
		.n_cc			= 1,
		.queue_tc_mapping	= da8xx_queue_tc_mapping,
		.queue_priority_mapping	= da8xx_queue_priority_mapping,
	},
	{
		.n_channel		= 32,
		.n_region		= 4,
		.n_slot			= 128,
		.n_tc			= 1,
		.n_cc			= 1,
		.queue_tc_mapping	= da850_queue_tc_mapping,
		.queue_priority_mapping	= da850_queue_priority_mapping,
	},
};

static struct edma_soc_info *da850_edma_info[EDMA_MAX_CC] = {
	&da850_edma_cc_info[0],
	&da850_edma_cc_info[1],
};

static struct resource da830_edma_resources[] = {
	{
		.name	= "edma_cc0",
		.start	= DA8XX_TPCC_BASE,
		.end	= DA8XX_TPCC_BASE + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_tc0",
		.start	= DA8XX_TPTC0_BASE,
		.end	= DA8XX_TPTC0_BASE + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_tc1",
		.start	= DA8XX_TPTC1_BASE,
		.end	= DA8XX_TPTC1_BASE + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma0",
		.start	= IRQ_DA8XX_CCINT0,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "edma0_err",
		.start	= IRQ_DA8XX_CCERRINT,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct resource da850_edma_resources[] = {
	{
		.name	= "edma_cc0",
		.start	= DA8XX_TPCC_BASE,
		.end	= DA8XX_TPCC_BASE + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_tc0",
		.start	= DA8XX_TPTC0_BASE,
		.end	= DA8XX_TPTC0_BASE + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_tc1",
		.start	= DA8XX_TPTC1_BASE,
		.end	= DA8XX_TPTC1_BASE + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_cc1",
		.start	= DA850_TPCC1_BASE,
		.end	= DA850_TPCC1_BASE + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_tc2",
		.start	= DA850_TPTC2_BASE,
		.end	= DA850_TPTC2_BASE + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma0",
		.start	= IRQ_DA8XX_CCINT0,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "edma0_err",
		.start	= IRQ_DA8XX_CCERRINT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "edma1",
		.start	= IRQ_DA850_CCINT1,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.name	= "edma1_err",
		.start	= IRQ_DA850_CCERRINT1,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device da830_edma_device = {
	.name		= "edma",
	.id		= -1,
	.dev = {
		.platform_data = da830_edma_info,
	},
	.num_resources	= ARRAY_SIZE(da830_edma_resources),
	.resource	= da830_edma_resources,
};

static struct platform_device da850_edma_device = {
	.name		= "edma",
	.id		= -1,
	.dev = {
		.platform_data = da850_edma_info,
	},
	.num_resources	= ARRAY_SIZE(da850_edma_resources),
	.resource	= da850_edma_resources,
};

int __init da830_register_edma(struct edma_rsv_info *rsv)
{
	da830_edma_cc0_info.rsv = rsv;

	return platform_device_register(&da830_edma_device);
}

int __init da850_register_edma(struct edma_rsv_info *rsv[2])
{
	if (rsv) {
		da850_edma_cc_info[0].rsv = rsv[0];
		da850_edma_cc_info[1].rsv = rsv[1];
	}

	return platform_device_register(&da850_edma_device);
}

static struct resource da8xx_i2c_resources0[] = {
	{
		.start	= DA8XX_I2C0_BASE,
		.end	= DA8XX_I2C0_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_DA8XX_I2CINT0,
		.end	= IRQ_DA8XX_I2CINT0,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device da8xx_i2c_device0 = {
	.name		= "i2c_davinci",
	.id		= 1,
	.num_resources	= ARRAY_SIZE(da8xx_i2c_resources0),
	.resource	= da8xx_i2c_resources0,
};

static struct resource da8xx_i2c_resources1[] = {
	{
		.start	= DA8XX_I2C1_BASE,
		.end	= DA8XX_I2C1_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_DA8XX_I2CINT1,
		.end	= IRQ_DA8XX_I2CINT1,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device da8xx_i2c_device1 = {
	.name		= "i2c_davinci",
	.id		= 2,
	.num_resources	= ARRAY_SIZE(da8xx_i2c_resources1),
	.resource	= da8xx_i2c_resources1,
};

int __init da8xx_register_i2c(int instance,
		struct davinci_i2c_platform_data *pdata)
{
	struct platform_device *pdev;

	if (instance == 0)
		pdev = &da8xx_i2c_device0;
	else if (instance == 1)
		pdev = &da8xx_i2c_device1;
	else
		return -EINVAL;

	pdev->dev.platform_data = pdata;
	return platform_device_register(pdev);
}

static struct resource da8xx_watchdog_resources[] = {
	{
		.start	= DA8XX_WDOG_BASE,
		.end	= DA8XX_WDOG_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

struct platform_device davinci_wdt_device = {
	.name		= "watchdog",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(da8xx_watchdog_resources),
	.resource	= da8xx_watchdog_resources,
};

int __init da8xx_register_watchdog(void)
{
	return platform_device_register(&davinci_wdt_device);
}

static struct resource da8xx_emac_resources[] = {
	{
		.start	= DA8XX_EMAC_CPPI_PORT_BASE,
		.end	= DA8XX_EMAC_CPPI_PORT_BASE + 0x5000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_DA8XX_C0_RX_THRESH_PULSE,
		.end	= IRQ_DA8XX_C0_RX_THRESH_PULSE,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DA8XX_C0_RX_PULSE,
		.end	= IRQ_DA8XX_C0_RX_PULSE,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DA8XX_C0_TX_PULSE,
		.end	= IRQ_DA8XX_C0_TX_PULSE,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DA8XX_C0_MISC_PULSE,
		.end	= IRQ_DA8XX_C0_MISC_PULSE,
		.flags	= IORESOURCE_IRQ,
	},
};

struct emac_platform_data da8xx_emac_pdata = {
	.ctrl_reg_offset	= DA8XX_EMAC_CTRL_REG_OFFSET,
	.ctrl_mod_reg_offset	= DA8XX_EMAC_MOD_REG_OFFSET,
	.ctrl_ram_offset	= DA8XX_EMAC_RAM_OFFSET,
	.mdio_reg_offset	= DA8XX_MDIO_REG_OFFSET,
	.ctrl_ram_size		= DA8XX_EMAC_CTRL_RAM_SIZE,
	.version		= EMAC_VERSION_2,
};

static struct platform_device da8xx_emac_device = {
	.name		= "davinci_emac",
	.id		= 1,
	.dev = {
		.platform_data	= &da8xx_emac_pdata,
	},
	.num_resources	= ARRAY_SIZE(da8xx_emac_resources),
	.resource	= da8xx_emac_resources,
};

int __init da8xx_register_emac(void)
{
	return platform_device_register(&da8xx_emac_device);
}

static struct resource da830_mcasp1_resources[] = {
	{
		.name	= "mcasp1",
		.start	= DAVINCI_DA830_MCASP1_REG_BASE,
		.end	= DAVINCI_DA830_MCASP1_REG_BASE + (SZ_1K * 12) - 1,
		.flags	= IORESOURCE_MEM,
	},
	/* TX event */
	{
		.start	= DAVINCI_DA830_DMA_MCASP1_AXEVT,
		.end	= DAVINCI_DA830_DMA_MCASP1_AXEVT,
		.flags	= IORESOURCE_DMA,
	},
	/* RX event */
	{
		.start	= DAVINCI_DA830_DMA_MCASP1_AREVT,
		.end	= DAVINCI_DA830_DMA_MCASP1_AREVT,
		.flags	= IORESOURCE_DMA,
	},
};

static struct platform_device da830_mcasp1_device = {
	.name		= "davinci-mcasp",
	.id		= 1,
	.num_resources	= ARRAY_SIZE(da830_mcasp1_resources),
	.resource	= da830_mcasp1_resources,
};

static struct resource da850_mcasp_resources[] = {
	{
		.name	= "mcasp",
		.start	= DAVINCI_DA8XX_MCASP0_REG_BASE,
		.end	= DAVINCI_DA8XX_MCASP0_REG_BASE + (SZ_1K * 12) - 1,
		.flags	= IORESOURCE_MEM,
	},
	/* TX event */
	{
		.start	= DAVINCI_DA8XX_DMA_MCASP0_AXEVT,
		.end	= DAVINCI_DA8XX_DMA_MCASP0_AXEVT,
		.flags	= IORESOURCE_DMA,
	},
	/* RX event */
	{
		.start	= DAVINCI_DA8XX_DMA_MCASP0_AREVT,
		.end	= DAVINCI_DA8XX_DMA_MCASP0_AREVT,
		.flags	= IORESOURCE_DMA,
	},
};

static struct platform_device da850_mcasp_device = {
	.name		= "davinci-mcasp",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(da850_mcasp_resources),
	.resource	= da850_mcasp_resources,
};

void __init da8xx_register_mcasp(int id, struct snd_platform_data *pdata)
{
	/* DA830/OMAP-L137 has 3 instances of McASP */
	if (cpu_is_davinci_da830() && id == 1) {
		da830_mcasp1_device.dev.platform_data = pdata;
		platform_device_register(&da830_mcasp1_device);
	} else if (cpu_is_davinci_da850()) {
		da850_mcasp_device.dev.platform_data = pdata;
		platform_device_register(&da850_mcasp_device);
	}
}

static const struct display_panel disp_panel = {
	QVGA,
	16,
	16,
	COLOR_ACTIVE,
};

static struct lcd_ctrl_config lcd_cfg = {
	&disp_panel,
	.ac_bias		= 255,
	.ac_bias_intrpt		= 0,
	.dma_burst_sz		= 16,
	.bpp			= 16,
	.fdd			= 255,
	.tft_alt_mode		= 0,
	.stn_565_mode		= 0,
	.mono_8bit_mode		= 0,
	.invert_line_clock	= 1,
	.invert_frm_clock	= 1,
	.sync_edge		= 0,
	.sync_ctrl		= 1,
	.raster_order		= 0,
	.fifo_th		= 6,
};

struct da8xx_lcdc_platform_data sharp_lcd035q3dg01_pdata = {
	.manu_name		= "sharp",
	.controller_data	= &lcd_cfg,
	.type			= "Sharp_LCD035Q3DG01",
};

struct da8xx_lcdc_platform_data sharp_lk043t1dg01_pdata = {
	.manu_name		= "sharp",
	.controller_data	= &lcd_cfg,
	.type			= "Sharp_LK043T1DG01",
};

#if !defined(CONFIG_FB_DA8XX) && !defined(CONFIG_FB_DA8XX_MODULE)
static struct da8xx_clcd_platform_data da8xx_evm_clcd_pdata = {
	.version = CONFIG_SPACE_1,
};
#endif

static struct resource da8xx_lcdc_resources[] = {
	[0] = { /* registers */
		.start  = DA8XX_LCD_CNTRL_BASE,
		.end    = DA8XX_LCD_CNTRL_BASE + SZ_4K - 1,
		.flags  = IORESOURCE_MEM,
	},
	[1] = { /* interrupt */
		.start  = IRQ_DA8XX_LCDINT,
		.end    = IRQ_DA8XX_LCDINT,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device da8xx_lcdc_device = {
	.name		= "da8xx_lcdc",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(da8xx_lcdc_resources),
	.resource	= da8xx_lcdc_resources,
};

int __init da8xx_register_lcdc(struct da8xx_lcdc_platform_data *pdata)
{
#if !defined(CONFIG_FB_DA8XX) && !defined(CONFIG_FB_DA8XX_MODULE)
	if (cpu_is_davinci_da830())
		da8xx_evm_clcd_pdata.version = CONFIG_SPACE_0;

	da8xx_lcdc_device.dev.platform_data = &da8xx_evm_clcd_pdata;
#else
	da8xx_lcdc_device.dev.platform_data = pdata;
#endif
	return platform_device_register(&da8xx_lcdc_device);
}

static struct resource da8xx_mmcsd0_resources[] = {
	{		/* registers */
		.start	= DA8XX_MMCSD0_BASE,
		.end	= DA8XX_MMCSD0_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{		/* interrupt */
		.start	= IRQ_DA8XX_MMCSDINT0,
		.end	= IRQ_DA8XX_MMCSDINT0,
		.flags	= IORESOURCE_IRQ,
	},
	{		/* DMA RX */
		.start	= EDMA_CTLR_CHAN(0, 16),
		.end	= EDMA_CTLR_CHAN(0, 16),
		.flags	= IORESOURCE_DMA,
	},
	{		/* DMA TX */
		.start	= EDMA_CTLR_CHAN(0, 17),
		.end	= EDMA_CTLR_CHAN(0, 17),
		.flags	= IORESOURCE_DMA,
	},
};

static struct platform_device da8xx_mmcsd0_device = {
	.name		= "davinci_mmc",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(da8xx_mmcsd0_resources),
	.resource	= da8xx_mmcsd0_resources,
};

int __init da8xx_register_mmcsd0(struct davinci_mmc_config *config)
{
	da8xx_mmcsd0_device.dev.platform_data = config;
	return platform_device_register(&da8xx_mmcsd0_device);
}

static struct resource da8xx_rtc_resources[] = {
	{
		.start		= DA8XX_RTC_BASE,
		.end		= DA8XX_RTC_BASE + SZ_4K - 1,
		.flags		= IORESOURCE_MEM,
	},
	{ /* timer irq */
		.start		= IRQ_DA8XX_RTC,
		.end		= IRQ_DA8XX_RTC,
		.flags		= IORESOURCE_IRQ,
	},
	{ /* alarm irq */
		.start		= IRQ_DA8XX_RTC,
		.end		= IRQ_DA8XX_RTC,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device da8xx_rtc_device = {
	.name           = "omap_rtc",
	.id             = -1,
	.num_resources	= ARRAY_SIZE(da8xx_rtc_resources),
	.resource	= da8xx_rtc_resources,
};

int da8xx_register_rtc(void)
{
	int ret;

	/* Unlock the rtc's registers */
	__raw_writel(0x83e70b13, IO_ADDRESS(DA8XX_RTC_BASE + 0x6c));
	__raw_writel(0x95a4f1e0, IO_ADDRESS(DA8XX_RTC_BASE + 0x70));

	ret = platform_device_register(&da8xx_rtc_device);
	if (!ret)
		/* Atleast on DA850, RTC is a wakeup source */
		device_init_wakeup(&da8xx_rtc_device.dev, true);

	return ret;
}

static void __iomem *da8xx_ddr2_ctlr_base;
void __iomem * __init da8xx_get_mem_ctlr(void)
{
	if (da8xx_ddr2_ctlr_base)
		return da8xx_ddr2_ctlr_base;

	da8xx_ddr2_ctlr_base = ioremap(DA8XX_DDR2_CTL_BASE, SZ_32K);
	if (!da8xx_ddr2_ctlr_base)
		pr_warning("%s: Unable to map DDR2 controller",	__func__);

	return da8xx_ddr2_ctlr_base;
}

static struct resource da8xx_cpuidle_resources[] = {
	{
		.start		= DA8XX_DDR2_CTL_BASE,
		.end		= DA8XX_DDR2_CTL_BASE + SZ_32K - 1,
		.flags		= IORESOURCE_MEM,
	},
};

/* DA8XX devices support DDR2 power down */
static struct davinci_cpuidle_config da8xx_cpuidle_pdata = {
	.ddr2_pdown	= 1,
};


static struct platform_device da8xx_cpuidle_device = {
	.name			= "cpuidle-davinci",
	.num_resources		= ARRAY_SIZE(da8xx_cpuidle_resources),
	.resource		= da8xx_cpuidle_resources,
	.dev = {
		.platform_data	= &da8xx_cpuidle_pdata,
	},
};

int __init da8xx_register_cpuidle(void)
{
	da8xx_cpuidle_pdata.ddr2_ctlr_base = da8xx_get_mem_ctlr();

	return platform_device_register(&da8xx_cpuidle_device);
}

//static struct davinci_spi_platform_data da850_spi0_pdata = {
//	.version 	= SPI_VERSION_2,
//	.num_chipselect = 1,
//	.wdelay		= 0,
//	.odd_parity	= 0,
//	.parity_enable	= 0,
//	.wait_enable	= 0,
//	.timer_disable  = 0,
//	.clk_internal	= 1,
//	.cs_hold	= 1,
//	.intr_level	= 0,
//	.poll_mode	= 1,
//	.use_dma	= 1, 
//	.c2tdelay	= 8,
//	.t2cdelay	= 8,
//};

//static struct davinci_spi_platform_data da850_spi1_pdata = {
//	.version 	= SPI_VERSION_2,
//	.num_chipselect = 1,
//	.wdelay		= 0,
//	.odd_parity	= 0,
//	.parity_enable	= 0,
//	.wait_enable	= 0,
//	.timer_disable  = 0,
//	.clk_internal	= 1,
//	.cs_hold	= 1,
//	.intr_level	= 0,
//	.poll_mode	= 1,
//	.use_dma	= 0,
//	.c2tdelay	= 8,
//	.t2cdelay	= 8,
//};

static struct resource da8xx_spi0_resources[] = {
	[0] = {
		.start = 0x01C41000,
		.end = 0x01C41000 + 0xfff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_DA8XX_SPINT0,
		.end = IRQ_DA8XX_SPINT0,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
		.start = EDMA_CTLR_CHAN(0, 14),
		.end = EDMA_CTLR_CHAN(0, 14),
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.start = EDMA_CTLR_CHAN(0, 15),
		.end = EDMA_CTLR_CHAN(0, 15),
		.flags = IORESOURCE_DMA,
	},
	[4] = {
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_DMA,
	},
};

static struct resource da8xx_spi1_resources[] = {
	[0] = {
		.start = 0x01F0E000,
		.end = 0x01F0E000 + 0xfff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_DA8XX_SPINT1,
		.end = IRQ_DA8XX_SPINT1,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
		.start = EDMA_CTLR_CHAN(0, 18),
		.end = EDMA_CTLR_CHAN(0, 18),
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.start = EDMA_CTLR_CHAN(0, 19),
		.end = EDMA_CTLR_CHAN(0, 19),
		.flags = IORESOURCE_DMA,
	},
	[4] = {
		.start = 1,
		.end = 1,
		.flags = IORESOURCE_DMA,
	},
};


//static struct platform_device da850_spi0_device = {
//	.name = "spi_davinci",
//	.id = 0,
//	.resource = da850_spi0_resources,
//	.num_resources = ARRAY_SIZE(da850_spi0_resources),
//	.dev = {
//		.platform_data = &da850_spi0_pdata,
//	},
//};

static struct platform_device da8xx_spi_device[] = {
	[0] = {
		.name = "spi_davinci",
		.id = 0,
		.num_resources = ARRAY_SIZE(da8xx_spi0_resources),
		.resource = da8xx_spi0_resources,
	},
	[1] = {
		.name = "spi_davinci",
		.id = 1,
		.num_resources = ARRAY_SIZE(da8xx_spi1_resources),
		.resource = da8xx_spi1_resources,
 	},
 };

int __init da8xx_register_spi(int instance,
			      struct davinci_spi_platform_data *pdata)
 {
	struct platform_device *pdev;

	if (instance == 0)
		pdev = &da8xx_spi_device[0];
	else if (instance == 1)
		pdev = &da8xx_spi_device[1];
	else
		return -EINVAL;

	pdev->dev.platform_data = pdata;

	return platform_device_register(pdev);
 }

//void __init da850_init_spi0(unsigned chipselect_mask,
//		struct spi_board_info *info, unsigned len)
//{
//	spi_register_board_info(info, len);
//
//	platform_device_register(&da850_spi0_device);
//}

//static struct platform_device da850_spi1_device = {
//	.name = "spi_davinci",
//	.id = 1,
//	.resource = da850_spi1_resources,
//	.num_resources = ARRAY_SIZE(da850_spi1_resources),
//	.dev = {
//		.platform_data = &da850_spi1_pdata,
//	},
//};

//void __init da850_init_spi1(unsigned chipselect_mask,
//		struct spi_board_info *info, unsigned len)
//{
//	spi_register_board_info(info, len);

//	platform_device_register(&da850_spi1_device);
//}

//static struct davinci_spi_platform_data da830_spi0_pdata = {
//	.version 	= SPI_VERSION_2,
//	.num_chipselect = 1,
//	.wdelay		= 0,
//	.odd_parity	= 0,
//	.parity_enable	= 0,
//	.wait_enable	= 0,
//	.timer_disable  = 0,
//	.clk_internal	= 1,
//	.cs_hold	= 1,
//	.intr_level	= 0,
//	.poll_mode	= 1,
//	.use_dma	= 1,
//	.c2tdelay	= 8,
//	.t2cdelay	= 8,
//};

/*static struct resource da830_spi0_resources[] = {
	[0] = {
		.start = 0x01C41000,
		.end = 0x01C41000 + 0xfff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_DA8XX_SPINT0,
		.end = IRQ_DA8XX_SPINT0,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
		.start = EDMA_CTLR_CHAN(0, 14),
		.end = EDMA_CTLR_CHAN(0, 14),
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.start = EDMA_CTLR_CHAN(0, 15),
		.end = EDMA_CTLR_CHAN(0, 15),
		.flags = IORESOURCE_DMA,
	},
	[4] = {
		.start = 1,
		.end = 1,
		.flags = IORESOURCE_DMA,
	},
};

static struct platform_device da830_spi0_device = {
	.name = "spi_davinci",
	.id = 0,
	.resource = da830_spi0_resources,
	.num_resources = ARRAY_SIZE(da830_spi0_resources),
	.dev = {
		.platform_data = &da830_spi0_pdata,
	},
};

void __init da830_init_spi0(unsigned chipselect_mask,
		struct spi_board_info *info, unsigned len)
{
	spi_register_board_info(info, len);

	platform_device_register(&da830_spi0_device);
}*/

static struct resource da850_mcbsp0_resources[] = {
	{
		.start	= 0x01D10000,
		.end	= 0x01D10FFF,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= 0x01F10000,
		.end	= 0x01F10FFF,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_DA850_MCBSP0RINT,
		.end	= IRQ_DA850_MCBSP0RINT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DA850_MCBSP0XINT,
		.end	= IRQ_DA850_MCBSP0XINT,
		.flags	= IORESOURCE_IRQ,
	},
	/* first RX, then TX */
	{
		.start	= 2,
		.end	= 2,
		.flags	= IORESOURCE_DMA,
	},
	{
		.start	= 3,
		.end	= 3,
		.flags	= IORESOURCE_DMA,
	},
};

static struct resource da850_mcbsp1_resources[] = {
	{
		.start	= 0x01D11000,
		.end	= 0x01D11FFF,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= 0x01F11000,
		.end	= 0x01F11FFF,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_DA850_MCBSP1RINT,
		.end	= IRQ_DA850_MCBSP1RINT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DA850_MCBSP1XINT,
		.end	= IRQ_DA850_MCBSP1XINT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= 4,
		.end	= 4,
		.flags	= IORESOURCE_DMA,
	},
	{
		.start	= 5,
		.end	= 5,
		.flags	= IORESOURCE_DMA,
	},
};
static struct platform_device da850_mcbsp0_device = {
	.name		= "davinci-mcbsp",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(da850_mcbsp0_resources),
	.resource	= da850_mcbsp0_resources,
};

static struct platform_device da850_mcbsp1_device = {
	.name		= "davinci-mcbsp",
	.id		= 1,
	.num_resources	= ARRAY_SIZE(da850_mcbsp1_resources),
	.resource	= da850_mcbsp1_resources,
};

int
__init da850_init_mcbsp(struct davinci_mcbsp_platform_data *pdata)
{
	struct platform_device *pdev;

	if (!pdata->inst) {
		pdev = &da850_mcbsp0_device;
	} else if (pdata->inst == 1) {
		pdev = &da850_mcbsp1_device;
	} else {
		printk(KERN_ERR "Cannot initialize McBSP device, Invalid id\n");
		return -EINVAL;
	}

	pdev->dev.platform_data = pdata;
	return platform_device_register(pdev);
}

static struct resource da850_ahci_resources[] = {
	{
		.start	=	DA850_SATA_BASE,
		.end	=	DA850_SATA_BASE + 0x1fff,
		.flags	=	IORESOURCE_MEM,
	},
	{
		.start	=	IRQ_DA850_SATAINT,
		.flags	=	IORESOURCE_IRQ,
	}
};

static int da850_ahci_data = 10;
static struct platform_device da850_ahci_device = {
	.name	=	"ahci",
	.id	=	-1,
	.dev	=	{
				.platform_data = &da850_ahci_data,
				.coherent_dma_mask = 0xffffffff,
			},
	.num_resources = ARRAY_SIZE(da850_ahci_resources),
	.resource	= da850_ahci_resources,
};

int da8xx_register_sata(void)
{
	return platform_device_register(&da850_ahci_device);
}

#define CFGCHIP2	DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP2_REG)
/*
 * Configure the USB PHY for DA8xx platforms.
 */
static int da8xx_usb_phy_config(struct device *dev, u8 mode, int is_on)
{
	u32 cfgchip2;

	/*
	 * Start the on-chip PHY and its PLL.
	 */
	cfgchip2 = __raw_readl(CFGCHIP2);

	if (is_on) {
		/* Check whether USB0 PHY is already powered on */
		if (cfgchip2 & CFGCHIP2_PHY_PLLON)
			return 0;

		cfgchip2 &= ~(CFGCHIP2_RESET | CFGCHIP2_PHYPWRDN |
				CFGCHIP2_OTGPWRDN | CFGCHIP2_OTGMODE |
				CFGCHIP2_REFFREQ);
		cfgchip2 |= CFGCHIP2_SESENDEN | CFGCHIP2_VBDTCTEN |
				CFGCHIP2_PHY_PLLON | CFGCHIP2_REFFREQ_24MHZ;
		switch (mode) {
		case MUSB_OTG:
		case MUSB_DUAL_ROLE:
			cfgchip2 |= CFGCHIP2_NO_OVERRIDE;
			break;
		case MUSB_HOST:
			cfgchip2 |= CFGCHIP2_FORCE_HOST;
			break;
		case MUSB_PERIPHERAL:
			cfgchip2 |= CFGCHIP2_FORCE_DEVICE;
			break;
		default:
			pr_err("Trying to set unsupported mode");
			break;
		}
	} else {
		/* Ensure that usb1.1 interface clk is not being sourced from
		 * usb0 interface.  If so do not power down usb0 PHY
		 */
		if ((cfgchip2 & CFGCHIP2_USB1SUSPENDM) &&
			!(cfgchip2 & CFGCHIP2_USB1PHYCLKMUX)) {
			printk(KERN_WARNING "USB1 interface active -\
				Cannot Power down USB0 PHY\n");
			return 0;
		}

		cfgchip2 &= ~CFGCHIP2_PHY_PLLON;
		cfgchip2 |= CFGCHIP2_PHYPWRDN | CFGCHIP2_OTGPWRDN;
	}

	__raw_writel(cfgchip2, CFGCHIP2);

	if (is_on) {
		while (!(__raw_readl(CFGCHIP2) & CFGCHIP2_PHYCLKGD))
			cpu_relax();
		pr_info("Waiting for USB PHY clock good...\n");
	}

	return 0;
}

static struct resource usb_resources[] = {
	{
		.start  = DA8XX_USB0_BASE,
		.end    = DA8XX_USB0_BASE + 0x5ff,
		.flags  = IORESOURCE_MEM,
	},
	{
		.start  = IRQ_DA8XX_USB_INT,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct plat_res_data da8xx_usb_res;
static struct usb_plat_data da8xx_usb_plat_data;

/*
 * Initialize DA8xx related MUSB information such as Memory maps, IRQ etc.
 * Since DA8xx supprot a single MUSB controller initialize the instance
 * value to 1.
 */
void da8xx_usb20_configure(struct musb_hdrc_platform_data *pdata, u8 num_inst)
{
	pdata->phy_config = da8xx_usb_phy_config;

	da8xx_usb_res.plat_data = pdata;
	da8xx_usb_res.res_data = usb_resources;
	da8xx_usb_res.num_res = ARRAY_SIZE(usb_resources);

	da8xx_usb_plat_data.prdata = &da8xx_usb_res;
	da8xx_usb_plat_data.num_inst = num_inst;

	/* There are multiple USB instances on DA8xx platform hence populate
	 * the clock information accordingly.  This overrides the generic
	 * clock setting done in usb.c file.
	 */
	pdata->clock = "usb20";

	/* Call the generic platform register function.  The USB
	 * configuration w.r.t no. of ep's, capabalities etc. are common
	 * across DA8xx/OMAPL13x platforms and hence allow the generic handler
	 * to populate the information.
	 */
	setup_usb(&da8xx_usb_plat_data);
}

static struct resource da8xx_usb11_resources[] = {
	[0] = {
		.start	= DA8XX_USB1_BASE,
		.end	= DA8XX_USB1_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_DA8XX_IRQN,
		.end	= IRQ_DA8XX_IRQN,
		.flags	= IORESOURCE_IRQ,
	},
};

static u64 da8xx_usb11_dma_mask = DMA_BIT_MASK(32);

static struct platform_device da8xx_usb11_device = {
	.name		= "ohci",
	.id			= 0,
	.dev = {
		.dma_mask		= &da8xx_usb11_dma_mask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.num_resources	= ARRAY_SIZE(da8xx_usb11_resources),
	.resource	= da8xx_usb11_resources,
};

int __init da8xx_register_usb11(struct da8xx_ohci_root_hub *pdata)
{
	da8xx_usb11_device.dev.platform_data = pdata;
	return platform_device_register(&da8xx_usb11_device);
}
