/*
 * DA850/OMAP-L138 McBSP driver
 *
 * McBSP- Multi-channel Buffer Serial Port peripheral can be used
 * for serial data communication with compatible devices. It can also
 * be used to inerface audio devices. The peripheral supports DMA mode
 * of operation and can be used in real-time applications. The device
 * supports upto 128 channels of data for TDM mode. There is a FIFO
 * internal to the device which helps in data buffering.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed .as is. WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/compiler.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>

#include <mach/memory.h>
#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/mux.h>
#include <mach/edma.h>
#include <mach/mcbsp.h>

/**
 * TODO: check master receiver and slave transmitter config
 */

/* Register offset macros */
#define MCBSP_DRR	(0x00)
#define MCBSP_DXR	(0x04)
#define MCBSP_SPCR	(0x08)
#define MCBSP_RCR	(0x0C)
#define MCBSP_XCR	(0x10)
#define MCBSP_SRGR	(0x14)
#define MCBSP_MCR	(0x18)
#define MCBSP_RCER0	(0x1C)
#define MCBSP_XCER0	(0x20)
#define MCBSP_PCR	(0x24)
#define MCBSP_RCER1	(0x28)
#define MCBSP_XCER1	(0x2C)
#define MCBSP_RCER2	(0x30)
#define MCBSP_XCER2	(0x34)
#define MCBSP_RCER3	(0x38)
#define MCBSP_XCER3	(0x3C)
#define MCBSP_WFIFOCTL	(0x810)
#define MCBSP_WFIFOSTS	(0x814)
#define MCBSP_RFIFOCTL	(0x818)
#define MCBSP_RFIFOSTS	(0x81C)

/* Register bit definitions */

/* SPCR */

#define FRST		BIT(23)
#define GRST		BIT(22)
#define XINTM_MASK	(BIT(21) | BIT(20))
#define XSYNCERR	BIT(19)
#define XEMPTY		BIT(18)
#define XRDY		BIT(17)
#define	XRST		BIT(16)
#define DLB		BIT(15)
#define RJUST_MASK	(BIT(14) | BIT(13))
#define CLKSTP_MASK	(BIT(12) | BIT(11))
#define DXENA		BIT(7)
#define	RINTM_MASK	(BIT(5) | BIT(4))
#define RSYNCERR	BIT(3)
#define RFULL		BIT(2)
#define RRDY		BIT(1)
#define RRST		BIT(0)

#define	RINTM(value)	(value << 4)
#define CLKSTP(value)	(value << 11)
#define RJUST(value)	(value << 13)
#define XINTM(value)	(value << 20)

/* PCR */

#define FSXM		BIT(11)
#define FSRM		BIT(10)
#define CLKXM		BIT(9)
#define CLKRM		BIT(8)
#define SCLKME		BIT(7)
#define FSXP		BIT(3)
#define FSRP		BIT(2)
#define CLKXP		BIT(1)
#define CLKRP		BIT(0)

/* RCR / XCR */

#define PHASE		BIT(31)
#define FRLEN2(value)	(value << 24)
#define WDLEN2(value)	(value << 21)
#define COMPAND(value)	(value << 19)
#define FIG		BIT(18)
#define DATDLY(value)	(value << 16)
#define FRLEN1(value)	(value << 8)
#define WDLEN1(value)	(value << 5)
#define WDREVRS		BIT(4)

/* SRGR */
/*
 * 31		30	29	28	27 16	15 8	7 0
 * GSYNC	CLKSP	CLKSM	FSGM	FPER	FWID	CLKGDV
 * RW, +0	RW, +0	RW	RW, +0	RW, +0	RW, +0	RW
 *
 */

#define GSYNC		BIT(31)
#define CLKSP		BIT(30)
#define CLKSM		BIT(29)
#define	FSGM		BIT(28)
#define	FPER(value)	(value << 16)
#define	FWID(value)	(value << 8)
#define CLKGDV(value)	(value)
#define CLKGDV_MASK	(0xff)

/* MCR */

#define XMCME		BIT(25)
#define XPBBLK(value)	(value << 23)
#define XPABLK(value)	(value << 21)
#define XCBLK		(BIT(20) | BIT(19) | BIT(18))
#define XMCM(value)	(value << 16)
#define RMCME		BIT(9)
#define RPBBLK(value)	(value << 7)
#define RPABLK(value)	(value << 5)
#define RCBLK		(BIT(4) | BIT(3) | BIT(2))
#define RMCM		BIT(0)
#define XMCM_MASK	(BIT(16) | BIT(17))

/* WFIFOCTL / RFIFOCTL */
#define FIFOEN		BIT(16)
#define NUMEVT(value)	(value << 8)
#define NUMDMA(value)	(value)
#define NUMEVT_MASK	(0xff << 8)
#define NUMDMA_MASK	0xff

/* WFIFOSTS / RFIFOSTS */
#define LVL_MASK	0xff

#define NUM_MCBSP_INSTANCES 2

static struct davinci_mcbsp_dev *mcbsp[NUM_MCBSP_INSTANCES];

static inline void davinci_mcbsp_write_reg(struct davinci_mcbsp_dev *dev,
					   int reg, u32 val)
{
	writel(val, dev->io_base + reg);
}

static inline u32 davinci_mcbsp_read_reg(struct davinci_mcbsp_dev *dev, int reg)
{
	return readl(dev->io_base + reg);
}

/**
 * davinci_mcbsp_dump_reg: Register dump for McBSP
 * @dev: McBSP device pointer
 *
 * Useful for debugging
 */
