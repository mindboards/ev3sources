/*
 * Texas Instruments DA830/OMAP-L137 "glue layer"
 *
 * Copyright (c) 2008-2009 MontaVista Software, Inc. <source@mvista.com>
 *
 * Based on the DaVinci "glue layer" code.
 * Copyright (C) 2005-2006 by Texas Instruments
 *
 * This file is part of the Inventra Controller Driver for Linux.
 *
 * The Inventra Controller Driver for Linux is free software; you
 * can redistribute it and/or modify it under the terms of the GNU
 * General Public License version 2 as published by the Free Software
 * Foundation.
 *
 * The Inventra Controller Driver for Linux is distributed in
 * the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with The Inventra Controller Driver for Linux ; if not,
 * write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/init.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <mach/usb.h>
#include <mach/da8xx.h>

#include "musb_core.h"
#include "da8xx.h"

#ifdef CONFIG_USB_TI_CPPI41_DMA
#include "cppi41.h"
#include "cppi41_dma.h"
#endif

/*
 * DA830 specific definitions
 */

#define DA830_TX_EP_MASK	0x1f		/* EP0 + 4 Tx EPs */
#define DA830_RX_EP_MASK	0x1e		/* 4 Rx EPs */

#define DA830_TX_INTR_MASK	(DA830_TX_EP_MASK << USB_INTR_TX_SHIFT)
#define DA830_RX_INTR_MASK	(DA830_RX_EP_MASK << USB_INTR_RX_SHIFT)

#define DA830_USB_INTR_USB_SHIFT      16
#define DA830_USB_INTR_USB_MASK       (0x1ff << DA830_USB_INTR_USB_SHIFT)

#define A_WAIT_BCON_TIMEOUT	1100		/* in ms */

#define CFGCHIP2		DA8XX_SYSCFG0_VIRT(DA8XX_CFGCHIP2_REG)

#ifdef CONFIG_USB_TI_CPPI41_DMA

/*
 * CPPI 4.1 resources used for USB OTG controller module:
 *
 * USB   DMA  DMA  QMgr  Tx     Src
 *       Tx   Rx         QNum   Port
 * ---------------------------------
 * EP0   0    0    0     16,17  1
 * ---------------------------------
 * EP1   1    1    0     18,19  2
 * ---------------------------------
 * EP2   2    2    0     20,21  3
 * ---------------------------------
 * EP3   3    3    0     22,23  4
 * ---------------------------------
 */

static const u16 tx_comp_q[] = { 24, 25 };
static const u16 rx_comp_q[] = { 26, 27 };

const struct usb_cppi41_info usb_cppi41_info = {
	.dma_block	= 0,
	.ep_dma_ch	= { 0, 1, 2, 3 },
	.q_mgr		= 0,
	.num_tx_comp_q	= 2,
	.num_rx_comp_q	= 2,
	.tx_comp_q	= tx_comp_q,
	.rx_comp_q	= rx_comp_q
};

/* DMA block configuration */
static const struct cppi41_tx_ch tx_ch_info[] = {
	[0] = {
		.port_num       = 1,
		.num_tx_queue   = 2,
		.tx_queue       = { { 0, 16 }, { 0, 17 } }
	},
	[1] = {
		.port_num       = 2,
		.num_tx_queue   = 2,
		.tx_queue       = { { 0, 18 }, { 0, 19 } }
	},
	[2] = {
		.port_num       = 3,
		.num_tx_queue   = 2,
		.tx_queue       = { { 0, 20 }, { 0, 21 } }
	},
	[3] = {
		.port_num       = 4,
		.num_tx_queue   = 2,
		.tx_queue       = { { 0, 22 }, { 0, 23 } }
	}
};

#define DA8XX_USB0_CFG_BASE IO_ADDRESS(DA8XX_USB0_BASE)
struct cppi41_dma_block cppi41_dma_block[CPPI41_NUM_DMA_BLOCK] = {
	[0] = {
		.global_ctrl_base  = DA8XX_USB0_CFG_BASE + 0x1000,
		.ch_ctrl_stat_base = DA8XX_USB0_CFG_BASE + 0x1800,
		.sched_ctrl_base  = DA8XX_USB0_CFG_BASE + 0x2000,
		.sched_table_base = DA8XX_USB0_CFG_BASE + 0x2800,
		.num_tx_ch        = 4,
		.num_rx_ch        = 4,
		.tx_ch_info       = tx_ch_info
	}
};
EXPORT_SYMBOL(cppi41_dma_block);

