/********************************************************************
* Copyright (C) 2003-2008 Texas Instruments Incorporated.
* All Rights Reserved
*********************************************************************
* file: cslr_psc.h
*
* Brief: This file contains the Register Description for psc
*
*********************************************************************/
#ifndef _CSLR_PSC_H_
#define _CSLR_PSC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "csl/cslr.h"
#include "tistdtypes.h"

/* NOTE1
 * This is the superset list for all modules present in this family
 * Please refer to individual device datasheet to see which modules
 * apply on the device being used
 * e.g. c6747 does not have ARM module, so PSC0 CSL_PSC_ARM does not
 * exist for c6747                                                  */

/* NOTE2
 * The actual number of MDCTL and MDSTAT register depend on number of
 * LPSC modules in a PSC. The number of MDCTL/MDSTAT registers defined
 * here would be a superset
 * e.g. PSC0 has 16 MDCTL/MDSTAT register, PSC1 has 32 MDCTL/MDSTAT
 * registers                                                       */

/* NOTE3
 * Please refer to the device specific PSC user guide to see what
 *  register bit fields apply to individual registers
 *  e.g. For PSC0 MERRPR0 bits 14,15 exist but for PSC1 MERRPR0
 *  these bits are RESERVED                                        */

	typedef enum {
		CSL_PSC_CC0 = 0,
		CSL_PSC_TC0 = 1,
		CSL_PSC_TC1 = 2,
		CSL_PSC_EMIFA = 3,
		CSL_PSC_SPI0 = 4,
		CSL_PSC_MMCSD0 = 5,
		CSL_PSC_AINTC = 6,
		CSL_PSC_ARM_RAMROM = 7,
		CSL_PSC_UART0 = 9,
		CSL_PSC_SCR0_SS = 10,
		CSL_PSC_SCR1_SS = 11,
		CSL_PSC_SCR2_SS = 12,
		CSL_PSC_PRU = 13,
		CSL_PSC_ARM = 14,
		CSL_PSC_DSP = 15
	} CSL_Psc0Peripheral;

	typedef enum {
		CSL_PSC_CC1 = 0,
		CSL_PSC_USB0 = 1,
		CSL_PSC_USB1 = 2,
		CSL_PSC_GPIO = 3,
		CSL_PSC_UHPI = 4,
		CSL_PSC_EMAC = 5,
		CSL_PSC_DDR2_MDDR = 6,
		CSL_PSC_MCASP0 = 7,
		CSL_PSC_SATA = 8,
		CSL_PSC_VPIF = 9,
		CSL_PSC_SPI1 = 10,
		CSL_PSC_I2C1 = 11,
		CSL_PSC_UART1 = 12,
		CSL_PSC_UART2 = 13,
		CSL_PSC_MCBSP0 = 14,
		CSL_PSC_MCBSP1 = 15,
		CSL_PSC_LCDC = 16,
		CSL_PSC_EHRPWM = 17,
		CSL_PSC_MMCSD1 = 18,
		CSL_PSC_UPP = 19,
		CSL_PSC_ECAP0_1_2 = 20,
		CSL_PSC_TC2 = 21,
		CSL_PSC_SCRF0_SS = 24,
		CSL_PSC_SCRF1_SS = 25,
		CSL_PSC_SCRF2_SS = 26,
		CSL_PSC_SCRF6_SS = 27,
		CSL_PSC_SCRF7_SS = 28,
		CSL_PSC_SCRF8_SS = 29,
		CSL_PSC_BR_F7 = 30,
		CSL_PSC_SHRAM = 31
	} CSL_Psc1Peripheral;

