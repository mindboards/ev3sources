/*
 * socketcan/can/isotp.h
 *
 * Definitions for isotp CAN sockets
 *
 * $Id: isotp.h 1038 2009-08-21 10:00:21Z hartkopp $
 *
 * Author: Oliver Hartkopp <oliver.hartkopp@volkswagen.de>
 * Copyright (c) 2008 Volkswagen Group Electronic Research
 * All rights reserved.
 *
 * Send feedback to <socketcan-users@lists.berlios.de>
 *
 */

#ifndef CAN_ISOTP_H
#define CAN_ISOTP_H

#include <socketcan/can.h>

#define SOL_CAN_ISOTP (SOL_CAN_BASE + CAN_ISOTP)

/* for socket options affecting the socket (not the global system) */

#define CAN_ISOTP_OPTS		1
#define CAN_ISOTP_RECV_FC	2

struct can_isotp_options {

	__u32 flags;		/* set flags for isotp behaviour.	*/
				/* __u32 value : flags see below	*/

	__u32 frame_txtime;	/* frame transmission time (N_As/N_Ar)	*/
				/* __u32 value : time in nano secs	*/

	__u8  ext_address;	/* set address for extended addressing	*/
				/* __u8 value : extended address	*/

	__u8  txpad_content;	/* set content of padding byte (tx)	*/
				/* __u8 value : content	on tx path	*/

	__u8  rxpad_content;	/* set content of padding byte (rx)	*/
				/* __u8 value : content	on rx path	*/
};

struct can_isotp_fc_options {

	__u8  bs;		/* blocksize provided in FC frame	*/
				/* __u8 value : blocksize. 0 = off	*/

	__u8  stmin;		/* separation time provided in FC frame	*/
				/* __u8 value :				*/
				/* 0x00 - 0x7F : 0 - 127 ms		*/
				/* 0x80 - 0xF0 : reserved		*/
				/* 0xF1 - 0xF9 : 100 us - 900 us	*/
				/* 0xFA - 0xFF : reserved		*/

	__u8  wftmax;		/* max. number of wait frame transmiss.	*/
				/* __u8 value : 0 = omit FC N_PDU WT	*/
};


/* flags for isotp behaviour */

#define CAN_ISOTP_LISTEN_MODE	0x01	/* listen only (do not send FC) */
#define CAN_ISOTP_EXTEND_ADDR	0x02	/* enable extended addressing */
#define CAN_ISOTP_TX_PADDING	0x04	/* enable CAN frame padding tx path */
#define CAN_ISOTP_RX_PADDING	0x08	/* enable CAN frame padding rx path */
#define CAN_ISOTP_CHK_PAD_LEN	0x10	/* check received CAN frame padding */
#define CAN_ISOTP_CHK_PAD_DATA	0x20	/* check received CAN frame padding */
#define CAN_ISOTP_HALF_DUPLEX	0x40	/* half duplex error state handling */


/* default values */

#define CAN_ISOTP_DEFAULT_FLAGS		0
#define CAN_ISOTP_DEFAULT_EXT_ADDRESS	0x00
#define CAN_ISOTP_DEFAULT_RXPAD_CONTENT	0x00
#define CAN_ISOTP_DEFAULT_TXPAD_CONTENT	0x00
#define CAN_ISOTP_DEFAULT_FRAME_TXTIME	0
#define CAN_ISOTP_DEFAULT_RECV_BS	0
#define CAN_ISOTP_DEFAULT_RECV_STMIN	0x00
#define CAN_ISOTP_DEFAULT_RECV_WFTMAX	0

/*
 * Remark on CAN_ISOTP_DEFAULT_RECV_* values:
 *
 * We can strongly assume, that the Linux Kernel implementation of
 * CAN_ISOTP is capable to run with BS=0, STmin=0 and WFTmax=0.
 * But as we like to be able to behave as a commonly available ECU,
 * these default settings can be changed via sockopts.
 * For that reason the STmin value is intentionally _not_ checked for
 * consistency and copied directly into the flow control (FC) frame.
 *
 */

#endif
