/*
 *  TI OMAPL PRU CAN Emulation device driver
 *  Author: subhasish@mistralsolutions.com
 * 
 *  This driver supports TI's PRU CAN Emulation and the
 *  specs for the same is available at <http://www.ti.com>
 * 
 *  Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 * 
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation version 2.
 * 
 *  This program is distributed as is WITHOUT ANY WARRANTY of any
 *  kind, whether express or implied; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/platform_device.h>
#include <linux/firmware.h>
#include <linux/clk.h>

#include <linux/can.h>
#include <linux/can/dev.h>
#include <linux/can/error.h>
#include <mach/da8xx.h>
#include <pru_can_emulation_api.h>

#define DRV_NAME "ti_omapl_pru_can"
#define DRV_DESC "TI PRU CAN Controller Driver v0.1"
#define PRU_CAN_START	1
#define PRU_CAN_STOP	0
#define MB_MIN			0
#define MB_MAX			7
#define CAN_RX_PRU_0			CSL_PRUCORE_0
#define CAN_TX_PRU_1			CSL_PRUCORE_1

#define PRU_CANMID_IDE			BIT(29)	/* Extended frame format */

#define PRU_CAN_ISR_BIT_CCI		BIT(15)
#define PRU_CAN_ISR_BIT_ESI		BIT(14)
#define PRU_CAN_ISR_BIT_SRDI	BIT(13)
#define PRU_CAN_ISR_BIT_RRI		BIT(8)

#define PRU_CAN_MBXSR_BIT_STATE		BIT(7)
#define PRU_CAN_MBXSR_BIT_TC		BIT(6)
#define PRU_CAN_MBXSR_BIT_ERR		BIT(5)
#define PRU_CAN_MBXSR_BIT_OF		BIT(0)

#define PRU_CAN_GSR_BIT_TXM		BIT(7)
#define PRU_CAN_GSR_BIT_RXM		BIT(6)
#define PRU_CAN_GSR_BIT_CM		BIT(5)
#define PRU_CAN_GSR_BIT_EPM		BIT(4)
#define PRU_CAN_GSR_BIT_BFM		BIT(3)
#define RTR_MBX_NO 	8
#define MAX_INIT_RETRIES	20
#define L138_PRU_ARM_FREQ		312000
#define DFLT_PRU_FREQ			156000000
#define DFLT_PRU_BITRATE		125000

#ifdef __CAN_DEBUG
#define __can_debug(fmt, args...) printk(KERN_DEBUG "can_debug: " fmt, ## args)
#else
#define __can_debug(fmt, args...)
#endif
#define __can_err(fmt, args...) printk(KERN_ERR "can_err: " fmt, ## args)

/*
 * omapl_pru can private data
 */
struct omapl_pru_can_priv {
	struct can_priv can;	/* must be the first member,can_priv = netdev_priv(ndev) */
	struct workqueue_struct *pru_can_wQ;
	struct work_struct rx_work;
	struct net_device *ndev;
	struct clk *clk;
	struct clk *clk_timer;
	u32 timer_freq;
	arm_pru_iomap pru_arm_iomap;
	can_emulation_app_hndl can_tx_hndl;
	can_emulation_app_hndl can_rx_hndl;
	const struct firmware *fw_rx;
	const struct firmware *fw_tx;
	spinlock_t mbox_lock;
	u32 trx_irq;
	u32 tx_head;
	u32 tx_tail;
	u32 tx_next;
	u32 rx_next;
};

static int omapl_pru_can_get_state(const struct net_device *ndev,
				   enum can_state *state)
{
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);
	*state = priv->can.state;
	return 0;
}