/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
	typedef struct {
		volatile Uint32 REVID;
		volatile Uint8 RSVD0[20];
		volatile Uint32 INTEVAL;
		volatile Uint8 RSVD1[36];
		volatile Uint32 MERRPR0;
		volatile Uint8 RSVD2[12];
		volatile Uint32 MERRCR0;
		volatile Uint8 RSVD3[12];
		volatile Uint32 PERRPR;
		volatile Uint8 RSVD4[4];
		volatile Uint32 PERRCR;
		volatile Uint8 RSVD5[180];
		volatile Uint32 PTCMD;
		volatile Uint8 RSVD6[4];
		volatile Uint32 PTSTAT;
		volatile Uint8 RSVD7[212];
		volatile Uint32 PDSTAT0;
		volatile Uint32 PDSTAT1;
		volatile Uint8 RSVD8[248];
		volatile Uint32 PDCTL0;
		volatile Uint32 PDCTL1;
		volatile Uint8 RSVD9[248];
		volatile Uint32 PDCFG0;
		volatile Uint32 PDCFG1;
		volatile Uint8 RSVD10[1016];
		volatile Uint32 MDSTAT[32];
		volatile Uint8 RSVD11[384];
		volatile Uint32 MDCTL[32];
	} CSL_PscRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
	typedef volatile CSL_PscRegs *CSL_PscRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* REVID */

#define CSL_PSC_REVID_REV_MASK             (0xFFFFFFFFu)
#define CSL_PSC_REVID_REV_SHIFT            (0x00000000u)
#define CSL_PSC_REVID_REV_RESETVAL         (0x44823A00u)

#define CSL_PSC_REVID_RESETVAL             (0x44823A00u)

/* INTEVAL */

#define CSL_PSC_INTEVAL_ALLEV_MASK      (0x00000001u)
#define CSL_PSC_INTEVAL_ALLEV_SHIFT     (0x00000000u)
#define CSL_PSC_INTEVAL_ALLEV_RESETVAL  (0x00000000u)
/*----ALLEV Tokens----*/
#define CSL_PSC_INTEVAL_ALLEV_NO_EFFECT (0x00000000u)
#define CSL_PSC_INTEVAL_ALLEV_RE_EVALUATE (0x00000001u)

#define CSL_PSC_INTEVAL_RESETVAL        (0x00000000u)

/* MERRPR0 */

#define CSL_PSC_MERRPR0_M15_MASK        (0x0000C000u)
#define CSL_PSC_MERRPR0_M15_SHIFT       (0x0000000Eu)
#define CSL_PSC_MERRPR0_M15_RESETVAL    (0x00000000u)
/*----M15 Tokens----*/
#define CSL_PSC_MERRPR0_M15_NO_ERR      (0x00000000u)
#define CSL_PSC_MERRPR0_M15_ERROR       (0x00000001u)

#define CSL_PSC_MERRPR0_M14_MASK        (0x00006000u)
#define CSL_PSC_MERRPR0_M14_SHIFT       (0x0000000Du)
#define CSL_PSC_MERRPR0_M14_RESETVAL    (0x00000000u)
/*----M14 Tokens----*/
#define CSL_PSC_MERRPR0_M14_NO_ERR      (0x00000000u)
#define CSL_PSC_MERRPR0_M14_ERROR       (0x00000001u)

#define CSL_PSC_MERRPR0_RESETVAL        (0x00000000u)

/* MERRCR0 */

#define CSL_PSC_MERRCR0_M15_MASK        (0x0000C000u)
#define CSL_PSC_MERRCR0_M15_SHIFT       (0x0000000Eu)
#define CSL_PSC_MERRCR0_M15_RESETVAL    (0x00000000u)
/*----M15 Tokens----*/
#define CSL_PSC_MERRCR0_M15_NO_EFFECT   (0x00000000u)
#define CSL_PSC_MERRCR0_M15_CLR_ERR     (0x00000001u)

