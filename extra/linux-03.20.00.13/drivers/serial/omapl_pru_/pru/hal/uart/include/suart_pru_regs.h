/*
 * linux/<file location within the kernel tree>
 *
 * Copyright (C) 2010 Texas Instruments Incorporated
 * Author: jitendra@mistralsolutions.com
 *
 * Based on <Give reference of old kernel file from which this file is derived from>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as  published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 * whether express or implied; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifndef _SUART_PRU_REGS_H_
#define _SUART_PRU_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *====================
 * Includes
 *====================
 */
#include "tistdtypes.h"

/*
 *====================
 * Defines
 *====================
 */

#define PRU_DATARAM_BASE_ADDR			(0x01C30000u)

#define PRU0_DATARAM0_OFFSET			(0x0000u)
/** PRU0 DATA RAM base address */

#define PRU0_DATARAM1_OFFSET			(0x2000u)
/** PRU0 DATA RAM base address */

#define PRU_SUART_DATARAM0_BASE			(PRU_DATARAM_BASE_ADDR + PRU0_DATARAM0_OFFSET)
/** PRU1 DATA RAM base address */

#define PRU_SUART_DATARAM1_BASE			(PRU_DATARAM_BASE_ADDR + PRU0_DATARAM0_OFFSET)
/** PRU1 DATA RAM base address */

#define PRU_SUART_PRU0_CH0_OFFSET		(0x0000)
/** Channel 0 */
#define PRU_SUART_PRU0_CH1_OFFSET		(0x0010)
/** Channel 1 */
#define PRU_SUART_PRU0_CH2_OFFSET		(0x0020)
/** Channel 2 */
#define PRU_SUART_PRU0_CH3_OFFSET		(0x0030)
/** Channel 3 */
#define PRU_SUART_PRU0_CH4_OFFSET		(0x0040)
/** Channel 4 */
#define PRU_SUART_PRU0_CH5_OFFSET		(0x0050)
/** Channel 5 */
#define PRU_SUART_PRU0_CH6_OFFSET		(0x0060)
/** Channel 6 */
#define PRU_SUART_PRU0_CH7_OFFSET		(0x0070)
/** Channel 7 */

#define PRU_SUART_PRU0_IMR_OFFSET		(0x0080)
/** Interrupt Mask Register */

#define PRU_SUART_PRU0_ISR_OFFSET		(0x0082)
/** Interrupt Status Register */

#define PRU_SUART_PRU0_ID_ADDR      		(0x0084)
/** PRU ID Register */

#define PRU_SUART_PRU0_RX_TX_MODE      		(0x0085)

#define PRU_SUART_PRU0_DELAY_OFFSET		(0x0086)
#define PRU_SUART_PRU0_IDLE_TIMEOUT_OFFSET	(0x0088)

/* ********* PRU 1 Macros ************* */
#define PRU_SUART_PRU1_CH0_OFFSET		(0x2000)
/** Channel 0 */

#define PRU_SUART_PRU1_CH1_OFFSET		(0x2010)
/** Channel 1 */

#define PRU_SUART_PRU1_CH2_OFFSET		(0x2020)
/** Channel 2 */

#define PRU_SUART_PRU1_CH3_OFFSET		(0x2030)
/** Channel 3 */

#define PRU_SUART_PRU1_CH4_OFFSET		(0x2040)
/** Channel 4 */

#define PRU_SUART_PRU1_CH5_OFFSET		(0x2050)
/** Channel 5 */

#define PRU_SUART_PRU1_CH6_OFFSET		(0x2060)
/** Channel 6 */

#define PRU_SUART_PRU1_CH7_OFFSET		(0x2070)
/** Channel 7 */

#define PRU_SUART_PRU1_IMR_OFFSET		(0x2080)
/** Interrupt Mask Register */

#define PRU_SUART_PRU1_ISR_OFFSET		(0x2082)
/** Interrupt Status Register */

#define PRU_SUART_PRU1_ID_ADDR          	(0x2084)
/** PRU ID Register */

#define PRU_SUART_PRU1_RX_TX_MODE      		(0x2085)

#define PRU_SUART_PRU1_DELAY_OFFSET		(0x2086)

#define PRU_SUART_PRU1_IDLE_TIMEOUT_OFFSET	(0x2088)


/* SUART Channel Control Register bit descriptions */
#define PRU_SUART_CH_CTRL_MODE_SHIFT		0x0000
#define PRU_SUART_CH_CTRL_MODE_MASK		0x0003
#define PRU_SUART_CH_CTRL_TX_MODE		0x0001
#define PRU_SUART_CH_CTRL_RX_MODE		0x0002

/* Service Request */
#define PRU_SUART_CH_CTRL_SREQ_SHIFT		0x0002
#define PRU_SUART_CH_CTRL_SREQ_MASK		0x0004
#define PRU_SUART_CH_CTRL_SREQ			0x0001