static int omapl_pru_can_set_bittiming(struct net_device *dev)
{
	struct omapl_pru_can_priv *priv = netdev_priv(dev);
	struct can_bittiming *bt = &priv->can.bittiming;
	long bit_error = 0;

	if (priv->can.ctrlmode & CAN_CTRLMODE_3_SAMPLES) {
		dev_warn(priv->ndev->dev.parent, "WARN: Triple"
			 "sampling not set due to h/w limitations");
	}
	if (pru_can_calculatetiming
	    (priv->can.clock.freq, bt->bitrate) != 0) {
		return -EINVAL;
	}
	bit_error = (((priv->timer_freq/(priv->timer_freq / bt->bitrate)) - bt->bitrate) * 1000) / bt->bitrate;
	if(bit_error){
		bit_error = (((priv->timer_freq/(priv->timer_freq / bt->bitrate)) - bt->bitrate) * 1000000) / bt->bitrate;
		printk("\nBitrate error %ld.%ld%%\n", bit_error/10000, bit_error % 1000);
	} else
		printk("\nBitrate error 0.0%%\n");

	return 0;
}

static void omapl_pru_can_stop(struct net_device *ndev)
{
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);
	u16 int_mask = 0;

	pru_can_mask_ints(int_mask);	/* mask all ints */
	pru_can_start_or_abort_transmission(PRU_CAN_STOP);
	priv->can.state = CAN_STATE_STOPPED;
}

/* 
 * This is to just set the can state to ERROR_ACTIVE 
 *	ip link set canX up type can bitrate 125000
 */
static void omapl_pru_can_start(struct net_device *ndev)
{
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);
	u16 int_mask = 0xFFFF;

	if (priv->can.state != CAN_STATE_STOPPED)
		omapl_pru_can_stop(ndev);

	pru_can_mask_ints(int_mask);	/* unmask all ints */

	pru_can_get_global_status(&priv->can_tx_hndl);
	pru_can_get_global_status(&priv->can_rx_hndl);

	if (PRU_CAN_GSR_BIT_EPM & priv->can_tx_hndl.u32globalstatus)
		priv->can.state = CAN_STATE_ERROR_PASSIVE;
	else if (PRU_CAN_GSR_BIT_BFM & priv->can_tx_hndl.u32globalstatus)
		priv->can.state = CAN_STATE_BUS_OFF;
	else
		priv->can.state = CAN_STATE_ERROR_ACTIVE;
}

static int omapl_pru_can_set_mode(struct net_device *ndev, enum can_mode mode)
{
	int ret = 0;

	switch (mode) {
	case CAN_MODE_START:
		omapl_pru_can_start(ndev);
		if (netif_queue_stopped(ndev))
			netif_wake_queue(ndev);
		break;
	case CAN_MODE_STOP:
		omapl_pru_can_stop(ndev);
		if (!netif_queue_stopped(ndev))
			netif_stop_queue(ndev);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}
	return ret;
}

static netdev_tx_t omapl_pru_can_start_xmit(struct sk_buff *skb,
					    struct net_device *ndev)
{
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);
	struct can_frame *cf = (struct can_frame *)skb->data;
	int count;
	u8 *data = cf->data;
	u8 dlc = cf->can_dlc;
	u8 *ptr8data = NULL;

	netif_stop_queue(ndev);
	if (cf->can_id & CAN_EFF_FLAG)	/* Extended frame format */
		*((u32 *) & priv->can_tx_hndl.strcanmailbox) =
		    (cf->can_id & CAN_EFF_MASK) | PRU_CANMID_IDE;
	else			/* Standard frame format */
		*((u32 *) & priv->can_tx_hndl.strcanmailbox) =
		    (cf->can_id & CAN_SFF_MASK) << 18;

	if (cf->can_id & CAN_RTR_FLAG)	/* Remote transmission request */
		*((u32 *) & priv->can_tx_hndl.strcanmailbox) |= CAN_RTR_FLAG;

	ptr8data = &priv->can_tx_hndl.strcanmailbox.u8data7 + (dlc - 1);
	for (count = 0; count < (u8) dlc; count++) {
		*ptr8data-- = *data++;
	}
	*((u32 *) & priv->can_tx_hndl.strcanmailbox.u16datalength) = (u32) dlc;