/* Queues 0 to 27 are pre-assigned, others are spare */
static const u32 assigned_queues[] = { 0x0fffffff, 0 };


/* Queue manager information */
struct cppi41_queue_mgr cppi41_queue_mgr[CPPI41_NUM_QUEUE_MGR] = {
	[0] = {
		.q_mgr_rgn_base    = DA8XX_USB0_CFG_BASE + 0x4000,
		.desc_mem_rgn_base = DA8XX_USB0_CFG_BASE + 0x5000,
		.q_mgmt_rgn_base   = DA8XX_USB0_CFG_BASE + 0x6000,
		.q_stat_rgn_base   = DA8XX_USB0_CFG_BASE + 0x6800,

		.num_queue       = 64,
		.queue_types     = CPPI41_FREE_DESC_BUF_QUEUE |
					CPPI41_UNASSIGNED_QUEUE,
		.base_fdbq_num    = 0,
		.assigned       = assigned_queues
	}
};
EXPORT_SYMBOL(cppi41_queue_mgr);

/* Fair scheduling */
u32 dma_sched_table[] = {
	0x81018000, 0x83038202,
};

int cppi41_init(void)
{
	u16 order;
	u8 q_mgr, dma_num = 0, numch;

	for (q_mgr = 0; q_mgr < CPPI41_NUM_QUEUE_MGR; ++q_mgr) {
		/* Allocate memory for region 0 */
		cppi41_queue_mgr[q_mgr].ptr_rgn0 = dma_alloc_coherent(NULL,
						USB_CPPI41_QMGR_REG0_MAX_SIZE,
						&cppi41_queue_mgr[q_mgr].
						phys_ptr_rgn0,
						GFP_KERNEL | GFP_DMA);

		/* Initialize Queue Manager 0, alloc for region 0 */
		cppi41_queue_mgr_init(q_mgr,
			cppi41_queue_mgr[q_mgr].phys_ptr_rgn0,
			USB_CPPI41_QMGR_REG0_ALLOC_SIZE);

		numch =  USB_CPPI41_NUM_CH * 2;
		order = get_count_order(numch);

		if (order < 5)
			order = 5;

		cppi41_dma_block_init(dma_num, q_mgr, order,
			dma_sched_table, numch);
	}
	return 0;
}
EXPORT_SYMBOL(cppi41_init);

void cppi41_deinit(void)
{
	u8 q_mgr = 0, dma_block = 0;

	for (q_mgr = 0; q_mgr < CPPI41_NUM_QUEUE_MGR; ++q_mgr) {
		/* deinit dma block */
		cppi41_dma_block_deinit(dma_block, q_mgr);

		/* deinit queue manager */
		cppi41_queue_mgr_deinit(q_mgr);

		/* free the allocated region0 memory */
		dma_free_coherent(NULL, USB_CPPI41_QMGR_REG0_MAX_SIZE,
			cppi41_queue_mgr[q_mgr].ptr_rgn0,
			cppi41_queue_mgr[q_mgr].phys_ptr_rgn0);

		cppi41_queue_mgr[q_mgr].phys_ptr_rgn0 = 0;
		cppi41_queue_mgr[q_mgr].ptr_rgn0 = 0;
	}
}
EXPORT_SYMBOL(cppi41_deinit);
#endif /* CONFIG_USB_TI_CPPI41_DMA */

/*
 * Because we don't set CTRL.UINT, it's "important" to:
 *	- not read/write INTRUSB/INTRUSBE (except during
 *	  initial setup, as a workaround);
 *	- use INTSET/INTCLR instead.
 */

/**
 * musb_platform_enable - enable interrupts
 */
