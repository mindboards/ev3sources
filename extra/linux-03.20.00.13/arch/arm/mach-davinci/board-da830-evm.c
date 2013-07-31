/*
 * TI DA830/OMAP L137 EVM board
 *
 * Author: Mark A. Greer <mgreer@mvista.com>
 * Derived from: arch/arm/mach-davinci/board-dm644x-evm.c
 *
 * 2007, 2009 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c/pcf857x.h>
#include <linux/i2c/at24.h>
#include <linux/i2c/tsc2004.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/usb/musb.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <mach/cp_intc.h>
#include <mach/mux.h>
#include <mach/nand.h>
#include <mach/da8xx.h>
#include <mach/usb.h>

#define DA830_EVM_PHY_MASK		0x0
#define DA830_EVM_MDIO_FREQUENCY	2200000	/* PHY bus frequency */

#define DA830_EMIF25_ASYNC_DATA_CE3_BASE	0x62000000
#define DA830_EMIF25_CONTROL_BASE		0x68000000

/*
 * USB1 VBUS is controlled by GPIO1[15], over-current is reported on GPIO2[4].
 */
#define ON_BD_USB_DRV	GPIO_TO_PIN(1, 15)
#define ON_BD_USB_OVC	GPIO_TO_PIN(2, 4)

static const short da830_evm_usb11_pins[] = {
	DA830_GPIO1_15, DA830_GPIO2_4,
	-1
};

static da8xx_ocic_handler_t da830_evm_usb_ocic_handler;

static int da830_evm_usb_set_power(unsigned port, int on)
{
	gpio_set_value(ON_BD_USB_DRV, on);
	return 0;
}

static int da830_evm_usb_get_power(unsigned port)
{
	return gpio_get_value(ON_BD_USB_DRV);
}

static int da830_evm_usb_get_oci(unsigned port)
{
	return !gpio_get_value(ON_BD_USB_OVC);
}

static irqreturn_t da830_evm_usb_ocic_irq(int, void *);

static int da830_evm_usb_ocic_notify(da8xx_ocic_handler_t handler)
{
	int irq 	= gpio_to_irq(ON_BD_USB_OVC);
	int error	= 0;

	if (handler != NULL) {
		da830_evm_usb_ocic_handler = handler;

		error = request_irq(irq, da830_evm_usb_ocic_irq, IRQF_DISABLED |
				    IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
				    "OHCI over-current indicator", NULL);
		if (error)
			printk(KERN_ERR "%s: could not request IRQ to watch "
			       "over-current indicator changes\n", __func__);
	} else
		free_irq(irq, NULL);

	return error;
}

static struct da8xx_ohci_root_hub da830_evm_usb11_pdata = {
	.set_power	= da830_evm_usb_set_power,
	.get_power	= da830_evm_usb_get_power,
	.get_oci	= da830_evm_usb_get_oci,
	.ocic_notify	= da830_evm_usb_ocic_notify,

	/* TPS2065 switch @ 5V */
	.potpgt		= (3 + 1) / 2,	/* 3 ms max */
};

static irqreturn_t da830_evm_usb_ocic_irq(int irq, void *dev_id)
{
	da830_evm_usb_ocic_handler(&da830_evm_usb11_pdata, 1);
	return IRQ_HANDLED;
}

static struct musb_hdrc_platform_data usb_evm_data[] = {
	{
#ifdef CONFIG_USB_MUSB_OTG
		.mode = MUSB_OTG,
#elif defined(CONFIG_USB_MUSB_DUAL_ROLE)
		.mode = MUSB_DUAL_ROLE,
#elif defined(CONFIG_USB_MUSB_PERIPHERAL)
		.mode =  MUSB_PERIPHERAL,
#elif defined(CONFIG_USB_MUSB_HOST)
		.mode = MUSB_HOST,
#endif
		.power = 255,
		.potpgt = 8,
		.set_vbus = NULL, /* VBUs is directly controlled by the IP */
	}
};