/* McASP Instance */
#define PRU_SUART_CH_CTRL_MCASP_SHIFT		0x0003
#define PRU_SUART_CH_CTRL_MCASP_MASK		0x0018

#define PRU_SUART_CH_CTRL_SR_SHIFT		0x0008
#define PRU_SUART_CH_CTRL_SR_MASK		0x0F00

/* SUART channel configuration1 register descriptions */

/* clock divisor -  relative baud value */
#define PRU_SUART_CH_CONFIG1_DIVISOR_SHIFT	0x0000
#define PRU_SUART_CH_CONFIG1_DIVISOR_MASK	0x03FF
/* oversampling */
#define PRU_SUART_CH_CONFIG1_OVS_SHIFT		0x000A
#define PRU_SUART_CH_CONFIG1_OVS_MASK		0x0C00

/* SUART channel configuration2 register descriptions */
/* Bits per character */
#define PRU_SUART_CH_CONFIG2_BITPERCHAR_SHIFT	0x0000
#define PRU_SUART_CH_CONFIG2_BITPERCHAR_MASK	0x000F

/* Bits per character */
#define PRU_SUART_CH_CONFIG2_DATALEN_SHIFT	0x0008
#define PRU_SUART_CH_CONFIG2_DATALEN_MASK	0x0F00

/* SUART Channel STATUS Register*/
#define PRU_SUART_CH_STATUS_EN_BIT_MASK		0x8000

/* SUART Channel register offsets */
#define PRU_SUART_CH_CTRL_OFFSET		0x00
#define PRU_SUART_CH_CONFIG1_OFFSET		0x02
#define PRU_SUART_CH_CONFIG2_OFFSET		0x04
#define PRU_SUART_CH_TXRXSTATUS_OFFSET		0x06
#define PRU_SUART_CH_TXRXDATA_OFFSET		0x08
#define PRU_SUART_CH_BYTESDONECNTR_OFFSET	0x0C

/* SUART Event Numbers macros */
#define PRU_SUART0_TX_EVT			34
#define PRU_SUART0_RX_EVT			35
#define PRU_SUART1_TX_EVT			36
#define PRU_SUART1_RX_EVT			37
#define PRU_SUART2_TX_EVT			38
#define PRU_SUART2_RX_EVT			39
#define PRU_SUART3_TX_EVT			40
#define PRU_SUART3_RX_EVT			41
#define PRU_SUART4_TX_EVT			42
#define PRU_SUART4_RX_EVT			43
#define PRU_SUART5_TX_EVT			44
#define PRU_SUART5_RX_EVT			45
#define PRU_SUART6_TX_EVT			46
#define PRU_SUART6_RX_EVT			47
#define PRU_SUART7_TX_EVT			48
#define PRU_SUART7_RX_EVT			49

#define PRU_SUART0_TX_EVT_BIT			BIT(2)
#define PRU_SUART0_RX_EVT_BIT			BIT(3)				
#define PRU_SUART1_TX_EVT_BIT			BIT(4)				
#define PRU_SUART1_RX_EVT_BIT			BIT(5)
#define PRU_SUART2_TX_EVT_BIT			BIT(6)				
#define PRU_SUART2_RX_EVT_BIT			BIT(7)				
#define PRU_SUART3_TX_EVT_BIT			BIT(8)				
#define PRU_SUART3_RX_EVT_BIT			BIT(9)				
#define PRU_SUART4_TX_EVT_BIT			BIT(10)				
#define PRU_SUART4_RX_EVT_BIT			BIT(11)				
#define PRU_SUART5_TX_EVT_BIT			BIT(12)				
#define PRU_SUART5_RX_EVT_BIT			BIT(13)				
#define PRU_SUART6_TX_EVT_BIT			BIT(14)				
#define PRU_SUART6_RX_EVT_BIT			BIT(15)				
#define PRU_SUART7_TX_EVT_BIT			BIT(16)				
#define PRU_SUART7_RX_EVT_BIT			BIT(17)		

/**
 * \brief One line description of the structure
 *
 *  Detailed description of the structure
 */
	typedef struct {
		unsigned short CHn_Ctrl;/**< UART number (Range 1 to 16) */
		unsigned short CHn_Config1;
					  /**< Type of the UART i.e., Full UART, Half UART */
		unsigned short CHn_Config2;
				      /**< Soft UART Channel for Transmission */
		unsigned short CHn_TXRXStatus;
					 /**< Soft UART Channel for Reception */
		unsigned int CHn_TXRXData;/**< Status of the UART */
	} suart_struct_pru_regs;

#ifdef __cplusplus
}				/* End of extern C */
#endif				/* #ifdef __cplusplus */
#endif
