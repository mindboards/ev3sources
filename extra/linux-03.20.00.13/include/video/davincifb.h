/*
 * include/video/davincifb.h
 *
 * Framebuffer driver for Texas Instruments DM644x display controller.
 *
 * Copyright (C) 2006 Texas Instruments, Inc.
 * Rishi Bhattacharya <support@ti.com>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#ifndef _DAVINCIFB_H_
#define _DAVINCIFB_H_

#include <mach/io.h>

/* Base registers */
#define	VPBE_REG_BASE				0x01c72780
#define	VENC_REG_BASE				0x01c72400
#define	OSD_REG_BASE				0x01c72600
#define OSD_REG_SIZE				0x00000180

/* VPBE Global Registers */
#define	VPBE_PID				(VPBE_BASE + 0x0)
#define	VPBE_PCR				(VPBE_BASE + 0x4)

/* VPSS Clock Control Register */
#define	VPSS_CLKCTL				0x01c40044

/* VPBE Video Encoder / Digital LCD Subsystem Registers (VENC) */
#define	VENC_VMOD				(VENC_REG_BASE + 0x00)
#define	VENC_VIDCTL				(VENC_REG_BASE + 0x04)
#define	VENC_VDPRO				(VENC_REG_BASE + 0x08)
#define	VENC_SYNCCTL				(VENC_REG_BASE + 0x0C)
#define	VENC_HSPLS				(VENC_REG_BASE + 0x10)
#define	VENC_VSPLS				(VENC_REG_BASE + 0x14)
#define	VENC_HINT				(VENC_REG_BASE + 0x18)
#define	VENC_HSTART				(VENC_REG_BASE + 0x1C)
#define	VENC_HVALID				(VENC_REG_BASE + 0x20)
#define	VENC_VINT				(VENC_REG_BASE + 0x24)
#define	VENC_VSTART				(VENC_REG_BASE + 0x28)
#define	VENC_VVALID				(VENC_REG_BASE + 0x2C)
#define	VENC_HSDLY				(VENC_REG_BASE + 0x30)
#define	VENC_VSDLY				(VENC_REG_BASE + 0x34)
#define	VENC_YCCCTL				(VENC_REG_BASE + 0x38)
#define	VENC_RGBCTL				(VENC_REG_BASE + 0x3C)
#define	VENC_RGBCLP				(VENC_REG_BASE + 0x40)
#define	VENC_LINECTL				(VENC_REG_BASE + 0x44)
#define	VENC_CULLLINE				(VENC_REG_BASE + 0x48)
#define	VENC_LCDOUT				(VENC_REG_BASE + 0x4C)
#define	VENC_BRTS				(VENC_REG_BASE + 0x50)
#define	VENC_BRTW				(VENC_REG_BASE + 0x54)
#define	VENC_ACCTL				(VENC_REG_BASE + 0x58)
#define	VENC_PWMP				(VENC_REG_BASE + 0x5C)
#define	VENC_PWMW				(VENC_REG_BASE + 0x60)
#define	VENC_DCLKCTL				(VENC_REG_BASE + 0x64)
#define	VENC_DCLKPTN0				(VENC_REG_BASE + 0x68)
#define	VENC_DCLKPTN1				(VENC_REG_BASE + 0x6C)
#define	VENC_DCLKPTN2				(VENC_REG_BASE + 0x70)
#define	VENC_DCLKPTN3				(VENC_REG_BASE + 0x74)
#define	VENC_DCLKPTN0A				(VENC_REG_BASE + 0x78)
#define	VENC_DCLKPTN1A				(VENC_REG_BASE + 0x7C)
#define	VENC_DCLKPTN2A				(VENC_REG_BASE + 0x80)
#define	VENC_DCLKPTN3A				(VENC_REG_BASE + 0x84)
#define	VENC_DCLKHS				(VENC_REG_BASE + 0x88)
#define	VENC_DCLKHSA				(VENC_REG_BASE + 0x8C)
#define	VENC_DCLKHR				(VENC_REG_BASE + 0x90)
#define	VENC_DCLKVS				(VENC_REG_BASE + 0x94)
#define	VENC_DCLKVR				(VENC_REG_BASE + 0x98)
#define	VENC_CAPCTL				(VENC_REG_BASE + 0x9C)
#define	VENC_CAPDO				(VENC_REG_BASE + 0xA0)
#define	VENC_CAPDE				(VENC_REG_BASE + 0xA4)
#define	VENC_ATR0				(VENC_REG_BASE + 0xA8)
#define	VENC_ATR1				(VENC_REG_BASE + 0xAC)
#define	VENC_ATR2				(VENC_REG_BASE + 0xB0)
#define	VENC_EPSON_LCDCTL			(VENC_REG_BASE + 0xB4)
#define	VENC_CASIO_LCDCTL			(VENC_REG_BASE + 0xB4)
#define	VENC_UDISP_LCDCT			(VENC_REG_BASE + 0xB4)
#define	VENC_STN_LCDCT				(VENC_REG_BASE + 0xB4)
#define	VENC_VSTAT				(VENC_REG_BASE + 0xB8)
#define	VENC_RAMADR				(VENC_REG_BASE + 0xBC)
#define	VENC_RAMPORT				(VENC_REG_BASE + 0xC0)
#define	VENC_DACTST				(VENC_REG_BASE + 0xC4)
#define	VENC_YCOLVL				(VENC_REG_BASE + 0xC8)
#define	VENC_SCPROG				(VENC_REG_BASE + 0xCC)
#define	VENC_CVBS				(VENC_REG_BASE + 0xDC)
#define	VENC_CMPNT				(VENC_REG_BASE + 0xE0)
#define	VENC_ETMG0				(VENC_REG_BASE + 0xE4)
#define	VENC_ETMG1				(VENC_REG_BASE + 0xE8)
#define	VENC_ETMG2				(VENC_REG_BASE + 0xEC)
#define	VENC_ETMG3				(VENC_REG_BASE + 0xF0)
#define	VENC_DACSEL				(VENC_REG_BASE + 0xF4)
#define	VENC_ARGBX0				(VENC_REG_BASE + 0x100)
#define	VENC_ARGBX1				(VENC_REG_BASE + 0x104)
#define	VENC_ARGBX2				(VENC_REG_BASE + 0x108)
#define	VENC_ARGBX3				(VENC_REG_BASE + 0x10C)
#define	VENC_ARGBX4				(VENC_REG_BASE + 0x110)
#define	VENC_DRGBX0				(VENC_REG_BASE + 0x114)
#define	VENC_DRGBX1				(VENC_REG_BASE + 0x118)
#define	VENC_DRGBX2				(VENC_REG_BASE + 0x11C)
#define	VENC_DRGBX3				(VENC_REG_BASE + 0x120)
#define	VENC_DRGBX4				(VENC_REG_BASE + 0x124)
#define	VENC_VSTARTA				(VENC_REG_BASE + 0x128)
#define	VENC_OSDCLK0				(VENC_REG_BASE + 0x12C)
#define	VENC_OSDCLK1				(VENC_REG_BASE + 0x130)
#define	VENC_HVLDCL0				(VENC_REG_BASE + 0x134)
#define	VENC_HVLDCL1				(VENC_REG_BASE + 0x138)
#define	VENC_OSDHAD				(VENC_REG_BASE + 0x13C)

