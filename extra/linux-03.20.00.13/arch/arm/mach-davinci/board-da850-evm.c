/*
 * TI DA850/OMAP-L138 EVM board
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Derived from: arch/arm/mach-davinci/board-da830-evm.c
 * Original Copyrights follow:
 *
 * 2007, 2009 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/i2c.h>
#include <linux/i2c/at24.h>
#include <linux/i2c/pca953x.h>
#include <linux/mfd/tps6507x.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/regulator/machine.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/usb/musb.h>
#include <linux/usb/g_hid.h>
#include <linux/i2c-gpio.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <mach/cp_intc.h>
#include <mach/da8xx.h>
#include <mach/nand.h>
#include <mach/mux.h>
#include <mach/flash.h>
#include <mach/vpif.h>

#include <media/tvp514x.h>
#include <video/st7586fb.h>

#define DA850_EVM_PHY_MASK		0x1
#define DA850_EVM_MDIO_FREQUENCY	2200000 /* PHY bus frequency */

#define DA850_LCD_PWR_PIN		GPIO_TO_PIN(2, 8)
#define DA850_LCD_BL_PIN		GPIO_TO_PIN(2, 15)

//#define DA850_MMCSD_CD_PIN		GPIO_TO_PIN(4, 0)
#define DA850_MMCSD_CD_PIN		GPIO_TO_PIN(4, 2) //Lego
//#define DA850_MMCSD_WP_PIN		GPIO_TO_PIN(4, 1) //Lego
#define DA850_PRU_CAN_TRX_PIN	GPIO_TO_PIN(2, 0)

#define DA850_MII_MDIO_CLKEN_PIN	GPIO_TO_PIN(2, 6)

#define DA850_BT_EN			GPIO_TO_PIN(0, 15)


#define TVP5147_CH0		"tvp514x-0"
#define TVP5147_CH1		"tvp514x-1"

#define VPIF_STATUS	(0x002C)
#define VPIF_STATUS_CLR	(0x0030)
#define DA850_USB1_VBUS_PIN		GPIO_TO_PIN(1, 4) //GPIO_TO_PIN(6, 14) //GPIO_TO_PIN(2, 4)
#define DA850_USB1_OC_PIN		GPIO_TO_PIN(6, 3) //GPIO_TO_PIN(6, 11) //GPIO_TO_PIN(6, 13)

#define  DA850_BT_SHUT_DOWN		GPIO_TO_PIN(4, 1)      //LEGO BT
#define  DA850_BT_SHUT_DOWN_EP2		GPIO_TO_PIN(4, 9)      //LEGO BT

static struct davinci_spi_platform_data da850_spi0_pdata = {
        .version        = SPI_VERSION_2,
        .num_chipselect = 1,
        .intr_line      = 1,
};

static struct davinci_spi_platform_data da850_spi1_pdata = {
        .version        = SPI_VERSION_2,
        .num_chipselect = 1,
        .intr_line      = 1,
};


static struct mtd_partition da850_evm_norflash_partition[] = {
	{
		.name           = "bootloaders + env",
		.offset         = 0,
		.size           = SZ_512K,
		.mask_flags     = MTD_WRITEABLE,
	},
	{
		.name           = "kernel",
		.offset         = MTDPART_OFS_APPEND,
		.size           = SZ_2M,
		.mask_flags     = 0,
	},
	{
		.name           = "filesystem",
		.offset         = MTDPART_OFS_APPEND,
		.size           = MTDPART_SIZ_FULL,
		.mask_flags     = 0,
	},
};

static struct davinci_aemif_timing da850_evm_norflash_timing = {
	.wsetup		= 10,
	.wstrobe	= 60,
	.whold		= 10,
	.rsetup		= 10,
	.rstrobe	= 110,
	.rhold		= 10,
	.ta		= 30,
};

static struct davinciflash_pdata da850_evm_norflash_data = {
	.width		= 2,
	.parts		= da850_evm_norflash_partition,
	.nr_parts	= ARRAY_SIZE(da850_evm_norflash_partition),
	.timing		= &da850_evm_norflash_timing,
};

