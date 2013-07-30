/*
 * LEGO® MINDSTORMS EV3
 *
 * Copyright (C) 2010-2013 The LEGO Group
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef C_DAISY_H_
#define C_DAISY_H_
#include "libusb.h"

      /* The public libusb header file
			 *
			 * The libusb library is free software and it can
			 * redistribute it and/or modify it under the terms 
			 * of the GNU Lesser General Publil License as 				 
			 * published by the Free Software Foundation; either
 			 * version 2.1 of the License, or (at your option) any 				 
			 * later version.
			 * See the libusb.h for more informations
			 *
			 * Thanks to: Daniel Drake and Johannes Erdfelt! */

#include "lms2012.h"

#include "c_com.h"

#define   FALSE                         0
#define   TRUE                          1

/*
  Daisy Chain Command Bytes:

  ,------,------,------,------,------,------,------,------,------,------,

  |Byte 0|Byte 1|Byte 2|Byte 3|Byte 4|Byte 5|Byte 6|Byte 7|Byte 8|Byte 9|

  '------'------'------'------'------'------'------'------'------'------'

  Byte 0 – 1: Command size, Little Endian\n

  Byte 2 – 3: Message counter, Little Endian\n

  Byte 4:     Command type. see following defines
*/

#define     DAISY_COMMAND_REPLY             0x0A    //  Daisy command, reply required
#define     DAISY_COMMAND_NO_REPLY          0x8A    //  Daisy command, reply not required

/*
  Byte 5:   Daisy sub Command. see following defines
*/

#define     DAISY_CHAIN_DOWNSTREAM            0xA0  //  Write down into the branch of DaisyChained Bricks
#define     DAISY_CHAIN_UPSTREAM              0xA1  //  Data packet from port expanding (DaisyChained) downstream Bricks
#define     DAISY_CHAIN_INFO                  0xA2  //  Sensor information packet (max. 54 bytes)
#define     DAISY_UNLOCK_SLAVE                0xA3  //  Ask the slave to start pushing data upstream
#define		  DAISY_SET_TYPE					          0xA4	//  Set mode/type downstream (no reply needed - status via array)
#define		  DAISY_CHAIN_DOWNSTREAM_WITH_BUSY	0XA5	//  Payload also includes Magic Cookies (4 pcs)

/*
  Byte 6:     Layer Counter (Down = source)         //  Incremented for each layer hit downstream
                            (Up   = destination)    //  Decremented for each layer hit - when zero destination reached.
  Byte 7:     Layer Counter (Down = destination)    //  Decremented for each layer hit - when zero destination reached.
                            (Up   = source)         //  Incremented for each layer hit upstream

  Byte 8:     The normal payload. E.g. bytecode(s) with parameters or sensor-data


Daisy Chain Command Response Bytes:

  ,------,------,------,------,------,------,------,------,

  |Byte 0|Byte 1|Byte 2|Byte 3|      |      |      |Byte n|

  '------'------'------'------'------'------'------'------'

  Byte 0 – 1: Reply size, Little Endian\n

  Byte 2 – 3: Message counter, Little Endian\n

  Byte 4:     Reply type. see following defines
*/

#define     DAISY_REPLY                     0x08    //  Daisy command reply
#define     DAISY_REPLY_ERROR               0x09    //  Daisy command reply error

/*
  Byte 5 - n: Response buffer (Layered sensor information)

  Byte 5:     Layer Counter (Down = source)         //  Incremented for each layer hit downstream
                            (Up   = destination)    //  Decremented for each layer hit - when zero destination reached.
  Byte 6:     Layer Counter (Down = destination)    //  Decremented for each layer hit - when zero destination reached.
                            (Up   = source)         //  Incremented for each layer hit upstream

  Byte 7:     The layered sensordata

*/

/*** DAISY USB defines ***/

#define DAISY_VENDOR_ID		0x0694        // LEGO Group
#define DAISY_PRODUCT_ID	0x0005				// EV3, the 5'th USB device
#define INTERFACE_NUMBER	0							// One and only

#define DIR_IN			0x80								// USB EP direction
#define DIR_OUT			0x00								// -
#define DAISY_INT_EP_ADR	0x01 					// HW specific

#define	DAISY_INTERRUPT_EP_IN	DIR_IN + DAISY_INT_EP_ADR  			// Interrupt endpoint used for UPSTREAM
#define	DAISY_INTERRUPT_EP_OUT	DIR_OUT + DAISY_INT_EP_ADR  	// Interrupt endpoint used for DOWNSTREAM