static __init void da830_evm_usb_init(void)
{
	u32 cfgchip2;
	int ret;

	/*
	 * Setup the Ref. clock frequency for the EVM at 24 MHz.
	 */
	cfgchip2 = __raw_readl(DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP2_REG));
	cfgchip2 &= ~CFGCHIP2_REFFREQ;
	cfgchip2 |=  CFGCHIP2_REFFREQ_24MHZ;
	__raw_writel(cfgchip2, DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP2_REG));

	/* USB_REFCLKIN is not used. */
	ret = davinci_cfg_reg(DA830_USB0_DRVVBUS);
	if (ret)
		pr_warning("%s: USB 2.0 PinMux setup failed: %d\n",
			   __func__, ret);

	da8xx_usb20_configure(usb_evm_data, ARRAY_SIZE(usb_evm_data));

	ret = da8xx_pinmux_setup(da830_evm_usb11_pins);
	if (ret) {
		pr_warning("%s: USB 1.1 PinMux setup failed: %d\n",
			   __func__, ret);
		return;
	}

	ret = gpio_request(ON_BD_USB_DRV, "ON_BD_USB_DRV");
	if (ret) {
		printk(KERN_ERR "%s: failed to request GPIO for USB 1.1 port "
		       "power control: %d\n", __func__, ret);
		return;
	}
	gpio_direction_output(ON_BD_USB_DRV, 0);

	ret = gpio_request(ON_BD_USB_OVC, "ON_BD_USB_OVC");
	if (ret) {
		printk(KERN_ERR "%s: failed to request GPIO for USB 1.1 port "
		       "over-current indicator: %d\n", __func__, ret);
		return;
	}
	gpio_direction_input(ON_BD_USB_OVC);

	ret = da8xx_register_usb11(&da830_evm_usb11_pdata);
	if (ret)
		pr_warning("%s: USB 1.1 registration failed: %d\n",
			   __func__, ret);
}

static __init int da830_evm_config_pru_suart(void)
{
    int ret;

    if (!machine_is_davinci_da830_evm())
        return 0;

    ret = da8xx_pinmux_setup(da830_pru_suart_pins);
    if (ret)
        pr_warning("da830_evm_init: da830_pru_suart_pins mux setup failed: %d\n",
                ret);

    ret = da8xx_register_pru_suart();
    if (ret)
        pr_warning("da830_evm_init: pru suart registration failed: %d\n", ret);
    return ret;
}
device_initcall(da830_evm_config_pru_suart);

static struct davinci_uart_config da830_evm_uart_config __initdata = {
	.enabled_uarts = 0x7,
};

static const short da830_evm_mcasp1_pins[] = {
	DA830_AHCLKX1, DA830_ACLKX1, DA830_AFSX1, DA830_AHCLKR1, DA830_AFSR1,
	DA830_AMUTE1, DA830_AXR1_0, DA830_AXR1_1, DA830_AXR1_2, DA830_AXR1_5,
	DA830_ACLKR1, DA830_AXR1_6, DA830_AXR1_7, DA830_AXR1_8, DA830_AXR1_10,
	DA830_AXR1_11,
	-1
};

static u8 da830_iis_serializer_direction[] = {
	RX_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	TX_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
};

static struct snd_platform_data da830_evm_snd_data = {
	.tx_dma_offset  = 0x2000,
	.rx_dma_offset  = 0x2000,
	.op_mode        = DAVINCI_MCASP_IIS_MODE,
	.num_serializer = ARRAY_SIZE(da830_iis_serializer_direction),
	.tdm_slots      = 2,
	.serial_dir     = da830_iis_serializer_direction,
	.eventq_no      = EVENTQ_0,
	.version	= MCASP_VERSION_2,
	.txnumevt	= 1,
	.rxnumevt	= 1,
};

/*
 * GPIO2[1] is used as MMC_SD_WP and GPIO2[2] as MMC_SD_INS.
 */
static const short da830_evm_mmc_sd_pins[] = {
	DA830_MMCSD_DAT_0, DA830_MMCSD_DAT_1, DA830_MMCSD_DAT_2,
	DA830_MMCSD_DAT_3, DA830_MMCSD_DAT_4, DA830_MMCSD_DAT_5,
	DA830_MMCSD_DAT_6, DA830_MMCSD_DAT_7, DA830_MMCSD_CLK,
	DA830_MMCSD_CMD,   DA830_GPIO2_1,     DA830_GPIO2_2,
	-1
};