void musb_platform_enable(struct musb *musb)
{
	void __iomem *reg_base = musb->ctrl_base;
	u32 mask;

	/* Workaround: setup IRQs through both register sets. */
	mask = ((musb->epmask & DA830_TX_EP_MASK) << USB_INTR_TX_SHIFT) |
	       ((musb->epmask & DA830_RX_EP_MASK) << USB_INTR_RX_SHIFT) |
	       USB_INTR_USB_MASK;
	musb_writel(reg_base, USB_INTR_MASK_SET_REG, mask);

	/* Force the DRVVBUS IRQ so we can start polling for ID change. */
	if (is_otg_enabled(musb))
		musb_writel(reg_base, USB_INTR_SRC_SET_REG,
			    USB_INTR_DRVVBUS << USB_INTR_USB_SHIFT);
}

/**
 * musb_platform_disable - disable HDRC and flush interrupts
 */
void musb_platform_disable(struct musb *musb)
{
	void __iomem *reg_base = musb->ctrl_base;

	musb_writel(reg_base, USB_INTR_MASK_CLEAR_REG, USB_INTR_USB_MASK |
		    DA830_TX_INTR_MASK | DA830_RX_INTR_MASK);
	musb_writeb(musb->mregs, MUSB_DEVCTL, 0);
	musb_writel(reg_base, USB_END_OF_INTR_REG, 0);
}

#ifdef CONFIG_USB_MUSB_HDRC_HCD
#define portstate(stmt) 	stmt
#else
#define portstate(stmt)
#endif

static void da830_set_vbus(struct musb *musb, int is_on)
{
	WARN_ON(is_on && is_peripheral_active(musb));
}

#define	POLL_SECONDS	2

static struct timer_list otg_workaround;

static void otg_timer(unsigned long _musb)
{
	struct musb		*musb = (void *)_musb;
	void __iomem		*mregs = musb->mregs;
	u8			devctl;
	unsigned long		flags;

	/*
	 * We poll because DaVinci's won't expose several OTG-critical
	 * status change events (from the transceiver) otherwise.
	 */
	devctl = musb_readb(mregs, MUSB_DEVCTL);
	DBG(7, "Poll devctl %02x (%s)\n", devctl, otg_state_string(musb));
	if (musb->is_active)
		return;

	spin_lock_irqsave(&musb->lock, flags);
	switch (musb->xceiv->state) {
	case OTG_STATE_A_WAIT_BCON:
		devctl &= ~MUSB_DEVCTL_SESSION;
		musb_writeb(musb->mregs, MUSB_DEVCTL, devctl);

		devctl = musb_readb(musb->mregs, MUSB_DEVCTL);
		if (devctl & MUSB_DEVCTL_BDEVICE) {
			musb->xceiv->state = OTG_STATE_B_IDLE;
			MUSB_DEV_MODE(musb);
		} else {
			musb->xceiv->state = OTG_STATE_A_IDLE;
			MUSB_HST_MODE(musb);
		}
		break;
	case OTG_STATE_A_WAIT_VFALL:
		/*
		 * Wait till VBUS falls below SessionEnd (~0.2 V); the 1.3
		 * RTL seems to mis-handle session "start" otherwise (or in
		 * our case "recover"), in routine "VBUS was valid by the time
		 * VBUSERR got reported during enumeration" cases.
		 */
		if (devctl & MUSB_DEVCTL_VBUS) {
			mod_timer(&otg_workaround, jiffies + POLL_SECONDS * HZ);
			break;
		}
		musb->xceiv->state = OTG_STATE_A_WAIT_VRISE;
		musb_writel(musb->ctrl_base, USB_INTR_SRC_SET_REG,
			    MUSB_INTR_VBUSERROR << USB_INTR_USB_SHIFT);
		break;
	case OTG_STATE_B_IDLE:
		if (!is_peripheral_enabled(musb))
			break;

		/*
		 * There's no ID-changed IRQ, so we have no good way to tell
		 * when to switch to the A-Default state machine (by setting
		 * the DEVCTL.Session bit).
		 *
		 * Workaround:  whenever we're in B_IDLE, try setting the
		 * session flag every few seconds.  If it works, ID was
		 * grounded and we're now in the A-Default state machine.
		 *
		 * NOTE: setting the session flag is _supposed_ to trigger
		 * SRP but clearly it doesn't.
		 */
		musb_writeb(mregs, MUSB_DEVCTL, devctl | MUSB_DEVCTL_SESSION);
		devctl = musb_readb(mregs, MUSB_DEVCTL);
		if (devctl & MUSB_DEVCTL_BDEVICE)
			mod_timer(&otg_workaround, jiffies + POLL_SECONDS * HZ);
		else
			if (!is_dr_enabled(musb))
				musb->xceiv->state = OTG_STATE_A_IDLE;
			else
				mod_timer(&otg_workaround, jiffies +
						POLL_SECONDS * HZ);
		break;
	case OTG_STATE_A_WAIT_VRISE:
		 if (is_dr_enabled(musb)) {
			devctl &= ~MUSB_DEVCTL_SESSION;
			musb_writeb(mregs, MUSB_DEVCTL, devctl);
			musb->xceiv->state = OTG_STATE_B_IDLE;
			mod_timer(&otg_workaround, jiffies + POLL_SECONDS * HZ);
		}
		break;
	default:
		break;
	}
	spin_unlock_irqrestore(&musb->lock, flags);
}

