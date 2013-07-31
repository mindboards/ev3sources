/*
 * Chip specific defines for DA8XX/OMAP L1XX SoC
 *
 * Author: Mark A. Greer <mgreer@mvista.com>
 *
 * 2007, 2009 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#ifndef __ASM_ARCH_DAVINCI_DA8XX_H
#define __ASM_ARCH_DAVINCI_DA8XX_H

#include <video/da8xx-fb.h>

#include <linux/davinci_emac.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>

#include <mach/serial.h>
#include <mach/edma.h>
#include <mach/i2c.h>
#include <mach/asp.h>
#include <mach/mmc.h>
#include <mach/usb.h>
#include <mach/pm.h>
#include <mach/mcbsp.h>
#include <mach/spi.h>
#include <mach/vpif.h>
#include <mach/cputype.h>

#include <linux/usb/musb.h>

extern void __iomem *da8xx_syscfg0_base;
extern void __iomem *da8xx_syscfg1_base;


extern void __iomem *da8xx_psc1_base; 		// LEGO BT slow clock
extern void __iomem *da8xx_ecap2_base; 		// LEGO BT slow clock

/*
 * The cp_intc interrupt controller for the da8xx isn't in the same
 * chunk of physical memory space as the other registers (like it is
 * on the davincis) so it needs to be mapped separately.  It will be
 * mapped early on when the I/O space is mapped and we'll put it just
 * before the I/O space in the processor's virtual memory space.
 */
#define DA8XX_CP_INTC_BASE	0xfffee000
#define DA8XX_CP_INTC_SIZE	SZ_8K
#define DA8XX_CP_INTC_VIRT	(IO_VIRT - DA8XX_CP_INTC_SIZE - SZ_4K)

#define DA8XX_SYSCFG0_BASE	(IO_PHYS + 0x14000)
#define DA8XX_SYSCFG0_VIRT(x)	(da8xx_syscfg0_base + (x))
#define DA8XX_JTAG_ID_REG	0x18
#define DA8XX_CHIPREV_ID_REG	0x24
#define DA8XX_MSTPRI2_REG	0x118
#define DA8XX_CFGCHIP0_REG	0x17c
#define DA8XX_CFGCHIP2_REG	0x184
#define DA8XX_CFGCHIP3_REG	0x188

#define DA8XX_SYSCFG1_BASE	(IO_PHYS + 0x22C000)
#define DA8XX_SYSCFG1_VIRT(x)	(da8xx_syscfg1_base + (x))
#define DA8XX_PSC1_VIRT(x)	(da8xx_psc1_base + (x))       // LEGO BT Slow clock
#define DA8XX_ECAP2_VIRT(x)	(da8xx_ecap2_base + (x))      // LEGO BT Slow clock


#define DA8XX_DEEPSLEEP_REG	0x8


#define DA8XX_PSC0_BASE		0x01c10000
#define DA8XX_PLL0_BASE		0x01c11000
#define DA8XX_TIMER64P0_BASE	0x01c20000
#define DA8XX_TIMER64P1_BASE	0x01c21000
#define DA8XX_GPIO_BASE		0x01e26000
#define DA8XX_PSC1_BASE		0x01e27000
#define DA8XX_LCD_CNTRL_BASE	0x01e13000
#define DA8XX_PLL1_BASE		0x01e1a000
#define DA8XX_MMCSD0_BASE	0x01c40000
#define DA8XX_AEMIF_CS2_BASE	0x60000000
#define DA8XX_AEMIF_CS3_BASE	0x62000000
#define DA8XX_AEMIF_CTL_BASE	0x68000000
#define DA8XX_DDR2_CTL_BASE	0xb0000000
#define DA8XX_ARM_RAM_BASE	0xffff0000
#define DA8XX_VPIF_BASE		0x01e17000
#define DA8XX_USB0_BASE		0x01e00000
#define DA850_SATA_BASE		0x01E18000
#define DA850_SATA_CLK_PWRDN	0x01E2C018
#define DA8XX_ECAP2_BASE        0x01F08000	// LEGO BT Slow clock


#define PINMUX0			0x00
#define PINMUX1			0x04
#define PINMUX2			0x08
#define PINMUX3			0x0c
#define PINMUX4			0x10
#define PINMUX5			0x14
#define PINMUX6			0x18
#define PINMUX7			0x1c
#define PINMUX8			0x20
#define PINMUX9			0x24
#define PINMUX10		0x28
#define PINMUX11		0x2c
#define PINMUX12		0x30
#define PINMUX13		0x34
#define PINMUX14		0x38
#define PINMUX15		0x3c
#define PINMUX16		0x40
#define PINMUX17		0x44
#define PINMUX18		0x48
#define PINMUX19		0x4c

/* Speed detection API */
#ifdef CONFIG_DA8XX_MAX_SPEED_456
#define cpu_is_davinci_da8xx_456mhz()	cpu_is_davinci_da8xx()
#else
#define cpu_is_davinci_da8xx_456mhz()	false
#endif

#ifdef CONFIG_DA8XX_MAX_SPEED_408
#define cpu_is_davinci_da8xx_408mhz()	cpu_is_davinci_da8xx()
#else
#define cpu_is_davinci_da8xx_408mhz()	false
#endif

#ifdef CONFIG_DA8XX_MAX_SPEED_372
#define cpu_is_davinci_da8xx_372mhz()	cpu_is_davinci_da8xx()
#else
#define cpu_is_davinci_da8xx_372mhz()	false
#endif