#define DA830_MMCSD_WP_PIN		GPIO_TO_PIN(2, 1)
#define DA830_MMCSD_CD_PIN		GPIO_TO_PIN(2, 2)

static int da830_evm_mmc_get_ro(int index)
{
	return gpio_get_value(DA830_MMCSD_WP_PIN);
}

static int da830_evm_mmc_get_cd(int index)
{
	return !gpio_get_value(DA830_MMCSD_CD_PIN);
}

static struct davinci_mmc_config da830_evm_mmc_config = {
	.get_ro			= da830_evm_mmc_get_ro,
	.get_cd			= da830_evm_mmc_get_cd,
	.wires			= 4,
	.max_freq		= 50000000,
	.caps			= MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED,
	.version		= MMC_CTLR_VERSION_2,
};

static inline void da830_evm_init_mmc(void)
{
	int ret;

	ret = da8xx_pinmux_setup(da830_evm_mmc_sd_pins);
	if (ret) {
		pr_warning("da830_evm_init: mmc/sd mux setup failed: %d\n",
				ret);
		return;
	}

	ret = gpio_request(DA830_MMCSD_WP_PIN, "MMC WP");
	if (ret) {
		pr_warning("da830_evm_init: can not open GPIO %d\n",
			   DA830_MMCSD_WP_PIN);
		return;
	}
	gpio_direction_input(DA830_MMCSD_WP_PIN);

	ret = gpio_request(DA830_MMCSD_CD_PIN, "MMC CD\n");
	if (ret) {
		pr_warning("da830_evm_init: can not open GPIO %d\n",
			   DA830_MMCSD_CD_PIN);
		gpio_free(DA830_MMCSD_WP_PIN);
		return;
	}
	gpio_direction_input(DA830_MMCSD_CD_PIN);

	ret = da8xx_register_mmcsd0(&da830_evm_mmc_config);
	if (ret) {
		pr_warning("da830_evm_init: mmc/sd registration failed: %d\n",
				ret);
		gpio_free(DA830_MMCSD_WP_PIN);
		gpio_free(DA830_MMCSD_CD_PIN);
	}
}

/*
 * UI board NAND/NOR flashes only use 8-bit data bus.
 */
static const short da830_evm_emif25_pins[] = {
	DA830_EMA_D_0, DA830_EMA_D_1, DA830_EMA_D_2, DA830_EMA_D_3,
	DA830_EMA_D_4, DA830_EMA_D_5, DA830_EMA_D_6, DA830_EMA_D_7,
	DA830_EMA_A_0, DA830_EMA_A_1, DA830_EMA_A_2, DA830_EMA_A_3,
	DA830_EMA_A_4, DA830_EMA_A_5, DA830_EMA_A_6, DA830_EMA_A_7,
	DA830_EMA_A_8, DA830_EMA_A_9, DA830_EMA_A_10, DA830_EMA_A_11,
	DA830_EMA_A_12, DA830_EMA_BA_0, DA830_EMA_BA_1, DA830_NEMA_WE,
	DA830_NEMA_CS_2, DA830_NEMA_CS_3, DA830_NEMA_OE, DA830_EMA_WAIT_0,
	-1
};

#if defined(CONFIG_MMC_DAVINCI) || defined(CONFIG_MMC_DAVINCI_MODULE)
#define HAS_MMC	1
#else
#define HAS_MMC	0
#endif