#define DAISY_MAX_EP_IN_SIZE	64															// FULL speed record size
#define DAISY_MAX_EP_OUT_SIZE	64															// FULL speed record size
#define DAISY_DEFAULT_MAX_EP_SIZE	DAISY_MAX_EP_OUT_SIZE
#define DAISY_UPSTREAM_DATA_LENGTH	DAISY_DEFAULT_MAX_EP_SIZE
#define DAISY_DEFAULT_TIMEOUT	1000														// Timeout set to 1 sec by default
#define NO_USER_DATA	0																				// No user data embedded in transfer
#define DAISY_MAX_INPUT_PER_LAYER   4                         // Hardware size
#define DAISY_MAX_OUTPUT_PER_LAYER  4                         // -
#define DAISY_MAX_SENSORS_PER_LAYER (DAISY_MAX_INPUT_PER_LAYER + DAISY_MAX_OUTPUT_PER_LAYER)  // Total per layer
#define DAISY_MAX_LAYER_DEPT 4                                // Max layer depth

#define DEVICE_MAX_DATA  32                                   // Max data size
#define BUSYFLAGS	16                                          // Motor busyflags (grand total)
#define DEVICE_MAX_INCL_BUSYFLAGS	(DEVICE_MAX_DATA + BUSYFLAGS)

#define SLAVE_PROD_ID   "0005"
#define SLAVE_VENDOR_ID "0694"

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  DATA8   PayLoad[];
}DAISY_POLL;

#define   SIZEOF_DAISY_POLL       6

typedef struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  DATA8   Status;
}DAISY_UNLOCK_REPLY;

#define   SIZEOF_UNLOCK_REPLY       7

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Layer;
  UBYTE   SensorNo;
  DATA8   PayLoad[];
}DAISY_READ;

#define   SIZEOF_DAISY_READ       8

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  DATA8   PayLoad[];
}DAISY_WRITE;

#define   SIZEOF_DAISY_WRITE       6

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  DATA8   Layer;
  DATA8   SensorNo;
  DATA8   Status;
  DATA8   Type;
  DATA8   Mode;
  UBYTE   DeviceData[DEVICE_MAX_DATA];
  DATA8   CheckSum;
  UBYTE	  FirstCookie;
}DAISY_DEVICE_DATA;

#define   SIZEOF_DAISY_DEVICE_DATA_PROLOG 11
#define   SIZEOF_REPLY_DAISY_READ (DAISY_DEFAULT_MAX_EP_SIZE - sizeof(CMDSIZE))
#define   DAISY_DEVICE_PAYLOAD_SIZE (DEVICE_MAX_DATA + 4)   // Status, Type, Mode, DeviceData, CheckSum
#define   DAISY_DEVICE_CHKSUM_SIZE 1
#define   DAISY_MAX_INPUT_SENSOR_INDEX 3
#define   DAISY_SENSOR_OUTPUT_OFFSET 12
#define   DAISY_SENSOR_DATA_SIZE (SIZEOF_DAISY_DEVICE_DATA_PROLOG + DEVICE_MAX_DATA + DAISY_DEVICE_CHKSUM_SIZE - sizeof(CMDSIZE))
#define   BUSYFLAGS_START_POS	(SIZEOF_DAISY_DEVICE_DATA_PROLOG + DEVICE_MAX_DATA + DAISY_DEVICE_CHKSUM_SIZE)
typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Info[];
}DAISY_INFO;

#define   SIZEOF_DAISY_INFO       6
#define   SIZEOF_DAISY_INFO_READ (DAISY_DEFAULT_MAX_EP_SIZE - SIZEOF_DAISY_INFO)

#define LAYER_POS 6
#define SENSOR_POS  (LAYER_POS + 1)


#define DAISY_MAX_DATASIZE DAISY_DEFAULT_MAX_EP_SIZE
#define DAISY_DATA_PACKET (DAISY_DEFAULT_MAX_EP_SIZE - sizeof(CMDSIZE))

#define PAYLOAD_OFFSET  4

typedef   struct            // Used for downstream
{
  UBYTE   DaisyCmd;
  UBYTE   Layer;
  UBYTE   SensorNo;
  DATA8   PayLoad[];
}
DAISYCMD;