void musb_platform_try_idle(struct musb *musb, unsigned long timeout)
{
	static unsigned long last_timer;

	if (!(is_otg_enabled(musb) || is_dr_enabled(musb)))
		return;

	if (timeout == 0)
		timeout = jiffies + msecs_to_jiffies(3);

	/* Never idle if active, or when VBUS timeout is not set as host */
	if (musb->is_active || (musb->a_wait_bcon == 0 &&
				musb->xceiv->state == OTG_STATE_A_WAIT_BCON)) {
		DBG(4, "%s active, deleting timer\n", otg_state_string(musb));
		del_timer(&otg_workaround);
		last_timer = jiffies;
		return;
	}

	if (time_after(last_timer, timeout) && timer_pending(&otg_workaround)) {
		DBG(4, "Longer idle timer already pending, ignoring...\n");
		return;
	}
	last_timer = timeout;

	DBG(4, "%s inactive, starting idle timer for %u ms\n",
	    otg_state_string(musb), jiffies_to_msecs(timeout - jiffies));
	mod_timer(&otg_workaround, timeout);
}

static irqreturn_t da8xx_interrupt(int irq, void *hci)
{
	struct musb  *musb = hci;
	void __iomem *reg_base = musb->ctrl_base;
	unsigned long flags;
	irqreturn_t ret = IRQ_NONE;
	u32 status, pend0 = 0;

	spin_lock_irqsave(&musb->lock, flags);

	/*
	 * NOTE: DA830 shadows the Mentor IRQs.  Don't manage them through
	 * the Mentor registers (except for setup), use the TI ones and EOI.
	 */

	/*
	 * CPPI 4.1 interrupts share the same IRQ and the EOI register but
	 * don't get reflected in the interrupt source/mask registers.
	 */
#ifdef CONFIG_USB_TI_CPPI41_DMA
	if (is_cppi41_enabled()) {
		/*
		 * Check for the interrupts from Tx/Rx completion queues; they
		 * are level-triggered and will stay asserted until the queues
		 * are emptied.  We're using the queue pending register 0 as a
		 * substitute for the interrupt status register and reading it
		 * directly for speed.
		 */
		pend0 = musb_readl(reg_base, 0x4000 +
				   QMGR_QUEUE_PENDING_REG(0));
		if (pend0 & (0xf << 24)) {		/* queues 24 to 27 */
			u32 tx = (pend0 >> 24) & 0x3;
			u32 rx = (pend0 >> 26) & 0x3;

			DBG(4, "CPPI 4.1 IRQ: Tx %x, Rx %x\n", tx, rx);
			cppi41_completion(musb, rx, tx);
			ret = IRQ_HANDLED;
		}
	}
#endif

	/* Acknowledge and handle non-CPPI interrupts */
	status = musb_readl(reg_base, USB_INTR_SRC_MASKED_REG);
	if (!status)
		goto eoi;

	musb_writel(reg_base, USB_INTR_SRC_CLEAR_REG, status);
	DBG(4, "USB IRQ %08x\n", status);

	musb->int_rx = (status & DA830_RX_INTR_MASK) >> USB_INTR_RX_SHIFT;
	musb->int_tx = (status & DA830_TX_INTR_MASK) >> USB_INTR_TX_SHIFT;
	musb->int_usb = (status & DA830_USB_INTR_USB_MASK) >>
				DA830_USB_INTR_USB_SHIFT;

	/*
	 * DRVVBUS IRQs are the only proxy we have (a very poor one!) for
	 * DA830's missing ID change IRQ.  We need an ID change IRQ to
	 * switch appropriately between halves of the OTG state machine.
	 * Managing DEVCTL.Session per Mentor docs requires that we know its
	 * value but DEVCTL.BDevice is invalid without DEVCTL.Session set.
	 * Also, DRVVBUS pulses for SRP (but not at 5 V)...
	 */
	if (status & (USB_INTR_DRVVBUS << USB_INTR_USB_SHIFT)) {
		int drvvbus = musb_readl(reg_base, USB_STAT_REG);
		void __iomem *mregs = musb->mregs;
		u8 devctl = musb_readb(mregs, MUSB_DEVCTL);
		int err;

		err = is_host_enabled(musb) && (musb->int_usb &
						MUSB_INTR_VBUSERROR);
		if (err) {
			/*
			 * The Mentor core doesn't debounce VBUS as needed
			 * to cope with device connect current spikes. This
			 * means it's not uncommon for bus-powered devices
			 * to get VBUS errors during enumeration.
			 *
			 * This is a workaround, but newer RTL from Mentor
			 * seems to allow a better one: "re"-starting sessions
			 * without waiting for VBUS to stop registering in
			 * devctl.
			 */
			musb->int_usb &= ~MUSB_INTR_VBUSERROR;
			musb->xceiv->state = OTG_STATE_A_WAIT_VFALL;
			mod_timer(&otg_workaround, jiffies + POLL_SECONDS * HZ);
			WARNING("VBUS error workaround (delay coming)\n");
		} else if (is_host_enabled(musb) && drvvbus) {
			MUSB_HST_MODE(musb);
			musb->xceiv->default_a = 1;
			musb->xceiv->state = OTG_STATE_A_WAIT_VRISE;
			portstate(musb->port1_status |= USB_PORT_STAT_POWER);
			if (!is_dr_enabled(musb))
				del_timer(&otg_workaround);
		} else {
			musb->is_active = 0;
			MUSB_DEV_MODE(musb);
			musb->xceiv->default_a = 0;
			musb->xceiv->state = OTG_STATE_B_IDLE;
			portstate(musb->port1_status &= ~USB_PORT_STAT_POWER);
		}

		DBG(2, "VBUS %s (%s)%s, devctl %02x\n",
				drvvbus ? "on" : "off",
				otg_state_string(musb),
				err ? " ERROR" : "",
				devctl);
		ret = IRQ_HANDLED;
	}

	if (musb->int_tx || musb->int_rx || musb->int_usb)
		ret |= musb_interrupt(musb);

 eoi:
	/* EOI needs to be written for the IRQ to be re-asserted. */
	if (ret == IRQ_HANDLED || status)
		musb_writel(reg_base, USB_END_OF_INTR_REG, 0);

	/* Poll for ID change */
	if (is_otg_enabled(musb) && musb->xceiv->state == OTG_STATE_B_IDLE)
		mod_timer(&otg_workaround, jiffies + POLL_SECONDS * HZ);

	spin_unlock_irqrestore(&musb->lock, flags);

	if (ret != IRQ_HANDLED) {
		if (status){
			/*
			 * We sometimes get unhandled IRQs in the peripheral
			 * mode from EP0 and SOF...
			 */
//			ERR("Unhandled USB IRQ %08x\n", status);    //LEGO - removed to avoid printout on sensorport when connecting/disconn. usb 
    }
		else if (printk_ratelimit())
			/*
			 * We've seen series of spurious interrupts in the
			 * peripheral mode after USB reset and then after some
			 * time a real interrupt storm starting...
			 */
			ERR("Spurious IRQ, CPPI 4.1 status %08x\n" , pend0);
	}
	return ret;
}