#ifdef CONFIG_DA830_UI_NAND
static struct mtd_partition da830_evm_nand_partitions[] = {
	/* bootloader params in the first sector */
	[0] = {
		.name		= "u-boot env",
		.offset		= 0,
		.size		= SZ_128K,
		.mask_flags	= MTD_WRITEABLE,	/* force read-only */
	},
	/* primary bootloader in the next sector */
	[1] = {
		.name		= "UBL",
		.offset		= MTDPART_OFS_APPEND,
		.size		= SZ_128K,
		.mask_flags	= MTD_WRITEABLE,	/* force read-only */
	},
	/* bootloader (U-Boot, etc) in next sector */
	[2] = {
		.name		= "u-boot",
		.offset		= MTDPART_OFS_APPEND,
		.size		= 4 * SZ_128K,
		.mask_flags	= MTD_WRITEABLE,	/* force read-only */
	},
	/* kernel */
	[3] = {
		.name		= "kernel",
		.offset		= MTDPART_OFS_APPEND,
		.size		= SZ_2M,
		.mask_flags	= 0,
	},
	/* file system */
	[4] = {
		.name		= "filesystem",
		.offset		= MTDPART_OFS_APPEND,
		.size		= MTDPART_SIZ_FULL,
		.mask_flags	= 0,
	}
};

/* flash bbt decriptors */
static uint8_t da830_evm_nand_bbt_pattern[] = { 'B', 'b', 't', '0' };
static uint8_t da830_evm_nand_mirror_pattern[] = { '1', 't', 'b', 'B' };

static struct nand_bbt_descr da830_evm_nand_bbt_main_descr = {
	.options	= NAND_BBT_LASTBLOCK | NAND_BBT_CREATE |
			  NAND_BBT_WRITE | NAND_BBT_2BIT |
			  NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs		= 2,
	.len		= 4,
	.veroffs	= 16,
	.maxblocks	= 4,
	.pattern	= da830_evm_nand_bbt_pattern
};

static struct nand_bbt_descr da830_evm_nand_bbt_mirror_descr = {
	.options	= NAND_BBT_LASTBLOCK | NAND_BBT_CREATE |
			  NAND_BBT_WRITE | NAND_BBT_2BIT |
			  NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs		= 2,
	.len		= 4,
	.veroffs	= 16,
	.maxblocks	= 4,
	.pattern	= da830_evm_nand_mirror_pattern
};

static struct davinci_aemif_timing da830_evm_nandflash_timing = {
	.wsetup         = 24,
	.wstrobe        = 21,
	.whold          = 14,
	.rsetup         = 19,
	.rstrobe        = 50,
	.rhold          = 0,
	.ta             = 20,
};

static struct davinci_nand_pdata da830_evm_nand_pdata = {
	.parts		= da830_evm_nand_partitions,
	.nr_parts	= ARRAY_SIZE(da830_evm_nand_partitions),
	.ecc_mode	= NAND_ECC_HW,
	.ecc_bits	= 4,
	.options	= NAND_USE_FLASH_BBT,
	.bbt_td		= &da830_evm_nand_bbt_main_descr,
	.bbt_md		= &da830_evm_nand_bbt_mirror_descr,
	.timing         = &da830_evm_nandflash_timing,
};