static struct resource da850_evm_norflash_resource[] = {
	{
		.start	= DA8XX_AEMIF_CS2_BASE,
		.end	= DA8XX_AEMIF_CS2_BASE + SZ_32M - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= DA8XX_AEMIF_CTL_BASE,
		.end	= DA8XX_AEMIF_CTL_BASE + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device da850_evm_norflash_device = {
	.name		= "davinci-flash",
	.id		= 0,
	.dev		= {
		.platform_data  = &da850_evm_norflash_data,
	},
	.num_resources	= ARRAY_SIZE(da850_evm_norflash_resource),
	.resource	= da850_evm_norflash_resource,
};

static struct davinci_pm_config da850_pm_pdata = {
	.sleepcount = 128,
};

static struct platform_device da850_pm_device = {
	.name           = "pm-davinci",
	.dev = {
		.platform_data	= &da850_pm_pdata,
	},
	.id             = -1,
};

/* DA850/OMAP-L138 EVM includes a 512 MByte large-page NAND flash
 * (128K blocks). It may be used instead of the (default) SPI flash
 * to boot, using TI's tools to install the secondary boot loader
 * (UBL) and U-Boot.
 */
struct mtd_partition da850_evm_nandflash_partition[] = {
	{
		.name		= "u-boot env",
		.offset		= 0,
		.size		= SZ_128K,
		.mask_flags	= MTD_WRITEABLE,
	 },
	{
		.name		= "UBL",
		.offset		= MTDPART_OFS_APPEND,
		.size		= SZ_128K,
		.mask_flags	= MTD_WRITEABLE,
	},
	{
		.name		= "u-boot",
		.offset		= MTDPART_OFS_APPEND,
		.size		= 4 * SZ_128K,
		.mask_flags	= MTD_WRITEABLE,
	},
	{
		.name		= "kernel",
		.offset		= 0x200000,
		.size		= SZ_4M,
		.mask_flags	= 0,
	},
	{
		.name		= "filesystem",
		.offset		= MTDPART_OFS_APPEND,
		.size		= MTDPART_SIZ_FULL,
		.mask_flags	= 0,
	},
};

static struct davinci_aemif_timing da850_evm_nandflash_timing = {
	.wsetup		= 24,
	.wstrobe	= 21,
	.whold		= 14,
	.rsetup		= 19,
	.rstrobe	= 50,
	.rhold		= 0,
	.ta		= 20,
};

static struct davinci_nand_pdata da850_evm_nandflash_data = {
	.parts		= da850_evm_nandflash_partition,
	.nr_parts	= ARRAY_SIZE(da850_evm_nandflash_partition),
	.ecc_mode	= NAND_ECC_HW,
	.ecc_bits	= 4,
	.options	= NAND_USE_FLASH_BBT,
	.timing		= &da850_evm_nandflash_timing,
};

static struct resource da850_evm_nandflash_resource[] = {
	{
		.start	= DA8XX_AEMIF_CS3_BASE,
		.end	= DA8XX_AEMIF_CS3_BASE + SZ_512K + 2 * SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= DA8XX_AEMIF_CTL_BASE,
		.end	= DA8XX_AEMIF_CTL_BASE + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device da850_evm_nandflash_device = {
	.name		= "davinci_nand",
	.id		= 1,
	.dev		= {
		.platform_data	= &da850_evm_nandflash_data,
	},
	.num_resources	= ARRAY_SIZE(da850_evm_nandflash_resource),
	.resource	= da850_evm_nandflash_resource,
};

static struct platform_device *da850_evm_devices[] __initdata = {
	&da850_evm_nandflash_device,
	&da850_evm_norflash_device,
};
// LEGO CHANGED - 20120425 + 20120830
static struct mtd_partition spi0_flash_partitions[] = {
	[0] = {
		.name = "U-Boot",
		.offset = 0,
		.size = SZ_256K,
		.mask_flags = MTD_WRITEABLE,
	},
	[1] = {
		.name = "U-Boot Env",
		.offset = MTDPART_OFS_APPEND,
		.size = SZ_64K,
		.mask_flags = MTD_WRITEABLE,
	},
	[2] = {
		.name = "Kernel",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_2M,
		.mask_flags = 0,
	},
	[3] = {
		.name = "Filesystem",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_8M + SZ_2M + SZ_256K + SZ_128K,
		.mask_flags = 0,
	},
	[4] = {
		.name = "Storage",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_2M + SZ_1M + SZ_256K + SZ_64K,
		.mask_flags = 0,
	},

};
/*
static struct mtd_partition spi1_display_partitions[] = {
	[0] = {
		.name = "U-Boot",
		.offset = 0,
		.size = SZ_128K + SZ_64K,
		.mask_flags = MTD_WRITEABLE,
	},
	[1] = {
		.name = "U-Boot Environment",
		.offset = MTDPART_OFS_APPEND,
		.size = SZ_64K,
		.mask_flags = MTD_WRITEABLE,
	},
	[2] = {
		.name = "Linux uImage",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_2M + SZ_64K,
		.mask_flags = 0,
	},
	[3] = {
		.name = "File System (CRAMFS) ",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_8M + SZ_2M + SZ_256K + SZ_128K,
		.mask_flags = 0,
	},
	[4] = {
		.name = "User Data (RAMDisk)",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_2M + SZ_1M + SZ_256K + SZ_64K,
		.mask_flags = 0,
	},

};
*/
/*
// LEGO - Early MTD config
static struct mtd_partition spi0_flash_partitions[] = {
        [0] = {
                .name = "U-Boot",
                .offset = 0,
                .size = SZ_256K,
                .mask_flags = MTD_WRITEABLE,
        },
        [1] = {
                .name = "U-Boot Environment",
                .offset = MTDPART_OFS_APPEND,
                .size = SZ_64K,
                .mask_flags = MTD_WRITEABLE,
        },
        [2] = {
                .name = "Linux uImage",
                .offset = MTDPART_OFS_NXTBLK,
                .size = SZ_8M - (SZ_256K + SZ_64K + SZ_64K),
                .mask_flags = 0,
        },
};
*/
static struct flash_platform_data spi0_flash_data = {
	.name = "m25p80",
	.parts = spi0_flash_partitions,
	.nr_parts = ARRAY_SIZE(spi0_flash_partitions),
	//.type = "m25p64",
	.type = "s25sl12801",
};
/*
static struct flash_platform_data spi1_display_data = {
	.name = "st7586fb",
	.parts = spi1_display_partitions,
	.nr_parts = ARRAY_SIZE(spi1_display_partitions),
	//.type = "m25p64",
//	.type = "s25sl12801",
};
*/
static const struct st7586fb_platform_data lms2012_st7586fb_data = {
	.rst_gpio	= GPIO_TO_PIN(5, 0),
	.a0_gpio	= GPIO_TO_PIN(2, 11),
	.cs_gpio	= GPIO_TO_PIN(2, 12),
};


static struct davinci_spi_config da850_spiflash_cfg = {
	.io_type	= SPI_IO_TYPE_DMA,
	.c2tdelay	= 8,
	.t2cdelay	= 8,
};



static struct davinci_spi_config lms2012_st7586fb_cfg = {
        .io_type	= SPI_IO_TYPE_DMA,
        .c2tdelay	= 10,
        .t2cdelay	= 10,
 };

static struct spi_board_info da850_spi_board_info[] = {
	[0] = { // SPI0
		.modalias = "m25p80",
		.platform_data = &spi0_flash_data,
// LEGO - CHANGED 20120830
		.controller_data= &da850_spiflash_cfg,
		.mode = SPI_MODE_0,
//		.max_speed_hz = 30000000,       /* max sample rate at 3V */
		.max_speed_hz = 50000000,       /* max sample rate at 3V */
// LEGO - CHANGE END
		.bus_num = 0,
		.chip_select = 0,
	},
	[1] = { // SPI1
//		.modalias = "st7586fb",
//		.platform_data = &spi1_display_data,
//		.controller_data= &da850_spidisplay_cfg,
//		.mode = SPI_MODE_0,
//		.max_speed_hz = 30000000,       /* max sample rate at 3V */
//		.bus_num = 1,
//		.chip_select = 0,
		.modalias		= "lms2012_lcd",
		.platform_data		= &lms2012_st7586fb_data,
		.controller_data	= &lms2012_st7586fb_cfg,
		.mode			= SPI_MODE_3 | SPI_NO_CS,
		.max_speed_hz		= 10000000,
 		.bus_num		= 1,
 		.chip_select		= 0,

    },
};


static void __init da850_init_spi(struct spi_board_info *info, unsigned len)
{
        int ret;

	printk ("start  da850_init_spi %d\n");      
	ret = spi_register_board_info(info, len);
        if (ret)
                pr_warning("failed to register board info : %d\n", ret);

        ret = da8xx_register_spi(0, &da850_spi0_pdata);
        if (ret)
                pr_warning("failed to register spi 0 device : %d\n", ret);
        
	ret = da8xx_register_spi(1, &da850_spi1_pdata);
        if (ret)
                pr_warning("failed to register spi 1 device : %d\n", ret);

	printk ("end  da850_init_spi %d\n");    
}

static u32 ui_card_detected;

#if defined(CONFIG_MMC_DAVINCI) || \
    defined(CONFIG_MMC_DAVINCI_MODULE)
#define HAS_MMC 1
#else
#define HAS_MMC 0
#endif

static __init void da850_evm_setup_nor_nand(void)
{
	int ret = 0;

	if (ui_card_detected & !HAS_MMC) {
		ret = da8xx_pinmux_setup(da850_nand_pins);
		if (ret)
			pr_warning("da850_evm_init: nand mux setup failed: "
					"%d\n", ret);

		ret = da8xx_pinmux_setup(da850_nor_pins);
		if (ret)
			pr_warning("da850_evm_init: nor mux setup failed: %d\n",
				ret);

		platform_add_devices(da850_evm_devices,
					ARRAY_SIZE(da850_evm_devices));
	}
}

#ifdef CONFIG_DA850_UI_RMII
static inline void da850_evm_setup_emac_rmii(int rmii_sel)
{
	struct davinci_soc_info *soc_info = &davinci_soc_info;

	soc_info->emac_pdata->rmii_en = 1;
	gpio_set_value(rmii_sel, 0);
}
#else
static inline void da850_evm_setup_emac_rmii(int rmii_sel) { }
#endif

#ifdef CONFIG_DA850_UI_CLCD
static inline void da850_evm_setup_char_lcd(int a, int b, int c)
{
	gpio_set_value(a, 0);
	gpio_set_value(b, 0);
	gpio_set_value(c, 0);
}
#else
static inline void da850_evm_setup_char_lcd(int a, int b, int c) { }
#endif
static struct at24_platform_data da850_evm_i2c_eeprom_info = {
	.byte_len = SZ_256K/8,
	.page_size = 64,
	.flags	= AT24_FLAG_ADDR16,
	.setup  = davinci_get_mac_addr,
	.context = (void*)0x7f00,
};

#ifdef CONFIG_DA850_UI_VIDEO_PORT
static inline void da850_evm_setup_video_port(int video_sel)
{
	gpio_set_value(video_sel, 0);
}
#else
static inline void da850_evm_setup_video_port(int video_sel) { }
#endif

static int da850_evm_ui_expander_setup(struct i2c_client *client, unsigned gpio,
						unsigned ngpio, void *c)
{
	int sel_a, sel_b, sel_c, ret;

	sel_a = gpio + 7;
	sel_b = gpio + 6;
	sel_c = gpio + 5;

	ret = gpio_request(sel_a, "sel_a");
	if (ret) {
		pr_warning("Cannot open UI expander pin %d\n", sel_a);
		goto exp_setup_sela_fail;
	}

	ret = gpio_request(sel_b, "sel_b");
	if (ret) {
		pr_warning("Cannot open UI expander pin %d\n", sel_b);
		goto exp_setup_selb_fail;
	}

	ret = gpio_request(sel_c, "sel_c");
	if (ret) {
		pr_warning("Cannot open UI expander pin %d\n", sel_c);
		goto exp_setup_selc_fail;
	}

	/* deselect all functionalities */
	gpio_direction_output(sel_a, 1);
	gpio_direction_output(sel_b, 1);
	gpio_direction_output(sel_c, 1);

	ui_card_detected = 1;
	pr_info("DA850/OMAP-L138 EVM UI card detected\n");

	da850_evm_setup_nor_nand();

	da850_evm_setup_emac_rmii(sel_a);

	da850_evm_setup_char_lcd(sel_a, sel_b, sel_c);

	da850_evm_setup_video_port(sel_c);

	return 0;

exp_setup_selc_fail:
	gpio_free(sel_b);
exp_setup_selb_fail:
	gpio_free(sel_a);
exp_setup_sela_fail:
	return ret;
}

static int da850_evm_ui_expander_teardown(struct i2c_client *client,
					unsigned gpio, unsigned ngpio, void *c)
{
	/* deselect all functionalities */
	gpio_set_value(gpio + 5, 1);
	gpio_set_value(gpio + 6, 1);
	gpio_set_value(gpio + 7, 1);

	gpio_free(gpio + 5);
	gpio_free(gpio + 6);
	gpio_free(gpio + 7);

	return 0;
}

static struct pca953x_platform_data da850_evm_ui_expander_info = {
	.gpio_base	= DAVINCI_N_GPIO,
	.setup		= da850_evm_ui_expander_setup,
	.teardown	= da850_evm_ui_expander_teardown,
};

/* TPS65070 voltage regulator support */

/* 3.3V */
struct regulator_consumer_supply tps65070_dcdc1_consumers[] = {
	{
		.supply = "usb0_vdda33",
	},
	{
		.supply = "usb1_vdda33",
	},
};

/* 3.3V or 1.8V */
struct regulator_consumer_supply tps65070_dcdc2_consumers[] = {
	{
		.supply = "dvdd3318_a",
	},
	{
		.supply = "dvdd3318_b",
	},
	{
		.supply = "dvdd3318_c",
	},
};

/* 1.2V */
struct regulator_consumer_supply tps65070_dcdc3_consumers[] = {
	{
		.supply = "cvdd",
	},
};

/* 1.8V LDO */
struct regulator_consumer_supply tps65070_ldo1_consumers[] = {
	{
		.supply = "sata_vddr",
	},
	{
		.supply = "usb0_vdda18",
	},
	{
		.supply = "usb1_vdda18",
	},
	{
		.supply = "ddr_dvdd18",
	},
};

/* 1.2V LDO */
struct regulator_consumer_supply tps65070_ldo2_consumers[] = {
	{
		.supply = "sata_vdd",
	},
	{
		.supply = "pll0_vdda",
	},
	{
		.supply = "pll1_vdda",
	},
	{
		.supply = "usbs_cvdd",
	},
	{
		.supply = "vddarnwa1",
	},
};

struct regulator_init_data tps65070_regulator_data[] = {
	/* dcdc1 */
	{
		.constraints = {
			.min_uV = 3150000,
			.max_uV = 3450000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65070_dcdc1_consumers),
		.consumer_supplies = tps65070_dcdc1_consumers,
	},