#define CSL_PSC_MERRCR0_M14_MASK        (0x00006000u)
#define CSL_PSC_MERRCR0_M14_SHIFT       (0x0000000Du)
#define CSL_PSC_MERRCR0_M14_RESETVAL    (0x00000000u)
/*----M14 Tokens----*/
#define CSL_PSC_MERRCR0_M14_NO_EFFECT   (0x00000000u)
#define CSL_PSC_MERRCR0_M14_CLR_ERR     (0x00000001u)

#define CSL_PSC_MERRCR0_RESETVAL        (0x00000000u)

/* PERRPR */

#define CSL_PSC_PERRPR_P1_MASK          (0x00000002u)
#define CSL_PSC_PERRPR_P1_SHIFT         (0x00000001u)
#define CSL_PSC_PERRPR_P1_RESETVAL      (0x00000000u)
/*----P1 Tokens----*/
#define CSL_PSC_PERRPR_P1_NO_ERR        (0x00000000u)
#define CSL_PSC_PERRPR_P1_ERROR         (0x00000001u)

#define CSL_PSC_PERRPR_P0_MASK          (0x00000001u)
#define CSL_PSC_PERRPR_P0_SHIFT         (0x00000000u)
#define CSL_PSC_PERRPR_P0_RESETVAL      (0x00000000u)
/*----P0 Tokens----*/
#define CSL_PSC_PERRPR_P0_NO_ERR        (0x00000000u)
#define CSL_PSC_PERRPR_P0_ERROR         (0x00000001u)

#define CSL_PSC_PERRPR_RESETVAL         (0x00000000u)

/* PERRCR */

#define CSL_PSC_PERRCR_P1_MASK          (0x00000002u)
#define CSL_PSC_PERRCR_P1_SHIFT         (0x00000001u)
#define CSL_PSC_PERRCR_P1_RESETVAL      (0x00000000u)
/*----P1 Tokens----*/
#define CSL_PSC_PERRCR_P1_NO_EFFECT     (0x00000000u)
#define CSL_PSC_PERRCR_P1_CLR_ERR       (0x00000001u)

#define CSL_PSC_PERRCR_P0_MASK          (0x00000001u)
#define CSL_PSC_PERRCR_P0_SHIFT         (0x00000000u)
#define CSL_PSC_PERRCR_P0_RESETVAL      (0x00000000u)
/*----P0 Tokens----*/
#define CSL_PSC_PERRCR_P0_NO_EFFECT     (0x00000000u)
#define CSL_PSC_PERRCR_P0_CLR_ERR       (0x00000001u)

#define CSL_PSC_PERRCR_RESETVAL         (0x00000000u)

/* PTCMD */

#define CSL_PSC_PTCMD_GO1_MASK          (0x00000002u)
#define CSL_PSC_PTCMD_GO1_SHIFT         (0x00000001u)
#define CSL_PSC_PTCMD_GO1_RESETVAL      (0x00000000u)
/*----GO1 Tokens----*/
#define CSL_PSC_PTCMD_GO1_NO_EFFECT     (0x00000000u)
#define CSL_PSC_PTCMD_GO1_SET           (0x00000001u)

#define CSL_PSC_PTCMD_GO0_MASK          (0x00000001u)
#define CSL_PSC_PTCMD_GO0_SHIFT         (0x00000000u)
#define CSL_PSC_PTCMD_GO0_RESETVAL      (0x00000000u)
/*----GO0 Tokens----*/
#define CSL_PSC_PTCMD_GO0_NO_EFFECT     (0x00000000u)
#define CSL_PSC_PTCMD_GO0_SET           (0x00000001u)

#define CSL_PSC_PTCMD_RESETVAL          (0x00000000u)

/* PTSTAT */

#define CSL_PSC_PTSTAT_GOSTAT1_MASK     (0x00000002u)
#define CSL_PSC_PTSTAT_GOSTAT1_SHIFT    (0x00000001u)
#define CSL_PSC_PTSTAT_GOSTAT1_RESETVAL (0x00000000u)
/*----GOSTAT1 Tokens----*/
#define CSL_PSC_PTSTAT_GOSTAT1_NO_TRANSITION (0x00000000u)
#define CSL_PSC_PTSTAT_GOSTAT1_IN_TRANSITION (0x00000001u)