#define VID0	0
#define VID1	1
#define OSD0	3
#define OSD1	4

/* VPBE On-Screen Display Subsystem Registers (OSD) */
#define	OSD_MODE				(OSD_REG_BASE + 0x00)
#define	OSD_VIDWINMD				(OSD_REG_BASE + 0x04)
#define	OSD_OSDWIN0MD				(OSD_REG_BASE + 0x08)
#define	OSD_OSDWIN1MD				(OSD_REG_BASE + 0x0C)
#define	OSD_OSDATRMD				(OSD_REG_BASE + 0x0C)
#define	OSD_RECTCUR				(OSD_REG_BASE + 0x10)
#define		OSD_WINOFST(i)			(OSD_REG_BASE + 0x18 + (i)*0x4)
#define	OSD_VIDWIN0OFST				(OSD_REG_BASE + 0x18)
#define	OSD_VIDWIN1OFST				(OSD_REG_BASE + 0x1C)
#define	OSD_OSDWIN0OFST				(OSD_REG_BASE + 0x20)
#define	OSD_OSDWIN1OFST				(OSD_REG_BASE + 0x24)
#define		OSD_WINADR(i)			(OSD_REG_BASE + 0x2C + (i)*0x4)
#define	OSD_VIDWIN0ADR				(OSD_REG_BASE + 0x2C)
#define	OSD_VIDWIN1ADR				(OSD_REG_BASE + 0x30)
#define	OSD_OSDWIN0ADR				(OSD_REG_BASE + 0x38)
#define	OSD_OSDWIN1ADR				(OSD_REG_BASE + 0x3C)
#define	OSD_BASEPX				(OSD_REG_BASE + 0x40)
#define	OSD_BASEPY				(OSD_REG_BASE + 0x44)
#define	OSD_WINXP(i)				(OSD_REG_BASE + 0x48 + (i)*0x10)
#define	OSD_WINYP(i)				(OSD_REG_BASE + 0x4C + (i)*0x10)
#define	OSD_WINXL(i)				(OSD_REG_BASE + 0x50 + (i)*0x10)
#define	OSD_WINYL(i)				(OSD_REG_BASE + 0x54 + (i)*0x10)
#define	OSD_VIDWIN0XP				(OSD_REG_BASE + 0x48)
#define	OSD_VIDWIN0YP				(OSD_REG_BASE + 0x4C)
#define	OSD_VIDWIN0XL				(OSD_REG_BASE + 0x50)
#define	OSD_VIDWIN0YL				(OSD_REG_BASE + 0x54)
#define	OSD_VIDWIN1XP				(OSD_REG_BASE + 0x58)
#define	OSD_VIDWIN1YP				(OSD_REG_BASE + 0x5C)
#define	OSD_VIDWIN1XL				(OSD_REG_BASE + 0x60)
#define	OSD_VIDWIN1YL				(OSD_REG_BASE + 0x64)
#define	OSD_OSDWIN0XP				(OSD_REG_BASE + 0x68)
#define	OSD_OSDWIN0YP				(OSD_REG_BASE + 0x6C)
#define	OSD_OSDWIN0XL				(OSD_REG_BASE + 0x70)
#define	OSD_OSDWIN0YL				(OSD_REG_BASE + 0x74)
#define	OSD_OSDWIN1XP				(OSD_REG_BASE + 0x78)
#define	OSD_OSDWIN1YP				(OSD_REG_BASE + 0x7C)
#define	OSD_OSDWIN1XL				(OSD_REG_BASE + 0x80)
#define	OSD_OSDWIN1YL				(OSD_REG_BASE + 0x84)
#define	OSD_CURXP				(OSD_REG_BASE + 0x88)
#define	OSD_CURYP				(OSD_REG_BASE + 0x8C)
#define	OSD_CURXL				(OSD_REG_BASE + 0x90)
#define	OSD_CURYL				(OSD_REG_BASE + 0x94)
#define	OSD_W0BMP01				(OSD_REG_BASE + 0xA0)
#define	OSD_W0BMP23				(OSD_REG_BASE + 0xA4)
#define	OSD_W0BMP45				(OSD_REG_BASE + 0xA8)
#define	OSD_W0BMP67				(OSD_REG_BASE + 0xAC)
#define	OSD_W0BMP89				(OSD_REG_BASE + 0xB0)
#define	OSD_W0BMPAB				(OSD_REG_BASE + 0xB4)
#define	OSD_W0BMPCD				(OSD_REG_BASE + 0xB8)
#define	OSD_W0BMPEF				(OSD_REG_BASE + 0xBC)
#define	OSD_W1BMP0				(OSD_REG_BASE + 0xC0)
#define	OSD_W1BMP2				(OSD_REG_BASE + 0xC4)
#define	OSD_W1BMP4				(OSD_REG_BASE + 0xC8)
#define	OSD_W1BMP6				(OSD_REG_BASE + 0xCC)
#define	OSD_W1BMP8				(OSD_REG_BASE + 0xD0)
#define	OSD_W1BMPA				(OSD_REG_BASE + 0xD4)
#define	OSD_W1BMPC				(OSD_REG_BASE + 0xD8)
#define	OSD_W1BMPE				(OSD_REG_BASE + 0xDC)
#define	OSD_TI_TES				(OSD_REG_BASE + 0xE0)
#define	OSD_MISCCT				(OSD_REG_BASE + 0xE8)
#define	OSD_CLUTRAMYC				(OSD_REG_BASE + 0xEC)
#define	OSD_CLUTRAMC				(OSD_REG_BASE + 0xF0)
#define	OSD_TRANSPVA				(OSD_REG_BASE + 0xF0)
#define	OSD_PPVWIN0AD				(OSD_REG_BASE + 0xFC)