	/* dcdc2 */
	{
		.constraints = {
			.min_uV = 1710000,
			.max_uV = 3450000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65070_dcdc2_consumers),
		.consumer_supplies = tps65070_dcdc2_consumers,
		.driver_data = (void *) 1,
	},

	/* dcdc3 */
	{
		.constraints = {
			.min_uV = 950000,
			.max_uV = 1380000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65070_dcdc3_consumers),
		.consumer_supplies = tps65070_dcdc3_consumers,
		.driver_data = (void *) 1,
	},

	/* ldo1 */
	{
		.constraints = {
			.min_uV = 1710000,
			.max_uV = 1890000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65070_ldo1_consumers),
		.consumer_supplies = tps65070_ldo1_consumers,
	},

	/* ldo2 */
	{
		.constraints = {
			.min_uV = 1140000,
			.max_uV = 1320000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65070_ldo2_consumers),
		.consumer_supplies = tps65070_ldo2_consumers,
	},
};

static struct tps6507x_board tps_board = {
	.tps6507x_pmic_init_data = &tps65070_regulator_data[0],
};

static struct i2c_board_info __initdata da850_evm_i2c_devices[] = {
	{
//		I2C_BOARD_INFO("tps6507x", 0x48),
//		.platform_data = &tps_board,
//	},
//	{
//		I2C_BOARD_INFO("tlv320aic3x", 0x18),
//	},
//	{
//		I2C_BOARD_INFO("tca6416", 0x20),
//		.platform_data = &da850_evm_ui_expander_info,
//	},
//	{
		I2C_BOARD_INFO("24FC128", 0x50),
	},
	{
		I2C_BOARD_INFO("PIC_CodedDataTo", 0x54),
	},
	{
		I2C_BOARD_INFO("PIC_ReadStatus", 0x55),
	},
	{
		I2C_BOARD_INFO("PIC_RawDataTo", 0x56),
	},
	{
		I2C_BOARD_INFO("PIC_ReadDataFrom", 0x57),
	},


};

static struct davinci_i2c_platform_data lego_i2c_0_pdata = {
	.bus_freq	= 400	/* kHz */,
	.bus_delay	= 0	/* usec */,
};

static struct davinci_uart_config da850_evm_uart_config __initdata = {
	.enabled_uarts = 0x7,
};

/* davinci da850 evm audio machine driver */
static u8 da850_iis_serializer_direction[] = {
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	TX_MODE,
	RX_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
};

static struct snd_platform_data da850_evm_snd_data = {
	.tx_dma_offset	= 0x2000,
	.rx_dma_offset	= 0x2000,
	.op_mode	= DAVINCI_MCASP_IIS_MODE,
	.num_serializer	= ARRAY_SIZE(da850_iis_serializer_direction),
	.tdm_slots	= 2,
	.serial_dir	= da850_iis_serializer_direction,
	.eventq_no	= EVENTQ_1,
	.version	= MCASP_VERSION_2,
	.txnumevt	= 1,
	.rxnumevt	= 1,
};

static struct davinci_mcbsp_platform_data da850_mcbsp0_config = {
	.inst	= 0,
};

static struct davinci_mcbsp_platform_data da850_mcbsp1_config = {
	.inst	= 1,
};

//static int da850_evm_mmc_get_ro(int index)
//{
//	return gpio_get_value(DA850_MMCSD_WP_PIN);
//}

static int da850_evm_mmc_get_cd(int index)
{
//	return !gpio_get_value(DA850_MMCSD_CD_PIN);
	return 1 ;
}

static struct davinci_mmc_config da850_mmc_config = {
//	.get_ro		= da850_evm_mmc_get_ro,
//	.get_cd		= da850_evm_mmc_get_cd,
	.wires		= 4,
	.max_freq	= 50000000,
	.caps		= MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED,
	.version	= MMC_CTLR_VERSION_2,
};

static void da850_panel_power_ctrl(int val)
{
	/* lcd power */
	gpio_set_value(DA850_LCD_PWR_PIN, val);

	mdelay(200);

	/* lcd backlight */
	gpio_set_value(DA850_LCD_BL_PIN, val);
}

static int da850_lcd_hw_init(void)
{
	void __iomem *cfg_mstpri2_base;
	int status;
	u32 val;

	/*
	 * Reconfigure the LCDC priority to the highest to ensure that
	 * the throughput/latency requirements for the LCDC are met.
	 */
	cfg_mstpri2_base = DA8XX_SYSCFG0_VIRT(DA8XX_MSTPRI2_REG);

	val = __raw_readl(cfg_mstpri2_base);
	val &= 0x0fffffff;
	__raw_writel(val, cfg_mstpri2_base);

	status = gpio_request(DA850_LCD_BL_PIN, "lcd bl\n");
	if (status < 0)
		return status;

	status = gpio_request(DA850_LCD_PWR_PIN, "lcd pwr\n");
	if (status < 0) {
		gpio_free(DA850_LCD_BL_PIN);
		return status;
	}

	gpio_direction_output(DA850_LCD_BL_PIN, 0);
	gpio_direction_output(DA850_LCD_PWR_PIN, 0);

	return 0;
}

static const short da850_evm_lcdc_pins[] = {
	DA850_GPIO2_8, DA850_GPIO2_15,
	-1
};

static int __init da850_evm_config_emac(void)
{
	void __iomem *cfg_chip3_base;
	int ret;
	u32 val;
	struct davinci_soc_info *soc_info = &davinci_soc_info;
	u8 rmii_en = soc_info->emac_pdata->rmii_en;

	if (!machine_is_davinci_da850_evm())
		return 0;

	cfg_chip3_base = DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP3_REG);

