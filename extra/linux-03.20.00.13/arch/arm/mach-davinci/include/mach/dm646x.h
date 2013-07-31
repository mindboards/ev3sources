/*
 * Chip specific defines for DM646x SoC
 *
 * Author: Kevin Hilman, Deep Root Systems, LLC
 *
 * 2007 (c) Deep Root Systems, LLC. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#ifndef __ASM_ARCH_DM646X_H
#define __ASM_ARCH_DM646X_H

#include <mach/hardware.h>
#include <mach/asp.h>
#include <mach/vpif.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/davinci_emac.h>
#include <linux/usb/musb.h>

#define DM646X_EMAC_BASE		(0x01C80000)
#define DM646X_EMAC_CNTRL_OFFSET	(0x0000)
#define DM646X_EMAC_CNTRL_MOD_OFFSET	(0x1000)
#define DM646X_EMAC_CNTRL_RAM_OFFSET	(0x2000)
#define DM646X_EMAC_MDIO_OFFSET		(0x4000)
#define DM646X_EMAC_CNTRL_RAM_SIZE	(0x2000)

#define DM646X_ATA_REG_BASE		(0x01C66000)

void __init dm646x_init(void);
void __init dm646x_init_ide(void);
void __init dm646x_init_mcasp0(struct snd_platform_data *pdata);
void __init dm646x_init_mcasp1(struct snd_platform_data *pdata);
void __init dm646x_board_setup_refclk(struct clk *clk);

void dm646x_video_init(void);
extern void dm646x_usb_configure(struct musb_hdrc_platform_data *pdata,
					u8 num_inst);

void dm646x_setup_vpif(struct vpif_display_config *,
		       struct vpif_capture_config *);

#endif /* __ASM_ARCH_DM646X_H */