RESULT  cDaisyReady(void);
int     cDaisyWrite(void);                              // Write (ASYNC) one packet DOWNSTREAM
RESULT  cDaisyGetDownstreamData(DATA8 Layer,DATA8 Sensor,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData);
void    cDaisyPollFromDownstream(void);                 // Read (ASYNC) one packet UPSTREAM
RESULT  cDaisyDownStreamCmd(DATA8 *pData, DATA8 Length, DATA8 Layer);	// DaisyChained commands e.g. Motor cmd's																																// (up from a lower layer)
int     cDaisyWriteDone(void);												  // Async check INTERNAL for Write finished
int     cDaisyGetLastWriteState(void);						      // Async check used for EXTERNAL check for Write finished
int     cDaisyGetLastWriteResult(void);							    // Get last ERROR code (or success ;-))
RESULT  cDaisyInit(void);												        // Initialize LIBUSB and open an USB DaisyChain link
RESULT  cDaisyOpen(void);												        // Open() the DaisyChain link to the slave
RESULT  cDaisyClose(void);												      // Close() the DaisyChain stuff
RESULT  cDaisyExit(void);												        // Close the DaisyChain link and deinitializes the LIBUSB
void    cDaisyControl(void);     									      // Get cycles for the DAISY stuff
void    cDaisySetTimeout(int TimeOut);						      // TimeOut value for read and write
DATA8   cDaisyGetUsbUpStreamSpeed(void);                // The connection speed upstream (Gadget side)
void    cDaisyReadFromDownStream(void);                 // Read array and hand over upstream
int     cDaisyGetInterruptPacketSize(void);             // Max packet the Daisychained device supports
void    cDaisyCmd(RXBUF *pRxBuf, TXBUF *pTxBuf);        // "Internal" Daisy and daisy-chained commands
RESULT  cDaisySetDeviceInfo(DATA8 Length,UBYTE *pInfo);	// Device Info pushed by INPUT module
RESULT	cDaisyGetDeviceInfo(DATA8 Length,UBYTE *pInfo); // Get Device Info for upstream TX
RESULT	cDaisySetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode);
UWORD 	cDaisyData(UBYTE **pData);	                    // Separate buffered DaisyData - NOT conflicting with normal transfer e.g. answers, errors etc.
void	  cDaisyStuffTxed(void);	                        // Tell when bits'n'bytes has left first (our) BUFFER
uint    GetDaisyPushCounter(void);
void    ResetDaisyPushCounter(void);
RESULT  cDaisyChained(void);
void 	  DecrementDaisyPushCounter(void);
void    cDaisyPushUpStream(void);                       // Flood upward
void    cDaisyPrepareNext(void);
void    SetUnlocked(int Status);
int     GetUnlocked(void);
void    SetSlaveUnlocked(int Status);
int     GetSlaveUnlocked(void);
DATA8   cDaisyGetUsbUpStreamSpeed(void);
void    cDaisySetOwnLayer(UBYTE Layer);
UBYTE   cDaisyGetOwnLayer(void);
UBYTE   cDaisyCheckBusyBit(UBYTE Layer, UBYTE PortBits);  // Used by the "ONFOR" motor stuff
RESULT  cDaisyCheckBusyIndex(UBYTE Layer, UBYTE Port);
void    cDaisySetBusyFlags(UBYTE Layer, UBYTE Port, UBYTE MagicCookie);
void    cDaisySetOwnLayer(UBYTE Layer);                   // The nice to known - used for "self"-layer insertion downstream
RESULT  cDaisyMotorDownStream(DATA8 *pData, DATA8 Length, DATA8 Layer, DATA8 PortField);	// Looks like the normal CMD, but
																						              // port field added - i.e. NOT too much
																						              // specific protocol down @ the transport layer
enum
{
  DAISY_WR_NOT_CONNECTED,   // No DAISY device connected :-(
  DAISY_WR_CONNECTED,       // Connected but not opened yet
  DAISY_WR_IDLE,            // Connected and an unlocked "slave"-device
  DAISY_WR_REQUESTED,				// Write session started
  DAISY_WR_COMPLETED,				// Write finished asyncronous
  DAISY_WR_TIMED_OUT,				// Write process timed out
  DAISY_WR_FAIL							// Write failed (non-specific)
}
DAISY_WRITE_STATES;

enum
{
  DAISY_RD_IDLE,              // Daisy READ control state idling
  DAISY_RD_REQUESTED,         // Read request issued - async
  DAISY_RD_DONE,              // Read done - happily
                              // Read ERRORs:
  DAISY_RD_DISCONNECTED,      // Daisy chained device downstream disconnected :-(
  DAISY_RD_TIMEDOUT,          // A read is timed out - too lazy??
  DAISY_RD_ERROR              // Read NON-specific ERROR
}
DAISY_READ_STATES;

enum
{
  DAISY_DOWN_DISCONNECTED,    // Daisy downstream states
  DAISY_DOWN_CONNECTED,
  DAISY_DOWN_UNLOCKING,
  DAISY_CHECK_UNLOCK,
  DAISY_DOWN_UNLOCKED,
  DAISY_DOWN_SET_DEVICETYPE,
  DAISY_DOWNSTREAM_CHECK_WRITE_DONE
}
DAISY_DOWN_CONNECTION_STATES;

enum
{
  DAISY_PUSH_NOT_CONNECTED,
  DAISY_PUSH_CONNECTED
}
DAISY_PUSH_CONNECTION_STATES;

/*** Below LibUsb ERROR codes - I will try to lower the count upwards :-) ***/