	val = __raw_readl(cfg_chip3_base);

//	if (rmii_en) {
//		val |= BIT(8);
//		ret = da8xx_pinmux_setup(da850_rmii_pins);
//		pr_info("EMAC: RMII PHY configured, MII PHY will not be"
//							" functional\n");
//	} else {
//		val &= ~BIT(8);
//		ret = da8xx_pinmux_setup(da850_cpgmac_pins);
//		pr_info("EMAC: MII PHY configured, RMII PHY will not be"
//							" functional\n");
//	}

//	if (ret)
//		pr_warning("da850_evm_init: cpgmac/rmii mux setup failed: %d\n",
//				ret);

	/* configure the CFGCHIP3 register for RMII or MII */
//	__raw_writel(val, cfg_chip3_base);

//	ret = davinci_cfg_reg(DA850_GPIO2_6);
//	if (ret)
//		pr_warning("da850_evm_init:GPIO(2,6) mux setup "
//							"failed\n");

//	ret = gpio_request(DA850_MII_MDIO_CLKEN_PIN, "mdio_clk_en");
//	if (ret) {
//		pr_warning("Cannot open GPIO %d\n",
//					DA850_MII_MDIO_CLKEN_PIN);
//		return ret;
//	}

	/* Enable/Disable MII MDIO clock */
//	gpio_direction_output(DA850_MII_MDIO_CLKEN_PIN, rmii_en);

//	soc_info->emac_pdata->phy_mask = DA850_EVM_PHY_MASK;
//	soc_info->emac_pdata->mdio_max_freq = DA850_EVM_MDIO_FREQUENCY;

//	ret = da8xx_register_emac();
//	if (ret)
//		pr_warning("da850_evm_init: emac registration failed: %d\n",
//				ret);

//	return 0;
}
device_initcall(da850_evm_config_emac);

/* Retaining these APIs, since the VPIF drivers do not check NULL handlers */
static int da850_set_vpif_clock(int mux_mode, int hd)
{
	return 0;
}

static int da850_setup_vpif_input_channel_mode(int mux_mode)
{
	return 0;
}

static int da850_vpif_intr_status(void __iomem *vpif_base, int channel)
{
	int status = 0;
	int mask;

	if (channel < 0 || channel > 3)
		return 0;

	mask = 1 << channel;
	status = __raw_readl((vpif_base + VPIF_STATUS)) & mask;
	__raw_writel(status, (vpif_base + VPIF_STATUS_CLR));

	return status;
}

/* VPIF capture configuration */
static struct tvp514x_platform_data tvp5146_pdata = {
	.clk_polarity = 0,
	.hs_polarity = 1,
	.vs_polarity = 1
};

#define TVP514X_STD_ALL (V4L2_STD_NTSC | V4L2_STD_PAL)

static struct vpif_subdev_info da850_vpif_capture_sdev_info[] = {
	{
		.name	= TVP5147_CH0,
		.board_info = {
			I2C_BOARD_INFO("tvp5146", 0x5d),
			.platform_data = &tvp5146_pdata,
		},
		.input = INPUT_CVBS_VI2B,
		.output = OUTPUT_10BIT_422_EMBEDDED_SYNC,
		.can_route = 1,
		.vpif_if = {
			.if_type = VPIF_IF_BT656,
			.hd_pol = 1,
			.vd_pol = 1,
			.fid_pol = 0,
		},
	},
	{
		.name	= TVP5147_CH1,
		.board_info = {
			I2C_BOARD_INFO("tvp5146", 0x5c),
			.platform_data = &tvp5146_pdata,
		},
		.input = INPUT_SVIDEO_VI2C_VI1C,
		.output = OUTPUT_10BIT_422_EMBEDDED_SYNC,
		.can_route = 1,
		.vpif_if = {
			.if_type = VPIF_IF_BT656,
			.hd_pol = 1,
			.vd_pol = 1,
			.fid_pol = 0,
		},
	},
};

static const struct vpif_input da850_ch0_inputs[] = {
	{
		.input = {
			.index = 0,
			.name = "Composite",
			.type = V4L2_INPUT_TYPE_CAMERA,
			.std = TVP514X_STD_ALL,
		},
		.subdev_name = TVP5147_CH0,
	},
};

static const struct vpif_input da850_ch1_inputs[] = {
       {
		.input = {
			.index = 0,
			.name = "S-Video",
			.type = V4L2_INPUT_TYPE_CAMERA,
			.std = TVP514X_STD_ALL,
		},
		.subdev_name = TVP5147_CH1,
	},
};

static struct vpif_capture_config da850_vpif_capture_config = {
	.setup_input_channel_mode = da850_setup_vpif_input_channel_mode,
	.intr_status = da850_vpif_intr_status,
	.subdev_info = da850_vpif_capture_sdev_info,
	.subdev_count = ARRAY_SIZE(da850_vpif_capture_sdev_info),
	.chan_config[0] = {
		.inputs = da850_ch0_inputs,
		.input_count = ARRAY_SIZE(da850_ch0_inputs),
	},
	.chan_config[1] = {
		.inputs = da850_ch1_inputs,
		.input_count = ARRAY_SIZE(da850_ch1_inputs),
	},
	.card_name      = "DA850/OMAP-L138 Video Capture",
};

/* VPIF display configuration */
static struct vpif_subdev_info da850_vpif_subdev[] = {
	{
		.name	= "adv7343",
		.board_info = {
			I2C_BOARD_INFO("adv7343", 0x2a),
		},
	},
};

static const char *vpif_output[] = {
	"Composite",
	"Component",
	"S-Video",
};

static struct vpif_display_config da850_vpif_display_config = {
	.set_clock	= da850_set_vpif_clock,
	.intr_status	= da850_vpif_intr_status,
	.subdevinfo	= da850_vpif_subdev,
	.subdev_count	= ARRAY_SIZE(da850_vpif_subdev),
	.output		= vpif_output,
	.output_count	= ARRAY_SIZE(vpif_output),
	.card_name	= "DA850/OMAP-L138 Video Display",
};

#if defined(CONFIG_DAVINCI_MCBSP0)
#define HAS_MCBSP0 1
#else
#define HAS_MCBSP0 0
#endif

#if defined(CONFIG_DAVINCI_MCBSP1)
#define HAS_MCBSP1 1
#else
#define HAS_MCBSP1 0
#endif

#if defined(CONFIG_TI_DAVINCI_EMAC) || \
	defined(CONFIG_TI_DAVINCI_EMAC_MODULE)
#define HAS_EMAC 1
#else
#define HAS_EMAC 0
#endif

#if defined(CONFIG_SND_DA850_SOC_EVM) || \
	defined(CONFIG_SND_DA850_SOC_EVM_MODULE)
#define HAS_MCASP 1
#else
#define HAS_MCASP 0
#endif

#if defined(CONFIG_DA850_UI_RMII) && (HAS_EMAC)
#define HAS_RMII 1
#else
#define HAS_RMII 0
#endif

#if defined(CONFIG_DA850_UI_LCD) && defined(CONFIG_FB_DA8XX) ||\
		defined(CONFIG_FB_DA8XX_MODULE)
#define HAS_GLCD 1
#else
#define HAS_GLCD 0
#endif

#if defined(CONFIG_VIDEO_DAVINCI_VPIF_DISPLAY) ||\
		defined(CONFIG_VIDEO_DAVINCI_VPIF_DISPLAY_MODULE)
#define HAS_VPIF_DISPLAY 1
#else
#define HAS_VPIF_DISPLAY 0
#endif

#if defined(CONFIG_VIDEO_DAVINCI_VPIF_CAPTURE) ||\
		defined(CONFIG_VIDEO_DAVINCI_VPIF_CAPTURE_MODULE)
#define HAS_VPIF_CAPTURE 1
#else
#define HAS_VPIF_CAPTURE 0
#endif

static da8xx_ocic_handler_t da850_evm_usb_ocic_handler;

static int da850_evm_usb_set_power(unsigned port, int on)
{
	pr_info("da850_evm_usb_set_power\n");	
	gpio_set_value(DA850_USB1_VBUS_PIN, on); //on on AM18xx EVM
	return 0;
}

static int da850_evm_usb_get_power(unsigned port)
{
	return gpio_get_value(DA850_USB1_VBUS_PIN);
}

static int da850_evm_usb_get_oci(unsigned port)
{
	return !gpio_get_value(DA850_USB1_OC_PIN);
}

static irqreturn_t da850_evm_usb_ocic_irq(int, void *);

static int da850_evm_usb_ocic_notify(da8xx_ocic_handler_t handler)
{
	int irq 	= gpio_to_irq(DA850_USB1_OC_PIN);
	int error	= 0;

	pr_info("da850_evm_usb_ocic_notify\n");

	if (handler != NULL) {
		da850_evm_usb_ocic_handler = handler;
		pr_info("da850_evm_usb_ocic_handler = handler\n");

		error = request_irq(irq, da850_evm_usb_ocic_irq, IRQF_DISABLED |
				    IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
				    "OHCI over-current indicator", NULL);
		if (error)
			printk(KERN_ERR "%s: could not request IRQ to watch "
			       "over-current indicator changes\n", __func__);
	} else
		free_irq(irq, NULL);

	return error;
}

static struct da8xx_ohci_root_hub da850_evm_usb11_pdata = {
	.set_power	= da850_evm_usb_set_power,
	.get_power	= da850_evm_usb_get_power,
	.get_oci	= da850_evm_usb_get_oci,
	.ocic_notify	= da850_evm_usb_ocic_notify,