int musb_platform_set_mode(struct musb *musb, u8 musb_mode)
{
	u32 cfgchip2 = __raw_readl(CFGCHIP2);

	cfgchip2 &= ~CFGCHIP2_OTGMODE;
	switch (musb_mode) {
#ifdef	CONFIG_USB_MUSB_HDRC_HCD
	case MUSB_HOST:		/* Force VBUS valid, ID = 0 */
		cfgchip2 |= CFGCHIP2_FORCE_HOST;
		break;
#endif
#ifdef	CONFIG_USB_GADGET_MUSB_HDRC
	case MUSB_PERIPHERAL:	/* Force VBUS valid, ID = 1 */
		cfgchip2 |= CFGCHIP2_FORCE_DEVICE;
		break;
#endif
#ifdef	CONFIG_USB_MUSB_OTG
	case MUSB_OTG:		/* Don't override the VBUS/ID comparators */
		cfgchip2 |= CFGCHIP2_NO_OVERRIDE;
		break;
#endif
	default:
		DBG(2, "Trying to set unsupported mode %u\n", musb_mode);
	}

	__raw_writel(cfgchip2, CFGCHIP2);
	return 0;
}

int __init musb_platform_init(struct musb *musb)
{
	void __iomem *reg_base = musb->ctrl_base;
	u32 rev;
	struct device   *dev = musb->controller;
	struct musb_hdrc_platform_data  *plat = dev->platform_data;

	musb->mregs += USB_MENTOR_CORE_OFFSET;

	musb->clock = clk_get(NULL, "usb20");
	if (IS_ERR(musb->clock))
		return PTR_ERR(musb->clock);

	if (clk_enable(musb->clock) < 0)
		return -ENODEV;

	/* Returns zero if e.g. not clocked */
	rev = musb_readl(reg_base, USB_REVISION_REG);
	if (!rev)
		return -ENODEV;

#ifdef CONFIG_USB_TI_CPPI41_DMA
	cppi41_init();
#endif

	if (is_host_enabled(musb))
		setup_timer(&otg_workaround, otg_timer, (unsigned long) musb);

	musb->board_set_vbus = da830_set_vbus;

	/* Reset the controller */
	musb_writel(reg_base, USB_CTRL_REG, USB_SOFT_RESET_MASK);

	usb_nop_xceiv_register();
	musb->xceiv = otg_get_transceiver();

	/* Start the on-chip PHY and its PLL. */
	plat->phy_config(dev, musb->board_mode, 1);
	msleep(5);

	/* NOTE: IRQs are in mixed mode, not bypass to pure MUSB */
	pr_debug("DA830 OTG revision %08x, PHY %03x, control %02x\n",
		 rev, __raw_readl(CFGCHIP2),
		 musb_readb(reg_base, USB_CTRL_REG));

	musb->a_wait_bcon = A_WAIT_BCON_TIMEOUT;
	musb->isr = da8xx_interrupt;
	return 0;
}