/* bit definitions */
#define VPBE_PCR_VENC_DIV			(1 << 1)
#define VPBE_PCR_CLK_OFF			(1 << 0)
#define VENC_VMOD_VDMD_SHIFT			12
#define VENC_VMOD_VDMD_YCBCR16			0
#define VENC_VMOD_VDMD_YCBCR8			1
#define VENC_VMOD_VDMD_RGB666			2
#define VENC_VMOD_VDMD_RGB8			3
#define VENC_VMOD_VDMD_EPSON			4
#define VENC_VMOD_VDMD_CASIO			5
#define VENC_VMOD_VDMD_UDISPQVGA		6
#define VENC_VMOD_VDMD_STNLCD			7
#define VENC_VMOD_VDMD				(7 << 12)
#define VENC_VMOD_ITLCL				(1 << 11)
#define VENC_VMOD_ITLC				(1 << 10)
#define VENC_VMOD_NSIT				(1 << 9)
#define VENC_VMOD_HDMD				(1 << 8)
#define VENC_VMOD_TVTYP				(3 << 6)
#define VENC_VMOD_SLAVE				(1 << 5)
#define VENC_VMOD_VMD				(1 << 4)
#define VENC_VMOD_BLNK				(1 << 3)
#define VENC_VMOD_VIE				(1 << 1)
#define VENC_VMOD_VENC				(1 << 0)
/* other VENC registers' bit positions not defined yet */