	/* TPS2065 switch @ 5V */
	.potpgt		= (3 + 1) / 2,	/* 3 ms max */
};

static irqreturn_t da850_evm_usb_ocic_irq(int irq, void *dev_id)
{
	pr_info("da850_evm_usb_ocic_irq\n");	
	da850_evm_usb_ocic_handler(&da850_evm_usb11_pdata, 1);
	pr_info("da850_evm_usb_ocic_handler\n");
	return IRQ_HANDLED;
}

/* hid descriptor for a test */
static struct hidg_func_descriptor rudolf_hid1_data = {
	.subclass		= 0, /* Subclass NONE */
	.protocol		= 0, /* Protocol NONE */
	.report_length		= 0x40,
	.report_desc_length	= 25,
	.report_desc		= {

		0x06, 0x00, 0xFF, // Usage page (vendor defined) 
      		0x09, 0x01, // Usage ID (vendor defined) 
      		0xA1, 0x01, // Collection (application) 

    		// The Input report 
        	// 0x09, 0x03,       // Usage ID - vendor defined 
        	0x15, 0x00,       // Logical Minimum (0) 
        	0x26, 0xFF, 0x00,   // Logical Maximum (255) 
        	0x75, 0x08,       // Report Size (8 bits) 
        	0x95, 0x40,       // Report Count (64 fields)
 
		0x09, 0x01,	  // USAGE (vendor usage 1)
        	0x81, 0x02,       // Input (Data, Variable, Absolute) 

		0x09, 0x01,	  // USAGE (vendor usage 1)
        	0x91, 0x02,       // Output (Data, Variable, Absolute) 

      		0xc0		/* END_COLLECTION                         */

		}
};

static struct platform_device rudolf_hid1 = {
	.name			= "hidg",
	.id			= 0,
	.num_resources		= 0,
	.resource		= 0,
	.dev.platform_data	= &rudolf_hid1_data,
};


/* hid descriptor for a keyboard */
static struct hidg_func_descriptor rudolf_hid2_data = {
	.subclass		= 0, /* Subclass NONE */
	.protocol		= 0, /* Protocol NONE */
	.report_length		= 0x40,
	.report_desc_length	= 25,
	.report_desc		= {

		0x06, 0x00, 0xFF, // Usage page (vendor defined) 
      		0x09, 0x01, // Usage ID (vendor defined) 
      		0xA1, 0x01, // Collection (application) 

    		// The Input report 
        	// 0x09, 0x03,       // Usage ID - vendor defined 
        	0x15, 0x00,       // Logical Minimum (0) 
        	0x26, 0xFF, 0x00,   // Logical Maximum (255) 
        	0x75, 0x08,       // Report Size (8 bits) 
        	0x95, 0x40,       // Report Count (64 fields)
 
		0x09, 0x01,	  // USAGE (vendor usage 1)
        	0x81, 0x02,       // Input (Data, Variable, Absolute) 

		0x09, 0x01,	  // USAGE (vendor usage 1)
        	0x91, 0x02,       // Output (Data, Variable, Absolute) 

      		0xc0		/* END_COLLECTION                         */
		}
};

static struct platform_device rudolf_hid2 = {
	.name			= "hidg",
	.id			= 1,
	.num_resources		= 0,
	.resource		= 0,
	.dev.platform_data	= &rudolf_hid2_data,
};



/* Bluetooth Slow clock init using ecap 2 */
static __init void da850_evm_bt_slow_clock_init(void)						// LEGO BT
{												// LEGO BT		
  int PSC1;											// LEGO BT
												// LEGO BT
  PSC1 = __raw_readl(DA8XX_PSC1_VIRT(0x294 * 4));  // Old PSC1 is 32bit -> explains "* 4"	// LEGO BT
  PSC1 |= 3;											// LEGO BT
  __raw_writel(PSC1, DA8XX_PSC1_VIRT(0x294 * 4));						// LEGO BT
												// LEGO BT
  PSC1 = __raw_readl(DA8XX_PSC1_VIRT(0x48 * 4));						// LEGO BT
  PSC1 |= 3;											// LEGO BT
  __raw_writel(PSC1, DA8XX_PSC1_VIRT(0x48 * 4));						// LEGO BT

  PSC1 = __raw_readl(DA8XX_SYSCFG1_VIRT(0x3 * 4));						// LEGO BT
  PSC1 &= ~0x00000004;										// LEGO BT
  __raw_writel(PSC1, DA8XX_SYSCFG1_VIRT(0x3 * 4));						// LEGO BT

  __raw_writel(0,      DA8XX_ECAP2_VIRT(0 * 2));     // Old ECAP is 16bit -> explains "* 2"     // LEGO BT
  __raw_writel(0,      DA8XX_ECAP2_VIRT(2 * 2));     //						// LEGO BT
  __raw_writew(0x0690, DA8XX_ECAP2_VIRT(0x15 * 2));  // Setup					// LEGO BT
  __raw_writel(2014,   DA8XX_ECAP2_VIRT(0x06 * 2));  // Duty					// LEGO BT
  __raw_writel(4028,   DA8XX_ECAP2_VIRT(0x04 * 2));  // Freq					// LEGO BT

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

static __init void da850_evm_usb_init(void)
{
	int ret;
	u32 cfgchip2;

	/*
	 * Setup the Ref. clock frequency for the EVM at 24 MHz.
	 */
	cfgchip2 = __raw_readl(DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP2_REG));
	cfgchip2 &= ~CFGCHIP2_REFFREQ;
	cfgchip2 |=  CFGCHIP2_REFFREQ_24MHZ;
	__raw_writel(cfgchip2, DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP2_REG));