int musb_platform_exit(struct musb *musb)
{
	struct device   *dev = musb->controller;
	struct musb_hdrc_platform_data  *plat = dev->platform_data;

	if (is_host_enabled(musb))
		del_timer_sync(&otg_workaround);

	/* Delay to avoid problems with module reload... */
	if (is_host_enabled(musb) && musb->xceiv->default_a) {
		u8 devctl, warn = 0;
		int delay;

		/*
		 * If there's no peripheral connected, VBUS can take a
		 * long time to fall...
		 */
		for (delay = 30; delay > 0; delay--) {
			devctl = musb_readb(musb->mregs, MUSB_DEVCTL);
			if (!(devctl & MUSB_DEVCTL_VBUS))
				goto done;
			if ((devctl & MUSB_DEVCTL_VBUS) != warn) {
				warn = devctl & MUSB_DEVCTL_VBUS;
				DBG(1, "VBUS %d\n",
					warn >> MUSB_DEVCTL_VBUS_SHIFT);
			}
			msleep(1000);
		}

		/* In OTG mode, another host might be connected... */
		DBG(1, "VBUS off timeout (devctl %02x)\n", devctl);
	}
done:
	plat->phy_config(dev, musb->board_mode, 0);
	otg_put_transceiver(musb->xceiv);
	usb_nop_xceiv_unregister();

	return 0;
}