#define CSL_PSC_PTSTAT_GOSTAT0_MASK     (0x00000001u)
#define CSL_PSC_PTSTAT_GOSTAT0_SHIFT    (0x00000000u)
#define CSL_PSC_PTSTAT_GOSTAT0_RESETVAL (0x00000000u)
/*----GOSTAT0 Tokens----*/
#define CSL_PSC_PTSTAT_GOSTAT0_NO_TRANSITION (0x00000000u)
#define CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION (0x00000001u)

#define CSL_PSC_PTSTAT_RESETVAL         (0x00000000u)

/* PDSTAT0 */

#define CSL_PSC_PDSTAT0_EMUIHB_MASK     (0x00000800u)
#define CSL_PSC_PDSTAT0_EMUIHB_SHIFT    (0x0000000Bu)
#define CSL_PSC_PDSTAT0_EMUIHB_RESETVAL (0x00000000u)
/*----EMUIHB Tokens----*/
#define CSL_PSC_PDSTAT0_EMUIHB_INHIBIT_OFF (0x00000000u)
#define CSL_PSC_PDSTAT0_EMUIHB_INHIBIT_ON (0x00000001u)

#define CSL_PSC_PDSTAT0_STATE_MASK      (0x0000001Fu)
#define CSL_PSC_PDSTAT0_STATE_SHIFT     (0x00000000u)
#define CSL_PSC_PDSTAT0_STATE_RESETVAL  (0x00000000u)
/*----STATE Tokens----*/
#define CSL_PSC_PDSTAT0_STATE_OFF       (0x00000000u)
#define CSL_PSC_PDSTAT0_STATE_ON        (0x00000001u)

#define CSL_PSC_PDSTAT0_RESETVAL        (0x00000000u)

/* PDSTAT1 */

#define CSL_PSC_PDSTAT1_EMUIHB_MASK     (0x00000800u)
#define CSL_PSC_PDSTAT1_EMUIHB_SHIFT    (0x0000000Bu)
#define CSL_PSC_PDSTAT1_EMUIHB_RESETVAL (0x00000000u)
/*----EMUIHB Tokens----*/
#define CSL_PSC_PDSTAT1_EMUIHB_INHIBIT_OFF (0x00000000u)
#define CSL_PSC_PDSTAT1_EMUIHB_INHIBIT_ON (0x00000001u)

#define CSL_PSC_PDSTAT1_STATE_MASK      (0x0000001Fu)
#define CSL_PSC_PDSTAT1_STATE_SHIFT     (0x00000000u)
#define CSL_PSC_PDSTAT1_STATE_RESETVAL  (0x00000000u)
/*----STATE Tokens----*/
#define CSL_PSC_PDSTAT1_STATE_OFF       (0x00000000u)
#define CSL_PSC_PDSTAT1_STATE_ON        (0x00000001u)

#define CSL_PSC_PDSTAT1_RESETVAL        (0x00000000u)

/* PDCTL0 */

#define CSL_PSC_PDCTL0_WAKECNT_MASK     (0x00FF0000u)
#define CSL_PSC_PDCTL0_WAKECNT_SHIFT    (0x00000010u)
#define CSL_PSC_PDCTL0_WAKECNT_RESETVAL (0x0000001Fu)

#define CSL_PSC_PDCTL0_PDMODE_MASK      (0x0000F000u)
#define CSL_PSC_PDCTL0_PDMODE_SHIFT     (0x0000000Cu)
#define CSL_PSC_PDCTL0_PDMODE_RESETVAL  (0x0000000Fu)