/* 
 * search for the next available mbx 
 * if the next mbx is busy, then try the next + 1
 * do this until the head is reached.
 * if still unable to tx, stop accepting any packets
 * if able to tx and the head is reached, then reset next to tail, i.e mbx0
 * if head is not reached, then just point to the next mbx
 */
	for (; priv->tx_next <= priv->tx_head; priv->tx_next++) {
		priv->can_tx_hndl.ecanmailboxnumber =
		    (can_mailbox_number) priv->tx_next;
		if (-1 == pru_can_write_data_to_mailbox(&priv->can_tx_hndl)) {
			if (priv->tx_next == priv->tx_head) {
				priv->tx_next = priv->tx_tail;
				if (!netif_queue_stopped(ndev))
					netif_stop_queue(ndev);	/* IF stalled */
				dev_err(priv->ndev->dev.parent,
					"%s: no tx mbx available", __func__);
				return NETDEV_TX_BUSY;
			} else
				continue;
		} else {
			/* set transmit request */
			pru_can_transfer(priv->tx_next, CAN_TX_PRU_1);
			pru_can_transfer_mode_set(false, ecanreceive);
			pru_can_transfer_mode_set(true, ecantransmit);
			pru_can_start_or_abort_transmission(PRU_CAN_START);
			priv->tx_next++;
			can_put_echo_skb(skb, ndev, 0);
			break;
		}
	}
	if (priv->tx_next > priv->tx_head) {
		priv->tx_next = priv->tx_tail;
	}
	return NETDEV_TX_OK;
}

static int omapl_pru_can_rx(struct net_device *ndev, u32 mbxno)
{
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);
	struct net_device_stats *stats = &ndev->stats;
	struct can_frame *cf;
	struct sk_buff *skb;
	u32 pru_can_mbx_data;
	u8 *data = NULL;
	u8 *ptr8data = NULL;
	int count = 0;

	skb = alloc_can_skb(ndev, &cf);
	if (!skb) {
		if (printk_ratelimit())
			dev_err(priv->ndev->dev.parent,
				"ti_pru_can_rx_pkt: alloc_can_skb() failed\n");
		return -ENOMEM;
	}
	data = cf->data;
	/*      get payload */
	priv->can_rx_hndl.ecanmailboxnumber = (can_mailbox_number) mbxno;
	if (pru_can_get_data_from_mailbox(&priv->can_rx_hndl)) {
		__can_err("pru_can_get_data_from_mailbox: failed\n");
		return EAGAIN;
	}
	/* give ownweship to pru */
	pru_can_transfer(mbxno, CAN_RX_PRU_0);

	/* get data length code */
	cf->can_dlc =
	    get_can_dlc(*
			((u32 *) & priv->can_rx_hndl.strcanmailbox.
			 u16datalength) & 0xF);
	if (cf->can_dlc <= 4) {
		ptr8data =
		    &priv->can_rx_hndl.strcanmailbox.u8data3 + (4 -
								cf->can_dlc);
		for (count = 0; count < cf->can_dlc; count++) {
			*data++ = *ptr8data++;
		}
	} else {
		ptr8data = &priv->can_rx_hndl.strcanmailbox.u8data3;
		for (count = 0; count < 4; count++) {
			*data++ = *ptr8data++;
		}
		ptr8data =
		    &priv->can_rx_hndl.strcanmailbox.u8data4 - (cf->can_dlc -
								5);
		for (count = 0; count < cf->can_dlc - 4; count++) {
			*data++ = *ptr8data++;
		}
	}

	pru_can_mbx_data = *((u32 *) & priv->can_rx_hndl.strcanmailbox);
	/* get id extended or std */
	if (pru_can_mbx_data & PRU_CANMID_IDE)
		cf->can_id = (pru_can_mbx_data & CAN_EFF_MASK) | CAN_EFF_FLAG;
	else
		cf->can_id = (pru_can_mbx_data >> 18) & CAN_SFF_MASK;

	if (pru_can_mbx_data & CAN_RTR_FLAG)
		cf->can_id |= CAN_RTR_FLAG;

	netif_rx_ni(skb);
	stats->rx_packets++;
	stats->rx_bytes += cf->can_dlc;
	return 0;
}