// LIBUSB_ERROR_IO						= 	-1		Input/output error
// LIBUSB_ERROR_INVALID_PARAM = 	-2 		Invalid parameter
// LIBUSB_ERROR_ACCESS 				= 	-3		Access denied (insufficient permissions)
// LIBUSB_ERROR_NO_DEVICE 		= 	-4		No such device (it may have been disconnected)
// LIBUSB_ERROR_NOT_FOUND 		= 	-5		Entity not found
// LIBUSB_ERROR_BUSY 					= 	-6		Resource busy
// LIBUSB_ERROR_TIMEOUT 			= 	-7		Operation timed out#define   SIZEOF_DAISY_INFO       6
// LIBUSB_ERROR_OVERFLOW 			= 	-8		Overflow
// LIBUSB_ERROR_PIPE 					= 	-9		Pipe error
// LIBUSB_ERROR_INTERRUPTED 	= 	-10		System call interrupted (perhaps due to signal)
// LIBUSB_ERROR_NO_MEM 				= 	-11		Insufficient memory
// LIBUSB_ERROR_NOT_SUPPORTED = 	-12		Operation not supported or unimplemented on this platform
// LIBUSB_ERROR_OTHER 				= 	-99 	Other error
// DAISY_ERROR_RD_DONE				=		-80		Signal READ done - NOT an error

enum
{
  DAISY_WR_OK = LIBUSB_SUCCESS,							      // Write started OK - REMEMBER we try to be asyncronous :-)
  DAISY_WR_DONE = DAISY_WR_OK,							      // Request fullfilled else DAISY_WR_NOT_FINISHED
  DAISY_WR_NOT_FINISHED = LIBUSB_ERROR_BUSY,			// Previous write not finished yet :-(
  DAISY_WR_ERROR = LIBUSB_ERROR_OTHER,					  // ERROR bucket - NOT specified
  DAISY_WR_TIMEDOUT = LIBUSB_ERROR_TIMEOUT,				// Too slow...
  DAISY_WR_DISCONNECTED = LIBUSB_ERROR_NO_DEVICE	// No device - I must be disconnected
}
DAISY_WR_ERROR_CODES;

// PAYLOAD Helper defines

enum
{
  STATUS_POS,
  TYPE_POS,
  MODE_POS,
  DEVICE_DATA_POS
}
PAYLOAD_POSITIONS;

enum
{
  LEN1,
  LEN2,
  MSG1,
  MSG2,
  CMDTYP,
  SUBCMD,
  STAT
}
REPLY_INDEXES;

enum
{
  INFO_LEN1,
  INFO_LEN2,
  INFO_MSG1,
  INFO_MSG2,
  INFO_CMDTYP,
  INFO_SUBCMD,
  INFO_INFO
}
INFO_INDEXES;

#define LAYER_POS_DOWN	(STAT +1)
#define SENSOR_POS_DOWN	(LAYER_POS_DOWN +1)
#define TYPE_POS_DOWN	(SENSOR_POS_DOWN + 1)
#define MODE_POS_DOWN	(TYPE_POS_DOWN + 1)

#define LAYER_POS_TO_SLAVE  (STAT)
#define SENSOR_POS_TO_SLAVE (LAYER_POS_TO_SLAVE +1)
#define TYPE_POS_TO_SLAVE (SENSOR_POS_TO_SLAVE + 1)
#define MODE_POS_TO_SLAVE (TYPE_POS_TO_SLAVE + 1)

enum
{
  EMPTY,
  VALID
}
INFO_STATES;

#define CHECKSUM_POS  (DAISY_DEVICE_PAYLOAD_SIZE - 1)

#define BUSYTIME      25000   		    // [uS]
#define DAISY_PRIORITY_COUNT	2	      // 1:3
#define DAISY_PUSH_NOT_UNLOCKED 0x55

enum
{
  DAISY_RD_OK  = LIBUSB_SUCCESS,									  // Read request started OK - REMEMBER we try to be asyncronous :-)
  DAISY_RD_NO_DATA_YET =  LIBUSB_ERROR_BUSY,			  // Data not received yet
  //DAISY_RD_ERROR = LIBUSB_ERROR_OTHER,						// ERROR bucket - NOT specified
  //DAISY_RD_TIMEDOUT = LIBUSB_ERROR_TIMEOUT,				// Too slow...
	//DAISY_RD_DISCONNECTED = LIBUSB_ERROR_NO_DEVICE	// No device - I must be disconnected
}
DAISY_READ_ERROR_CODES;

#define TIME_TO_CHECK_FOR_DAISY_DOWNSTREAM  2000    //16000
#define DAISY_SLAVE_SEARCH_STRING "ID 0694:0005 Lego Group"

#endif /* C_DAISY_H_ */
