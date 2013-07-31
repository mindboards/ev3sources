/*
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Driver for SPI controller on DaVinci. Based on atmel_spi.c
 * by Atmel Corporation
 *
 * Copyright (C) 2007 Atmel Corporation
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <spi.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include "davinci_spi.h"

void spi_init()
{
	/* do nothing */
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
			unsigned int max_hz, unsigned int mode)
{
	struct davinci_spi_slave	*ds;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	ds = malloc(sizeof(*ds));
	if (!ds)
		return NULL;

	ds->slave.bus = bus;
	ds->slave.cs = cs;
	ds->regs = (struct davinci_spi_regs *)CONFIG_SYS_SPI_BASE;
	ds->freq = max_hz;

	return &ds->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct davinci_spi_slave *ds = to_davinci_spi(slave);

	free(ds);
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct davinci_spi_slave *ds = to_davinci_spi(slave);
	unsigned int scalar, data1_reg_val = 0;

	/* Enable the SPI hardware */
	writel(SPIGCR0_SPIRST_MASK, &ds->regs->gcr0);
	udelay(1000);
	writel(SPIGCR0_SPIENA_MASK, &ds->regs->gcr0);

	/* Set master mode, powered up and not activated */
	writel(SPIGCR1_MASTER_MASK | SPIGCR1_CLKMOD_MASK, &ds->regs->gcr1);

	/* CS, CLK, SIMO and SOMI are functional pins */
	writel((SPIPC0_EN0FUN_MASK | SPIPC0_CLKFUN_MASK |
		SPIPC0_DOFUN_MASK | SPIPC0_DIFUN_MASK), &ds->regs->pc0);

	/* setup format */
	scalar = ((CONFIG_SYS_SPI_CLK / ds->freq) - 1) & 0xFF;

	/*
	 * Use following format:
	 *   character length = 8,
	 *   clock signal delayed by half clk cycle,
	 *   clock low in idle state - Mode 0,
	 *   MSB shifted out first
	 */
	writel(8 | (scalar << SPIFMT_PRESCALE_SHIFT) |
		(1 << SPIFMT_PHASE_SHIFT), &ds->regs->fmt0);

	/* hold cs active at end of transfer until explicitly de-asserted */
	data1_reg_val = (1 << SPIDAT1_CSHOLD_SHIFT) |
			(slave->cs << SPIDAT1_CSNR_SHIFT);
	writel(data1_reg_val, &ds->regs->dat1);

	/*
	 * Including a minor delay. No science here. Should be good even with
	 * no delay
	 */
	writel((50 << SPI_C2TDELAY_SHIFT) |
		(50 << SPI_T2CDELAY_SHIFT), &ds->regs->delay);

	/* default chip select register */
	writel(SPIDEF_CSDEF0_MASK, &ds->regs->def);

	/* no interrupts */
	writel(0, &ds->regs->int0);
	writel(0, &ds->regs->lvl);

	/* enable SPI */
	writel((readl(&ds->regs->gcr1) | SPIGCR1_SPIENA_MASK), &ds->regs->gcr1);

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct davinci_spi_slave *ds = to_davinci_spi(slave);

	/* Disable the SPI hardware */
	writel(SPIGCR0_SPIRST_MASK, &ds->regs->gcr0);
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	struct davinci_spi_slave *ds = to_davinci_spi(slave);
// LEGO - CHANGED 20120830
//	unsigned int	len, data1_reg_val = readl(&ds->regs->dat1);
//	int		ret, i;
//	const u8	*txp = dout; /* dout can be NULL for read operation */
//	u8		*rxp = din;  /* din can be NULL for write operation */
	unsigned int	tx_len, rx_len, status;
	unsigned int    fmt0_reg_val, data1_reg_val;
	int	            ret;
	u8              *txbp = dout; /* dout can be NULL for read operation */
	u8              *rxbp = din;  /* din can be NULL for write operation */
	u16             *txwp = dout; /* dout can be NULL for read operation */
	u16	            *rxwp = din;  /* din can be NULL for write operation */
	u16             write_val;

	ret = 0;

	if (bitlen == 0)
		/* Finish any previously submitted transfers */
		goto out;

	/*
	 * It's not clear how non-8-bit-aligned transfers are supposed to be
	 * represented as a stream of bytes...this is a limitation of
	 * the current SPI interface - here we terminate on receiving such a
	 * transfer request.
	 */
	if (bitlen % 8) {
		/* Errors always terminate an ongoing transfer */
		flags |= SPI_XFER_END;
		goto out;
	}
// LEGO - CHANGED 20120830
//	len = bitlen / 8;
//	tx_len = rx_len = bitlen / 8;
	rx_len = tx_len = bitlen / 8;

	/* do an empty read to clear the current contents */
	readl(&ds->regs->buf);
// LEGO - CHANGED 20120830
//	/* keep writing and reading 1 byte until done */
//	for (i = 0; i < len; i++) {
//		/* wait till TXFULL is asserted */
//		while (readl(&ds->regs->buf) & SPIBUF_TXFULL_MASK);
//
//		/* write the data */
//		data1_reg_val &= ~0xFFFF;
//		if (txp) {
//			data1_reg_val |= *txp;
//			txp++;
//		}
//
//		/*
//		 * Write to DAT1 is required to keep the serial transfer going.
//		 * We just terminate when we reach the end.
//		 */
//		if ((i == (len - 1)) && (flags & SPI_XFER_END)) {
//			/* clear CS hold */
//			writel(data1_reg_val &
//				~(1 << SPIDAT1_CSHOLD_SHIFT), &ds->regs->dat1);
//		} else {
//			/* enable CS hold */
//			data1_reg_val |= ((1 << SPIDAT1_CSHOLD_SHIFT) |
//					(slave->cs << SPIDAT1_CSNR_SHIFT));
//			writel(data1_reg_val, &ds->regs->dat1);
//		}
//
//		/* read the data - wait for data availability */
//		while (readl(&ds->regs->buf) & SPIBUF_RXEMPTY_MASK);
//
//		if (rxp) {
//			*rxp = readl(&ds->regs->buf) & 0xFF;
//			rxp++;
//		} else {
//			/* simply drop the read character */
//			readl(&ds->regs->buf);
//		}
	/* enable CS hold */
	data1_reg_val = readl(&ds->regs->dat1);
	data1_reg_val &= 0xFFFF0000;
	data1_reg_val |= ((1 << SPIDAT1_CSHOLD_SHIFT) | (slave->cs << SPIDAT1_CSNR_SHIFT));

	fmt0_reg_val = readl(&ds->regs->fmt0);
	fmt0_reg_val &= 0xFFFFFFE0;
	
	if ( (tx_len & 1) || ((int)txbp & 1) || ((int)rxbp & 1) ) {

	    // Setup for 8-bit transfers
	    writel(8 | fmt0_reg_val, &ds->regs->fmt0);

	    /* keep writing and reading 1 byte until done */
	    while(rx_len) {
	        status = readl(&ds->regs->buf);
	    
	        // If we are to transfer last byte correct Chip Select setting
	        if ((tx_len == 1) && (flags & SPI_XFER_END))
	            data1_reg_val &= ~(1 << SPIDAT1_CSHOLD_SHIFT);
	    
	        // Check if it's OK to add another byte to TX buffer
	        if ((!(status & SPIBUF_TXFULL_MASK)) && tx_len) {
	            if (txbp) {
		        writel(data1_reg_val | *txbp++, &ds->regs->dat1);
	            } else {
		        writel(data1_reg_val, &ds->regs->dat1);
	            }
	            tx_len--;
	        }
	    
	        // Check if we can read another byte from RX buffer
	        if (!(status & SPIBUF_RXEMPTY_MASK)) {
	            if (rxbp) *rxbp++ = status & 0xFF;
	            rx_len--;
	        }
	    }

	} else {

	    // Setup for 16-bit transfers
	    writel(16 | fmt0_reg_val, &ds->regs->fmt0);
	    tx_len /= 2;
	    rx_len /= 2;

	    /* keep writing and reading 2 byte until done */
	    while(rx_len) {
	        status = readl(&ds->regs->buf);
	    
	        // If we are to transfer last byte correct Chip Select setting
	        if ((tx_len == 1) && (flags & SPI_XFER_END))
	            data1_reg_val &= ~(1 << SPIDAT1_CSHOLD_SHIFT);
	    
	        // Check if it's OK to add another byte to TX buffer
	        if ((!(status & SPIBUF_TXFULL_MASK)) && tx_len) {
	            if (txwp) {
		        write_val = ((*txwp & 0x00FF) << 8) | ((*txwp & 0xFF00) >> 8);
		        writel(data1_reg_val | write_val, &ds->regs->dat1);
		        txwp++;
	            } else {
		        writel(data1_reg_val, &ds->regs->dat1);
	            }
	            tx_len--;
	        }
	    
	        // Check if we can read another byte from RX buffer
	        if (!(status & SPIBUF_RXEMPTY_MASK)) {
	            if (rxwp) *rxwp++ = ((status & 0x00FF) << 8) | ((status & 0xFF00) >> 8);
	            rx_len--;
	        }
	    }
// LEGO - CHANGE END
	}
	return 0;

out:
	if (flags & SPI_XFER_END) {
		writel(data1_reg_val &
			~(1 << SPIDAT1_CSHOLD_SHIFT), &ds->regs->dat1);
	}
	return 0;
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs == 0;
}

void spi_cs_activate(struct spi_slave *slave)
{
	/* do nothing */
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	/* do nothing */
}