#define	OSD_MODE_CS				(1 << 15)
#define	OSD_MODE_OVRSZ				(1 << 14)
#define	OSD_MODE_OHRSZ				(1 << 13)
#define	OSD_MODE_EF				(1 << 12)
#define	OSD_MODE_VVRSZ				(1 << 11)
#define	OSD_MODE_VHRSZ				(1 << 10)
#define	OSD_MODE_FSINV				(1 << 9)
#define	OSD_MODE_BCLUT				(1 << 8)
#define	OSD_MODE_CABG				(0xff << 0)
#define	OSD_MODE_CABG_SHIFT			0

#define	OSD_VIDWINMD_VFINV			(1 << 15)
#define	OSD_VIDWINMD_V1EFC			(1 << 14)
#define	OSD_VIDWINMD_VHZ1			(3 << 12)
#define	OSD_VIDWINMD_VHZ1_SHIFT			12
#define	OSD_VIDWINMD_VVZ1			(3 << 10)
#define	OSD_VIDWINMD_VVZ1_SHIFT			10
#define	OSD_VIDWINMD_VFF1			(1 << 9)
#define	OSD_VIDWINMD_ACT1			(1 << 8)
#define	OSD_VIDWINMD_V0EFC			(1 << 6)
#define	OSD_VIDWINMD_VHZ0			(3 << 4)
#define	OSD_VIDWINMD_VHZ0_SHIFT			4
#define	OSD_VIDWINMD_VVZ0			(3 << 2)
#define	OSD_VIDWINMD_VVZ0_SHIFT			2
#define	OSD_VIDWINMD_VFF0			(1 << 1)
#define	OSD_VIDWINMD_ACT0			(1 << 0)

#define	OSD_OSDWIN0MD_ATN0E			(1 << 14)
#define	OSD_OSDWIN0MD_RGB0E			(1 << 13)
#define	OSD_OSDWIN0MD_CLUTS0			(1 << 12)
#define	OSD_OSDWIN0MD_OHZ0			(3 << 10)
#define	OSD_OSDWIN0MD_OHZ0_SHIFT		10
#define	OSD_OSDWIN0MD_OVZ0			(3 << 8)
#define	OSD_OSDWIN0MD_OVZ0_SHIFT		8
#define	OSD_OSDWIN0MD_BMW0			(3 << 6)
#define	OSD_OSDWIN0MD_BMW0_SHIFT		6
#define	OSD_OSDWIN0MD_BLND0			(3 << 3)
#define	OSD_OSDWIN0MD_BLND0_SHIFT		3
#define	OSD_OSDWIN0MD_TE0			(1 << 2)
#define	OSD_OSDWIN0MD_OFF0			(1 << 1)
#define	OSD_OSDWIN0MD_OACT0			(1 << 0)