#define CSL_PSC_PDCTL0_EMUIHBIE_MASK    (0x00000200u)
#define CSL_PSC_PDCTL0_EMUIHBIE_SHIFT   (0x00000009u)
#define CSL_PSC_PDCTL0_EMUIHBIE_RESETVAL (0x00000000u)
/*----EMUIHBIE Tokens----*/
#define CSL_PSC_PDCTL0_EMUIHBIE_DISABLE (0x00000000u)
#define CSL_PSC_PDCTL0_EMUIHBIE_ENABLE  (0x00000001u)

#define CSL_PSC_PDCTL0_NEXT_MASK        (0x00000001u)
#define CSL_PSC_PDCTL0_NEXT_SHIFT       (0x00000000u)
#define CSL_PSC_PDCTL0_NEXT_RESETVAL    (0x00000001u)
/*----NEXT Tokens----*/
#define CSL_PSC_PDCTL0_NEXT_OFF         (0x00000000u)
#define CSL_PSC_PDCTL0_NEXT_ON          (0x00000001u)

#define CSL_PSC_PDCTL0_RESETVAL         (0x001FF101u)

/* PDCTL1 */

#define CSL_PSC_PDCTL1_WAKECNT_MASK     (0x00FF0000u)
#define CSL_PSC_PDCTL1_WAKECNT_SHIFT    (0x00000010u)
#define CSL_PSC_PDCTL1_WAKECNT_RESETVAL (0x0000001Fu)

#define CSL_PSC_PDCTL1_PDMODE_MASK      (0x0000F000u)
#define CSL_PSC_PDCTL1_PDMODE_SHIFT     (0x0000000Cu)
#define CSL_PSC_PDCTL1_PDMODE_RESETVAL  (0x0000000Fu)
/*----PDMODE Tokens----*/
#define CSL_PSC_PDCTL1_PDMODE_OFF       (0x00000000u)
#define CSL_PSC_PDCTL1_PDMODE_RAM_OFF   (0x00000008u)
#define CSL_PSC_PDCTL1_PDMODE_DEEP_SLEEP (0x00000009u)
#define CSL_PSC_PDCTL1_PDMODE_LIGHT_SLEEP (0x0000000Au)
#define CSL_PSC_PDCTL1_PDMODE_RETENTION (0x0000000Bu)
#define CSL_PSC_PDCTL1_PDMODE_ON        (0x0000000Fu)

#define CSL_PSC_PDCTL1_EMUIHBIE_MASK    (0x00000200u)
#define CSL_PSC_PDCTL1_EMUIHBIE_SHIFT   (0x00000009u)
#define CSL_PSC_PDCTL1_EMUIHBIE_RESETVAL (0x00000000u)
/*----EMUIHBIE Tokens----*/
#define CSL_PSC_PDCTL1_EMUIHBIE_DISABLE (0x00000000u)
#define CSL_PSC_PDCTL1_EMUIHBIE_ENABLE  (0x00000001u)

#define CSL_PSC_PDCTL1_NEXT_MASK        (0x00000001u)
#define CSL_PSC_PDCTL1_NEXT_SHIFT       (0x00000000u)
#define CSL_PSC_PDCTL1_NEXT_RESETVAL    (0x00000001u)
/*----NEXT Tokens----*/
#define CSL_PSC_PDCTL1_NEXT_OFF         (0x00000000u)
#define CSL_PSC_PDCTL1_NEXT_ON          (0x00000001u)

#define CSL_PSC_PDCTL1_RESETVAL         (0x001FF101u)

/* PDCFG0 */

#define CSL_PSC_PDCFG0_PDLOCK_MASK      (0x00000008u)
#define CSL_PSC_PDCFG0_PDLOCK_SHIFT     (0x00000003u)
#define CSL_PSC_PDCFG0_PDLOCK_RESETVAL  (0x00000001u)
/*----PD LOCK Tokens----*/
#define CSL_PSC_PDCFG0_PDLOCK_YES       (0x00000000u)
#define CSL_PSC_PDCFG0_PDLOCK_NO        (0x00000001u)