static int omapl_pru_can_err(struct net_device *ndev, int int_status,
			     int err_status)
{
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);
	struct net_device_stats *stats = &ndev->stats;
	struct can_frame *cf;
	struct sk_buff *skb;
	int tx_err_cnt, rx_err_cnt;

	/* propogate the error condition to the can stack */
	skb = alloc_can_err_skb(ndev, &cf);
	if (!skb) {
		if (printk_ratelimit())
			dev_err(priv->ndev->dev.parent,
				"omapl_pru_can_err: alloc_can_err_skb() failed\n");
		return -ENOMEM;
	}

	if (err_status & PRU_CAN_GSR_BIT_EPM) {	/* error passive int */
		priv->can.state = CAN_STATE_ERROR_PASSIVE;
		++priv->can.can_stats.error_passive;
		cf->can_id |= CAN_ERR_CRTL;
		tx_err_cnt = pru_can_get_error_cnt(CAN_TX_PRU_1);
		rx_err_cnt = pru_can_get_error_cnt(CAN_RX_PRU_0);
		if (tx_err_cnt > 127)
			cf->data[1] |= CAN_ERR_CRTL_TX_PASSIVE;
		if (rx_err_cnt > 127)
			cf->data[1] |= CAN_ERR_CRTL_RX_PASSIVE;

		dev_dbg(priv->ndev->dev.parent, "Error passive interrupt\n");
	}

	if (err_status & PRU_CAN_GSR_BIT_BFM) {
		priv->can.state = CAN_STATE_BUS_OFF;
		cf->can_id |= CAN_ERR_BUSOFF;
		/*      
		 *      Disable all interrupts in bus-off to avoid int hog 
		 *      this should be handled by the pru       
		 */
		pru_can_mask_ints(0xFFFF);
		can_bus_off(ndev);
		dev_dbg(priv->ndev->dev.parent, "Bus off mode\n");
	}

	netif_rx(skb);
	stats->rx_packets++;
	stats->rx_bytes += cf->can_dlc;
	return 0;
}

void omapl_pru_can_rx_wQ(struct work_struct *work)
{
	struct omapl_pru_can_priv *priv = container_of(work,
						       struct
						       omapl_pru_can_priv,
						       rx_work);
	struct net_device *ndev = priv->ndev;
	u32 bit_set, mbxno = 0;

	if (-1 == pru_can_get_interrupt_status(&priv->can_rx_hndl))
		return;

	if (PRU_CAN_ISR_BIT_RRI & priv->can_rx_hndl.u32interruptstatus) {
		mbxno = RTR_MBX_NO;
		omapl_pru_can_rx(ndev, mbxno);
	} else {
		/* Extract the mboxno from the status */
		for (bit_set = 0; ((priv->can_rx_hndl.u32interruptstatus
				    & 0xFF) >> bit_set != 0); bit_set++) ;
		if (0 == bit_set) {
			dev_err(ndev->dev.parent,
				"%s: invalid mailbox number: %X\n", __func__,
				priv->can_rx_hndl.u32interruptstatus);
		} else {
			mbxno = bit_set - 1;	/* mail box numbering starts from 0 */
			if (PRU_CAN_ISR_BIT_ESI & priv->can_rx_hndl.
			    u32interruptstatus) {
				pru_can_get_global_status(&priv->can_rx_hndl);
				omapl_pru_can_err(ndev,
						  priv->can_rx_hndl.
						  u32interruptstatus,
						  priv->can_rx_hndl.
						  u32globalstatus);
			} else {
				omapl_pru_can_rx(ndev, mbxno);
			}
		}
	}
}