#define	OSD_OSDWIN1MD_OASW			(1 << 15)
#define	OSD_OSDWIN1MD_ATN1E			(1 << 14)
#define	OSD_OSDWIN1MD_RGB1E			(1 << 13)
#define	OSD_OSDWIN1MD_CLUTS1			(1 << 12)
#define	OSD_OSDWIN1MD_OHZ1			(3 << 10)
#define	OSD_OSDWIN1MD_OHZ1_SHIFT		10
#define	OSD_OSDWIN1MD_OVZ1			(3 << 8)
#define	OSD_OSDWIN1MD_OVZ1_SHIFT		8
#define	OSD_OSDWIN1MD_BMW1			(3 << 6)
#define	OSD_OSDWIN1MD_BMW1_SHIFT		6
#define	OSD_OSDWIN1MD_BLND1			(3 << 3)
#define	OSD_OSDWIN1MD_BLND1_SHIFT		3
#define	OSD_OSDWIN1MD_TE1			(1 << 2)
#define	OSD_OSDWIN1MD_OFF1			(1 << 1)
#define	OSD_OSDWIN1MD_OACT1			(1 << 0)


#define	OSD_OSDATRMD_OASW			(1 << 15)
#define	OSD_OSDATRMD_OHZA			(3 << 10)
#define	OSD_OSDATRMD_OHZA_SHIFT			10
#define	OSD_OSDATRMD_OVZA			(3 << 8)
#define	OSD_OSDATRMD_OVZA_SHIFT			8
#define	OSD_OSDATRMD_BLNKINT			(3 << 6)
#define	OSD_OSDATRMD_BLNKINT_SHIFT		6
#define	OSD_OSDATRMD_OFFA			(1 << 1)
#define	OSD_OSDATRMD_BLNK			(1 << 0)

#define	OSD_RECTCUR_RCAD			(0xff << 8)
#define	OSD_RECTCUR_RCAD_SHIFT			8
#define	OSD_RECTCUR_CLUTSR			(1 << 7)
#define	OSD_RECTCUR_RCHW			(3 << 4)
#define	OSD_RECTCUR_RCHW_SHIFT			4
#define	OSD_RECTCUR_RCVW			(3 << 1)
#define	OSD_RECTCUR_RCVW_SHIFT			1
#define	OSD_RECTCUR_RCACT			(1 << 0)