#define CSL_PSC_PDCFG0_ICEPICK_MASK     (0x00000004u)
#define CSL_PSC_PDCFG0_ICEPICK_SHIFT    (0x00000002u)
#define CSL_PSC_PDCFG0_ICEPICK_RESETVAL (0x00000001u)
/*----ICEPICK Tokens----*/
#define CSL_PSC_PDCFG0_ICEPICK_ABSENT   (0x00000000u)
#define CSL_PSC_PDCFG0_ICEPICK_PRESENT  (0x00000001u)

#define CSL_PSC_PDCFG0_RAM_PSM_MASK     (0x00000002u)
#define CSL_PSC_PDCFG0_RAM_PSM_SHIFT    (0x00000001u)
#define CSL_PSC_PDCFG0_RAM_PSM_RESETVAL (0x00000000u)
/*----RAM_PSM Tokens----*/
#define CSL_PSC_PDCFG0_RAM_PSM_NO       (0x00000000u)
#define CSL_PSC_PDCFG0_RAM_PSM_YES      (0x00000001u)

#define CSL_PSC_PDCFG0_ALWAYSON_MASK    (0x00000001u)
#define CSL_PSC_PDCFG0_ALWAYSON_SHIFT   (0x00000000u)
#define CSL_PSC_PDCFG0_ALWAYSON_RESETVAL (0x00000001u)
/*----ALWAYSON Tokens----*/
#define CSL_PSC_PDCFG0_ALWAYSON_NO      (0x00000000u)
#define CSL_PSC_PDCFG0_ALWAYSON_YES     (0x00000001u)

#define CSL_PSC_PDCFG0_RESETVAL         (0x0000000Du)

/* PDCFG1 */

#define CSL_PSC_PDCFG1_PDLOCK_MASK      (0x00000008u)
#define CSL_PSC_PDCFG1_PDLOCK_SHIFT     (0x00000003u)
#define CSL_PSC_PDCFG1_PDLOCK_RESETVAL  (0x00000001u)
/*----PD LOCK Tokens----*/
#define CSL_PSC_PDCFG1_PDLOCK_YES       (0x00000000u)
#define CSL_PSC_PDCFG1_PDLOCK_NO        (0x00000001u)

#define CSL_PSC_PDCFG1_ICEPICK_MASK     (0x00000004u)
#define CSL_PSC_PDCFG1_ICEPICK_SHIFT    (0x00000002u)
#define CSL_PSC_PDCFG1_ICEPICK_RESETVAL (0x00000001u)
/*----ICEPICK Tokens----*/
#define CSL_PSC_PDCFG1_ICEPICK_ABSENT   (0x00000000u)
#define CSL_PSC_PDCFG1_ICEPICK_PRESENT  (0x00000001u)

#define CSL_PSC_PDCFG1_RAM_PSM_MASK     (0x00000002u)
#define CSL_PSC_PDCFG1_RAM_PSM_SHIFT    (0x00000001u)
#define CSL_PSC_PDCFG1_RAM_PSM_RESETVAL (0x00000001u)
/*----RAM_PSM Tokens----*/
#define CSL_PSC_PDCFG1_RAM_PSM_NO       (0x00000000u)
#define CSL_PSC_PDCFG1_RAM_PSM_YES      (0x00000001u)

#define CSL_PSC_PDCFG1_ALWAYSON_MASK    (0x00000001u)
#define CSL_PSC_PDCFG1_ALWAYSON_SHIFT   (0x00000000u)
#define CSL_PSC_PDCFG1_ALWAYSON_RESETVAL (0x00000000u)
/*----ALWAYSON Tokens----*/
#define CSL_PSC_PDCFG1_ALWAYSON_NO      (0x00000000u)
#define CSL_PSC_PDCFG1_ALWAYSON_YES     (0x00000001u)

#define CSL_PSC_PDCFG1_RESETVAL         (0x0000000Eu)