void davinci_mcbsp_dump_reg(struct davinci_mcbsp_dev *dev)
{
	dev_dbg(dev->dev, "------ McBSP%d regs -----\n", dev->id);
	dev_dbg(dev->dev, "DRR:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_DRR));
	dev_dbg(dev->dev, "DXR:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_DXR));
	dev_dbg(dev->dev, "SPCR: 0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_SPCR));
	dev_dbg(dev->dev, "RCR:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_RCR));
	dev_dbg(dev->dev, "XCR:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_XCR));
	dev_dbg(dev->dev, "SRGR: 0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_SRGR));
	dev_dbg(dev->dev, "MCR: 0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_MCR));
	dev_dbg(dev->dev, "RCER0:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_RCER0));
	dev_dbg(dev->dev, "XCER0:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_XCER0));
	dev_dbg(dev->dev, "PCR:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_PCR));
	dev_dbg(dev->dev, "RCER1:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_RCER1));
	dev_dbg(dev->dev, "XCER1:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_XCER1));
	dev_dbg(dev->dev, "RCER2:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_RCER2));
	dev_dbg(dev->dev, "XCER2:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_XCER2));
	dev_dbg(dev->dev, "RCER3:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_RCER3));
	dev_dbg(dev->dev, "XCER3:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_XCER3));
	dev_dbg(dev->dev, "WFIFOCTL:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_WFIFOCTL));
	dev_dbg(dev->dev, "WFIFOSTS:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_WFIFOSTS));
	dev_dbg(dev->dev, "RFIFOCTL:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_RFIFOCTL));
	dev_dbg(dev->dev, "RFIFOSTS:  0x%08x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_RFIFOSTS));
	dev_dbg(dev->dev, "-------------------------\n");
}
EXPORT_SYMBOL(davinci_mcbsp_dump_reg);

/**
 * davinci_mcbsp_trigger_tx: Enable tht transmitter
 * @dev: McBSP device pointer
 *
 * Called by the transmit functions - in DMA mode, after the EDMA parameter
 * initialization.
 */
static void davinci_mcbsp_trigger_tx(struct davinci_mcbsp_dev *dev)
{
	u32 val;

	val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);

	/* DLB */
	if (dev->op_mode & DAVINCI_MCBSP_DLB_MODE)
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val | DLB);
	else
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val & ~DLB);

	/* Enable transmitter */
	val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val | XRST);

	udelay(100);

	/* Internal frame sync generator is used */

	/* FRST bit is set, wait for 7-8 clock cycles */
	if (dev->master) {
		dev_dbg(dev->dev, "Configuring TX in master mode\n");
		val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val | FRST);
	}

	udelay(1000);
}

/**
 * davinci_mcbsp_trigger_rx: Enable tht receiver
 * @dev: McBSP device pointer
 *
 * Called by the receive functions - in DMA mode, after the EDMA parameter
 * initialization.
 */
static void davinci_mcbsp_trigger_rx(struct davinci_mcbsp_dev *dev)
{
	u32 val;

	val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	/* DLB */
	if (dev->op_mode & DAVINCI_MCBSP_DLB_MODE)
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val | DLB);
	else
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val & ~DLB);

	/* Enable receiver */
	val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val | RRST);

	udelay(100);

	/* Internal frame sync generator is used */
	/* FRST bit is set, wait for 7-8 clock cycles */
	if (dev->master) {
		dev_dbg(dev->dev, "Configuring RX in master mode\n");
		val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val | FRST);
	}

	udelay(1000);
}

/**
 * davinci_mcbsp_get_cr_value: Prepare control register value
 * @params: McBSP device params
 *
 * Called by the McBSP parameter configuration fucntion. This returns the value
 * of the control register for receive and transmit module
 */
static unsigned int
davinci_mcbsp_get_cr_value(struct davinci_mcbsp_params *params)
{
	u32 val, word_len;
	int fl1 = params->frame_length1 - 1;
	int fl2 = params->frame_length2 - 1;

	if (fl1 < 0)
		fl1 = 0;
	if (fl2 < 0)
		fl2 = 0;

	val = FRLEN2(fl2) | FRLEN1(fl1);
	word_len = params->word_length2 / 4 - 2;
	if (word_len > 5)
		word_len = 5;
	val |= WDLEN2(word_len);

	word_len = params->word_length1 / 4 - 2;
	if (word_len > 5)
		word_len = 5;
	val |= WDLEN1(word_len);

	val |= DATDLY(params->data_delay);
	val |= COMPAND(params->compand);
	if (params->phase)
		val |= PHASE;

	if (params->word_rev)
		val |= WDREVRS;

	if (params->frame_ignore)
		val |= FIG;

	return val;
}

/**
 * davinci_mcbsp_config_pcr: Configure the Port Contorl Register
 * @dev: McBSP device pointer
 *
 * Called by the McBSP configuration fucntion to program the port as
 * inputs or outputs for master or slave operation
 */
static void davinci_mcbsp_config_pcr(struct davinci_mcbsp_dev *dev)
{
	u32 val = 0;

	if (dev->master)
		val |= (CLKXM | CLKRM | FSXM | FSRM);
	else
		val |= SCLKME;

	davinci_mcbsp_write_reg(dev, MCBSP_PCR, val);
	if (!dev->master) {
		/* clock source from CLKR */
		val = davinci_mcbsp_read_reg(dev, MCBSP_SRGR);
		val &= ~CLKSM;
		davinci_mcbsp_write_reg(dev, MCBSP_SRGR, val);
	}
}

/**
 * davinci_mcbsp_config_params: Configure the McBSP contorl registersr
 * @dev: McBSP device pointer
 * @master: Flag indicating master or slave mode of operation
 *
 * Called by the higher layer to set the parameters for reveive and transmit
 * frame length, word size, also configures the port as input or output
 * as required.
 */
void davinci_mcbsp_config_params(struct davinci_mcbsp_dev *dev, u8 master)
{
	struct davinci_mcbsp_params *params;
	u32 val;

	dev->master = master;
	davinci_mcbsp_config_pcr(dev);

	params = &dev->tx_params;
	val = davinci_mcbsp_get_cr_value(params);
	davinci_mcbsp_write_reg(dev, MCBSP_XCR, val);

	params = &dev->rx_params;
	val = davinci_mcbsp_get_cr_value(params);
	davinci_mcbsp_write_reg(dev, MCBSP_RCR, val);

	val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	val &= ~(XINTM_MASK | RINTM_MASK);
	val |= RINTM(dev->rx_params.intr_mode) |
			XINTM(dev->tx_params.intr_mode);
	davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val);
}
EXPORT_SYMBOL(davinci_mcbsp_config_params);

/**
 * davinci_mcbsp_config_clock: Configre the sample rate
 * @dev: McBSP device pointer
 * @freq: Sampling frequency
 *
 * This fucntion is used to set the divider values for the sample rate
 * generator when McBSP generates clocks for data transfer. It requires the
 * frame length and word length parameters to be filled in the device
 * data structure.
 */
void davinci_mcbsp_config_clock(struct davinci_mcbsp_dev *dev,
						unsigned int freq)
{
	unsigned long clk_rate;
	unsigned int div;
	u32 val, fper;

	clk_rate = clk_get_rate(dev->clk);

	dev_dbg(dev->dev, "Clock rate is %lu, Freq is %d\n", clk_rate, freq);
	/* calculate the divider value */
	div = (unsigned int)clk_rate /
			(freq * dev->tx_params.word_length1 *
				dev->tx_params.frame_length1) - 1;

	dev_dbg(dev->dev, "The divider  value is %d(0x%x)\n", div, div);
	val = davinci_mcbsp_read_reg(dev, MCBSP_SRGR);
	val &= ~(CLKGDV_MASK);
	val |= div;

	/* FPER for the frame sync is word length * frame_length */
	fper = dev->tx_params.word_length1 * dev->tx_params.frame_length1 - 1;
	val &= ~FPER(0xfff);
	val |= FPER(fper);

	/* FWID is one bit (Default) */
	val &= ~FWID(0xff);

	/* Set FSGM, CLKSM -> internal clock and frame sync from McBSP */
	if (dev->master)
		val |= (CLKSM | FSGM);

	dev_dbg(dev->dev, "value written to SRGR is %08x\n", val);
	davinci_mcbsp_write_reg(dev, MCBSP_SRGR, val);
}
EXPORT_SYMBOL(davinci_mcbsp_config_clock);

/**
 * davinci_mcbsp_config_fifo: Configure the FIFO buffer of McBSP
 * @dev: McBSP device pointer
 *
 * Configure the FIFO parameters.
 */
void davinci_mcbsp_config_fifo(struct davinci_mcbsp_dev *dev)
{
	u32 val;

	val = NUMEVT(dev->tx_params.numevt) | NUMDMA(dev->tx_params.numdma);
	dev_dbg(dev->dev, "WFIFO value %08x\n", val);
	davinci_mcbsp_write_reg(dev, MCBSP_WFIFOCTL, val);

	val = NUMEVT(dev->rx_params.numevt) | NUMDMA(dev->rx_params.numdma);
	dev_dbg(dev->dev, "RFIFO value %08x\n", val);
	davinci_mcbsp_write_reg(dev, MCBSP_RFIFOCTL, val);
}
EXPORT_SYMBOL(davinci_mcbsp_config_fifo);

/**
 * davinci_get_partition_blocks: For 2 partition mode gives the blocks
 * @params: McBSP device parameters
 *
 * Called by the mutli-channel mode configuration fucntion to find out the
 * maximum blocks that will be used by partition A and B
 */
static void davinci_get_partition_blocks(struct davinci_mcbsp_params *params)
{
	u8 frame_length;
	u8 tmp;

	frame_length = params->frame_length1 - 1;
	tmp = frame_length / 32;

	params->max_pa = params->max_pb = tmp;

	tmp = frame_length - params->max_pa * 32;
	params->max_pa += 1;
	if (tmp > 15)
		params->max_pb += 1;
}

/**
 * davinci_mcbsp_config_multichannel_mode: Multi-channel selection mode config
 * @dev: McBSP device pointer
 *
 * Fucntion to configure McBSP in multichannel selection mode either in 2
 * partition or 8 partition mode. Also writes the user set channel enable
 * values to the receive or transmit channel enable registers.
 */
void davinci_mcbsp_config_multichannel_mode(struct davinci_mcbsp_dev *dev)
{
	u32 val = 0;
	int i;

	/* configure the multichannel mode */
	if (dev->tx_params.mc_mode & DAVINCI_MCBSP_MC_XMCME)
		val |= XMCME;

	val &= ~XMCM_MASK;
	val |= XMCM((dev->tx_params.mc_mode & 0x03));
	if (dev->rx_params.mc_mode & DAVINCI_MCBSP_MC_RMCME)
		val |= RMCME;

	if (dev->rx_params.mc_mode & DAVINCI_MCBSP_MC_RMCM1)
		val |= RMCM;

	val &= ~(RPABLK(3) | RPBBLK(3));
	val |= RPABLK(dev->rx_params.pa_blk) | RPABLK(dev->rx_params.pb_blk);
	val &= ~(XPABLK(3) | XPBBLK(3));
	val |= XPABLK(dev->tx_params.pa_blk) | XPBBLK(dev->tx_params.pb_blk);
	davinci_mcbsp_write_reg(dev, MCBSP_MCR, val);

	val = dev->tx_params.cer[0];
	val |= (dev->tx_params.cer[1] << 16);
	davinci_mcbsp_write_reg(dev, MCBSP_XCER0, val);

	val = dev->rx_params.cer[0];
	val |= (dev->rx_params.cer[1] << 16);
	davinci_mcbsp_write_reg(dev, MCBSP_RCER0, val);

	if (dev->tx_params.mc_mode & DAVINCI_MCBSP_MC_XMCME) {
		for (i = 2; i < 8; i += 2) {
			val = dev->tx_params.cer[i];
			val |= (dev->tx_params.cer[i + 1] << 16);
			davinci_mcbsp_write_reg(dev, (MCBSP_XCER1 +
							(i - 2) * 4), val);
		}
	}

	if (dev->rx_params.mc_mode & DAVINCI_MCBSP_MC_RMCME) {
		for (i = 2; i < 8; i += 2) {
			val = dev->rx_params.cer[i];
			val |= (dev->rx_params.cer[i + 1] << 16);
			davinci_mcbsp_write_reg(dev, (MCBSP_RCER1 +
							(i - 2) * 4), val);
		}
	}

	val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	val &= ~DXENA;
	if (dev->dxena)
		val |= DXENA;

	davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val);

	/* Calculate the maximum blocks for 2 partition mode */
	davinci_get_partition_blocks(&dev->rx_params);
	davinci_get_partition_blocks(&dev->tx_params);
	dev_dbg(dev->dev, "Max TX A:%d, B:%d\n", dev->tx_params.max_pa,
							dev->tx_params.max_pb);
	dev_dbg(dev->dev, "Max RX A:%d, B:%d\n", dev->rx_params.max_pa,
							dev->rx_params.max_pb);
}
EXPORT_SYMBOL(davinci_mcbsp_config_multichannel_mode);

/**
 * davinci_mcbsp_dma_tx_cb: Transmit DMA callback fucntion
 * @lch: Channel number for which the callback occurs
 * @ch_status: Status of the DMA transfer
 * @data: private data
 *
 * EDMA layer executes this callback fucntion after transfer is done
 */
static void davinci_mcbsp_dma_tx_cb(unsigned lch, u16 ch_status, void *data)
{
	struct davinci_mcbsp_dev *dev = (struct davinci_mcbsp_dev *)data;

	dev_dbg(dev->dev, "TX DMA callback with status %u\n", ch_status);
	if (ch_status != DMA_COMPLETE)
		return;

	edma_stop(lch);
	edma_free_channel(lch);
	complete(&dev->tx_params.dma_completion);
}

/**
 * davinci_mcbsp_dma_rx_cb: Recive DMA callback fucntion
 * @lch: Channel number for which the callback occurs
 * @ch_status: Status of the DMA transfer
 * @data: private data
 *
 * EDMA layer executes this callback fucntion after transfer is done
 */
static void davinci_mcbsp_dma_rx_cb(unsigned lch, u16 ch_status, void *data)
{
	struct davinci_mcbsp_dev *dev = (struct davinci_mcbsp_dev *)data;

	dev_dbg(dev->dev, "RX DMA callback with status %u\n", ch_status);
	if (ch_status != DMA_COMPLETE)
		return;

	edma_stop(lch);
	edma_free_channel(lch);
	complete(&dev->rx_params.dma_completion);
}

/**
 * davinci_mcbsp_xmit_buf: DMA mode transmit fucntion
 * @dev: McBSP device pointer
 * @buf: DMA buffer address
 * @length: length of the buffer in bytes
 *
 * Fucntion to be used for data transmission. This requires the DMA address
 * and length of the buffer to be transferred. EDMA params are programmed
 * as per the word and frame lenght values and the buffer type.
 */
int davinci_mcbsp_xmit_buf(struct davinci_mcbsp_dev *dev, dma_addr_t buf,
							 unsigned int length)
{
	s16 frame_len = dev->tx_params.frame_length1;
	struct edmacc_param param_set;
	s16 data_type;
	int ret, lch;
	u16 ccnt = 1;
	s16 buf_size;
	u32 val;

	ret = edma_alloc_channel(dev->tx_params.dma_ch, davinci_mcbsp_dma_tx_cb,
								dev, EVENTQ_0);
	if (ret < 0)
		return ret;

	dev->tx_params.lch = ret;
	lch = ret;

	init_completion(&dev->tx_params.dma_completion);

	if (dev->tx_params.numevt) {
		val = davinci_mcbsp_read_reg(dev, MCBSP_WFIFOCTL);
		davinci_mcbsp_write_reg(dev, MCBSP_WFIFOCTL, val | FIFOEN);
		dev->tx_params.dma_addr = dev->fifo_base;
	}

	data_type = dev->tx_params.word_length1 / 8;
	if (dev->tx_params.numevt)
		data_type *= dev->tx_params.numevt;

	edma_set_dest(lch, dev->tx_params.dma_addr, INCR, W8BIT);
	edma_set_src(lch, buf, INCR, W8BIT);
	if (!dev->tx_params.buf_fmt) {
		dev_dbg(dev->dev, "TX interleaved buffer format\n");

		edma_set_src_index(lch, data_type, 0);
		edma_set_transfer_params(lch, data_type, length / data_type,
							1, 0, ASYNC);

	} else {
		dev_dbg(dev->dev, "TX non-interleaved buffer format\n");
		/* constraint is that buffers have to be contiguous */
		/* calculate the buf size for each slot */
		buf_size = length / frame_len;

		/* number of samples */
		ccnt = buf_size / data_type ;
		edma_set_src_index(lch, buf_size,
				(data_type - buf_size * (frame_len - 1)));

		dev_dbg(dev->dev, "TX: acnt is %x, bcnt is %x, cnt is %x\n",
						 data_type, frame_len, ccnt);
		edma_set_transfer_params(lch, data_type, frame_len, ccnt,
							frame_len, ASYNC);
	}

	edma_set_dest_index(lch, 0, 0);
	edma_read_slot(dev->tx_params.lch, &param_set);
	param_set.opt |= TCINTEN;
	param_set.opt |= EDMA_TCC(EDMA_CHAN_SLOT(dev->tx_params.lch));
	edma_write_slot(dev->tx_params.lch, &param_set);

	/* after this the frame sync should be enabled!!!!! */
	edma_start(lch);

	davinci_mcbsp_trigger_tx(dev);

	if (!(dev->op_mode & DAVINCI_MCBSP_DLB_MODE))
		wait_for_completion(&dev->tx_params.dma_completion);

	return 0;
}
EXPORT_SYMBOL(davinci_mcbsp_xmit_buf);

/**
 * davinci_mcbsp_recv_buf: DMA mode receive fucntion
 * @dev: McBSP device pointer
 * @buf: DMA buffer address
 * @length: length of the buffer in bytes
 *
 * Fucntion to be used for data reception. This requires the DMA address
 * and length of the buffer to be transferred. EDMA params are programmed
 * as per the word and frame lenght values and the buffer type.
 */
int davinci_mcbsp_recv_buf(struct davinci_mcbsp_dev *dev, dma_addr_t buf,
							unsigned int length)
{
	s16 frame_len = dev->rx_params.frame_length1;
	struct edmacc_param param_set;
	u16 ccnt = 1;
	s16 data_type;
	int ret, lch;
	s16 buf_size;
	u32 val;

	ret = edma_alloc_channel(dev->rx_params.dma_ch, davinci_mcbsp_dma_rx_cb,
								dev, EVENTQ_0);
	if (ret < 0)
		return ret;

	dev->rx_params.lch = ret;
	lch = ret;

	init_completion(&dev->rx_params.dma_completion);
	if (dev->rx_params.numevt) {
		val = davinci_mcbsp_read_reg(dev, MCBSP_RFIFOCTL);
		davinci_mcbsp_write_reg(dev, MCBSP_RFIFOCTL, val | FIFOEN);
		dev->rx_params.dma_addr = dev->fifo_base;
	}

	data_type = dev->rx_params.word_length1 / 8;
	if (dev->rx_params.numevt)
		data_type *= dev->rx_params.numevt;

	edma_set_src(lch, dev->rx_params.dma_addr, INCR, W8BIT);
	edma_set_dest(lch, buf, INCR, W8BIT);
	edma_set_src_index(lch, 0, 0);
	if (!dev->rx_params.buf_fmt) {
		dev_dbg(dev->dev, "RX buffer interleaved format\n");

		edma_set_dest_index(lch, data_type, 0);
		edma_set_transfer_params(lch, data_type, length / data_type, 1,
								0, ASYNC);
	} else {
		/* constraint is that buffers have to be contiguous */
		/* calculate the buf size for each slot */
		dev_dbg(dev->dev, "RX Non-interleaved buffer format\n");
		buf_size = length / dev->rx_params.frame_length1;
		ccnt = buf_size / data_type; /* number of samples */

		edma_set_dest_index(lch, buf_size,
				(data_type - buf_size * (frame_len - 1)));
		dev_dbg(dev->dev, "RX: acnt is %x, bcnt is %x, cnt is %x\n",
						data_type, frame_len, ccnt);
		edma_set_transfer_params(lch, data_type, frame_len, ccnt,
							frame_len, ASYNC);
	}

	edma_read_slot(dev->rx_params.lch, &param_set);
	param_set.opt |= TCINTEN;
	param_set.opt |= EDMA_TCC(EDMA_CHAN_SLOT(dev->rx_params.lch));
	edma_write_slot(dev->rx_params.lch, &param_set);

	/* after this the frame sync should be enabled!!!!! */
	edma_start(lch);

	dev_dbg(dev->dev, "Triggering the receiver\n");
	davinci_mcbsp_trigger_rx(dev);

	if (!(dev->op_mode & DAVINCI_MCBSP_DLB_MODE))
		wait_for_completion(&dev->rx_params.dma_completion);

	return 0;
}
EXPORT_SYMBOL(davinci_mcbsp_recv_buf);

/**
 * davinci_mcbsp_handle_subframe_interrupt: Sub-frame interrupt management
 * for two partition multi-channel selection mode
 * @dev: McBSP device pointer
 * @flag: Flag to decide transmit or receive mode
 *
 * Called by the interupt handler when interrupt mode is 1. The block
 * assignment is changed along with the channel enable registers
 */
static inline void davinci_mcbsp_handle_subframe_interrupt(
			struct davinci_mcbsp_dev *dev, u8 flag)
{
	struct davinci_mcbsp_params *params;
	u32 val;
	u32 mask;
	u8 shift;
	u32 ch_en;

	mask = RCBLK;
	params = &dev->rx_params;
	shift = 2;
	ch_en = davinci_mcbsp_read_reg(dev, MCBSP_RCER0);

	if (flag) {
		mask = XCBLK;
		params = &dev->tx_params;
		shift = 18;
		ch_en = davinci_mcbsp_read_reg(dev, MCBSP_XCER0);
	}

	val = davinci_mcbsp_read_reg(dev, MCBSP_MCR);
	if (((val & mask) >> shift) % 2) {
		/* Modify partiition A parameters */
		params->pa_blk += 1;
		if (params->pa_blk >= params->max_pa)
			params->pa_blk = 0;

		ch_en &= ~(0xffff);
		ch_en |= params->cer[2 * params->pa_blk];
		if (flag)
			davinci_mcbsp_write_reg(dev, MCBSP_XCER0, ch_en);
		else
			davinci_mcbsp_write_reg(dev, MCBSP_RCER0, ch_en);
	} else {
		/* Modify partition B parameters */
		params->pb_blk += 1;
		if (params->pb_blk >= params->max_pb)
			params->pb_blk = 0;

		ch_en &= ~(0xffff << 16);
		ch_en |= (params->cer[(2 * params->pb_blk) + 1] << 16);
		if (flag)
			davinci_mcbsp_write_reg(dev, MCBSP_XCER0, ch_en);
		else
			davinci_mcbsp_write_reg(dev, MCBSP_RCER0, ch_en);
	}

	val = davinci_mcbsp_read_reg(dev, MCBSP_MCR);
	if (!flag) {
		val &= ~(RPABLK(3) | RPBBLK(3));
		val |= RPABLK(dev->rx_params.pa_blk) |
					RPABLK(dev->rx_params.pb_blk);
	} else {
		val &= ~(XPABLK(3) | XPBBLK(3));
		val |= XPABLK(dev->tx_params.pa_blk) |
					XPBBLK(dev->tx_params.pb_blk);
	}

	davinci_mcbsp_write_reg(dev, MCBSP_MCR, val);
}

/**
 * davinci_mcbsp_rx_irq_handler: Receive interrupt handler
 * @irq: Interrupt number
 * @data: pointer to device specific data
 *
 * Receive interrupt handler
 */
static irqreturn_t davinci_mcbsp_rx_irq_handler(int irq, void *data)
{
	struct davinci_mcbsp_dev *dev = data;
	u32 val;

	if (dev->rx_params.intr_mode == 1) {
		spin_lock(&dev->lock);
		davinci_mcbsp_handle_subframe_interrupt(dev, 0);
		spin_unlock(&dev->lock);
	} else {
		val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
		dev_dbg(dev->dev, "RX IRQ callback : 0x%x\n", val);
		if (val & RFULL) {
			dev_dbg(dev->dev, "Receive overrun\n");
			val = davinci_mcbsp_read_reg(dev, MCBSP_DRR);
		}
		complete(&dev->rx_params.irq_completion);
	}

	return IRQ_HANDLED;

}

/**
 * davinci_mcbsp_tx_irq_handler: Transmit interrupt handler
 * @irq: Interrupt number
 * @data: pointer to device specific data
 *
 * Transmit interrupt handler
 */
static irqreturn_t davinci_mcbsp_tx_irq_handler(int irq, void *data)
{
	struct davinci_mcbsp_dev *dev = data;
	u32 val;

	if (dev->tx_params.intr_mode == 1) {
		spin_lock(&dev->lock);
		davinci_mcbsp_handle_subframe_interrupt(dev, 1);
		spin_unlock(&dev->lock);
	} else {
		dev_dbg(dev->dev, "TX IRQ callback : 0x%x\n",\
		davinci_mcbsp_read_reg(dev, MCBSP_SPCR));
		complete(&dev->tx_params.irq_completion);
		val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
		if (!(val & XEMPTY))
			dev_dbg(dev->dev, "Transmit underrun\n");

	}

	return IRQ_HANDLED;
}

/**
 * davinci_mcbsp_recv_word: Interrupt mode data receive fucntion
 * @dev: McBSP device pointer
 *
 * For reception in interrupt mode
 */
u32 davinci_mcbsp_recv_word(struct davinci_mcbsp_dev *dev)
{
	u32 word = 0;

	dev_dbg(dev->dev, "Current rx register value is %x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_DRR));

	wait_for_completion(&dev->rx_params.irq_completion);
	word = davinci_mcbsp_read_reg(dev, MCBSP_DRR);

	return word;
}
EXPORT_SYMBOL(davinci_mcbsp_recv_word);

/**
 * davinci_mcbsp_xmit_word: Interrupt mode data transmit fucntion
 * @dev: McBSP device pointer
 *
 * For transmission in interrupt mode
 */
void davinci_mcbsp_xmit_word(struct davinci_mcbsp_dev *dev, u32 word)
{
	wait_for_completion(&dev->tx_params.irq_completion);
	dev_dbg(dev->dev, "Current tx register value is %x\n",
			davinci_mcbsp_read_reg(dev, MCBSP_DXR));

	davinci_mcbsp_write_reg(dev, MCBSP_DXR, word);
}
EXPORT_SYMBOL(davinci_mcbsp_xmit_word);

/**
 * davinci_mcbsp_start_tx: Start the transmitter
 * @dev: McBSP device pointer
 *
 * Start the McBSP transmitter
 */
void davinci_mcbsp_start_tx(struct davinci_mcbsp_dev *dev)
{
	u32 val;

	/* wait for some clock cycles */
	udelay(100);

	/* start the sample rate generator */
	if (dev->master) {
		dev_dbg(dev->dev, "Configuring TX master mode\n");
		val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val | GRST);
	}

	/* wait for some clock cycles */
	udelay(100);

	/* transmitter -clear potential sync error */
	val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val | XRST);

	udelay(100);
	val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val & ~XRST);
	udelay(100);

	if (dev->op_mode & DAVINCI_MCBSP_IRQ_MODE)
		davinci_mcbsp_trigger_tx(dev);

	/* DMA related - when transmission begins */
	/* The remaining stpes are in the trigger fucntions */
}
EXPORT_SYMBOL(davinci_mcbsp_start_tx);

/**
 * davinci_mcbsp_start_rx: Start the receiver
 * @dev: McBSP device pointer
 *
 * Start the McBSP receiver
 */
void davinci_mcbsp_start_rx(struct davinci_mcbsp_dev *dev)
{
	u32 val;

	/* wait for some clock cycles */
	udelay(100);

	/* start the sample rate generator */
	if (dev->master) {
		dev_dbg(dev->dev, "Configuring RX in master mode\n");
		val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val | GRST);
	}

	/* wait for some clock cycles */
	udelay(100);

	if (dev->op_mode & DAVINCI_MCBSP_IRQ_MODE)
		davinci_mcbsp_trigger_rx(dev);

	/* DMA related - when transmission begins */
	/* The remaining stpes are in the trigger fucntions */
}
EXPORT_SYMBOL(davinci_mcbsp_start_rx);

/**
 * davinci_mcbsp_stop_tx: Stop the transmitter
 * @dev: McBSP device pointer
 *
 * Stop the transmitter
 */
void davinci_mcbsp_stop_tx(struct davinci_mcbsp_dev *dev)
{
	u32 val;

	do {
		val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	} while (val & XEMPTY);

	/* reset frame sync generator */
	val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	if (val & FRST) {
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val & ~FRST);
		val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	}

	/* reset transmitter */
	davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val & ~XRST);

	if (dev->tx_params.lch != -1)
		edma_clean_channel(dev->tx_params.lch);

	dev->tx_params.lch = -1;
}
EXPORT_SYMBOL(davinci_mcbsp_stop_tx);

/**
 * davinci_mcbsp_stop_rx: Stop the receiver
 * @dev: McBSP device pointer
 *
 * Stop the receiver
 */
void davinci_mcbsp_stop_rx(struct davinci_mcbsp_dev *dev)
{
	u32 val;

	/* reset frame sync generator */
	val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	if (val & FRST) {
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val & ~FRST);
		val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
	}

	/* reset receiver */
	davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val & ~RRST);

	if (dev->master) {
		/* reset sample rate generator */
		val = davinci_mcbsp_read_reg(dev, MCBSP_SPCR);
		davinci_mcbsp_write_reg(dev, MCBSP_SPCR, val & ~GRST);
	}

	if (dev->rx_params.lch != -1)
		edma_clean_channel(dev->rx_params.lch);

	dev->rx_params.lch = -1;
}
EXPORT_SYMBOL(davinci_mcbsp_stop_rx);

/**
 * davinci_mcbsp_request: Request for a McBSP instance
 * @id: instance number
 * @pdev: pointer to McBSP device pointer
 *
 * Called to request for a McBSP device, this gives a pointer to the device
 * which is needed to set the parameters
 */
int davinci_mcbsp_request(unsigned int id, struct davinci_mcbsp_dev **pdev)
{
	u32 val;

	/* Check if the id is valid */
	if (id >= NUM_MCBSP_INSTANCES) {
		printk(KERN_ERR "Invalid id\n");
		return -ENODEV;
	}

	spin_lock(&mcbsp[id]->lock);
	if (!mcbsp[id]->free) {
		dev_err(mcbsp[id]->dev, "McBSP%d is in use\n", id);
		spin_unlock(&mcbsp[id]->lock);
		return -EBUSY;
	}

	mcbsp[id]->free = 0;
	spin_unlock(&mcbsp[id]->lock);

	/* Ensure the transmitter, reciever, sample rate generator
	   are in reset */
	val = davinci_mcbsp_read_reg(mcbsp[id], MCBSP_SPCR);
	val &= ~(FRST | GRST | XRST | RRST);
	davinci_mcbsp_write_reg(mcbsp[id], MCBSP_SPCR, val);

	/* Add this later when adding request_irq */
	printk(KERN_INFO "Request for McBSP%d sucessful\n", id);
	*pdev = mcbsp[id];

	return 0;
}
EXPORT_SYMBOL(davinci_mcbsp_request);

/**
 * davinci_mcbsp_free: Free a McBSP device
 * @id: instance number
 *
 * Fucntion used to free a McBSP device
 */
int davinci_mcbsp_free(unsigned int id)
{
	/* Check if the id is valid */
	if (id >= NUM_MCBSP_INSTANCES) {
		printk(KERN_ERR "Invalid id\n");
		return -ENODEV;
	}

	spin_lock(&mcbsp[id]->lock);
	if (mcbsp[id]->free) {
		dev_err(mcbsp[id]->dev, "McBSP%d is free\n", id);
		spin_unlock(&mcbsp[id]->lock);
		return -EINVAL;
	}

	mcbsp[id]->free = 1;
	spin_unlock(&mcbsp[id]->lock);

	return 0;
}
EXPORT_SYMBOL(davinci_mcbsp_free);

/**
 * davinci_mcbsp_probe: McbSP probe fucntion
 * @pdev: Platfrom device for McBSP
 *
 * Called by the platform specific code. Used to allocate resouces, request
 * interrupts, initialize the device states and data structures. It
 * also enables the clock.
 */
static int __devinit davinci_mcbsp_probe(struct platform_device *pdev)
{
	struct davinci_mcbsp_platform_data *pdata;
	struct clk *mcbsp_clk;
	struct resource *res;
	unsigned long size;
	int inst, ret = 0;
	unsigned long base_addr;

	/* Check platform data */
	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "No platform data for McBSP\n");
		return -EINVAL;
	}

	/* Clock get */
	mcbsp_clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(mcbsp_clk)) {
		dev_err(&pdev->dev,
				"Clock configuration for McBSP is invalid\n");
		return -EINVAL;
	}

	/* Alloc mem for private data struct */
	inst = pdev->id;
	if (inst >= NUM_MCBSP_INSTANCES) {
		dev_err(&pdev->dev, "Platform data has invalid id\n");
		ret = -EINVAL;
		goto invalid_inst_exit;
	}

	mcbsp[inst] = kzalloc(sizeof(struct davinci_mcbsp_dev), GFP_KERNEL);
	if (!mcbsp[inst]) {
		ret = -ENOMEM;
		goto mcbsp_alloc_err_exit;
	}

	/* init spin locks */
	platform_set_drvdata(pdev, mcbsp[inst]);
	spin_lock_init(&mcbsp[inst]->lock);

	mcbsp[inst]->id = inst;
	mcbsp[inst]->free = 1;
	mcbsp[inst]->tx_params.lch = -1;
	mcbsp[inst]->rx_params.lch = -1;
	mcbsp[inst]->clk = mcbsp_clk;
	mcbsp[inst]->op_mode = DAVINCI_MCBSP_DMA_MODE;

	/* Resources */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "Error getting resource\n");
		goto res_fail_exit;
	}

	size = resource_size(res);
	base_addr = res->start;
	if (!request_mem_region(res->start, size, pdev->name)) {
		dev_err(&pdev->dev, "Error requesting module memory region\n");
		ret = -ENXIO;
		goto res_fail_exit;
	}

	/* ioremap */
	mcbsp[inst]->io_base = ioremap_nocache(res->start, size);
	if (!mcbsp[inst]->io_base) {
		ret = -ENOMEM;
		goto ioremap_fail_exit;
	}

	/* FIFO data resource */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!res) {
		dev_err(&pdev->dev, "Error getting resource\n");
		goto ioremap_fail_exit;
	}

	mcbsp[inst]->fifo_base = res->start;
	mcbsp[inst]->fifo_size = res->end - res->start + 1;
	if (!request_mem_region(res->start, mcbsp[inst]->fifo_size,
							pdev->name)) {
		dev_err(&pdev->dev, "Error requesting FIFO data region\n");
		ret = -ENXIO;
		goto fifo_fail_exit;
	}

	/* DMA resources */
	res = platform_get_resource(pdev, IORESOURCE_DMA, 0);
	if (!res) {
		dev_err(&pdev->dev, "No DMA resource\n");
		ret = -ENXIO;
		goto dma_res_fail;
	}

	mcbsp[inst]->rx_params.dma_ch = res->start;
	mcbsp[inst]->rx_params.dma_addr = (dma_addr_t)
					(base_addr + MCBSP_DRR);
	res = platform_get_resource(pdev, IORESOURCE_DMA, 1);
	if (!res) {
		dev_err(&pdev->dev, "No DMA resource\n");
		ret = -ENXIO;
		goto dma_res_fail;
	}

	mcbsp[inst]->tx_params.dma_ch = res->start;
	mcbsp[inst]->tx_params.dma_addr = (dma_addr_t)
					(base_addr + MCBSP_DXR);
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(&pdev->dev, "No IRQ resource\n");
		ret = -ENXIO;
		goto dma_res_fail;
	}

	mcbsp[inst]->rx_params.irq = res->start;
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
	if (!res) {
		dev_err(&pdev->dev, "No IRQ resource\n");
		ret = -ENXIO;
		goto dma_res_fail;
	}

	mcbsp[inst]->tx_params.irq = res->start;

	/* Interrupts */
	init_completion(&mcbsp[inst]->rx_params.irq_completion);
	ret = request_irq(mcbsp[inst]->rx_params.irq,
				davinci_mcbsp_rx_irq_handler,
				0, "McBSP_rx", (void *)mcbsp[inst]);
	if (ret != 0) {
		dev_err(&pdev->dev,
				"Unable to request TX IRQ %d for McBSP%d\n",
					mcbsp[inst]->tx_params.irq, inst);
		goto dma_res_fail;
	}

	init_completion(&mcbsp[inst]->tx_params.irq_completion);
	ret = request_irq(mcbsp[inst]->tx_params.irq,
				davinci_mcbsp_tx_irq_handler,
				0, "McBSP", (void *)mcbsp[inst]);
	if (ret != 0) {
		dev_err(&pdev->dev,
				"Unable to request TX IRQ %d for McBSP%d\n",
					mcbsp[inst]->tx_params.irq, inst);
		goto irq_fail;
	}

	/* Enable clock */
	clk_enable(mcbsp[inst]->clk);
	mcbsp[inst]->pdata = pdata;
	mcbsp[inst]->dev = &pdev->dev;
	platform_set_drvdata(pdev, mcbsp[inst]);

	printk(KERN_INFO "McBSP:Probed McBSP%d\n", inst);

	return 0;

