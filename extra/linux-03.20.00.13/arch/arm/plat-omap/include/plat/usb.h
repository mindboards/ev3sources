// include/asm-arm/mach-omap/usb.h

#ifndef	__ASM_ARCH_OMAP_USB_H
#define	__ASM_ARCH_OMAP_USB_H

#include <plat/board.h>

#define OMAP3_HS_USB_PORTS	3
enum ehci_hcd_omap_mode {
	EHCI_HCD_OMAP_MODE_UNKNOWN,
	EHCI_HCD_OMAP_MODE_PHY,
	EHCI_HCD_OMAP_MODE_TLL,
};

struct ehci_hcd_omap_platform_data {
	enum ehci_hcd_omap_mode		port_mode[OMAP3_HS_USB_PORTS];
	unsigned			phy_reset:1;

	/* have to be valid if phy_reset is true and portx is in phy mode */
	int	reset_gpio_port[OMAP3_HS_USB_PORTS];
	/* flag for aux regulators */
	u8	aux[OMAP3_HS_USB_PORTS];
};

/*-------------------------------------------------------------------------*/

#define OMAP1_OTG_BASE			0xfffb0400
#define OMAP1_UDC_BASE			0xfffb4000
#define OMAP1_OHCI_BASE			0xfffba000

#define OMAP2_OHCI_BASE			0x4805e000
#define OMAP2_UDC_BASE			0x4805e200
#define OMAP2_OTG_BASE			0x4805e300

#ifdef CONFIG_ARCH_OMAP1

#define OTG_BASE			OMAP1_OTG_BASE
#define UDC_BASE			OMAP1_UDC_BASE
#define OMAP_OHCI_BASE			OMAP1_OHCI_BASE

#else

#define OTG_BASE			OMAP2_OTG_BASE
#define UDC_BASE			OMAP2_UDC_BASE
#define OMAP_OHCI_BASE			OMAP2_OHCI_BASE

extern void usb_musb_init(void);

extern void usb_ehci_init(struct ehci_hcd_omap_platform_data *pdata);

#endif

void omap_usb_init(struct omap_usb_config *pdata);

/*-------------------------------------------------------------------------*/

/*
 * OTG and transceiver registers, for OMAPs starting with ARM926
 */