/* MDSTAT */

#define CSL_PSC_MDSTAT_EMUIHB_MASK      (0x00020000u)
#define CSL_PSC_MDSTAT_EMUIHB_SHIFT     (0x00000011u)
#define CSL_PSC_MDSTAT_EMUIHB_RESETVAL  (0x00000000u)
/*----EMUIHB Tokens----*/
#define CSL_PSC_MDSTAT_EMUIHB_DISABLE   (0x00000000u)
#define CSL_PSC_MDSTAT_EMUIHB_ENABLE    (0x00000001u)

#define CSL_PSC_MDSTAT_EMURST_MASK      (0x00010000u)
#define CSL_PSC_MDSTAT_EMURST_SHIFT     (0x00000010u)
#define CSL_PSC_MDSTAT_EMURST_RESETVAL  (0x00000000u)
/*----EMURST Tokens----*/
#define CSL_PSC_MDSTAT_EMURST_DISABLE   (0x00000000u)
#define CSL_PSC_MDSTAT_EMURST_ENABLE    (0x00000001u)

#define CSL_PSC_MDSTAT_MCKOUT_MASK      (0x00001000u)
#define CSL_PSC_MDSTAT_MCKOUT_SHIFT     (0x0000000Cu)
#define CSL_PSC_MDSTAT_MCKOUT_RESETVAL  (0x00000000u)
/*----MCKOUT Tokens----*/
#define CSL_PSC_MDSTAT_MCKOUT_OFF       (0x00000000u)
#define CSL_PSC_MDSTAT_MCKOUT_ON        (0x00000001u)

#define CSL_PSC_MDSTAT_MRSTDONE_MASK    (0x00000800u)
#define CSL_PSC_MDSTAT_MRSTDONE_SHIFT   (0x0000000Bu)
#define CSL_PSC_MDSTAT_MRSTDONE_RESETVAL (0x00000000u)
/*----MRSTDONE Tokens----*/
#define CSL_PSC_MDSTAT_MRSTDONE_COMPLETE (0x00000000u)
#define CSL_PSC_MDSTAT_MRSTDONE_INCOMPLETE (0x00000001u)

#define CSL_PSC_MDSTAT_MRST_MASK        (0x00000400u)
#define CSL_PSC_MDSTAT_MRST_SHIFT       (0x0000000Au)
#define CSL_PSC_MDSTAT_MRST_RESETVAL    (0x00000000u)
/*----MRST Tokens----*/
#define CSL_PSC_MDSTAT_MRST_ASSERT      (0x00000000u)
#define CSL_PSC_MDSTAT_MRST_DEASSERT    (0x00000001u)

#define CSL_PSC_MDSTAT_LRSTDONE_MASK    (0x00000200u)
#define CSL_PSC_MDSTAT_LRSTDONE_SHIFT   (0x00000009u)
#define CSL_PSC_MDSTAT_LRSTDONE_RESETVAL (0x00000000u)
/*----LRSTDONE Tokens----*/
#define CSL_PSC_MDSTAT_LRSTDONE_NOTDONE (0x00000000u)
#define CSL_PSC_MDSTAT_LRSTDONE_DONE    (0x00000001u)

#define CSL_PSC_MDSTAT_LRST_MASK        (0x00000100u)
#define CSL_PSC_MDSTAT_LRST_SHIFT       (0x00000008u)
#define CSL_PSC_MDSTAT_LRST_RESETVAL    (0x00000000u)
/*----LRST Tokens----*/
#define CSL_PSC_MDSTAT_LRST_ASSERT      (0x00000000u)
#define CSL_PSC_MDSTAT_LRST_DEASSERT    (0x00000001u)