#define	OSD_VIDWIN0OFST_V0LO			(0x1ff << 0)
#define	OSD_VIDWIN0OFST_V0LO_SHIFT		0
#define	OSD_VIDWIN1OFST_V1LO			(0x1ff << 0)
#define	OSD_VIDWIN1OFST_V1LO_SHIFT		0
#define	OSD_OSDWIN0OFST_O0LO			(0x1ff << 0)
#define	OSD_OSDWIN0OFST_O0LO_SHIFT		0
#define	OSD_OSDWIN1OFST_O1LO			(0x1ff << 0)
#define	OSD_OSDWIN1OFST_O1LO_SHIFT		0
#define	OSD_BASEPX_BPX				(0x3ff << 0)
#define	OSD_BASEPX_BPX_SHIFT			0
#define	OSD_BASEPY_BPY				(0x1ff << 0)
#define	OSD_BASEPY_BPY_SHIFT			0
#define	OSD_VIDWIN0XP_V0X			(0x3ff << 0)
#define	OSD_VIDWIN0XP_V0X_SHIFT			0
#define	OSD_VIDWIN0YP_V0Y			(0x1ff << 0)
#define	OSD_VIDWIN0YP_V0Y_SHIFT			0
#define	OSD_VIDWIN0XL_V0W			(0xfff << 0)
#define	OSD_VIDWIN0XL_V0W_SHIFT			0
#define	OSD_VIDWIN0YL_V0H			(0x7ff << 0)
#define	OSD_VIDWIN0YL_V0H_SHIFT			0
#define	OSD_VIDWIN1XP_V1X			(0x3ff << 0)
#define	OSD_VIDWIN1XP_V1X_SHIFT			0
#define	OSD_VIDWIN1YP_V1Y			(0x1ff << 0)
#define	OSD_VIDWIN1YP_V1Y_SHIFT			0
#define	OSD_VIDWIN1XL_V1W			(0xfff << 0)
#define	OSD_VIDWIN1XL_V1W_SHIFT			0
#define	OSD_VIDWIN1YL_V1H			(0x7ff << 0)
#define	OSD_VIDWIN1YL_V1H_SHIFT			0
#define	OSD_OSDWIN0XP_W0X			(0x3ff << 0)
#define	OSD_OSDWIN0XP_W0X_SHIFT			0
#define	OSD_OSDWIN0YP_W0Y			(0x1ff << 0)
#define	OSD_OSDWIN0YP_W0Y_SHIFT			0
#define	OSD_OSDWIN0XL_W0W			(0xfff << 0)
#define	OSD_OSDWIN0XL_W0W_SHIFT			0
#define	OSD_OSDWIN0YL_W0H			(0x7ff << 0)
#define	OSD_OSDWIN0YL_W0H_SHIFT			0
#define	OSD_OSDWIN1XP_W1X			(0x3ff << 0)
#define	OSD_OSDWIN1XP_W1X_SHIFT			0
#define	OSD_OSDWIN1YP_W1Y			(0x1ff << 0)
#define	OSD_OSDWIN1YP_W1Y_SHIFT			0
#define	OSD_OSDWIN1XL_W1W			(0xfff << 0)
#define	OSD_OSDWIN1XL_W1W_SHIFT			0
#define	OSD_OSDWIN1YL_W1H			(0x7ff << 0)
#define	OSD_OSDWIN1YL_W1H_SHIFT			0
#define	OSD_CURXP_RCSX				(0x3ff << 0)
#define	OSD_CURXP_RCSX_SHIFT			0
#define	OSD_CURYP_RCSY				(0x1ff << 0)
#define	OSD_CURYP_RCSY_SHIFT			0
#define	OSD_CURYL_RCSH				(0x7ff << 0)
#define	OSD_CURYL_RCSH_SHIFT			0
#define	OSD_W0BMP01_PAL01			(0xff << 8)
#define	OSD_W0BMP01_PAL01_SHIFT			8
#define	OSD_W0BMP01_PAL00			(0xff << 0)
#define	OSD_W0BMP01_PAL00_SHIFT			0
#define	OSD_W0BMP23_PAL03			(0xff << 8)
#define	OSD_W0BMP23_PAL03_SHIFT			8
#define	OSD_W0BMP23_PAL02			(0xff << 0)
#define	OSD_W0BMP23_PAL02_SHIFT			0
#define	OSD_W0BMP45_PAL05			(0xff << 8)
#define	OSD_W0BMP45_PAL05_SHIFT			8
#define	OSD_W0BMP45_PAL04			(0xff << 0)
#define	OSD_W0BMP45_PAL04_SHIFT			0
#define	OSD_W0BMP67_PAL07			(0xff << 8)
#define	OSD_W0BMP67_PAL07_SHIFT			8
#define	OSD_W0BMP67_PAL06			(0xff << 0)
#define	OSD_W0BMP67_PAL06_SHIFT			0
#define	OSD_W0BMP89_PAL09			(0xff << 8)
#define	OSD_W0BMP89_PAL09_SHIFT			8
#define	OSD_W0BMP89_PAL08			(0xff << 0)
#define	OSD_W0BMP89_PAL08_SHIFT			0
#define	OSD_W0BMPAB_PAL11			(0xff << 8)
#define	OSD_W0BMPAB_PAL11_SHIFT			8
#define	OSD_W0BMPAB_PAL10			(0xff << 0)
#define	OSD_W0BMPAB_PAL10_SHIFT			0
#define	OSD_W0BMPCD_PAL13			(0xff << 8)
#define	OSD_W0BMPCD_PAL13_SHIFT			8
#define	OSD_W0BMPCD_PAL12			(0xff << 0)
#define	OSD_W0BMPCD_PAL12_SHIFT			0
#define	OSD_W0BMPEF_PAL15			(0xff << 8)
#define	OSD_W0BMPEF_PAL15_SHIFT			8
#define	OSD_W0BMPEF_PAL14			(0xff << 0)
#define	OSD_W0BMPEF_PAL14_SHIFT			0
#define	OSD_W1BMP0_PAL01			(0xff << 8)
#define	OSD_W1BMP0_PAL01_SHIFT			8
#define	OSD_W1BMP0_PAL00			(0xff << 0)
#define	OSD_W1BMP0_PAL00_SHIFT			0
#define	OSD_W1BMP2_PAL03			(0xff << 8)
#define	OSD_W1BMP2_PAL03_SHIFT			8
#define	OSD_W1BMP2_PAL02			(0xff << 0)
#define	OSD_W1BMP2_PAL02_SHIFT			0
#define	OSD_W1BMP4_PAL05			(0xff << 8)
#define	OSD_W1BMP4_PAL05_SHIFT			8
#define	OSD_W1BMP4_PAL04			(0xff << 0)
#define	OSD_W1BMP4_PAL04_SHIFT			0
#define	OSD_W1BMP6_PAL07			(0xff << 8)
#define	OSD_W1BMP6_PAL07_SHIFT			8
#define	OSD_W1BMP6_PAL06			(0xff << 0)
#define	OSD_W1BMP6_PAL06_SHIFT			0
#define	OSD_W1BMP8_PAL09			(0xff << 8)
#define	OSD_W1BMP8_PAL09_SHIFT			8
#define	OSD_W1BMP8_PAL08			(0xff << 0)
#define	OSD_W1BMP8_PAL08_SHIFT			0
#define	OSD_W1BMPA_PAL11			(0xff << 8)
#define	OSD_W1BMPA_PAL11_SHIFT			8
#define	OSD_W1BMPA_PAL10			(0xff << 0)
#define	OSD_W1BMPA_PAL10_SHIFT			0
#define	OSD_W1BMPC_PAL13			(0xff << 8)
#define	OSD_W1BMPC_PAL13_SHIFT			8
#define	OSD_W1BMPC_PAL12			(0xff << 0)
#define	OSD_W1BMPC_PAL12_SHIFT			0
#define	OSD_W1BMPE_PAL15			(0xff << 8)
#define	OSD_W1BMPE_PAL15_SHIFT			8
#define	OSD_W1BMPE_PAL14			(0xff << 0)
#define	OSD_W1BMPE_PAL14_SHIFT			0