irqreturn_t omapl_tx_can_intr(int irq, void *dev_id)
{
	struct net_device *ndev = dev_id;
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);
	struct net_device_stats *stats = &ndev->stats;
	u32 bit_set, mbxno;

	pru_can_get_interrupt_status(&priv->can_tx_hndl);
	if ((PRU_CAN_ISR_BIT_CCI & priv->can_tx_hndl.u32interruptstatus)
	    || (PRU_CAN_ISR_BIT_SRDI & priv->can_tx_hndl.u32interruptstatus)) {
		__can_debug("tx_int_status = 0x%X\n",
			    priv->can_tx_hndl.u32interruptstatus);
		can_free_echo_skb(ndev, 0);
	} else {
		for (bit_set = 0; ((priv->can_tx_hndl.u32interruptstatus
				    & 0xFF) >> bit_set != 0); bit_set++) ;
		if (0 == bit_set) {
			__can_err("%s: invalid mailbox number\n", __func__);
			can_free_echo_skb(ndev, 0);
		} else {
			mbxno = bit_set - 1;	/* mail box numbering starts from 0 */
			if (PRU_CAN_ISR_BIT_ESI & priv->can_tx_hndl.
			    u32interruptstatus) {
				/* read gsr and ack pru */
				pru_can_get_global_status(&priv->can_tx_hndl);
				omapl_pru_can_err(ndev,
						  priv->can_tx_hndl.
						  u32interruptstatus,
						  priv->can_tx_hndl.
						  u32globalstatus);
			} else {
				stats->tx_packets++;
				/* stats->tx_bytes += dlc; */
				//can_//get_echo_skb(ndev, 0);
			}
		}
	}
	if (netif_queue_stopped(ndev))
		netif_wake_queue(ndev);

	can_get_echo_skb(ndev, 0);
	pru_can_transfer_mode_set(true, ecanreceive);
	return IRQ_HANDLED;
}

irqreturn_t omapl_rx_can_intr(int irq, void *dev_id)
{

	struct net_device *ndev = dev_id;
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);
	u32 intc_status = 0;

	intc_status = pru_can_intc_status_get();
	if(intc_status & 4){
		return omapl_tx_can_intr(irq, dev_id);
	}
	if(intc_status & 2){
		if (!work_pending(&priv->rx_work))
            queue_work(priv->pru_can_wQ, &priv->rx_work);
	}

	return IRQ_HANDLED;
}

static int omapl_pru_can_open(struct net_device *ndev)
{
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);
	int err;

	/* register interrupt handler */
	err = request_irq(priv->trx_irq, &omapl_rx_can_intr, IRQF_SHARED,
			  "pru_can_irq", ndev);
	if (err) {
		dev_err(ndev->dev.parent, "error requesting rx interrupt\n");
		goto exit_trx_irq;
	}
	/* common open */
	err = open_candev(ndev);
	if (err) {
		dev_err(ndev->dev.parent, "open_candev() failed %d\n", err);
		goto exit_open;
	}

	clk_enable(priv->clk);

	pru_can_emulation_init(&priv->pru_arm_iomap, priv->can.clock.freq);
	priv->tx_tail = MB_MIN;
	priv->tx_head = MB_MAX;

	pru_can_receive_id_map(CONFIG_OMAPL_PRU_CANID_MBX0, 0);
	pru_can_receive_id_map(CONFIG_OMAPL_PRU_CANID_MBX1, 1);
	pru_can_receive_id_map(CONFIG_OMAPL_PRU_CANID_MBX2, 2);
	pru_can_receive_id_map(CONFIG_OMAPL_PRU_CANID_MBX3, 3);
	pru_can_receive_id_map(CONFIG_OMAPL_PRU_CANID_MBX4, 4);
	pru_can_receive_id_map(CONFIG_OMAPL_PRU_CANID_MBX5, 5);
	pru_can_receive_id_map(CONFIG_OMAPL_PRU_CANID_MBX6, 6);
	pru_can_receive_id_map(CONFIG_OMAPL_PRU_CANID_MBX7, 7);

	omapl_pru_can_start(ndev);
	netif_start_queue(ndev);
	return 0;