#define OTG_REV				(OTG_BASE + 0x00)
#define OTG_SYSCON_1			(OTG_BASE + 0x04)
#	define	 USB2_TRX_MODE(w)	(((w)>>24)&0x07)
#	define	 USB1_TRX_MODE(w)	(((w)>>20)&0x07)
#	define	 USB0_TRX_MODE(w)	(((w)>>16)&0x07)
#	define	 OTG_IDLE_EN		(1 << 15)
#	define	 HST_IDLE_EN		(1 << 14)
#	define	 DEV_IDLE_EN		(1 << 13)
#	define	 OTG_RESET_DONE		(1 << 2)
#	define	 OTG_SOFT_RESET		(1 << 1)
#define OTG_SYSCON_2			(OTG_BASE + 0x08)
#	define	 OTG_EN			(1 << 31)
#	define	 USBX_SYNCHRO		(1 << 30)
#	define	 OTG_MST16		(1 << 29)
#	define	 SRP_GPDATA		(1 << 28)
#	define	 SRP_GPDVBUS		(1 << 27)
#	define	 SRP_GPUVBUS(w)		(((w)>>24)&0x07)
#	define	 A_WAIT_VRISE(w)	(((w)>>20)&0x07)
#	define	 B_ASE_BRST(w)		(((w)>>16)&0x07)
#	define	 SRP_DPW		(1 << 14)
#	define	 SRP_DATA		(1 << 13)
#	define	 SRP_VBUS		(1 << 12)
#	define	 OTG_PADEN		(1 << 10)
#	define	 HMC_PADEN		(1 << 9)
#	define	 UHOST_EN		(1 << 8)
#	define	 HMC_TLLSPEED		(1 << 7)
#	define	 HMC_TLLATTACH		(1 << 6)
#	define	 OTG_HMC(w)		(((w)>>0)&0x3f)
#define OTG_CTRL			(OTG_BASE + 0x0c)
#	define	 OTG_USB2_EN		(1 << 29)
#	define	 OTG_USB2_DP		(1 << 28)
#	define	 OTG_USB2_DM		(1 << 27)
#	define	 OTG_USB1_EN		(1 << 26)
#	define	 OTG_USB1_DP		(1 << 25)
#	define	 OTG_USB1_DM		(1 << 24)
#	define	 OTG_USB0_EN		(1 << 23)
#	define	 OTG_USB0_DP		(1 << 22)
#	define	 OTG_USB0_DM		(1 << 21)
#	define	 OTG_ASESSVLD		(1 << 20)
#	define	 OTG_BSESSEND		(1 << 19)
#	define	 OTG_BSESSVLD		(1 << 18)
#	define	 OTG_VBUSVLD		(1 << 17)
#	define	 OTG_ID			(1 << 16)
#	define	 OTG_DRIVER_SEL		(1 << 15)
#	define	 OTG_A_SETB_HNPEN	(1 << 12)
#	define	 OTG_A_BUSREQ		(1 << 11)
#	define	 OTG_B_HNPEN		(1 << 9)
#	define	 OTG_B_BUSREQ		(1 << 8)
#	define	 OTG_BUSDROP		(1 << 7)
#	define	 OTG_PULLDOWN		(1 << 5)
#	define	 OTG_PULLUP		(1 << 4)
#	define	 OTG_DRV_VBUS		(1 << 3)
#	define	 OTG_PD_VBUS		(1 << 2)
#	define	 OTG_PU_VBUS		(1 << 1)
#	define	 OTG_PU_ID		(1 << 0)
#define OTG_IRQ_EN			(OTG_BASE + 0x10)	/* 16-bit */
#	define	 DRIVER_SWITCH		(1 << 15)
#	define	 A_VBUS_ERR		(1 << 13)
#	define	 A_REQ_TMROUT		(1 << 12)
#	define	 A_SRP_DETECT		(1 << 11)
#	define	 B_HNP_FAIL		(1 << 10)
#	define	 B_SRP_TMROUT		(1 << 9)
#	define	 B_SRP_DONE		(1 << 8)
#	define	 B_SRP_STARTED		(1 << 7)
#	define	 OPRT_CHG		(1 << 0)
#define OTG_IRQ_SRC			(OTG_BASE + 0x14)	/* 16-bit */
	// same bits as in IRQ_EN
#define OTG_OUTCTRL			(OTG_BASE + 0x18)	/* 16-bit */
#	define	 OTGVPD			(1 << 14)
#	define	 OTGVPU			(1 << 13)
#	define	 OTGPUID		(1 << 12)
#	define	 USB2VDR		(1 << 10)
#	define	 USB2PDEN		(1 << 9)
#	define	 USB2PUEN		(1 << 8)
#	define	 USB1VDR		(1 << 6)
#	define	 USB1PDEN		(1 << 5)
#	define	 USB1PUEN		(1 << 4)
#	define	 USB0VDR		(1 << 2)
#	define	 USB0PDEN		(1 << 1)
#	define	 USB0PUEN		(1 << 0)
#define OTG_TEST			(OTG_BASE + 0x20)	/* 16-bit */
#define OTG_VENDOR_CODE			(OTG_BASE + 0xfc)	/* 16-bit */

/*-------------------------------------------------------------------------*/

/* OMAP1 */
#define	USB_TRANSCEIVER_CTRL		(0xfffe1000 + 0x0064)
#	define	CONF_USB2_UNI_R		(1 << 8)
#	define	CONF_USB1_UNI_R		(1 << 7)
#	define	CONF_USB_PORT0_R(x)	(((x)>>4)&0x7)
#	define	CONF_USB0_ISOLATE_R	(1 << 3)
#	define	CONF_USB_PWRDN_DM_R	(1 << 2)
#	define	CONF_USB_PWRDN_DP_R	(1 << 1)