#define	OSD_MISCCT_RGBEN			(1 << 7)
#define	OSD_MISCCT_RGBWIN			(1 << 6)
#define	OSD_MISCCT_TMON				(1 << 5)
#define	OSD_MISCCT_RSEL				(1 << 4)
#define	OSD_MISCCT_CPBSY			(1 << 3)
#define	OSD_MISCCT_PPSW				(1 << 2)
#define	OSD_MISCCT_PPRV				(1 << 1)

#define	OSD_CLUTRAMY_Y				(0xff << 8)
#define	OSD_CLUTRAMY_Y_SHIFT			8
#define	OSD_CLUTRAMY_CB				(0xff << 0)
#define	OSD_CLUTRAMY_CB_SHIFT			0
#define	OSD_CLUTRAM_CR				(0xff << 8)
#define	OSD_CLUTRAM_CR_SHIFT			8
#define	OSD_CLUTRAM_CADDR			(0xff << 0)
#define	OSD_CLUTRAM_CADDR_SHIFT			0
#define	OSD_TRANSPVA_RGBTRANS			(0xff << 0)
#define	OSD_TRANSPVA_RGBTRANS_SHIFT		0


#define LCD             0
#define NTSC            1
#define PAL             2

#define COMPOSITE       1
#define SVIDEO          2
#define COMPONENT       3
#define RGB             4

/* define the custom FBIO_WAITFORVSYNC ioctl */
#define FBIO_WAITFORVSYNC	_IOW('F', 0x20, u_int32_t)
#define FBIO_SETATTRIBUTE       _IOW('F', 0x21, struct fb_fillrect)
#define FBIO_SETPOSX		_IOW('F', 0x22, u_int32_t)
#define FBIO_SETPOSY		_IOW('F', 0x23, u_int32_t)
struct zoom_params
{
	u_int32_t window_id;
	u_int32_t zoom_h;
	u_int32_t zoom_v;
};
#define FBIO_SETZOOM		_IOW('F', 0x24, struct zoom_params)
#define FBIO_GETSTD		_IOR('F', 0x25, u_int32_t)
#endif /* _DAVINCIFB_H_ */