exit_open:
	free_irq(priv->trx_irq, ndev);
exit_trx_irq:
	return err;
}

static int omapl_pru_can_close(struct net_device *ndev)
{
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);

	if (!netif_queue_stopped(ndev))
		netif_stop_queue(ndev);

	close_candev(ndev);

	free_irq(priv->trx_irq, ndev);
	clk_disable(priv->clk);

	return 0;
}

static const struct net_device_ops omapl_pru_can_netdev_ops = {
	.ndo_open = omapl_pru_can_open,
	.ndo_stop = omapl_pru_can_close,
	.ndo_start_xmit = omapl_pru_can_start_xmit,
};

static int __devinit omapl_pru_can_probe(struct platform_device *pdev)
{
	struct net_device *ndev = NULL;
	struct omapl_pru_can_priv *priv = NULL;
	struct ti_pru_can_platform_data *pdata;
	struct resource *res_mem, *trx_irq;
	pru_can_firmware_structure fw_pru;
	u32 err, retries = 0;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "no platform data provided for pru!\n");
		return -ENODEV;
	}

	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res_mem) {
		dev_err(&pdev->dev, "unable to get pru memory resources!\n");
		err = -ENODEV;
		goto probe_exit;
	}

	trx_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!trx_irq) {
		dev_err(&pdev->dev, "unable to get pru interrupt resources!\n");
		err = -ENODEV;
		goto probe_exit;
	}

	if (!request_mem_region(res_mem->start, resource_size(res_mem),
				dev_name(&pdev->dev))) {
		dev_err(&pdev->dev, "pru memory region already claimed!\n");
		err = -EBUSY;
		goto probe_exit;
	}

	ndev = alloc_candev(sizeof(struct omapl_pru_can_priv), MB_MAX + 1);
	if (!ndev) {
		dev_err(&pdev->dev, "alloc_candev failed\n");
		err = -ENOMEM;
		goto probe_exit_free_region;
	}
	priv = netdev_priv(ndev);

	priv->pru_arm_iomap.pru_io_addr =
	    ioremap(res_mem->start, resource_size(res_mem));
	if (!priv->pru_arm_iomap.pru_io_addr) {
		dev_err(&pdev->dev, "ioremap failed\n");
		err = -ENOMEM;
		goto probe_exit_free_region;
	}

	priv->pru_arm_iomap.psc0_io_addr = IO_ADDRESS(DA8XX_PSC0_BASE);
	if (!priv->pru_arm_iomap.psc0_io_addr) {
		dev_err(&pdev->dev, "ioremap failed\n");
		err = -ENOMEM;
		goto probe_exit_iounmap;
	}

	priv->pru_arm_iomap.psc1_io_addr = IO_ADDRESS(DA8XX_PSC1_BASE);
	if (!priv->pru_arm_iomap.psc1_io_addr) {
		dev_err(&pdev->dev, "ioremap failed\n");
		err = -ENOMEM;
		goto probe_exit_iounmap;
	}

	priv->pru_arm_iomap.syscfg_io_addr = IO_ADDRESS(DA8XX_SYSCFG0_BASE);
	if (!priv->pru_arm_iomap.syscfg_io_addr) {
		dev_err(&pdev->dev, "ioremap failed\n");
		err = -ENOMEM;
		goto probe_exit_iounmap;
	}

	priv->ndev = ndev;
	priv->trx_irq = trx_irq->start;

	priv->can.bittiming_const = NULL;
	priv->can.do_set_bittiming = omapl_pru_can_set_bittiming;
	priv->can.do_set_mode = omapl_pru_can_set_mode;
	priv->can.do_get_state = omapl_pru_can_get_state;
	priv->can_tx_hndl.u8prunumber = CAN_TX_PRU_1;
	priv->can_rx_hndl.u8prunumber = CAN_RX_PRU_0;

	ndev->flags |= (IFF_ECHO | IFF_NOARP);	/* we support local echo, no arp */
	platform_set_drvdata(pdev, ndev);
	SET_NETDEV_DEV(ndev, &pdev->dev);
	ndev->netdev_ops = &omapl_pru_can_netdev_ops;

	priv->clk = clk_get(&pdev->dev, "pru_ck");
	if (IS_ERR(priv->clk)) {
		dev_err(&pdev->dev, "no clock available\n");
		err = PTR_ERR(priv->clk);
		priv->clk = NULL;
		goto probe_exit_candev;
	}
	priv->can.clock.freq = clk_get_rate(priv->clk);

	priv->clk_timer = clk_get(&pdev->dev, "pll1_sysclk2");
	if (IS_ERR(priv->clk_timer)) {
		dev_err(&pdev->dev, "no timer clock available\n");
		err = PTR_ERR(priv->clk_timer);
		priv->clk_timer = NULL;
		goto probe_exit_candev;
	}
	priv->timer_freq = clk_get_rate(priv->clk_timer);

	err = register_candev(ndev);
	if (err) {
		dev_err(&pdev->dev, "register_candev() failed\n");
		err = -ENODEV;
		goto probe_exit_clk;
	}

	err = request_firmware(&priv->fw_tx, "PRU_CAN_Emulation_Tx.bin",
			       &pdev->dev);
	if (err) {
		dev_err(&pdev->dev, "can't load firmware\n");
		err = -ENODEV;
		goto probe_exit_clk;
	}

	dev_info(&pdev->dev, "fw_tx size %d. downloading...\n",
		 priv->fw_tx->size);

	err = request_firmware(&priv->fw_rx, "PRU_CAN_Emulation_Rx.bin",
			       &pdev->dev);
	if (err) {
		dev_err(&pdev->dev, "can't load firmware\n");
		err = -ENODEV;
		goto probe_release_fw;
	}
	dev_info(&pdev->dev, "fw_rx size %d. downloading...\n",
		 priv->fw_rx->size);

	fw_pru.ptr_pru1 = kmalloc(priv->fw_tx->size, GFP_KERNEL);
	memcpy((void *)fw_pru.ptr_pru1, (const void *)priv->fw_tx->data,
	       priv->fw_tx->size);
	fw_pru.u32_pru1_code_size = priv->fw_tx->size;
	fw_pru.ptr_pru0 = kmalloc(priv->fw_rx->size, GFP_KERNEL);
	memcpy((void *)fw_pru.ptr_pru0, (const void *)priv->fw_rx->data,
	       priv->fw_rx->size);
	fw_pru.u32_pru0_code_size = priv->fw_rx->size;

	/* init the pru */
	pru_can_emulation_init(&priv->pru_arm_iomap, priv->can.clock.freq);

	while ((pru_can_check_init_status()) && (retries++ < MAX_INIT_RETRIES))
		udelay(retries);

	if (MAX_INIT_RETRIES <= retries) {
		dev_err(&pdev->dev, "failed to initialize the pru\n");
		err = -ENODEV;
		goto probe_release_fw_1;
	}

	pru_can_enable();
	/* download firmware into pru */
	err = pru_can_download_firmware(&fw_pru, 0);
	if (err) {
		dev_err(&pdev->dev, "firmware download error\n");
		err = -ENODEV;
		goto probe_release_fw_1;
	}
	err = pru_can_download_firmware(&fw_pru, 1);
	if (err) {
		dev_err(&pdev->dev, "firmware download error\n");
		err = -ENODEV;
		goto probe_release_fw_1;
	}

	if (pru_can_calculatetiming (DFLT_PRU_FREQ, DFLT_PRU_BITRATE) != 0) {
        return -EINVAL;
    }

	pru_can_run(0);
	pru_can_run(1);
	kfree((const void *)fw_pru.ptr_pru0);
	kfree((const void *)fw_pru.ptr_pru1);

	/*Create The Work Queue */
	if ((priv->pru_can_wQ =
	     create_freezeable_workqueue("omapl_pru_wQ")) == NULL)
		dev_err(&pdev->dev, "failed to create work queue\n");

	INIT_WORK(&priv->rx_work, omapl_pru_can_rx_wQ);
	dev_info(&pdev->dev,
		 "%s device registered"
		 "(&reg_base=0x%p, trx_irq = %d,  clk = %d)\n",
		 DRV_NAME, priv->pru_arm_iomap.pru_io_addr, priv->trx_irq,
		 priv->can.clock.freq);
	return 0;