	da8xx_usb20_configure(usb_evm_data, ARRAY_SIZE(usb_evm_data));

	platform_device_register(&rudolf_hid1);
	platform_device_register(&rudolf_hid2);

	ret = da8xx_pinmux_setup(da850_evm_usb11_pins);
	if (ret) {
		pr_warning("%s: USB 1.1 PinMux setup failed: %d\n",
			   __func__, ret);
		return;
	}

	ret = gpio_request(DA850_USB1_VBUS_PIN, "USB1 VBUS\n");
	if (ret) {
		printk(KERN_ERR "%s: failed to request GPIO for USB 1.1 port "
		       "power control: %d\n", __func__, ret);
		return;
	}
	gpio_direction_output(DA850_USB1_VBUS_PIN, 0);

	ret = gpio_request(DA850_USB1_OC_PIN, "USB1 OC");
	if (ret) {
		printk(KERN_ERR "%s: failed to request GPIO for USB 1.1 port "
		       "over-current indicator: %d\n", __func__, ret);
		return;
	}
	gpio_direction_input(DA850_USB1_OC_PIN);

	ret = da8xx_register_usb11(&da850_evm_usb11_pdata);
	if (ret)
		pr_warning("%s: USB 1.1 registration failed: %d\n",
			   __func__, ret);
}

static struct i2c_gpio_platform_data da850_gpio_i2c_pdata = {
	.sda_pin	= GPIO_TO_PIN(1, 4),
	.scl_pin	= GPIO_TO_PIN(1, 5),
	.udelay		= 2,			/* 250 KHz */
};

static struct platform_device da850_gpio_i2c = {
	.name		= "i2c-gpio",
	.id		= 1,
	.dev		= {
		.platform_data	= &da850_gpio_i2c_pdata,
	},
};

static int __init da850_evm_config_pru_can(void)
{
    int ret;

    if (!machine_is_davinci_da850_evm())
        return 0;

//	ret = da8xx_pinmux_setup(da850_pru_can_pins);
//	if (ret)
//        pr_warning("da850_evm_init: da850_pru_can_pins mux setup failed: %d\n",
//                ret);
//
//	ret = gpio_request(DA850_PRU_CAN_TRX_PIN, "pru_can_en");
//   if (ret) 
//         pr_warning("Cannot open GPIO %d\n", DA850_PRU_CAN_TRX_PIN);

	/* value = 0 to enable the can transceiver */
//    gpio_direction_output(DA850_PRU_CAN_TRX_PIN, 0);
//    ret = da8xx_register_pru_can();
//    if (ret)
//        pr_warning("da850_evm_init: pru can registration failed: %d\n", ret);
//    return ret;
}
device_initcall(da850_evm_config_pru_can);

static int __init da850_evm_config_pru_suart(void)
{
    int ret;

     pr_info("da850_evm_config_pru_suart configuration\n");
    if (!machine_is_davinci_da850_evm())
        return 0;

    ret = da8xx_pinmux_setup(da850_pru_suart_pins);
    if (ret)
        pr_warning("da850_evm_init: da850_pru_suart_pins mux setup failed: %d\n",
                ret);

    ret = da8xx_register_pru_suart();
    if (ret)
        pr_warning("da850_evm_init: pru suart registration failed: %d\n", ret);
    return ret;
}
device_initcall(da850_evm_config_pru_suart);

/*
 * The following EDMA channels/slots are not being used by drivers (for
 * example: Timer, GPIO, UART events etc) on da850/omap-l138 EVM, hence
 * they are being reserved for codecs on the DSP side.
 */
static const s16 da850_dma0_rsv_chans[][2] = {
        /* (offset, number) */
        { 8,  6},
        {24,  4},
        {30,  2},
        {-1, -1}
};

static const s16 da850_dma0_rsv_slots[][2] = {
        /* (offset, number) */
        { 8,  6},
        {24,  4},
        {30, 50},
        {-1, -1}
};

static const s16 da850_dma1_rsv_chans[][2] = {
        /* (offset, number) */
        { 0, 28},
        {30,  2},
        {-1, -1}
};

static const s16 da850_dma1_rsv_slots[][2] = {
        /* (offset, number) */
        { 0, 28},
        {30, 90},
        {-1, -1}
};

static struct edma_rsv_info da850_edma_cc0_rsv = {
        .rsv_chans      = da850_dma0_rsv_chans,
        .rsv_slots      = da850_dma0_rsv_slots,
};

static struct edma_rsv_info da850_edma_cc1_rsv = {
        .rsv_chans      = da850_dma1_rsv_chans,
        .rsv_slots      = da850_dma1_rsv_slots,
};

static struct edma_rsv_info *da850_edma_rsv[2] = {
        &da850_edma_cc0_rsv,
        &da850_edma_cc1_rsv,
};

static const short da850_lms2012_lcd_pins[] = {
	DA850_GPIO2_11, DA850_GPIO2_12, DA850_GPIO5_0,
	-1
};

static void lego_ev3_poweroff(void)
{
#define LEGO_EV3_POWER_PIN GPIO_TO_PIN(6, 11) /* 5VPENON on HW-IDs <= EP2 */
	if (!gpio_request(LEGO_EV3_POWER_PIN, "5VPENON\n"))
		gpio_direction_output(LEGO_EV3_POWER_PIN, 0);
	else
		pr_err("da850_evm_init: can not open GPIO %d for power off\n",
			LEGO_EV3_POWER_PIN);
}

static __init void da850_evm_init(void)
{
	int ret;

	pm_power_off = lego_ev3_poweroff;

	//ret = da8xx_register_edma();
	ret = da850_register_edma(da850_edma_rsv);
	if (ret)
		pr_warning("da850_evm_init: edma registration failed: %d\n",
				ret);

	ret = da8xx_pinmux_setup(da850_spi0_pins);
	if (ret)
		pr_warning("da850_evm_init: spi0 mux setup failed: %d\n",
				ret);

	ret = da8xx_pinmux_setup(da850_spi1_pins);
	if (ret)
		pr_warning("da850_evm_init: spi0 mux setup failed: %d\n",
				ret);

	ret = da8xx_pinmux_setup(da850_lms2012_lcd_pins);
	if (ret)
		pr_warning("da850_evm_init: lms2012 lcd mux setup failed: %d\n",
				ret);



	//da850_init_spi0(BIT(0), da850_spi0_board_info,
	da850_init_spi(da850_spi_board_info,
			ARRAY_SIZE(da850_spi_board_info));

	ret = da8xx_pinmux_setup(da850_i2c0_pins);
	if (ret)
		pr_warning("da850_evm_init: i2c0 mux setup failed: %d\n",
				ret);
        
  i2c_register_board_info(1, da850_evm_i2c_devices,
			ARRAY_SIZE(da850_evm_i2c_devices));
	da8xx_register_i2c(0,&lego_i2c_0_pdata);
//	platform_device_register(&da850_gpio_i2c);
	
	ret = da8xx_register_watchdog();
	if (ret)
		pr_warning("da830_evm_init: watchdog registration failed: %d\n",
				ret);

	if (HAS_MMC) {
		ret = da8xx_pinmux_setup(da850_mmcsd0_pins);
		if (ret)
			pr_warning("da850_evm_init: mmcsd0 mux setup failed:"
					" %d\n", ret);

		ret = gpio_request(DA850_MMCSD_CD_PIN, "MMC CD\n");
		if (ret)
			pr_warning("da850_evm_init: can not open GPIO %d\n",
					DA850_MMCSD_CD_PIN);
		gpio_direction_input(DA850_MMCSD_CD_PIN);

//		ret = gpio_request(DA850_MMCSD_WP_PIN, "MMC WP\n");
//		if (ret)
//			pr_warning("da850_evm_init: can not open GPIO %d\n",
//					DA850_MMCSD_WP_PIN);
//		gpio_direction_input(DA850_MMCSD_WP_PIN);

		ret = da8xx_register_mmcsd0(&da850_mmc_config);
		if (ret)
			pr_warning("da850_evm_init: mmcsd0 registration failed:"
					" %d\n", ret);
	}

//ret = da8xx_pinmux_setup(da850_uart0_pins);
//      if (ret)
//            pr_warning("da850_evm_init: uart0 mux setup failed: %d\n", ret);

//      ret = da8xx_pinmux_setup(da850_uart1_pins);
//      if (ret)
//            pr_warning("da850_evm_init: uart1 mux setup failed: %d\n", ret);

	/* Support for UART 1 */
	ret = da8xx_pinmux_setup(da850_uart1_pins);
	if (ret)
		pr_warning("da850_evm_init: UART 1 mux setup failed:"
						" %d\n", ret);

	/* Support for UART 2 */						// LEGO BT
	ret = da8xx_pinmux_setup(da850_uart2_pins);				// LEGO BT
	if (ret)								// LEGO BT
		pr_warning("da850_evm_init: UART 2 mux setup failed:"		// LEGO BT
						" %d\n", ret);			// LEGO BT


	pr_info("da850_evm_gpio_req_BT_EN\n");	
	
	ret = gpio_request(DA850_BT_EN, "WL1271_BT_EN");
	if (ret)
		pr_warning("da850_evm_init: can not open BT GPIO %d\n",
					DA850_BT_EN);
	gpio_direction_output(DA850_BT_EN, 1);
	udelay(1000);
	gpio_direction_output(DA850_BT_EN, 0);


	davinci_serial_init(&da850_evm_uart_config);

	//i2c_register_board_info(1, da850_evm_i2c_devices,
	//		ARRAY_SIZE(da850_evm_i2c_devices));

	/*
	 * shut down uart 0 and 1; they are not used on the board and
	 * accessing them causes endless "too much work in irq53" messages
	 * with arago fs
	 */
	//__raw_writel(0, IO_ADDRESS(DA8XX_UART1_BASE) + 0x30);
	//__raw_writel(0, IO_ADDRESS(DA8XX_UART0_BASE) + 0x30);

/*	if (HAS_MCBSP0) {
		if (HAS_EMAC)
			pr_warning("WARNING: both MCBSP0 and EMAC are "
				"enabled, but they share pins.\n"
				"\tDisable one of them.\n");

		ret = da8xx_pinmux_setup(da850_mcbsp0_pins);
		if (ret)
			pr_warning("da850_evm_init: mcbsp0 mux setup failed:"
					" %d\n", ret);

		ret = da850_init_mcbsp(&da850_mcbsp0_config);
		if (ret)
			pr_warning("da850_evm_init: mcbsp0 registration"
					"failed: %d\n",	ret);
	}

	if (HAS_MCBSP1) {
		ret = da8xx_pinmux_setup(da850_mcbsp1_pins);
		if (ret)
			pr_warning("da850_evm_init: mcbsp1 mux setup failed:"
					" %d\n", ret);

		ret = da850_init_mcbsp(&da850_mcbsp1_config);
		if (ret)
			pr_warning("da850_evm_init: mcbsp1 registration"
					" failed: %d\n", ret);
	}

	if (HAS_MCASP) {
		if ((HAS_MCBSP0 || HAS_MCBSP1))
			pr_warning("WARNING: both McASP and McBSP are enabled, "
					"but they share pins.\n"
					"\tDisable one of them.\n");*/

//		ret = da8xx_pinmux_setup(da850_mcasp_pins);
//		if (ret)
//			pr_warning("da850_evm_init: mcasp mux setup failed:"
//					"%d\n", ret);

//		da8xx_register_mcasp(0, &da850_evm_snd_data);
	//}

	ret = da8xx_pinmux_setup(da850_lcdcntl_pins);
	if (ret)
		pr_warning("da850_evm_init: lcdcntl mux setup failed: %d\n",
				ret);

	/* Handle board specific muxing for LCD here */
	ret = da8xx_pinmux_setup(da850_evm_lcdc_pins);
	if (ret)
		pr_warning("da850_evm_init: evm specific lcd mux setup "
				"failed: %d\n",	ret);

	ret = da850_lcd_hw_init();
	if (ret)
		pr_warning("da850_evm_init: lcd initialization failed: %d\n",
				ret);

	sharp_lk043t1dg01_pdata.panel_power_ctrl = da850_panel_power_ctrl,
	ret = da8xx_register_lcdc(&sharp_lk043t1dg01_pdata);
	if (ret)
		pr_warning("da850_evm_init: lcdc registration failed: %d\n",
				ret);

	ret = da8xx_register_rtc();
	if (ret)
		pr_warning("da850_evm_init: rtc setup failed: %d\n", ret);

	ret = da850_register_cpufreq();
	if (ret)
		pr_warning("da850_evm_init: cpufreq registration failed: %d\n",
				ret);

	ret = da8xx_register_cpuidle();
	if (ret)
		pr_warning("da850_evm_init: cpuidle registration failed: %d\n",
				ret);

	ret = da850_register_pm(&da850_pm_device);
	if (ret)
		pr_warning("da850_evm_init: suspend registration failed: %d\n",
				ret);

//	ret = da8xx_pinmux_setup(da850_spi1_pins);
//	if (ret)
//		pr_warning("da850_evm_init: spi1 mux setup failed: %d\n",
//				ret);
//
//	da850_init_spi1(BIT(0), da850_spi_board_info,
//			ARRAY_SIZE(da850_spi_board_info));

	da850_evm_usb_init();

	ret = da8xx_register_sata();
	if (ret)
		pr_warning("da850_evm_init: SATA registration failed: %d\n",
						ret);

	if (HAS_VPIF_DISPLAY || HAS_VPIF_CAPTURE) {
		ret = da850_register_vpif();
		if (ret)
			pr_warning("da850_evm_init: VPIF registration failed: "
					"%d\n",	ret);
	}

	if (!HAS_RMII && HAS_VPIF_CAPTURE) {
		ret = da8xx_pinmux_setup(da850_vpif_capture_pins);
		if (ret)
			pr_warning("da850_evm_init: vpif capture mux failed: "
					"%d\n",	ret);

		ret = da850_register_vpif_capture(&da850_vpif_capture_config);
		if (ret)
			pr_warning("da850_evm_init: VPIF registration failed: "
					"%d\n",	ret);

	}

	if (!HAS_GLCD && HAS_VPIF_DISPLAY) {
		ret = da8xx_pinmux_setup(da850_vpif_display_pins);
		if (ret)
			pr_warning("da850_evm_init: vpif capture mux failed: "
					"%d\n",	ret);

		ret = da850_register_vpif_display(&da850_vpif_display_config);
		if (ret)
			pr_warning("da850_evm_init: VPIF registration failed: "
					"%d\n",	ret);

	}


	if (gpio_request(DA850_BT_SHUT_DOWN, "bt_en")) {			// LEGO BT
		printk(KERN_ERR "Failed to request gpio DA850_BT_SHUT_DOWN\n");	// LEGO BT
		return;								// LEGO BT
	}									// LEGO BT

	if (gpio_request(DA850_BT_SHUT_DOWN_EP2, "bt_en_EP2")) {		// LEGO BT - EP2
		printk(KERN_ERR "Failed to request gpio DA850_BT_SHUT_DOWN\n");	// LEGO BT - EP2
		return;								// LEGO BT - EP2
	}									// LEGO BT - EP2

	gpio_set_value(DA850_BT_SHUT_DOWN_EP2, 0);				// LEGO BT - EP2
	gpio_direction_output(DA850_BT_SHUT_DOWN_EP2, 0);			// LEGO BT - EP2

	gpio_set_value(DA850_BT_SHUT_DOWN, 0);					// LEGO BT
	gpio_direction_output(DA850_BT_SHUT_DOWN, 0);				// LEGO BT

	/* Support for Bluetooth shut dw pin */					// LEGO BT
	pr_info("Support for Bluetooth shut dw pin\n");	
	ret = da8xx_pinmux_setup(da850_bt_shut_down_pin);			// LEGO BT
	if (ret)								// LEGO BT
		pr_warning("da850_evm_init: BT shut down mux setup failed:"	// LEGO BT
						" %d\n", ret);			// LEGO BT


	gpio_set_value(DA850_BT_SHUT_DOWN, 0);					// LEGO BT
	gpio_set_value(DA850_BT_SHUT_DOWN_EP2, 0);				// LEGO BT - EP2

        /* Support for Bluetooth slow clock */					// LEGO BT
	ret = da8xx_pinmux_setup(da850_bt_slow_clock_pin);			// LEGO BT
	if (ret)								// LEGO BT
		pr_warning("da850_evm_init: BT slow clock mux setup failed:"	// LEGO BT
						" %d\n", ret);			// LEGO BT

        da850_evm_bt_slow_clock_init();						// LEGO BT
        gpio_direction_input(DA850_ECAP2_OUT_ENABLE);				// LEGO BT

	gpio_set_value(DA850_BT_SHUT_DOWN, 1);					// LEGO BT
	gpio_set_value(DA850_BT_SHUT_DOWN_EP2, 1);				// LEGO BT - EP2


}

#ifdef CONFIG_SERIAL_8250_CONSOLE
static int __init da850_evm_console_init(void)
{
	return add_preferred_console("ttyS", 1, "115200"); //Nico
}
console_initcall(da850_evm_console_init);
#endif

static __init void da850_evm_irq_init(void)
{
	struct davinci_soc_info *soc_info = &davinci_soc_info;

	cp_intc_init((void __iomem *)DA8XX_CP_INTC_VIRT, DA850_N_CP_INTC_IRQ,
			soc_info->intc_irq_prios);
}

static void __init da850_evm_map_io(void)
{
	da850_init();
}
/*
MACHINE_START(DAVINCI_DA850_EVM, "DaVinci DA850/OMAP-L138/AM18xx EVM")
	.phys_io	= IO_PHYS,
	.io_pg_offst	= (__IO_ADDRESS(IO_PHYS) >> 18) & 0xfffc,
	.boot_params	= (DA8XX_DDR_BASE + 0x100),
	.map_io		= da850_evm_map_io,
	.init_irq	= da850_evm_irq_init,
	.timer		= &davinci_timer,
	.init_machine	= da850_evm_init,
MACHINE_END
*/
// LEGO CHANGED - 20120501
MACHINE_START(DAVINCI_DA850_EVM, "MindStorms EV3")
        .phys_io        = IO_PHYS,
        .io_pg_offst    = (__IO_ADDRESS(IO_PHYS) >> 18) & 0xfffc,
        .boot_params    = (DA8XX_DDR_BASE + 0x100),
        .map_io         = da850_evm_map_io,
        .init_irq       = da850_evm_irq_init,
        .timer          = &davinci_timer,
        .init_machine   = da850_evm_init,
MACHINE_END