static struct resource da830_evm_nand_resources[] = {
	[0] = {		/* First memory resource is NAND I/O window */
		.start	= DA830_EMIF25_ASYNC_DATA_CE3_BASE,
		.end	= DA830_EMIF25_ASYNC_DATA_CE3_BASE + PAGE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {		/* Second memory resource is AEMIF control registers */
		.start	= DA830_EMIF25_CONTROL_BASE,
		.end	= DA830_EMIF25_CONTROL_BASE + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device da830_evm_nand_device = {
	.name		= "davinci_nand",
	.id		= 1,
	.dev		= {
		.platform_data	= &da830_evm_nand_pdata,
	},
	.num_resources	= ARRAY_SIZE(da830_evm_nand_resources),
	.resource	= da830_evm_nand_resources,
};

static inline void da830_evm_init_nand(int mux_mode)
{
	int ret;

	if (HAS_MMC) {
		pr_warning("WARNING: both MMC/SD and NAND are "
				"enabled, but they share AEMIF pins.\n"
				"\tDisable MMC/SD for NAND support.\n");
		return;
	}

	ret = da8xx_pinmux_setup(da830_evm_emif25_pins);
	if (ret)
		pr_warning("da830_evm_init: emif25 mux setup failed: %d\n",
				ret);

	ret = platform_device_register(&da830_evm_nand_device);
	if (ret)
		pr_warning("da830_evm_init: NAND device not registered.\n");

	gpio_direction_output(mux_mode, 1);
}
#else
static inline void da830_evm_init_nand(int mux_mode) { }
#endif

static struct mtd_partition spi_flash_partitions[] = {
	[0] = {
		.name = "U-Boot",
		.offset = 0,
		.size = SZ_256K,
		.mask_flags = MTD_WRITEABLE,
	},
	[1] = {
		.name = "U-Boot Environment",
		.offset = MTDPART_OFS_APPEND,
		.size = SZ_16K,
		.mask_flags = MTD_WRITEABLE,
	},
	[2] = {
		.name = "Linux",
		.offset = MTDPART_OFS_NXTBLK,
		.size = MTDPART_SIZ_FULL,
		.mask_flags = 0,
	},
};

static struct flash_platform_data spi_flash_data = {
	.name = "m25p80",
	.parts = spi_flash_partitions,
	.nr_parts = ARRAY_SIZE(spi_flash_partitions),
	.type = "w25x32",
};

static struct spi_board_info da830_spi_board_info[] = {
	[0] = {
		.modalias = "m25p80",
		.platform_data = &spi_flash_data,
		.mode = SPI_MODE_0,
		.max_speed_hz = 30000000,       /* max sample rate at 3V */
		.bus_num = 0,
		.chip_select = 0,
	},
};

#ifdef CONFIG_DA830_UI_LCD
static int da830_lcd_hw_init(void)
{
	void __iomem *cfg_mstpri2_base;
	u32 val;

	/*
	 * Reconfigure the LCDC priority to the highest to ensure that
	 * the throughput/latency requirements for the LCDC are met.
	 */
	cfg_mstpri2_base = DA8XX_SYSCFG0_VIRT(DA8XX_MSTPRI2_REG);

	val = __raw_readl(cfg_mstpri2_base);
	val &= 0x0fffffff;
	__raw_writel(val, cfg_mstpri2_base);

	return 0;
}

static const short da830_evm_lcdc_pins[] = {
	DA830_NLCD_AC_ENB_CS,
	-1
};

static inline void da830_evm_init_lcdc(int mux_mode,
				struct da8xx_lcdc_platform_data *pdata)
{
	int ret;

	ret = da8xx_pinmux_setup(da830_lcdcntl_pins);
	if (ret)
		pr_warning("da830_evm_init: lcdcntl mux setup failed: %d\n",
				ret);

#if !defined(CONFIG_FB_DA8XX) && !defined(CONFIG_FB_DA8XX_MODULE)
	ret = da8xx_pinmux_setup(da830_evm_lcdc_pins);
	if (ret)
		pr_warning("da830_evm_init: evm lcd mux setup failed: %d\n",
				ret);
#endif

	ret = da830_lcd_hw_init();
	if (ret)
		pr_warning("da830_evm_init: lcd hw init failed: %d\n", ret);

	ret = da8xx_register_lcdc(pdata);
	if (ret)
		pr_warning("da830_evm_init: lcd setup failed: %d\n", ret);

	gpio_direction_output(mux_mode, 0);
}
#else
static inline void da830_evm_init_lcdc(int mux_mode,
				struct da8xx_lcdc_platform_data *pdata) {}
#endif

static struct at24_platform_data da830_evm_i2c_eeprom_info = {
	.byte_len	= SZ_256K / 8,
	.page_size	= 64,
	.flags		= AT24_FLAG_ADDR16,
	.setup		= davinci_get_mac_addr,
	.context	= (void *)0x7f00,
};

#define DA830_EVM_TSC2004_ADDRESS	0x49
#define DA830_LCD_BACKLIGHT_PIN		GPIO_TO_PIN(2, 12)
#define DA830_LCD_AC_ENB_CS		GPIO_TO_PIN(1, 11)

static int __init da830_evm_ui_expander_setup(struct i2c_client *client,
		int gpio, unsigned ngpio, void *context)
{
	struct da8xx_lcdc_platform_data *pdata = &sharp_lcd035q3dg01_pdata;
	struct i2c_msg tsc_detect = {
		.addr = DA830_EVM_TSC2004_ADDRESS,
	};
	int ret;

	ret = davinci_cfg_reg(DA830_GPIO1_11);
	if (ret) {
		pr_warning("%s: PinMux setup for GPIO %d failed: %d\n",
		   __func__, DA830_LCD_AC_ENB_CS, ret);
		return ret;
	}

	ret = gpio_request(DA830_LCD_AC_ENB_CS, "LCD CS\n");
	if (ret) {
		pr_warning("da830_evm_init: can not open GPIO %d\n",
		   DA830_LCD_AC_ENB_CS);
		return ret;
	}

	gpio_direction_output(DA830_LCD_AC_ENB_CS, 1);

	gpio_request(gpio + 6, "UI MUX_MODE");

	/* Drive mux mode low to match the default without UI card */
	gpio_direction_output(gpio + 6, 0);

	/* check if the new UI card with touchscreen LCD is connected */
	ret = i2c_transfer(client->adapter, &tsc_detect, 1);
	if (ret == 1) {
		ret = davinci_cfg_reg(DA830_GPIO2_12);
		if (ret) {
			pr_warning("%s: PinMux setup for GPIO %d failed: %d\n",
			   __func__, DA830_LCD_BACKLIGHT_PIN, ret);
			gpio_free(DA830_LCD_AC_ENB_CS);
			return ret;
		}

		ret = gpio_request(DA830_LCD_BACKLIGHT_PIN, "LCD BL\n");
		if (ret) {
			pr_warning("da830_evm_init: can not open GPIO %d\n",
			   DA830_LCD_BACKLIGHT_PIN);
			gpio_free(DA830_LCD_AC_ENB_CS);
			return ret;
		}

		gpio_direction_output(DA830_LCD_BACKLIGHT_PIN, 1);

		pdata = &sharp_lk043t1dg01_pdata;
	}

	da830_evm_init_lcdc(gpio + 6, pdata);

	da830_evm_init_nand(gpio + 6);

	return 0;
}

static int da830_evm_ui_expander_teardown(struct i2c_client *client, int gpio,
		unsigned ngpio, void *context)
{
	gpio_free(gpio + 6);
	return 0;
}

static struct pcf857x_platform_data __initdata da830_evm_ui_expander_info = {
	.gpio_base	= DAVINCI_N_GPIO,
	.setup		= da830_evm_ui_expander_setup,
	.teardown	= da830_evm_ui_expander_teardown,
};

/*
 * TSC 2004 Support
 */
#define TSC2004_GPIO_IRQ_PIN	GPIO_TO_PIN(2, 8)

static int tsc2004_init_irq(void)
{
	int ret = 0;

	ret = gpio_request(TSC2004_GPIO_IRQ_PIN, "tsc2004-irq");
	if (ret < 0) {
		pr_warning("%s: failed to TSC2004 IRQ GPIO: %d\n",
								__func__, ret);
		return ret;
	}

	ret = davinci_cfg_reg(DA830_GPIO2_8);
	if (ret) {
		pr_warning("%s: PinMux setup for GPIO %d failed: %d\n",
			   __func__, TSC2004_GPIO_IRQ_PIN, ret);
		return ret;
	}

	gpio_direction_input(TSC2004_GPIO_IRQ_PIN);

	return ret;
}

static void tsc2004_exit_irq(void)
{
	gpio_free(TSC2004_GPIO_IRQ_PIN);
}

static int tsc2004_get_irq_level(void)
{
	return gpio_get_value(TSC2004_GPIO_IRQ_PIN) ? 0 : 1;
}

struct tsc2004_platform_data da830evm_tsc2004data = {
	.model = 2004,
	.x_plate_ohms = 180,
	.get_pendown_state = tsc2004_get_irq_level,
	.init_platform_hw = tsc2004_init_irq,
	.exit_platform_hw = tsc2004_exit_irq,
};

static struct i2c_board_info __initdata da830_evm_i2c_devices[] = {
	{
		I2C_BOARD_INFO("24c256", 0x50),
		.platform_data	= &da830_evm_i2c_eeprom_info,
	},
	{
		I2C_BOARD_INFO("tlv320aic3x", 0x18),
	},
	{
		I2C_BOARD_INFO("pcf8574", 0x3f),
		.platform_data	= &da830_evm_ui_expander_info,
	},
};

static struct i2c_board_info __initdata da830_evm_tsc2004_dev = {
	I2C_BOARD_INFO("tsc2004", DA830_EVM_TSC2004_ADDRESS),
	.platform_data  = &da830evm_tsc2004data,
};

static struct davinci_i2c_platform_data da830_evm_i2c_0_pdata = {
	.bus_freq	= 100,	/* kHz */
	.bus_delay	= 0,	/* usec */
};

static __init void da830_evm_init(void)
{
	struct davinci_soc_info *soc_info = &davinci_soc_info;
	int ret;

	ret = da8xx_register_edma();
	if (ret)
		pr_warning("da830_evm_init: edma registration failed: %d\n",
				ret);

	ret = da8xx_pinmux_setup(da830_i2c0_pins);
	if (ret)
		pr_warning("da830_evm_init: i2c0 mux setup failed: %d\n",
				ret);

	ret = da8xx_register_i2c(0, &da830_evm_i2c_0_pdata);
	if (ret)
		pr_warning("da830_evm_init: i2c0 registration failed: %d\n",
				ret);

	da830_evm_usb_init();

	soc_info->emac_pdata->phy_mask = DA830_EVM_PHY_MASK;
	soc_info->emac_pdata->mdio_max_freq = DA830_EVM_MDIO_FREQUENCY;
	soc_info->emac_pdata->rmii_en = 1;

	ret = da8xx_pinmux_setup(da830_cpgmac_pins);
	if (ret)
		pr_warning("da830_evm_init: cpgmac mux setup failed: %d\n",
				ret);

	ret = da8xx_register_emac();
	if (ret)
		pr_warning("da830_evm_init: emac registration failed: %d\n",
				ret);

	ret = da8xx_register_watchdog();
	if (ret)
		pr_warning("da830_evm_init: watchdog registration failed: %d\n",
				ret);

	davinci_serial_init(&da830_evm_uart_config);
	i2c_register_board_info(1, da830_evm_i2c_devices,
			ARRAY_SIZE(da830_evm_i2c_devices));

	da830_evm_tsc2004_dev.irq = gpio_to_irq(TSC2004_GPIO_IRQ_PIN),
	i2c_register_board_info(1, &da830_evm_tsc2004_dev, 1);

	ret = da8xx_pinmux_setup(da830_evm_mcasp1_pins);
	if (ret)
		pr_warning("da830_evm_init: mcasp1 mux setup failed: %d\n",
				ret);

	da8xx_register_mcasp(1, &da830_evm_snd_data);

	da830_evm_init_mmc();

	ret = da8xx_register_rtc();
	if (ret)
		pr_warning("da830_evm_init: rtc setup failed: %d\n", ret);

	ret = da8xx_pinmux_setup(da830_spi0_pins);
	if (ret)
		pr_warning("da830_evm_init: spi0 mux setup failed: %d\n",
				ret);

	da830_init_spi0(BIT(0), da830_spi_board_info,
			ARRAY_SIZE(da830_spi_board_info));
}

#ifdef CONFIG_SERIAL_8250_CONSOLE
static int __init da830_evm_console_init(void)
{
	return add_preferred_console("ttyS", 2, "115200");
}
console_initcall(da830_evm_console_init);
#endif

static __init void da830_evm_irq_init(void)
{
	struct davinci_soc_info *soc_info = &davinci_soc_info;

	cp_intc_init((void __iomem *)DA8XX_CP_INTC_VIRT, DA830_N_CP_INTC_IRQ,
			soc_info->intc_irq_prios);
}

static void __init da830_evm_map_io(void)
{
	da830_init();
}

MACHINE_START(DAVINCI_DA830_EVM, "DaVinci DA830/OMAP-L137/AM17xx EVM")
	.phys_io	= IO_PHYS,
	.io_pg_offst	= (__IO_ADDRESS(IO_PHYS) >> 18) & 0xfffc,
	.boot_params	= (DA8XX_DDR_BASE + 0x100),
	.map_io		= da830_evm_map_io,
	.init_irq	= da830_evm_irq_init,
	.timer		= &davinci_timer,
	.init_machine	= da830_evm_init,
MACHINE_END