probe_release_fw_1:
	kfree((const void *)fw_pru.ptr_pru0);
	kfree((const void *)fw_pru.ptr_pru1);
	release_firmware(priv->fw_rx);
probe_release_fw:
	release_firmware(priv->fw_tx);
probe_exit_clk:
	clk_put(priv->clk);
probe_exit_candev:
	if (NULL != ndev)
		free_candev(ndev);
probe_exit_iounmap:
	iounmap(priv->pru_arm_iomap.pru_io_addr);
probe_exit_free_region:
	release_mem_region(res_mem->start, resource_size(res_mem));
probe_exit:
	return err;
}

static int __devexit omapl_pru_can_remove(struct platform_device *pdev)
{
	struct resource *res;
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct omapl_pru_can_priv *priv = netdev_priv(ndev);

	omapl_pru_can_stop(ndev);

	pru_can_emulation_exit();
	release_firmware(priv->fw_tx);
	release_firmware(priv->fw_rx);
	clk_put(priv->clk);
	flush_workqueue(priv->pru_can_wQ);
	destroy_workqueue(priv->pru_can_wQ);
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	iounmap(priv->pru_arm_iomap.pru_io_addr);
	release_mem_region(res->start, resource_size(res));
	unregister_candev(ndev);
	free_candev(ndev);
	platform_set_drvdata(pdev, NULL);
	return 0;
}

