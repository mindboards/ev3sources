/*
 * TI AHCI SATA controler 
 *
 * TI SATA controller on OMAPL138 platform.  AHCI compliant controller. 
 *
 * Copyright (C) 2009 Texas Instruments.
 *
 * ---------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ---------------------------------------------------------------------------
 * History:
 * 0-1 Swaminathan Subbarathnam : Initial creation of AHCI platform specific
 *     interface for non-PCI based SATA AHCI implementation. 
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/device.h>
#include <linux/dmi.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_cmnd.h>
#include <linux/libata.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <mach/cputype.h>
#include <mach/da8xx.h>

void ata_plat_init (void __iomem *base);
void ata_plat_remove(struct ata_host *host);

#define P0PHYCR		0x178  /* SATA PHY Control Register offset 
				* from AHCI base)
				*/

#define PHY_MPY		8 /* bits3:0     4 Clock Sources at 100MHz */
#define PHY_LB		0 /* bits5:4     2 */
#define PHY_LOS		1 /* bit6        1 */
#define PHY_RXINVPAIR	0 /* bit7        1 */
#define PHY_RXTERM	0 /* bits9:8     2 */
#define PHY_RXCDR	0 /* bits12:10   3 */
#define PHY_RXEQ	1 /* bits16:13   4 */
#define PHY_TXINVPAIR	0 /* bit17       1 */
#define PHY_TXCM	0 /* bit18       1 */
#define PHY_TXSWING	3 /* bits21:19   3 */
#define PHY_TXDE	0 /* bits25:22   4 */
#define PHY_OVERRIDE	0 /* bit30       1 */
#define PHY_ENPLL	1 /* bit31       1 */



/* ata_plat_init - Do platform specific init of the AHCI SATA controller.
 * @base - Base address of the AHCI SATA controller
 */
void ata_plat_init (void __iomem *base)
{
	u32	phy_val = 0;

	/* Power up the PHY clock source */
	if (cpu_is_davinci_da850())
		__raw_writel(0, IO_ADDRESS(DA850_SATA_CLK_PWRDN));

	phy_val = PHY_MPY << 0 | PHY_LB << 4 | PHY_LOS << 6 |
			PHY_RXINVPAIR << 7 | PHY_RXTERM << 8 |
			PHY_RXCDR  << 10 | PHY_RXEQ << 13 |
			PHY_RXINVPAIR << 17 | PHY_TXCM << 18 |
			PHY_TXSWING << 19 | PHY_TXDE << 22 | 
			PHY_OVERRIDE << 30 | PHY_ENPLL	<< 31;

	/* Initialize the SATA PHY */
	writel (phy_val, base + P0PHYCR);
}

/* ata_plat_remove - Do platform specific de-init of the AHCI SATA controller.
 * @host - ATA Host pointer of the controller.
 */
void ata_plat_remove(struct ata_host *host)
{
	/* Power down the PHY clock source */
	if (cpu_is_davinci_da850())
		__raw_writel(1, IO_ADDRESS(DA850_SATA_CLK_PWRDN));
}

void ahci_platform_suspend(struct platform_device *pdev)
{
	struct ata_host *host = dev_get_drvdata(&pdev->dev);
	u32	phy_val = 0;

	phy_val = readl(host->iomap + P0PHYCR);

	phy_val &= ~(PHY_ENPLL << 31);
	writel(phy_val, host->iomap + P0PHYCR);
}

void ahci_platform_resume(struct platform_device *pdev)
{
	struct ata_host *host = dev_get_drvdata(&pdev->dev);
	u32	phy_val = 0;

	phy_val = readl(host->iomap + P0PHYCR);

	phy_val |= PHY_ENPLL << 31;
	writel(phy_val, host->iomap + P0PHYCR);
}
EXPORT_SYMBOL_GPL(ahci_platform_resume);
EXPORT_SYMBOL_GPL(ata_plat_init);
EXPORT_SYMBOL_GPL(ata_plat_remove);
EXPORT_SYMBOL_GPL(ahci_platform_suspend);
