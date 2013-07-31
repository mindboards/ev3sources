/*
 * mcbsp definitions
 *
 * McBSP- Multi-channel Buffer Serial Port peripheral can be used
 * to transfer data {module description}
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

#ifndef MCBSP_H_
#define MCBSP_H_

/* Bit masks */
#define DAVINCI_MCBSP_RX_MODE	0x01
#define DAVINCI_MCBSP_TX_MODE	0x02
#define DAVINCI_MCBSP_DMA_MODE	0x01
#define DAVINCI_MCBSP_IRQ_MODE	0x02
#define DAVINCI_MCBSP_DLB_MODE	0x04

#define DAVINCI_MCBSP_MC_XMCM1	0x01
#define DAVINCI_MCBSP_MC_XMCM2	0x02
#define DAVINCI_MCBSP_MC_XMCM3	0x03
#define DAVINCI_MCBSP_MC_XMCME	0x08

#define DAVINCI_MCBSP_MC_RMCM1	0x01
#define DAVINCI_MCBSP_MC_RMCME	0x08

/* Platfrom data */
struct davinci_mcbsp_platform_data {
	u32 inst;
};

struct davinci_mcbsp_params {
	u8 word_length1;
	u8 word_length2;
	u8 frame_length1;
	u8 frame_length2;
	u8 phase;
	u8 compand;
	u8 frame_ignore;
	u8 data_delay;
	u8 word_rev;
	u8 intr_mode;
	u8 buf_fmt;

	/* FIFO */
	u8 numevt;
	u8 numdma;

	/* multichannel mode */
	u8 mc_mode;
	u8 pa_blk;
	u8 pb_blk;
	u8 max_pa;
	u8 max_pb;
	u16 cer[8];

	int irq;
	int lch;
	int dma_ch;
	dma_addr_t dma_addr;

	/* completion queues */
	struct completion irq_completion;
	struct completion dma_completion;
};

struct davinci_mcbsp_dev {
	struct davinci_mcbsp_params rx_params;
	struct davinci_mcbsp_params tx_params;

	u32 fifo_base;
	struct clk *clk;
	struct device *dev;
	void __iomem *io_base;
	unsigned long fifo_size;

	u8 dxena;

	u8 id;
	u8 free;
	u8 master;
	u8 op_mode;
	spinlock_t lock;
	struct davinci_mcbsp_platform_data *pdata;
};

void davinci_mcbsp_config_params(struct davinci_mcbsp_dev *dev, u8 master);
void davinci_mcbsp_config_clock(struct davinci_mcbsp_dev *dev,
							unsigned int freq);
int davinci_mcbsp_xmit_buf(struct davinci_mcbsp_dev *dev, dma_addr_t buf,
							unsigned int length);
int davinci_mcbsp_recv_buf(struct davinci_mcbsp_dev *dev, dma_addr_t buf,
							unsigned int length);
void davinci_mcbsp_start_tx(struct davinci_mcbsp_dev *dev);
void davinci_mcbsp_start_rx(struct davinci_mcbsp_dev *dev);
void davinci_mcbsp_stop_tx(struct davinci_mcbsp_dev *dev);
void davinci_mcbsp_stop_rx(struct davinci_mcbsp_dev *dev);
int davinci_mcbsp_request(unsigned int id, struct davinci_mcbsp_dev **pdev);
int davinci_mcbsp_free(unsigned int id);
void davinci_mcbsp_dump_reg(struct davinci_mcbsp_dev *dev);
void davinci_mcbsp_xmit_word(struct davinci_mcbsp_dev *dev, u32 word);
u32 davinci_mcbsp_recv_word(struct davinci_mcbsp_dev *dev);
void davinci_mcbsp_config_fifo(struct davinci_mcbsp_dev *dev);
void davinci_mcbsp_config_multichannel_mode(struct davinci_mcbsp_dev *dev);

#endif

