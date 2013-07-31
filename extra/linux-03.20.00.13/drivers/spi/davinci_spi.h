/*
 * Copyright (C) 2009 Texas Instruments.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __DAVINCI_SPI_H
#define __DAVINCI_SPI_H

#define SPI_MAX_CHIPSELECT	2

#define CS_DEFAULT	0xFF

#define SPI_BUFSIZ	(SMP_CACHE_BYTES + 1)
#define DAVINCI_DMA_DATA_TYPE_S8	0x01
#define DAVINCI_DMA_DATA_TYPE_S16	0x02
#define DAVINCI_DMA_DATA_TYPE_S32	0x04

#define SPIFMT_PHASE_MASK	BIT(16)
#define SPIFMT_POLARITY_MASK	BIT(17)
#define SPIFMT_DISTIMER_MASK	BIT(18)
#define SPIFMT_SHIFTDIR_MASK	BIT(20)
#define SPIFMT_WAITENA_MASK	BIT(21)
#define SPIFMT_PARITYENA_MASK	BIT(22)
#define SPIFMT_ODD_PARITY_MASK	BIT(23)
#define SPIFMT_WDELAY_MASK	0x3f000000u
#define SPIFMT_WDELAY_SHIFT	24
#define SPIFMT_CHARLEN_MASK	0x0000001Fu

/* SPIGCR1 */
#define SPIGCR1_SPIENA_MASK	0x01000000u

/* SPIPC0 */
#define SPIPC0_DIFUN_MASK	BIT(11)		/* MISO */
#define SPIPC0_DOFUN_MASK	BIT(10)		/* MOSI */
#define SPIPC0_CLKFUN_MASK	BIT(9)		/* CLK */
#define SPIPC0_SPIENA_MASK	BIT(8)		/* nREADY */
#define SPIPC0_EN1FUN_MASK	BIT(1)
#define SPIPC0_EN0FUN_MASK	BIT(0)

#define SPIINT_MASKALL		0x0101035F
#define SPI_INTLVL_1		0x000001FFu
#define SPI_INTLVL_0		0x00000000u

/* SPIDAT1 */
#define SPIDAT1_CSHOLD_SHIFT	28
#define SPIDAT1_CSNR_SHIFT	16
#define SPIGCR1_CLKMOD_MASK	BIT(1)
#define SPIGCR1_MASTER_MASK     BIT(0)
#define SPIGCR1_LOOPBACK_MASK	BIT(16)

/* SPIBUF */
#define SPIBUF_TXFULL_MASK	BIT(29)
#define SPIBUF_RXEMPTY_MASK	BIT(31)

/* Error Masks */
#define SPIFLG_DLEN_ERR_MASK		BIT(0)
#define SPIFLG_TIMEOUT_MASK		BIT(1)
#define SPIFLG_PARERR_MASK		BIT(2)
#define SPIFLG_DESYNC_MASK		BIT(3)
#define SPIFLG_BITERR_MASK		BIT(4)
#define SPIFLG_OVRRUN_MASK		BIT(6)
#define SPIFLG_RX_INTR_MASK		BIT(8)
#define SPIFLG_TX_INTR_MASK		BIT(9)
#define SPIFLG_BUF_INIT_ACTIVE_MASK	BIT(24)
#define SPIFLG_MASK			(SPIFLG_DLEN_ERR_MASK \
				| SPIFLG_TIMEOUT_MASK | SPIFLG_PARERR_MASK \
				| SPIFLG_DESYNC_MASK | SPIFLG_BITERR_MASK \
				| SPIFLG_OVRRUN_MASK | SPIFLG_RX_INTR_MASK \
				| SPIFLG_TX_INTR_MASK \
				| SPIFLG_BUF_INIT_ACTIVE_MASK)

#define SPIINT_DLEN_ERR_INTR	BIT(0)
#define SPIINT_TIMEOUT_INTR	BIT(1)
#define SPIINT_PARERR_INTR	BIT(2)
#define SPIINT_DESYNC_INTR	BIT(3)
#define SPIINT_BITERR_INTR	BIT(4)
#define SPIINT_OVRRUN_INTR	BIT(6)
#define SPIINT_RX_INTR		BIT(8)
#define SPIINT_TX_INTR		BIT(9)
#define SPIINT_DMA_REQ_EN	BIT(16)
#define SPIINT_ENABLE_HIGHZ	BIT(24)

#define SPI_T2CDELAY_SHIFT	16
#define SPI_C2TDELAY_SHIFT	24

/* SPI Controller registers */
#define SPIGCR0		0x00
#define SPIGCR1		0x04
#define SPIINT		0x08
#define SPILVL		0x0c
#define SPIFLG		0x10
#define SPIPC0		0x14
#define SPIPC1		0x18
#define SPIPC2		0x1c
#define SPIPC3		0x20
#define SPIPC4		0x24
#define SPIPC5		0x28
#define SPIPC6		0x2c
#define SPIPC7		0x30
#define SPIPC8		0x34
#define SPIDAT0		0x38
#define SPIDAT1		0x3c
#define SPIBUF		0x40
#define SPIEMU		0x44
#define SPIDELAY	0x48
#define SPIDEF		0x4c
#define SPIFMT0		0x50
#define SPIFMT1		0x54
#define SPIFMT2		0x58
#define SPIFMT3		0x5c
#define TGINTVEC0	0x60
#define TGINTVEC1	0x64

struct davinci_spi_slave {
	u32	cmd_to_write;
	u32	clk_ctrl_to_write;
	u32	bytes_per_word;
	u8	active_cs;
};

/* We have 2 DMA channels per CS, one for RX and one for TX */
struct davinci_spi_dma {
	int			dma_tx_channel;
	int			dma_rx_channel;
	int			dma_tx_sync_dev;
	int			dma_rx_sync_dev;
	enum dma_event_q	eventq;

	struct completion	dma_tx_completion;
	struct completion	dma_rx_completion;
};

/* SPI Controller driver's private data. */
struct davinci_spi {
	struct spi_bitbang	bitbang;
	struct clk		*clk;

	u8			version;
	resource_size_t		pbase;
	void __iomem		*base;
	size_t			region_size;
	u32			irq;
	struct completion	done;

	const void		*tx;
	void			*rx;
	u8			*tmp_buf;
	int			count;
	struct davinci_spi_dma	*dma_channels;
	struct			davinci_spi_platform_data *pdata;

	void			(*get_rx)(u32 rx_data, struct davinci_spi *);
	u32			(*get_tx)(struct davinci_spi *);

	u32			speed;
	u32			cs_num;
	bool			in_use;

	struct davinci_spi_slave slave[SPI_MAX_CHIPSELECT];

#ifdef CONFIG_CPU_FREQ
	struct notifier_block	freq_transition;
#endif
};

#endif /* __DAVINCI_SPI_H */