#ifdef CONFIG_PM
static int omapl_pru_can_suspend(struct platform_device *pdev,
				 pm_message_t mesg)
{
	dev_info(&pdev->dev, "%s not yet implemented\n", __func__);
	return 0;
}

static int omapl_pru_can_resume(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "%s not yet implemented\n", __func__);
	return 0;
}
#else
#define omapl_pru_can_suspend NULL
#define omapl_pru_can_resume NULL
#endif /* CONFIG_PM */

static struct platform_driver omapl_pru_can_driver = {
	.probe = omapl_pru_can_probe,
	.remove = __devexit_p(omapl_pru_can_remove),
	.suspend = omapl_pru_can_suspend,
	.resume = omapl_pru_can_resume,
	.driver = {
		   .name = DRV_NAME,
		   .owner = THIS_MODULE,
		   },
};

static int __init omapl_pru_can_init(void)
{
	__can_debug(KERN_INFO DRV_DESC "\n");
	return platform_driver_register(&omapl_pru_can_driver);
}

module_init(omapl_pru_can_init);

static void __exit omapl_pru_can_exit(void)
{
	__can_debug(KERN_INFO DRV_DESC " unloaded\n");
	platform_driver_unregister(&omapl_pru_can_driver);
}

module_exit(omapl_pru_can_exit);

MODULE_AUTHOR("Subhasish Ghosh <subhasish@mistralsolutions.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("omapl pru emulated CAN netdevice driver");
