/*
 * linux/<file location within the kernel tree>
 *
 * Copyright (C) 2010 Texas Instruments Incorporated
 * Author: Ganeshan N
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

#ifndef _SUART_ERR_H_
#define _SUART_ERR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PRU_SUART_SUCCESS			(0u)

#define PRU_SUART_FAILURE			(-1)

/** Returns success on Soft UART transaction */
#define PRU_SUART_ERR_DEVICE_NOT_OPEN		(1u)
/** Return */
#define PRU_SUART_ERR_UARTS_INIT_FAIL		(2u)
/** Return */
#define PRU_SUART_ERR_UARTS_RESET_FAIL		(3u)
/** Return */
#define PRU_SUART_ERR_HANDLE_INVALID		(4u)
/** Return */
#define PRU_SUART_ERR_PARAMETER_INVALID		(5u)
/** Return */

/* Transmit */
#define PRU_SUART_ERR_TX			(6u)
/** Return */
#define PRU_SUART_TX_COMPLETE			(7u)
/** Return */
#define PRU_SUART_TX_BUSY			(8u)
/** Return */
#define PRU_SUART_TX_UNDERRUN			(9u)
/** Return */

/* Receive */
#define PRU_SUART_ERR_RX			(10u)
/** Return */
#define PRU_SUART_RX_COMPLETE			(11u)
/** Return */
#define PRU_SUART_RX_BUSY			(12u)
/** Return */
#define PRU_SUART_RX_OVERRUN			(13u)
/** Return */

/* API Specific Errors */
#define SUART_INVALID_TX_BAUD			(14u)
#define SUART_INVALID_OVERSAMPLING		(15u)
#define SUART_INVALID_RX_BAUD			(16u)

#define SUART_UART_IN_USE			(17u)

#define SUART_INVALID_CLKDIVISOR		(18u)
#define SUART_INVALID_UART_NUM			(19u)
#define SUART_INVALID_SR_NUM			(20u)

#ifdef __cplusplus
}				/* End of extern C */
#endif				/* #ifdef __cplusplus */
#endif