/* OMAP2 */
#	define	USB_UNIDIR			0x0
#	define	USB_UNIDIR_TLL			0x1
#	define	USB_BIDIR			0x2
#	define	USB_BIDIR_TLL			0x3
#	define	USBTXWRMODEI(port, x)	((x) << (22 - (port * 2)))
#	define	USBT2TLL5PI		(1 << 17)
#	define	USB0PUENACTLOI		(1 << 16)
#	define	USBSTANDBYCTRL		(1 << 15)
/* AM3517 */
/* USB 2.0 OTG module registers */
#define USB_REVISION_REG	0x00
#define USB_CTRL_REG		0x04
#define USB_STAT_REG		0x08
#define USB_EMULATION_REG	0x0c
/* 0x10 Reserved */
#define USB_AUTOREQ_REG		0x14
#define USB_SRP_FIX_TIME_REG	0x18
#define USB_TEARDOWN_REG	0x1c
#define EP_INTR_SRC_REG		0x20
#define EP_INTR_SRC_SET_REG	0x24
#define EP_INTR_SRC_CLEAR_REG	0x28
#define EP_INTR_MASK_REG	0x2c
#define EP_INTR_MASK_SET_REG	0x30
#define EP_INTR_MASK_CLEAR_REG	0x34
#define EP_INTR_SRC_MASKED_REG	0x38
#define CORE_INTR_SRC_REG	0x40
#define CORE_INTR_SRC_SET_REG	0x44
#define CORE_INTR_SRC_CLEAR_REG	0x48
#define CORE_INTR_MASK_REG	0x4c
#define CORE_INTR_MASK_SET_REG	0x50
#define CORE_INTR_MASK_CLEAR_REG 0x54
#define CORE_INTR_SRC_MASKED_REG 0x58
/* 0x5c Reserved */
#define USB_END_OF_INTR_REG	0x60
#define MOP_SOP_INTR_ENABLE	0x64
/* 0x68-0x6c Reserved */
#define USB_TX_MODE_REG		0x70	/* Transparent, CDC, [Generic] RNDIS */
#define USB_RX_MODE_REG		0x74	/* Transparent, CDC, [Generic] RNDIS */
#define EP_COUNT_MODE_REG	0x78
#define USB_GENERIC_RNDIS_EP_SIZE_REG(n) (0x80 + (((n) - 1) << 2))

#define QUEUE_THRESHOLD_INTR_ENABLE_REG	0xc0
#define	QUEUE_63_THRESHOLD_REG	0xc4
#define QUEUE_63_THRESHOLD_INTR_CLEAR_REG 0xc8
#define	QUEUE_65_THRESHOLD_REG	0xd4
#define QUEUE_65_THRESHOLD_INTR_CLEAR_REG 0xd8

/* Control register bits */
#define USB_SOFT_RESET_MASK	1

/* Mode register bits */
#define USB_MODE_SHIFT(n)	((((n) - 1) << 1))
#define USB_MODE_MASK(n)	(3 << USB_MODE_SHIFT(n))
#define USB_RX_MODE_SHIFT(n)	USB_MODE_SHIFT(n)
#define USB_TX_MODE_SHIFT(n)	USB_MODE_SHIFT(n)
#define USB_RX_MODE_MASK(n)	USB_MODE_MASK(n)
#define USB_TX_MODE_MASK(n)	USB_MODE_MASK(n)
#define USB_TRANSPARENT_MODE	0
#define USB_RNDIS_MODE		1
#define USB_CDC_MODE		2
#define USB_GENERIC_RNDIS_MODE	3

/* AutoReq register bits */
#define USB_RX_AUTOREQ_SHIFT(n) (((n) - 1) << 1)
#define USB_RX_AUTOREQ_MASK(n)	(3 << USB_RX_AUTOREQ_SHIFT(n))
#define USB_NO_AUTOREQ		0
#define USB_AUTOREQ_ALL_BUT_EOP 1
#define USB_AUTOREQ_ALWAYS	3

/* Teardown register bits */
#define USB_TX_TDOWN_SHIFT(n)	(16 + (n))
#define USB_TX_TDOWN_MASK(n)	(1 << USB_TX_TDOWN_SHIFT(n))
#define USB_RX_TDOWN_SHIFT(n)	(n)
#define USB_RX_TDOWN_MASK(n)	(1 << USB_RX_TDOWN_SHIFT(n))

/* USB interrupt register bits */
#define USB_INTR_USB_SHIFT	16
#define USB_INTR_USB_MASK	(0x1ff << USB_INTR_USB_SHIFT) /* 8 Mentor */
					/* interrupts and DRVVBUS interrupt */
#define USB_INTR_DRVVBUS	0x100
#define USB_INTR_RX_SHIFT	16
#define USB_INTR_TX_SHIFT	0

#define USB_MENTOR_CORE_OFFSET	0x400

#define USB_CPPI41_NUM_CH	15

#endif	/* __ASM_ARCH_OMAP_USB_H */