irq_fail:
	free_irq(mcbsp[inst]->rx_params.irq, mcbsp[inst]);
dma_res_fail:
	release_mem_region(mcbsp[inst]->fifo_base, mcbsp[inst]->fifo_size);
fifo_fail_exit:
	iounmap(mcbsp[inst]->io_base);
ioremap_fail_exit:
	release_mem_region(base_addr, size);
res_fail_exit:
	kfree(mcbsp[inst]);
mcbsp_alloc_err_exit:
invalid_inst_exit:
	clk_put(mcbsp_clk);

	return ret;
}
/**
 * davinci_mcbsp_remove: McBSP remove fucntion
 * @dev: Platfrom device for McBSP
 *
 * Called when removing the device. Frees all the resources,
 * disables the clock.
 */
static int __devexit davinci_mcbsp_remove(struct platform_device *pdev)
{
	struct davinci_mcbsp_dev *dev = platform_get_drvdata(pdev);
	struct resource *res;

	platform_set_drvdata(pdev, NULL);
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (dev) {
		clk_disable(dev->clk);
		clk_put(dev->clk);
		iounmap(dev->io_base);
		release_mem_region(res->start, res->end - res->start + 1);
		res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
		release_mem_region(res->start, res->end - res->start + 1);
		free_irq(dev->tx_params.irq, dev);
		free_irq(dev->rx_params.irq, dev);
	}

	kfree(dev);

	return 0;
}

static struct platform_driver davinci_mcbsp_driver = {
	.probe		= davinci_mcbsp_probe,
	.remove		= __devexit_p(davinci_mcbsp_remove),
	.driver		= {
		.name	= "davinci-mcbsp",
	},
};

/**
 * davinci_mcbsp_init: Modue init
 *
 */
static int __init davinci_mcbsp_init(void)
{
	return platform_driver_register(&davinci_mcbsp_driver);
}

/**
 * davinci_mcbsp_exit: Module exit
 *
 */
static void __exit davinci_mcbsp_exit(void)
{
	return platform_driver_unregister(&davinci_mcbsp_driver);
}

module_init(davinci_mcbsp_init);
module_exit(davinci_mcbsp_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chaithrika U S <chaithrika@ti.com>");
MODULE_DESCRIPTION("DAVINCI/OMAP-L138 driver");