#ifdef CONFIG_DA8XX_MAX_SPEED_300
#define cpu_is_davinci_da8xx_300mhz()	cpu_is_davinci_da8xx()
#else
#define cpu_is_davinci_da8xx_300mhz()	false
#endif

static inline int cpu_is_davinci_da8xx_arm_only(void)
{
	if (!cpu_is_davinci_da8xx())
		return 0;

	/* BIT(5) is 0 for ARM only or DSP only devices. */
	return !(__raw_readl(DA8XX_SYSCFG0_VIRT(DA8XX_CHIPREV_ID_REG))
								& BIT(5));
}

void __init da830_init(void);
void __init da850_init(void);

int da850_register_edma(struct edma_rsv_info *rsv[2]);
int da8xx_register_i2c(int instance, struct davinci_i2c_platform_data *pdata);
int da8xx_register_watchdog(void);
int da8xx_register_usb20(unsigned mA, unsigned potpgt);
int da8xx_register_usb11(struct da8xx_ohci_root_hub *pdata);
int da8xx_register_emac(void);
int da8xx_register_pru_can(void);
int da8xx_register_pru_suart(void);
int da8xx_register_lcdc(struct da8xx_lcdc_platform_data *pdata);
int da8xx_register_mmcsd0(struct davinci_mmc_config *config);
void __init da8xx_register_mcasp(int id, struct snd_platform_data *pdata);
int da8xx_register_rtc(void);
int da850_register_cpufreq(void);
int da8xx_register_cpuidle(void);
void __iomem * __init da8xx_get_mem_ctlr(void);
int da850_register_pm(struct platform_device *pdev);
//void da850_init_spi0(unsigned chipselect_mask,
//	struct spi_board_info *info, unsigned len);
//void da850_init_spi1(unsigned chipselect_mask,
//	struct spi_board_info *info, unsigned len);
//void da830_init_spi0(unsigned chipselect_mask,
//	struct spi_board_info *info, unsigned len);
int da8xx_register_spi(int instance, struct davinci_spi_platform_data *pdata);
void da830_init_spi0(struct spi_board_info *info, unsigned len);
int da850_init_mcbsp(struct davinci_mcbsp_platform_data *pdata);
int __init da850_register_vpif(void);
int __init da850_register_vpif_display(struct vpif_display_config
							*display_config);
int __init da850_register_vpif_capture(struct vpif_capture_config
							*capture_config);

int cppi41_init(void);
int __init da8xx_register_sata(void);


extern struct platform_device da8xx_serial_device;
extern struct emac_platform_data da8xx_emac_pdata;
extern struct da8xx_lcdc_platform_data sharp_lcd035q3dg01_pdata;
extern struct da8xx_lcdc_platform_data sharp_lk043t1dg01_pdata;
void da8xx_usb20_configure(struct musb_hdrc_platform_data *pdata, u8 num_inst);

extern const short da830_emif25_pins[];
extern const short da830_spi0_pins[];
extern const short da830_spi1_pins[];
extern const short da830_mmc_sd_pins[];
extern const short da830_uart0_pins[];
extern const short da830_uart1_pins[];
extern const short da830_uart2_pins[];
extern const short da830_usb20_pins[];
extern const short da830_usb11_pins[];
extern const short da830_uhpi_pins[];
extern const short da830_cpgmac_pins[];
extern const short da830_emif3c_pins[];
extern const short da830_mcasp0_pins[];
extern const short da830_mcasp1_pins[];
extern const short da830_mcasp2_pins[];
extern const short da830_pru_suart_pins[];
extern const short da830_i2c0_pins[];
extern const short da830_i2c1_pins[];
extern const short da830_lcdcntl_pins[];
extern const short da830_pwm_pins[];
extern const short da830_ecap0_pins[];
extern const short da830_ecap1_pins[];
extern const short da830_ecap2_pins[];
extern const short da830_eqep0_pins[];
extern const short da830_eqep1_pins[];

extern const short da850_uart0_pins[];
extern const short da850_uart1_pins[];
extern const short da850_uart2_pins[];
extern const short da850_i2c0_pins[];
extern const short da850_i2c1_pins[];
extern const short da850_cpgmac_pins[];
extern const short da850_rmii_pins[];
extern const short da850_pru_can_pins[];
extern const short da850_pru_suart_pins[];
extern const short da850_mcasp_pins[];
extern const short da850_lcdcntl_pins[];
extern const short da850_mmcsd0_pins[];
extern const short da850_nand_pins[];
extern const short da850_nor_pins[];
extern const short da850_spi0_pins[];
extern const short da850_spi1_pins[];
extern const short da850_mcbsp0_pins[];
extern const short da850_mcbsp1_pins[];
extern const short da850_vpif_capture_pins[];
extern const short da850_vpif_display_pins[];
extern const short da850_evm_usb11_pins[];
extern const short da850_sata_pins[];

extern const short da850_bt_shut_down_pin[];   // LEGO BT
extern const short da850_bt_slow_clock_pin[];  // LEGO BT


#ifdef CONFIG_DAVINCI_MUX
int da8xx_pinmux_setup(const short pins[]);
#else
static inline int da8xx_pinmux_setup(const short pins[]) { return 0; }
#endif

#endif /* __ASM_ARCH_DAVINCI_DA8XX_H */