#define CSL_PSC_MDSTAT_STATE_MASK       (0x0000003Fu)
#define CSL_PSC_MDSTAT_STATE_SHIFT      (0x00000000u)
#define CSL_PSC_MDSTAT_STATE_RESETVAL   (0x00000000u)
/*----STATE Tokens----*/
#define CSL_PSC_MDSTAT_STATE_SWRSTDISABLE (0x00000000u)
#define CSL_PSC_MDSTAT_STATE_SYNCRST    (0x00000001u)
#define CSL_PSC_MDSTAT_STATE_DISABLE    (0x00000002u)
#define CSL_PSC_MDSTAT_STATE_ENABLE     (0x00000003u)
#define CSL_PSC_MDSTAT_STATE_AUTOSLEEP  (0x00000004u)
#define CSL_PSC_MDSTAT_STATE_AUTOWAKE   (0x00000005u)

#define CSL_PSC_MDSTAT_RESETVAL         (0x00000000u)

/* MDCTL */

#define CSL_PSC_MDCTL_FORCE_MASK        (0x80000000u)
#define CSL_PSC_MDCTL_FORCE_SHIFT       (0x0000001Fu)
#define CSL_PSC_MDCTL_FORCE_RESETVAL    (0x00000000u)
/*----FORCE Tokens----*/
#define CSL_PSC_MDCTL_FORCE_DISABLE     (0x00000000u)
#define CSL_PSC_MDCTL_FORCE_ENABLE      (0x00000001u)

#define CSL_PSC_MDCTL_EMUIHBIE_MASK     (0x00000400u)
#define CSL_PSC_MDCTL_EMUIHBIE_SHIFT    (0x0000000Au)
#define CSL_PSC_MDCTL_EMUIHBIE_RESETVAL (0x00000000u)
/*----EMUIHBIE Tokens----*/
#define CSL_PSC_MDCTL_EMUIHBIE_DISABLE  (0x00000000u)
#define CSL_PSC_MDCTL_EMUIHBIE_ENABLE   (0x00000001u)

#define CSL_PSC_MDCTL_EMURSTIE_MASK     (0x00000200u)
#define CSL_PSC_MDCTL_EMURSTIE_SHIFT    (0x00000009u)
#define CSL_PSC_MDCTL_EMURSTIE_RESETVAL (0x00000000u)
/*----EMURSTIE Tokens----*/
#define CSL_PSC_MDCTL_EMURSTIE_DISABLE  (0x00000000u)
#define CSL_PSC_MDCTL_EMURSTIE_ENABLE   (0x00000001u)

#define CSL_PSC_MDCTL_LRST_MASK         (0x00000100u)
#define CSL_PSC_MDCTL_LRST_SHIFT        (0x00000008u)
#define CSL_PSC_MDCTL_LRST_RESETVAL     (0x00000000u)
/*----LRST Tokens----*/
#define CSL_PSC_MDCTL_LRST_ASSERT       (0x00000000u)
#define CSL_PSC_MDCTL_LRST_DEASSERT     (0x00000001u)

#define CSL_PSC_MDCTL_NEXT_MASK         (0x0000001Fu)
#define CSL_PSC_MDCTL_NEXT_SHIFT        (0x00000000u)
#define CSL_PSC_MDCTL_NEXT_RESETVAL     (0x00000000u)
/*----NEXT Tokens----*/
#define CSL_PSC_MDCTL_NEXT_SWRSTDISABLE (0x00000000u)
#define CSL_PSC_MDCTL_NEXT_SYNCRST      (0x00000001u)
#define CSL_PSC_MDCTL_NEXT_DISABLE      (0x00000002u)
#define CSL_PSC_MDCTL_NEXT_ENABLE       (0x00000003u)
#define CSL_PSC_MDCTL_NEXT_AUTOSLEEP    (0x00000004u)
#define CSL_PSC_MDCTL_NEXT_AUTOWAKE     (0x00000005u)

#define CSL_PSC_MDCTL_RESETVAL          (0x00000000u)

#ifdef __cplusplus
}
#endif
#endif
