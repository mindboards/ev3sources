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

/*! \page DaisyChainLibrary Daisy Library
 *
 *- \subpage  DaisyChainLibraryDescription
 *- \subpage  DaisyChainLibraryCodes
 */


/*! \page DaisyChainLibraryDescription Description
 *
 *
 */


/*! \page DaisyChainLibraryCodes Byte Code Summary
 *
 *
 */

/***** Daisy var_s *****/
#include "c_input.h"
#include "c_daisy.h"
#include "c_output.h"


/*#if       (HARDWARE != SIMULATION)
*/
#include  <stdio.h>
#include <string.h>
#include  <fcntl.h>
#include  <stdlib.h>
#include  <unistd.h>
#include <sys/mman.h>

// Device vars etc.

int MaxInterruptPacketSize;
int TimeOutMs = 10;
struct libusb_device_handle *DeviceHandle;

// READ / WRITE stuff HOST (downstream side i.e the HOST side)

struct libusb_transfer *ReadTransfer;
struct libusb_transfer *WriteTransfer;
int WriteState;                         // NON-blocking states of WRITE to the downstream
int ReadState;                          // NON-blocking states of READ from the downstream side
int LastWriteResult;                    // Result from last WRITE
int BytesWritten;                       // Bytes actually written
int ReadLength;                         // Length of READ

// Layer versus Table pointers and limits

DATA8 ReadLayerPointer;
DATA8 ReadSensorPointer;
int MaxLowerLayer;

int DaisyMsgCounter;
int DaisyControlState;
UBYTE DaisyInfo;

int DownConnectionState = DAISY_DOWN_DISCONNECTED;
int PushState = DAISY_PUSH_NOT_CONNECTED;
int PlugAndPlay = TIME_TO_CHECK_FOR_DAISY_DOWNSTREAM;

UBYTE DataIn[DAISY_DEFAULT_MAX_EP_SIZE];
UBYTE DataOut[DAISY_DEFAULT_MAX_EP_SIZE];
UBYTE DaisyUpstreamDataBuffer[DAISY_DEFAULT_MAX_EP_SIZE];
DATA8 DaisyBuffers[DAISY_MAX_LAYER_DEPT][DAISY_MAX_SENSORS_PER_LAYER][DAISY_MAX_DATASIZE];

// Layer 0 (self) , Sensor No , Data bytes 0 - n
// -
// -
// Layer 3 ~ max  , Sensor No , Data bytes 0 - n

UBYTE DaisyInfoBuffer[DAISY_DEFAULT_MAX_EP_SIZE];
RESULT IsDaisyChained = FAIL;
MSGCNT ReplyNumber = 0;

DATA8 DaisyTempTypeLayer;	  // Normalised number used for direct indexing in array
DATA8 DaisyTempTypeSensor;	// -
DATA8 DaisyTempDownLayer;

USB_SPEED  *pUsbSpeed;
USB_SPEED  UpStreamConnection;

int Unlocked = FALSE;
int SlaveUnlocked = FALSE;

ULONG   BusyTimer  =  0;
uint	PushPriorityCounter = 0;
RESULT	DaisyReadyState = OK;
uint	DaisyPushCounter = 55;	      // Start with some DaisyStuff - HARD CODED :-)
uint	OldDaisyCounter = 0;

int DaisyStuffRefill = TRUE;
int DaisyUpstreamDataReady = TRUE;	// Used to flag (conduct) the dual use of the UpStream channel. Return, errors contra Daisy data

UBYTE 	cDaisyOwnLayer = 0;				  // Handy to know ;-)

UBYTE	cDaisyMasterCookieArray[16];  // Hands out a Cookie
UBYTE	DaisyBusyFlags[BUSYFLAGS];	  // Daisy BUSY flags always in a sensor transmission from downstream

									//  --- --- --- --- --- ---   --- --- --- --- --- --- ---
									// |3,3|3,2|3,1|3,0|2,3|2,2---1,2|1,1|1,0|0,3|0,2|0,1|0,0|
									//  --- --- --- --- --- ---   --- --- --- --- --- --- ---
									//
									// Each byte:
									//
									// S(tatus): 	BUSY	=	1
									// 				    READY	=	0
									//
									// M(agic)C(ookie):
									//
									// 				Together with the Layer, it forms a "sequence counter"
									//				Always incremented by 3 (due to position and if Layer included)
									// 				The reset of a given BUSY state is also done by
									//				looking at this "sequence number" i.e. Magic Cookie.
									//				A BUSY state can only be reset by the SET-cookie or a newer one!
									//
									// L(ayer)		Used by the SLAVE to know the position and where to RESET
									//				    the busy-flag (position in the row of 16 bytes) - also known by
                  //            array position
									//
									//  - -- -- -- -- -- - -
									// |S|MC|MC|MC|MC|MC|L|L|
									//  - -- -- -- -- -- - -

struct timeval TV = { 0 , 0};

void SetUnlocked(int Status)
{
	Unlocked = Status;
}

int GetUnlocked(void)
{
	return Unlocked;
}

void SetSlaveUnlocked(int Status)
{
  SlaveUnlocked = Status;
}

int GetSlaveUnlocked(void)
{
  return SlaveUnlocked;
}

int cDaisyGetLastWriteState(void)
{
	return WriteState;
}

int cDaisyGetLastWriteResult(void)
{
	return LastWriteResult;
}

void cDaisyFlagFail(DATA8 Layer, DATA8 Sensor)
{
	DaisyBuffers[Layer][Sensor][0] = FAIL;
}

void cDaisyFlagBusy(DATA8 Layer, DATA8 Sensor)
{
	DaisyBuffers[Layer][Sensor][0] = BUSY;
}

void cDaisyFlagDisconnected(DATA8 StartLayer)
{
	int Layer;
	int Sensor;

	// Set all sensor status to FAIL
	for (Layer = StartLayer; Layer < DAISY_MAX_LAYER_DEPT; Layer++)
	{
		for(Sensor = 0; Sensor < DAISY_MAX_SENSORS_PER_LAYER; Sensor++)
		{
			cDaisyFlagFail(Layer, Sensor);
		}
	}
}

void cDaisySetOwnLayer(UBYTE Layer)
{
	cDaisyOwnLayer = Layer;

	//#define DEBUG
  #undef DEBUG
	#ifdef DEBUG
		printf("SetOwnLayer = %d\n\r", cDaisyOwnLayer);
	#endif
}

UBYTE cDaisyGetOwnLayer(void)
{
	return cDaisyOwnLayer;
}

void cDaisyResetBusyTiming(void)
{
	BusyTimer  =  BUSYTIME;						          // re-shoot timer
	PushPriorityCounter = DAISY_PRIORITY_COUNT;	// init priority of BUS use
	DaisyReadyState = OK;
}

RESULT cDaisyTxDownStream(void)
{
	RESULT RetRes = FAIL;

	// ESCAPE FROM down connection errors
	if(DownConnectionState == DAISY_DOWN_DISCONNECTED) return OK;

	RetRes = cDaisyWrite();

	switch(RetRes)
	{
		case DAISY_WR_DISCONNECTED:

										//#define DEBUG
										#undef DEBUG
										#ifdef DEBUG
											printf("cDaisyDownTxStreamCmd - DAISY_WR_DISCONNECTED\n\r");
	      	  	  	  #endif

										DownConnectionState = DAISY_DOWN_DISCONNECTED;
										cDaisyFlagDisconnected(DaisyTempDownLayer);
										RetRes = FAIL;
										DaisyReadyState = FAIL;
										break;	                    // Leave unhappy :-(

			case DAISY_WR_NOT_FINISHED:	              // Started OK, but not finished. Set wait for
										                            // write completion and flag busy
										//#define DEBUG
										#undef DEBUG
										#ifdef DEBUG
											printf("cDaisyDownTxStreamCmd - DAISY_WR_NOT_FINISHED\n\r");
	      	  	  	  #endif

										DownConnectionState = DAISY_DOWNSTREAM_CHECK_WRITE_DONE;
										RetRes = OK;
										DaisyReadyState = OK;
										break;

			case  DAISY_WR_ERROR:

			              //#define DEBUG
										#undef DEBUG
										#ifdef DEBUG
											printf("cDaisyDownStreamCmd - DAISY_WR_ERROR\n\r");
	      	  	  	  #endif
										// Fall through
										// break;

			default:			DownConnectionState = DAISY_DOWN_UNLOCKED;

			              // Connected but current
										// operation went wrong!
										// If ERROR in starting the write -
										// flag error

			              //#define DEBUG
                    #undef DEBUG
                    #ifdef DEBUG
                      printf("cDaisyDownStreamCmd - DAISY_WR_ERROR\n\r");
                    #endif

										DaisyReadyState = FAIL;
									  RetRes = FAIL;
										break;
		}
	return RetRes;
}

//********** MOTOR DAISY STUFF ******************************************************************

void cDaisySetMasterCookie(uint	Index, UBYTE CookieValue)
{
	cDaisyMasterCookieArray[Index] = CookieValue;
}

void cDaisyClearMasterCookie(uint Index)
{
	cDaisyMasterCookieArray[Index] = 0x00;
}

void cDaisyIncrementMasterCookie(uint Index)
{
	if((cDaisyMasterCookieArray[Index]) < 0x1F)
			cDaisyMasterCookieArray[Index]++;
	else
		cDaisyMasterCookieArray[Index] = 0;	// "Overrun"
}

uint cDaisyGetPosFromLayerAndPort(UBYTE Layer, UBYTE Port)
{
	return (uint)(Port + (Layer * 4));
}

void cDaisySetupMagicCookies(uint StoreFrom, UBYTE Layer, UBYTE PortField)
{
    UBYTE MagicCookie = 0;
		UBYTE BitFieldPointer = 0x01; 	  // Pointing at Port 0 (1) okay 0000 0001 :-)
		uint i;
		uint LargePointer;
		uint StorePointer = StoreFrom;

		for(i = 0; i < 4; i++)
		{
			if(PortField & BitFieldPointer) // BitField set?
			{
				LargePointer = (uint)(i + (Layer * 4));       // Byte pointer within the Layer
				cDaisyIncrementMasterCookie(LargePointer);		// Adjust Cookie (newest)
				MagicCookie = (UBYTE)((cDaisyMasterCookieArray[LargePointer] << 2) & 0x7C);	// Place Cookie Magic Number
				MagicCookie |= 0x80;	                        // Set BUSY
				MagicCookie |= Layer;	                        // Place Layer
				DataOut[StorePointer] = MagicCookie;
				DaisyBusyFlags[LargePointer] = MagicCookie;	  // Store also in Masters own array
			}
			else
			{
				DataOut[StorePointer] = 0x00;
			}

			StorePointer++;			                            // Next Port
			BitFieldPointer = (UBYTE)(BitFieldPointer << 1);
		}
}

RESULT cDaisyCheckBusyIndex(UBYTE Layer, UBYTE Port)
{
	RESULT RetVal = OK;

	if(DaisyBusyFlags[cDaisyGetPosFromLayerAndPort(Layer, Port)] & 0x80)
		RetVal = BUSY;

	return RetVal;
}

UBYTE cDaisyCheckBusyBit(UBYTE Layer, UBYTE PortBits)
{
	UBYTE 	Port;	          // Iterator for checking - "flying bit"
	UBYTE	RetVal = 0;
	uint	StartPort;
	UBYTE	CheckBit = 0x01;

	StartPort = (4 * Layer);

	for(Port = StartPort ; Port < (StartPort + 4); Port++)
	{
		if((DaisyBusyFlags[Port] & 0x80) && (PortBits & CheckBit))
			RetVal |= CheckBit;
		CheckBit <<= 1;
	}

	//#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
	  printf("cDaisyCheckBusyBit = %X\n\r", RetVal);
  #endif

	return RetVal;
}

void cDaisyUpdateLocalBusyFlags(UBYTE Layer)
{
	// Used to keep the "repeating" slaves downstream up to date
	// Check if newer !!
}

void cDaisySetBusyFlags(UBYTE Layer, UBYTE Port, UBYTE MagicCookie)	// Called from cCom
{
	uint CookieIndex;

	CookieIndex = (uint)((Layer * 4) + Port);
	DaisyBusyFlags[CookieIndex] = MagicCookie;
}

RESULT cDaisyMotorDownStream(DATA8 *pData, DATA8 Length, DATA8 Layer, DATA8 PortField)
{
	RESULT Result = FAIL;
	DATA8 Len;
	DATA8 Msg1 = 0;	// Not used at the moment :-(
	DATA8 Msg2 = 0;	// -

	//#define DEBUG
  #undef DEBUG
	#ifdef DEBUG
	  printf("cDaisyMotorDownStream called\n\r");
	#endif

	if(DownConnectionState == DAISY_DOWN_DISCONNECTED) return OK;

	  // DaisyChained Motor commands using BUSY flags
		// pData points to the raw Direct Motor Command e.g.
		// len1, len2, msg1, msg2, cmdtype, global, local, bytecode"1", layer, sensor,....bytecode"n"
		// Length = total payload
		// Layer = delivery level: If 1 then the DaisyDecoration is removed and the "raw" payload is
		// tx'ed via the USB as a normal Direct Command
		// MAGIC Cookie added after the PAYLOAD

	DaisyTempDownLayer = Layer - 1;	  // Point into local table (Layer 1 at index 0, 2 @ 1 etc.

	cDaisyResetBusyTiming();		      // Make new prioritized access to channel

		if(Layer > 1)	                  // Tx as Daisy
		{
			// Setup Daisy prolog
			Len = Length + 5 + 4;		      // Payload + the prolog portion + MAGIC COOKIES
			DataOut[0] = Len;		          // The length only 1 (one) byte in Daisy
			DataOut[1] = 0;			          // -
			DataOut[2] = Msg1;		        // TODO Real MSG no
			DataOut[3] = Msg2;
			DataOut[4] = DAISY_COMMAND_NO_REPLY;
			DataOut[5] = DAISY_CHAIN_DOWNSTREAM_WITH_BUSY;		// Downstream special handling
			DataOut[6] = DaisyTempDownLayer;                  // Layer (destination?);

			// Byte 7 = PayLoad Start
			memcpy(&(DataOut[7]), pData, Length);
			cDaisySetupMagicCookies(7 + Length, Layer, PortField);

      //#define DEBUG
			#undef DEBUG
      #ifdef DEBUG
			  printf("cDaisyMotorDownStream sent as DAISY_COMMAND_NO_REPLY +  DAISY_CHAIN_DOWNSTREAM_WITH_BUSY\n\r");

			  int ii;
			  printf("DataOut =\n\r");
			  for(ii = 0; ii < (Len+2); ii++)
			    printf("%X, ", DataOut[ii]);

			  printf("DaisyBusyFlags =\n\r");
			  for(ii = 0; ii < 16; ii++)
			            printf("%X, ",DaisyBusyFlags[ii]);
			  printf("\r\n");
      #endif

		}
		else
		{
			// We send the payload only as a Direct Command
			// Setup "NORMAL" prolog

			Len = Length + 3 + 4;		// Payload + the prolog portion + 4 MAGIC COOKIES
			DataOut[0] = Len;		// The length only 1 (one) byte in Daisy
			DataOut[1] = 0;			// -
			DataOut[2] = Msg1;		// TODO Real MSG no
			DataOut[3] = Msg2;
			DataOut[4] = DIR_CMD_NO_REPLY_WITH_BUSY;
			// Byte 5 = PayLoad Start
			memcpy(&(DataOut[5]), pData, Length);

			// cDaisySetupMagicCookie(DataOutPos, Layer, Port);
			cDaisySetupMagicCookies(5 + Length, Layer, PortField);

      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
        printf("cDaisyMotorDownStream sent as a normal DIR_CMD_NO_REPLY_WITH_BUSY\n\r");
        printf("After cDaisySetupMagicCookies\n\r");
						int help2;
						for(help2 = 0; help2 < (Len + 2); help2++)
						{
							printf("%X, ", DataOut[help2]);
						}
						printf("\n\r");
      #endif
		}

		Result = cDaisyTxDownStream();

		return Result;
}

void 	cDaisyStuffRefill(void)
{
	DaisyStuffRefill = TRUE;
}

UWORD cDaisyData(UBYTE **pData)
{
	UWORD	RetVal = 0;

		*pData = DaisyUpstreamDataBuffer;		// Specific Daisy Data Buffer (NOT the answer/error one!!)
		RetVal = DAISY_UPSTREAM_DATA_LENGTH;	// As it says - We have some data

		return RetVal;
}

uint GetDaisyPushCounter(void)
{
	return DaisyPushCounter;
}

void SetDaisyPushCounter(int Count)
{
  DaisyPushCounter = Count;
}

void ResetDaisyPushCounter(void)
{
  SetDaisyPushCounter(3);
}

void DecrementDaisyPushCounter(void)
{
	if (DaisyPushCounter > 0)
		DaisyPushCounter--;
}

RESULT  cDaisyReady(void)
{
  RESULT Result = FAIL;

  if ((BusyTimer) && (PushPriorityCounter))
  {
    Result  =  BUSY;
  }
  else
  {
	   Result = DaisyReadyState;
  }

  return (Result);
}

void DaisyAsyncReadCallBack(struct libusb_transfer *ReadUsbTransfer)
{
  // Callback -> Asyncronous Read-job done

  //#define DEBUG
	#undef DEBUG
  #ifdef DEBUG
    printf("Did we reach here??\n\r");
  #endif

  switch(ReadUsbTransfer->status)
  {
      case LIBUSB_TRANSFER_COMPLETED:   // An normal completion
    	  	  	  	  	  	  	  	  	  ReadLength = ReadUsbTransfer->actual_length;
                                        ReadState = DAISY_RD_DONE;

                                        //#define DEBUG
                                        #undef DEBUG
                                        #ifdef DEBUG
                                          printf("\n\rcDaisyAsyncReadCallBack - LIBUSB_TRANSFER_COMPLETED\n\r");
                                        #endif

                                        break;

      case LIBUSB_TRANSFER_TIMED_OUT:   // The asynchronous transfer ended up in a timeout
    	  	  	  	  	  	  	  	  	  ReadState = DAISY_RD_TIMEDOUT;

    	  	  	  	  	  	  	  	  	  //#define DEBUG
										                    #undef DEBUG
                                        #ifdef DEBUG
                                          printf("\n\rcDaisyAsyncReadCallBack - LIBUSB_TRANSFER_TIMED_OUT\n\r");
                                        #endif

                                        break;

      case LIBUSB_TRANSFER_NO_DEVICE:   // The device (slave) has gone...
    	  	  	  	  	  	  	  	      ReadState = DAISY_RD_DISCONNECTED;
    	  	  	  	  	  	  	  	      DownConnectionState = DAISY_DOWN_DISCONNECTED;

    	  	  	  	  	  	  	  	      //#define DEBUG
										                    #undef DEBUG
                                        #ifdef DEBUG
                                          printf("\n\rcDaisyAsyncReadCallBack - LIBUSB_TRANSFER_NO_DEVICE\n\r");
                                        #endif

                                        break;

      default:                          // Fall through
      case LIBUSB_TRANSFER_CANCELLED:   // Fall through   Could be more detailed - but all are unhappy states ;-)
      case LIBUSB_TRANSFER_STALL:       // Fall through
      case LIBUSB_TRANSFER_OVERFLOW:    // Fall through

      case LIBUSB_TRANSFER_ERROR:       ReadState = DAISY_RD_ERROR;

                                        //#define DEBUG
                                        #ifdef DEBUG
                                          printf("\n\rcDaisyAsyncReadCallBack - LIBUSB_ALL_ERROR\n\r");
                                        #endif

                                        break;
    }
}

void cDaisyStartReadUnlockAnswer(void)
{
  // Startup of a read from downstream - are we ready (UNLOCKED)?
  int ReadResult = DAISY_RD_OK;

  libusb_fill_interrupt_transfer( ReadTransfer,
                                  DeviceHandle,
                                  DAISY_INTERRUPT_EP_IN,
                                  DataIn,
                                  DAISY_MAX_EP_IN_SIZE,
                                  &DaisyAsyncReadCallBack,
                                  NO_USER_DATA, // No user data
                                  TimeOutMs);

   ReadResult = libusb_submit_transfer(ReadTransfer);

   switch(ReadResult)
   {
     case LIBUSB_SUCCESS:          ReadResult = DAISY_RD_OK;             // Read initiated OK
                                   ReadState = DAISY_RD_REQUESTED;       // Ready for the callback to be called
                                   ReadLength = 0;                       // Nothing read yet

                                   //#define DEBUG
								                   #undef DEBUG
                                   #ifdef DEBUG
                                     printf("\n\rcDaisyStartReadUnlockAnswer - LIBUSB_SUCCESS\n\r");
                                   #endif

                                   break;

     case LIBUSB_ERROR_NO_DEVICE:  ReadResult = DAISY_RD_DISCONNECTED;   // Device gone, but can be requested later
                                   ReadState = DAISY_RD_IDLE;            // No active job - Device is disconnected
                                   DownConnectionState = DAISY_DOWN_DISCONNECTED;

                                   //#define DEBUG
                                   #undef DEBUG
                                   #ifdef DEBUG
                                     printf("\n\rDaisyStartReadUnlockAnswer - LIBUSB_ERROR_NO_DEVICE\n\r");
                                   #endif

                                   break;                                // Try again later....

     default:                      ReadResult = DAISY_RD_ERROR;          // Some ERROR should not happen at start
                                   ReadState = DAISY_RD_IDLE;            // if device present - No active job

                                   //#define DEBUG
                                   #undef DEBUG
                                   #ifdef DEBUG
                                     printf("\n\rDaisyStartReadUnlockAnswer - LIBUSB_DEFAULT\n\r");
                                   #endif

                                   break;                                // Try again later....
   }
}

RESULT cDaisyDownStreamCmd(DATA8 *pData, DATA8 Length, DATA8 Layer)
{
	RESULT RetRes = FAIL;
	DATA8 Len;
	DATA8 Msg1 = 0;	// Not used at the moment :-(
	DATA8 Msg2 = 0;	// -

	// The HOSTing brick command to the downstream friends
	// DaisyChained commands e.g. Motor cmd's
	// pData points to the raw Direct Command e.g.
	// len1, len2, msg1, msg2, cmdtype, global, local, bytecode"1", layer, sensor,....bytecode"n"
	// Length = total payload
	// Layer = delivery level: If 1 then the DaisyDecoration is removed and the "raw" payload is
	// tx'ed via the USB as a normal Direct Command

	// ESCAPE FROM down connection errors
	if(DownConnectionState == DAISY_DOWN_DISCONNECTED) return OK;

	DaisyTempDownLayer = Layer - 1; // Point into local table (Layer 1 at index 0, 2 @ 1 etc.

	cDaisyResetBusyTiming();	      // Make new prioritized access to channel

	if(Layer > 1)	                      // Tx as Daisy
	{
		// Setup Daisy prolog
		Len = Length + 5;		              // Payload + the prolog portion
		DataOut[0] = Len;		              // The length only 1 (one) byte in Daisy
		DataOut[1] = 0;			              // -
		DataOut[2] = Msg1;		            // TODO Real MSG no
		DataOut[3] = Msg2;
		DataOut[4] = DAISY_COMMAND_NO_REPLY;
		DataOut[5] = DAISY_CHAIN_DOWNSTREAM;
		DataOut[6] = DaisyTempDownLayer;  // Layer - Where to deliver
		// Byte 7 = PayLoad Start
		memcpy(&(DataOut[7]), pData, Length);
	}
	else
	{
		// We send the payload only as a Direct Command
		// Setup "NORMAL" prolog

		Len = Length + 3;		  // Payload + the prolog portion
		DataOut[0] = Len;		  // The length only 1 (one) byte in Daisy
		DataOut[1] = 0;			  // -
		DataOut[2] = Msg1;		// TODO Real MSG no
		DataOut[3] = Msg2;
		DataOut[4] = DIRECT_COMMAND_NO_REPLY;
		// Byte 5 = PayLoad Start
		memcpy(&(DataOut[5]), pData, Length);
	}

	RetRes = cDaisyTxDownStream();

	return RetRes;
}


void cDaisyCmd(RXBUF *pRxBuf, TXBUF *pTxBuf)
{
  COMCMD    *pComCmd;
  DAISY_UNLOCK_REPLY  *pDaisyUnlockReply;
  DAISYCMD *pDaisyCmd;
  DAISY_READ *pDaisyRead;
  uint	PortIndex;
  int	CommandPointer;
  UBYTE	ThisMagicCookie;

  // Here the commands come from an upstream friend or if Layer 1 from the HOSTing Brick

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printf("Here is the call from the HOST to cDaisyCmd\n\r");
  #endif

  pComCmd = (COMCMD*)(ComInstance.RxBuf[USBDEV].Buf);                         // Overall pointer to Rec. Command stuff
  pDaisyCmd = (DAISYCMD*)(*pComCmd).PayLoad;
  pDaisyRead = (DAISY_READ*)(ComInstance.RxBuf[USBDEV].Buf);
  pDaisyUnlockReply = (DAISY_UNLOCK_REPLY*)(ComInstance.TxBuf[USBDEV].Buf);   // Overall pointer to Reply stuff
  UBYTE LayerByte;

  switch((*pDaisyCmd).DaisyCmd)
  {
    case DAISY_UNLOCK_SLAVE:      // Decorate the reply
                                  pDaisyUnlockReply->CmdSize = SIZEOF_UNLOCK_REPLY - sizeof(CMDSIZE);
                                  pDaisyUnlockReply->MsgCount = pDaisyRead->MsgCount;
                                  pDaisyUnlockReply->CmdType = DAISY_REPLY;
                                  pDaisyUnlockReply->Cmd = DAISY_UNLOCK_SLAVE;
                                  pDaisyUnlockReply->Status = OK;

                                  //#define DEBUG
                                  #undef DEBUG
								                  #ifdef DEBUG
                                    printf("CmdSize1 = %d\n\r", ComInstance.TxBuf[USBDEV].Buf[0]);
                                    printf("CmdSize2 = %d\n\r", ComInstance.TxBuf[USBDEV].Buf[1]);
                                    printf("MSG1 = %d\n\r", ComInstance.TxBuf[USBDEV].Buf[2]);
                                    printf("MSG2 = %d\n\r", ComInstance.TxBuf[USBDEV].Buf[3]);
                                    printf("CmdTyp = %x\n\r", ComInstance.TxBuf[USBDEV].Buf[4]);
                                    printf("Cmd = %x\n\r", ComInstance.TxBuf[USBDEV].Buf[5]);
                                    printf("Stat = %x\n\r", ComInstance.TxBuf[USBDEV].Buf[6]);
                                  #endif

                                  // Fill the control stuff
                                  ComInstance.TxBuf[USBDEV].BlockLen = DAISY_DATA_PACKET;
                                  ComInstance.TxBuf[USBDEV].BufSize = DAISY_DEFAULT_MAX_EP_SIZE;
                                  ComInstance.TxBuf[USBDEV].Writing = 1;
                                  SetDaisyPushCounter(DAISY_PUSH_NOT_UNLOCKED); // Flag special handling to cCom
                                  PushState = DAISY_PUSH_CONNECTED;             // We should push all we're able to!!

                                  SetUnlocked(TRUE);
                                  cInputStartTypeDataUpload();                  // Force type table from SLAVE devices
                                  usleep(1500);
                                  cDaisyPushUpStream();                         // Flood upward
                                  cDaisyPrepareNext();                          // Ready for the next sensor in the array
                                  usleep(1500);
                                  break;

    case DAISY_SET_TYPE:		  // Set type exception

								              //#define DEBUG
                              #undef DEBUG
    							            #ifdef DEBUG
    		  	  	  	  	  	    printf("DAISY_SET_TYPE\n\r");
								              #endif


    		  	  	  	  	  	  if((*pDaisyCmd).Layer < 1)
	  	  	  	  	  	  	  	{
    		  	  	  	  	  	    // Destination layer reached


    		  	  	  	  	  	    //#define DEBUG
                                #undef DEBUG
    								            #ifdef DEBUG
    		  	  	  	  	  	      printf("DAISY_SET_TYPE layer < 1 : ");
    								            #endif

    		  	  	  	  	  	    cInputSetChainedDeviceType(0, (*pDaisyCmd).SensorNo, (*pDaisyCmd).PayLoad[0], (*pDaisyCmd).PayLoad[1]);
	  	  	  	  	  	  	  	}
	  	  	  	  	  	  	  	else	// Further down
	  	  	  	  	  	  	  	{
	  	  	  	  	  	  		    #undef DEBUG
	  	  	  	  	  	  	    	//#define DEBUG
	  	  	  	  	  	  	    	#ifdef DEBUG
	  	  	  	  	  	  	    	  printf("DAISY_SET_TYPE Further DOWN\n\r");
				                        #endif

	  	  	  	  	  	  	  		(*pDaisyCmd).Layer = ((*pDaisyCmd).Layer - 1);

	  	  	  	  	  	  	  		  // TX as is...
	  	  	  	  	  	  	    	  memcpy(&(DataOut[0]), pComCmd, ((*pComCmd).CmdSize + 2));
	  	  	  	  	  	  	    	  cDaisyTxDownStream();
	  	  	  	  	  	  	  	}

    		  	  	  	  	  	  break;

    case DAISY_CHAIN_DOWNSTREAM:  // If Layer == 1 we should tx downstream as a "normal" DIRECT CMD
    							                // if Layer > 1 we should decrement Layer and send downstream as
    							                // a DaisyChain payload

                                  //#define DEBUG
                                  #undef DEBUG
                                  #ifdef DEBUG
                                    printf("Here is the call from the HOST to cDaisyCmd DAISY_CHAIN_DOWNSTREAM - Daisy Layer set to %d\n\r", (*pDaisyCmd).Layer);
                                  #endif

                                 if((*pDaisyCmd).Layer == 1)
                                 {
                                   // Convert the message to a DIRECT COMMAND (i.e. replace the Daisy stuff)
                                   // TODO check for reply or not
                                   (*pComCmd).Cmd = DIRECT_COMMAND_NO_REPLY;			// Setup as "normal" Direct Command;

                                   // Adjust the size (i.e. remove the SubCmd
                                   (*pComCmd).CmdSize = (*pComCmd).CmdSize - 2;
                                   // Copy first portion of command (all before SubCmd
                                   memcpy(&(DataOut[0]), &((*pComCmd).CmdSize), 5);

                                   // Move Payload to "normal" position
                                   memcpy(&(DataOut[5]), &((*pComCmd).PayLoad[2]), ((*pComCmd).CmdSize - 3));

                                   //#define DEBUG
                                   #undef DEBUG
                                   #ifdef DEBUG
                                     printf("DAISY_CHAIN_DOWNSTREAM sent as a DIRECT command\n\r");
                                   #endif
                                 }
                                 else
                                 {
                                   (*pDaisyCmd).Layer = ((*pDaisyCmd).Layer - 1);
                                   // TX as is...
                                   memcpy(&(DataOut[0]), pComCmd, ((*pComCmd).CmdSize + 2));

                                   //#define DEBUG
                                   #undef DEBUG
                                   #ifdef DEBUG
                                     printf("DAISY_CHAIN_DOWNSTREAM sent as a further down command\n\r");
                                   #endif
                                 }

                                 cDaisyTxDownStream();

                                 break;

    case DAISY_CHAIN_DOWNSTREAM_WITH_BUSY:

                                  //#define DEBUG
                                  #undef DEBUG
    		  	  	  	  	  	      #ifdef DEBUG
    		  	  	  	  	  	        printf(" case DAISY_CHAIN_DOWNSTREAM_WITH_BUSY - (*pDaisyCmd).Layer ==  %d\n\r", (*pDaisyCmd).Layer );
                                  #endif

    		  	  	  	  	  	      // If Layer == 1 we should tx downstream as a "normal" DIRECT CMD
    		  	  	  	  	  	      // if Layer > 1 we should decrement Layer and send downstream as
    		  	  	  	  	  	      // a DaisyChain payload

    		  	  	  	  	  	      // Here we set the new and updated BUSY flags
    		  	  	  	  	  	      // Only the LAYER, ports is affected. 4 of the total 16

    		  	  	  	  	  	     LayerByte = 0;

    		  	  	  	  	  	     PortIndex = 0;
    		  	  	  	  	  	     for(CommandPointer = 3; CommandPointer >= 0; CommandPointer--)
    		  	  	  	  	  	     {
    		  	  	  	  	  	       // PAYLOAD_OFFSET = 4
    		  	  	  	  	  	       ThisMagicCookie = (*pComCmd).PayLoad[((*pComCmd).CmdSize) - CommandPointer - PAYLOAD_OFFSET];

    		  	  	  	  	  	       // Set Cookies in array E.g. layer 3(2) xxxx xxxx CCCC xxxx

    		  	  	  	  	  	       LayerByte |= (UBYTE)(ThisMagicCookie & 0x03);

    		  	  	  	  	  	       cDaisySetBusyFlags((UBYTE)(ThisMagicCookie & 0x03), PortIndex++, ThisMagicCookie);
    		  	  	  	  	  	     }

    		  	  	  	  	  	     cDaisySetOwnLayer((UBYTE)(LayerByte - (*pDaisyCmd).Layer));

    		  	  	  	  	  	     //#define DEBUG
                                 #undef DEBUG
                                 #ifdef DEBUG

    		  	  	  	  	  	     int DebugP;

    		  	  	  	  	  	     printf("DaisyBusyFlags DOWN:\r\n");

    		  	  	  	  	  	     for(DebugP = 0; DebugP < 16; DebugP++)
    		  	  	  	  	  	       printf("%X, ", DaisyBusyFlags[DebugP]);

    		  	  	  	  	  	     printf("\r\nCookie setting done..(*pDaisyCmd).Layer == %d OwnLayer == %d\r\n", (*pDaisyCmd).Layer, cDaisyOwnLayer);
    		  	  	  	  	  	     printf("\r\n");
                                 #endif

    		  	  	  	  	  	     if((*pDaisyCmd).Layer == 1)
    		  	  	  	  	  	     {
    		  	  	  	  	  	       // Convert the message to a DIRECT COMMAND (i.e. replace the Daisy stuff)
    		  	  	  	  	  	       // TODO check for reply or not
    		  	  	  	  	  	       (*pComCmd).Cmd = DIR_CMD_NO_REPLY_WITH_BUSY;     // Setup as "normal" Direct Command;

    		  	  	  	  	  	       // Adjust the size (i.e. remove the SubCmd and Layer
    		  	  	  	  	  	       (*pComCmd).CmdSize = (*pComCmd).CmdSize - 2;

    		  	  	  	  	  	       // Copy first portion of command (all before SubCmd
    		  	  	  	  	  	       memcpy(&(DataOut[0]), &((*pComCmd).CmdSize), 5);

    		  	  	  	  	  	       // Move Payload to "normal" position
    		  	  	  	  	  	       memcpy(&(DataOut[5]), &((*pComCmd).PayLoad[2]), ((*pComCmd).CmdSize - 4));
    		  	  	  	  	  	     }
    		  	  	  	  	  	     else
    		  	  	  	  	  	     {
    		  	  	  	  	  	       (*pDaisyCmd).Layer = ((*pDaisyCmd).Layer - 1);

    		  	  	  	  	  	       // TX as is...
    		  	  	  	  	  	       memcpy(&(DataOut[0]), pComCmd, ((*pComCmd).CmdSize + 2));
    		  	  	  	  	  	     }

    		  	  	  	  	  	     cDaisyTxDownStream();

    		  	  	  	  	  	     // Fall through
    		  	  	  	  	  	     // break;

      default:                   // ERROR
                                 break;
    }
}

int cDaisyGetMaxPacketSize(libusb_device_handle *DeviceHandle)
{
  // What's the smallest packet size for the HOSTed daisychained slave?
  int InSize = 0;
  int OutSize = 0;

  InSize = libusb_get_max_packet_size(libusb_get_device(DeviceHandle), DAISY_INTERRUPT_EP_IN);
  OutSize = libusb_get_max_packet_size(libusb_get_device(DeviceHandle), DAISY_INTERRUPT_EP_OUT);

  return (InSize <= OutSize) ? InSize : OutSize;  // Return the smallest size
}

DATA8 cDaisyGetUsbUpStreamSpeed(void)
{
  // Get the GADGET side speed (PC or DAISY host?)
  // I.e. HIGH speed = PC, FULL speed = DAISY stuff (or old PC/cheap HUB ;-))

  DATA8 SpeedResult = FULL_SPEED;

  // Check for UPSTREAM connection
  if((*pUsbSpeed).Speed == FULL_SPEED)
  {

	  #undef DEBUG
    //#define DEBUG
    #ifdef DEBUG
      printf("Connected to FULL_SPEED on Gadget side - DAISY chained\n\r");
    #endif

    IsDaisyChained = OK;
    //SpeedResult already set
  }
  else
  {
    SpeedResult = HIGH_SPEED;
    IsDaisyChained = FAIL;

    //#define DEBUG
	  #undef DEBUG
    #ifdef DEBUG
      printf("Connected to HIGH_SPEED on Gadget side\n\r");
    #endif
  }
  return SpeedResult;
}

int GetLayerPointer(void)
{
  return ReadLayerPointer;  // Returns the layer we just points to (index)
}

int GetSensorPointer(void)
{
  return ReadSensorPointer; // Returns the sensor we just points to (index)
}

RESULT cDaisyChained(void)
{
  return IsDaisyChained;    // Are we daisychained or not?
}

void cDaisySetTimeout(int TimeOut)
{
  TimeOutMs = TimeOut;      // Set the Daisy HOST timeout time
}

int cDaisyGetInterruptPacketSize(void)
{
  return MaxInterruptPacketSize;  // Returns the packet size (is the smallest of the IN/OUT endpoint)
}

RESULT cDaisyCreateSpeedInfo(void)
{
  // Makes the connection (shared mem) between Gadget driver
  // kernel layer and the Userspace layer

  RESULT Result = FAIL;
  int   UsbFile;
  USB_SPEED *pUsbSpeedTemp;

  // Create a Shared Memory entry for signaling the driver state FULL- or HIGHSPEED

  UsbFile = open(USBDEV_DEVICE_NAME,O_RDWR | O_SYNC);

  #undef DEBUG
  //#define DEBUG
  #ifdef DEBUG
    printf("UsbFile = %d\r\n", UsbFile);
  #endif

  if(UsbFile >= 0)
  {
    pUsbSpeedTemp  =  (USB_SPEED*)mmap(0, sizeof(UWORD), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, UsbFile, 0);

    if(pUsbSpeedTemp == MAP_FAILED)
    {
		  #undef DEBUG
    	//#define DEBUG
      #ifdef DEBUG
        printf("MAP_FAILED\n\r");
      #endif

      LogErrorNumber(USB_SHARED_MEMORY);
    }
    else
    {
      pUsbSpeed = pUsbSpeedTemp;
      Result  =  OK;

      #undef DEBUG
      //#define DEBUG
      #ifdef DEBUG
        printf("OK pUsbSpeed in c_Daisy\n\r");
      #endif
    }
    close(UsbFile);
  }

  return Result;
}

RESULT cDaisyWriteTypeDownstream(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode)
{
	RESULT RetRes = BUSY;
	int ReturnValue = DAISY_WR_ERROR;

	//#define DEBUG
	#undef DEBUG
	#ifdef DEBUG
		printf("Writing TYPE/MODE to SLAVE - Layer = %d, Port = %d, Type = %d, Mode = %d\r\n", Layer, Port, Type, Mode);
	#endif

	cDaisyResetBusyTiming();  // Make new prioritized access to channel

	DataOut[LEN1]   = 8;
	DataOut[LEN2]   = 0;
	DataOut[MSG1]   = 1;	    // TODO Real MSG no
	DataOut[MSG2]   = 0;
	DataOut[CMDTYP] = DAISY_COMMAND_NO_REPLY;
	DataOut[SUBCMD] = DAISY_SET_TYPE;
	DataOut[LAYER_POS_TO_SLAVE] = Layer;  // The TO_SLAVE added/changed for "FENCE_ERROR" protection!!
	DataOut[SENSOR_POS_TO_SLAVE] = Port;
	DataOut[TYPE_POS_TO_SLAVE] = Type;
	DataOut[MODE_POS_TO_SLAVE] = Mode;

	ReturnValue = cDaisyWrite();

	switch(ReturnValue)
	{
		case DAISY_WR_DISCONNECTED:

		              //#define DEBUG
									#undef DEBUG
									#ifdef DEBUG
										printf("cDaisyWriteTypeDownstream - DAISY_WR_DISCONNECTED\n\r");
      	  	  	  #endif

									DownConnectionState = DAISY_DOWN_DISCONNECTED;
									cDaisyFlagDisconnected(DaisyTempTypeLayer);
									RetRes = FAIL;
									DaisyReadyState = FAIL;

									break;	// Leave unhappy :-(

		case DAISY_WR_NOT_FINISHED:	// Started OK, but not finished. Set wait for
									              // write completion and flag busy

		              //#define DEBUG
									#undef DEBUG
									#ifdef DEBUG
										printf("cDaisyWriteTypeDownstream - DAISY_WR_NOT_FINISHED\n\r");
      	  	  	  #endif

									DownConnectionState = DAISY_DOWN_SET_DEVICETYPE;
									cDaisyFlagBusy(DaisyTempTypeLayer, DaisyTempTypeSensor);
									RetRes = OK;
									DaisyReadyState = OK;

									break;        // Leave and wait polite ;-)

		case  DAISY_WR_ERROR:

		              //#define DEBUG
									#undef DEBUG
									#ifdef DEBUG
										printf("cDaisyWriteTypeDownstream - DAISY_WR_ERROR\n\r");
      	  	  	  #endif

									// Fall through
									// break;

		default:			DownConnectionState = DAISY_DOWN_UNLOCKED;	// Connected but current
																			  	                    // operation went wrong!
									// If ERROR in starting the write -
									// flag BUSY, so user can decide
									// what to do - hopefully a new try ;-)
									// (normalised layer/sensor number)

									cDaisyFlagBusy(DaisyTempTypeLayer, DaisyTempTypeSensor);
									RetRes = FAIL;
									DaisyReadyState = FAIL;

									break;
	}

	return RetRes;
}

RESULT	cDaisySetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode)
{
	DATA8 NormSensor;
	RESULT RetVal = FAIL;

	//#define DEBUG
	#ifdef DEBUG
		printf("cDaisySetDeviceType in cDaisy\n\r");
	#endif
	#undef DEBUG

	if(Layer >= 1)	                  // Layer > self
	{
		DaisyTempTypeLayer = Layer - 1;	// Preserve for later use

		// Set BUSY in table
		if(Port > DAISY_MAX_INPUT_SENSOR_INDEX)
			NormSensor = Port - DAISY_SENSOR_OUTPUT_OFFSET;
		else
			NormSensor = Port;

		DaisyTempTypeSensor = NormSensor;
		cDaisyFlagBusy(DaisyTempTypeLayer, DaisyTempTypeSensor);

		RetVal = cDaisyWriteTypeDownstream(DaisyTempTypeLayer, Port, Type, Mode);
	}
	return RetVal;
}

void DaisyResetTheBuffers( int FromLayer)
{
  int i;
  int j;

  // As it says - Zero set the Daisy Array
  for(i = FromLayer; i < DAISY_MAX_LAYER_DEPT; i++)
  {
    for(j = 0; j < DAISY_MAX_SENSORS_PER_LAYER; j++)
    {
      memset((&(DaisyBuffers[i][j])), 0, DAISY_MAX_DATASIZE);
      DaisyBuffers[i][j][0] = FAIL;	  // No activity should equal INVALID, NO_SENSOR and
      	  	  	  	  	  	  	  	  // "----" in display on Master ;-)
    }
  }
}

void DaisyZeroTheBuffers(void)
{
  DaisyResetTheBuffers( 0 );          // All including MySelf
}

void DaisyZeroAllExceptMySelf(void)
{
  DaisyResetTheBuffers( 1 );          // All below MySelf
}

RESULT cDaisyInit(void)
{
  RESULT  Result = FAIL;
  int Temp;

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printf("\n\rDaisy Init called\n\r");
  #endif

  DownConnectionState = DAISY_DOWN_DISCONNECTED;
  WriteState = DAISY_WR_DISCONNECTED;
  ReadState = DAISY_RD_DISCONNECTED;

  DaisyMsgCounter = 0;        // Used by the Daisy, not in sync with the HOST PC app. message counter

  // Layer, Table-pointers and size limit initialisation
  ReadLayerPointer = 0;
  ReadSensorPointer = 0;
  MaxLowerLayer = 0;
  DaisyInfo = EMPTY;          // No info

  DaisyUpstreamDataReady = FALSE;
  DaisyPushCounter = 0x55;
  OldDaisyCounter = 0;

  DaisyZeroTheBuffers();      // Start @ a known stage of data

  // Make a handle to the kernel USB speed
  Result = cDaisyCreateSpeedInfo();

  if(Result == OK)
  {
    // Check for UPSTREAM connection
    if((*pUsbSpeed).Speed == HIGH_SPEED)
    {
      UpStreamConnection.Speed = HIGH_SPEED;

      #undef DEBUG
      #ifdef DEBUG
        printf("Connected to HIGH_SPEED on Gadget side\n\r");
      #endif
    }
    else
    {
      UpStreamConnection.Speed = FULL_SPEED;

      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
        printf("Connected to FULL_SPEED on Gadget side\n\r");
      #endif
    }

    // Start initializing the UsbLib stuff
    Temp = libusb_init(NULL);

    if(Temp < 0)
    {
      Result = FAIL;
    }
    else
    {
      //#define DEBUG
   	  #undef DEBUG
      #ifdef DEBUG
        printf("\n\rDaisy Temp >= 0 \n\r");
      #endif

      DeviceHandle = libusb_open_device_with_vid_pid(NULL, DAISY_VENDOR_ID, DAISY_PRODUCT_ID);
      if(DeviceHandle != NULL)
      {
        // We've found our HID and want to use it
        // Detach the hidusb from our HID to let us
        // and the libusb use it :-)

        libusb_detach_kernel_driver(DeviceHandle, INTERFACE_NUMBER);      // No simple ERROR check
                                                                          // an ERROR will be catched
        if(libusb_claim_interface(DeviceHandle, INTERFACE_NUMBER) >= 0)   // from here.....
        {
          // We have an valid Interface

          //#define DEBUG
          #undef DEBUG
          #ifdef DEBUG
            printf("\n\rDaisy valid InterFace??? We have if this is seen...;-)\n\r");
          #endif

          Result = OK;
          MaxInterruptPacketSize = cDaisyGetMaxPacketSize(DeviceHandle);  // Be sure we have the right size

          //#define DEBUG
          #undef DEBUG
          #ifdef DEBUG
            printf("\n\rDaisy MaxInterruptPacketSize: %d\n\r", MaxInterruptPacketSize);
          #endif

          cDaisySetTimeout(DAISY_DEFAULT_TIMEOUT);
          DownConnectionState = DAISY_DOWN_CONNECTED;
          WriteState = DAISY_WR_IDLE; // We have a connection, but no communication yet
          ReadState = DAISY_RD_IDLE;
          ReadTransfer = libusb_alloc_transfer(0);
          WriteTransfer = libusb_alloc_transfer(0);
          BytesWritten = 0;
          ReadLength = 0;
        }
        else
          Result = FAIL;              // Could not claim the interface to the HID device
      }
    }
  }
  	//#define DEBUG
    #undef DEBUG
    #ifdef DEBUG
      printf("\n\rDaisy Init ended (HOST)\n\r");
    #endif

  return Result;
}

int cDaisyWriteUnlockToSlave()
{
  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printf("Writing UNLOCK to SLAVE\r\n");
  #endif

  DataOut[LEN1]   = 4;
  DataOut[LEN2]   = 0;
  DataOut[MSG1]   = 1;	// TODO real no
  DataOut[MSG2]   = 0;
  DataOut[CMDTYP] = DAISY_COMMAND_REPLY;
  DataOut[SUBCMD] = DAISY_UNLOCK_SLAVE;

  return cDaisyWrite();
}

int GetLayerStorePointer(void)
{
  return DataIn[LAYER_POS];     // Get the LAYER from the in the message embedded layer
}

int GetSensorStorePointer(void)
{
  return DataIn[SENSOR_POS];    // Get the Sensor-position from the in the message embedded sensor-"address"
}

MSGCNT  cDaisyReplyNumber(void)
{
  if(ReplyNumber < 10000)
    ReplyNumber++;
  else
    ReplyNumber = 0;
  return ReplyNumber;
}

RESULT    cDaisyGetDeviceInfo(DATA8 Length,UBYTE *pInfo)
{
  RESULT RetVal = FAIL;

  // Polled by cInput module
  if((PushState != DAISY_PUSH_CONNECTED) && (DaisyInfo != EMPTY))
  {	  	  	  	  	  	  	  	  	  	// Last device I.e. the MASTER
  										                  // OK to get'n'delete the INFO
	  	  	  	  	  	  	  	  	      // else TX it upwards

    if(Length > MAX_DEVICE_INFOLENGTH) Length = MAX_DEVICE_INFOLENGTH;

	  #undef DEBUG
    //#define DEBUG
	  #ifdef DEBUG
			printf("%s\r\n", &(DaisyInfoBuffer[0]));
	  #endif

    memcpy(pInfo, DaisyInfoBuffer, Length);
    DaisyInfo = EMPTY;
    RetVal = OK;
  }

  return RetVal;
}

RESULT    cDaisySetDeviceInfo(DATA8 Length,UBYTE *pInfo)
{
  // UpStream command from Input module from slave via DaisyChain
  RESULT RetResult = BUSY;

  if(PushState == DAISY_PUSH_CONNECTED)
	{
		//#define DEBUG
		#undef DEBUG
		#ifdef DEBUG
			printf("cDaisySetDeviceInfo, PushState == DAISY_PUSH_CONNECTED\n\r");
		#endif

		if(DaisyInfo == EMPTY)	// Ready for new info stuff
		{
			//#define DEBUG
			#undef DEBUG
			#ifdef DEBUG
				printf("cDaisySetDeviceInfo, DaisyInfo == EMPTY\n\r");
			#endif

			if(Length > MAX_DEVICE_INFOLENGTH) Length = MAX_DEVICE_INFOLENGTH;
			memcpy(DaisyInfoBuffer, pInfo, Length);

			//#define DEBUG
			#undef DEBUG
			#ifdef DEBUG
				int loop;
				for(loop = 0; loop < Length; loop++)
				{
					printf("DaisyInfoBuffer[%d] = 0x%x\r\n", loop, DaisyInfoBuffer[loop]);
				}
			#endif

			cDaisyResetBusyTiming();
			DaisyInfo = VALID;
			RetResult = OK;
		}
	}
  else
  {
    RetResult  =  FAIL;
  }
	return RetResult;
}

void cDaisyPushUpStream(void)
{
  // PUSH data like a mouse-device upwards in the (daisy-) chain
  // NOT as a master-slave but as a slave who is "forcing" data
  // upstream as fast as possible - still scheduled voluntary from
  // the VM/Application thread

  DAISY_DEVICE_DATA *pDaisyReply;
  DAISY_INFO *pDaisyInfo;
  DATA8 DeviceNo;               // The normalized number used in the request
                                // (I.e. 0-3 left as.. 4-7 changed to 16-19)
  RESULT SensorResult = FAIL;
  UBYTE MotorFlags;

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printf("Her we call c_daisy cDaisyPushUpStream\n\r");
  #endif

	pDaisyReply = (DAISY_DEVICE_DATA*)(DaisyUpstreamDataBuffer);   // Overall pointer to Reply stuff

	pDaisyReply->CmdType = DAISY_REPLY;
	pDaisyReply->MsgCount = cDaisyReplyNumber();

	// If information about sensors is available,
	// and we're "man in the middle" - I.e. not
	// at top-level (MASTER-level) the content
	// of the INFO buffer shall be pushed upwards
	// in the chain

	if((DaisyInfo == VALID) && (PushState == DAISY_PUSH_CONNECTED))
	{
		// We have some "VALID" info
		pDaisyReply->CmdSize = SIZEOF_REPLY_DAISY_READ; // If we get the length in the future
                                                    // it should be set here
		// Fill in the INFO in the reply
		pDaisyReply->Cmd = DAISY_CHAIN_INFO;

		pDaisyInfo = (DAISY_INFO*)(DaisyUpstreamDataBuffer);

		memcpy(pDaisyInfo->Info, DaisyInfoBuffer, SIZEOF_DAISY_INFO_READ);
		DaisyInfo = EMPTY;                              // We've used the stuff

    #undef DEBUG
		//#define DEBUG
    #ifdef DEBUG
		  printf("cDaisyPushUpStream INFO done!!!!\n\r");
    #endif
	}
	else	// NO info, so do the DATA job
	{
		pDaisyReply->CmdSize = DAISY_SENSOR_DATA_SIZE + BUSYFLAGS;
		pDaisyReply->Cmd = DAISY_CHAIN_UPSTREAM;
		pDaisyReply->Layer = ReadLayerPointer;

		if(ReadSensorPointer > 3)
		  DeviceNo = (ReadSensorPointer + DAISY_SENSOR_OUTPUT_OFFSET);
		else
			DeviceNo = ReadSensorPointer;

		pDaisyReply->SensorNo = DeviceNo;             // The normalized number :-)

		if(ReadLayerPointer == 0) // I.e. if we want to read our own layer
		{                         // we read directly @ the in-/output device
                              // else we read in the table (array) indexed
                              // 0-3 and 16-19 (in and "output" sensors)

		  SensorResult = cInputGetDeviceData(ReadLayerPointer, DeviceNo, DEVICE_MAX_DATA, &(pDaisyReply->Type), &(pDaisyReply->Mode), (DATA8*)(&(pDaisyReply->DeviceData[0])));

		  pDaisyReply->Status = SensorResult;         // Status from device;

			//#define DEBUG
			#undef DEBUG
			#ifdef DEBUG
			  if(DeviceNo == 1)
				  printf("Self DATA (PUSH) collected here: ReadLayerP: %d, ReadSensorP: %d, DeviceNo: %d, Status %d\r\n", ReadLayerPointer, ReadSensorPointer, DeviceNo, SensorResult);
			#endif

		}
		else
		{
			// We should read from the array we uses the real ReadSensorPointer
			// Remember layer is 1 offset (i.e. 0 == Layer 1, 1 == 2, so Read Array Layer 1 == read[1 - 1]

			//#define DEBUG
   	  #undef DEBUG
	  	#ifdef DEBUG
    	  printf("Array Data (PUSH) are collected here: ReadLayerPointer > 0: %d, ReadSensorPointer: %d, DeviceNo: %d - STATUS = %d\r\n", ReadLayerPointer, ReadSensorPointer, DeviceNo, DaisyBuffers[ReadLayerPointer - 1][ReadSensorPointer][0]);
	  	#endif

    	memcpy(&(pDaisyReply->Status), (&(DaisyBuffers[ReadLayerPointer - 1][ReadSensorPointer])), DAISY_DEVICE_PAYLOAD_SIZE);
		}

		//#define DEBUG
    #undef DEBUG
    #ifdef DEBUG

		int w;

		  printf("Before own in PUSH:");

		  for(w = 0; w < 16; w++)
		    if(DaisyBusyFlags[w] != 0x00)
		      printf("[%d]=%X ", w, DaisyBusyFlags[w]);
      printf("\n\r");
    #endif

    // refresh the motor status
		MotorFlags = cMotorGetBusyFlags();

		//#define DEBUG
		#undef DEBUG
		#ifdef DEBUG
		  printf("Push MotorFlags from cMotorGetBusyFlags() = %X\n\r", MotorFlags);
    #endif

		// Pack (reset) the stuff

		uint j;
		UBYTE p = 0x01;
		uint StartIndex;

		StartIndex = 4 * cDaisyGetOwnLayer();

		//#define DEBUG
		#undef DEBUG
		#ifdef DEBUG
		  printf("StartIndex Getting cMotorGetBusyFlags() = %d\n\r", StartIndex);
    #endif

		for(j = StartIndex; j < (StartIndex + 4); j++)
		{
		  // Remember it's only the BUSY flag - the Cookie number isn't changed

		  if(p & MotorFlags)
		    DaisyBusyFlags[j] |= 0x80;
			else
				DaisyBusyFlags[j] &= 0x7F;

		  p <<= 1;

		  //#define DEBUG
		  #undef DEBUG
		  #ifdef DEBUG
			  printf("DaisyBusyFlags[%d] = %X, ", j, DaisyBusyFlags[j]);
      #endif

		}

		//#define DEBUG
		#undef DEBUG
		#ifdef DEBUG
		  printf("\n\r");
    #endif

    //We add the Motor BUSY stuff
    memcpy(&(pDaisyReply->FirstCookie), DaisyBusyFlags, 16);

    //#define DEBUG
    #undef DEBUG
    #ifdef DEBUG

      int z;

      printf("PUSHed:");

      for(z = 0; z < 16; z++)
        if(DaisyBusyFlags[z] != 0x00)
          printf("[%d]=%X @ StartIndex:%d", z, DaisyBusyFlags[z], StartIndex);
      printf("\n\r");
    #endif

	}

	// Common portion

	//#define DEBUG
	#undef DEBUG
	#ifdef DEBUG
	  printf("StreamDataReady set TRUE\n\r");
  #endif

	DaisyUpstreamDataReady = TRUE;

	//#define DEBUG
	#undef DEBUG
	#ifdef DEBUG

	  int nn;

	  for(nn = 2; nn < 4; nn++)
    {
	    printf("[%d]=%d, ", nn, DaisyUpstreamDataBuffer[nn]);
    }

	  printf("\n\r");
  #endif

}

void cDaisyPrepareNext(void)
{
  // Adjust for next poll
  ReadSensorPointer++;
  if(ReadSensorPointer >= DAISY_MAX_SENSORS_PER_LAYER)
  {

    //#define DEBUG
	  #undef DEBUG
	  #ifdef DEBUG
	    printf("ReadSensorPointer %d at PrepareNext lige foer reset\n\r", ReadSensorPointer);
	  #endif

    ReadSensorPointer = 0;    // Reset position
    ReadLayerPointer++;
    if(ReadLayerPointer > 3)  // >= MaxLowerLayer)
      ReadLayerPointer = 0;   // Start at the very beginning of table once more
  }

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printf("ReadSensorPointer %d at PrepareNext\n\r", ReadSensorPointer);
  #endif

}

RESULT cDaisyGetDownstreamData(DATA8 Layer,DATA8 Sensor,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
{
  //Normalized Sensor I.e. 16,17 to 4,5 etc.

  DATA8 NormSensor;
  RESULT Result = FAIL;

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
  	  printf("Layer (+1): %d, Port: %d", Layer, Sensor);
  #endif

  if(Layer > 0)	  // Self should not be called here, but directly from the I/P's
  {
	  Layer--;	    // Normalize Layer Index into storage ( Layer 1 is positioned @ index 0, 2 @ index 1 etc.

	  if(Sensor > (DAISY_MAX_INPUT_SENSOR_INDEX + DAISY_SENSOR_OUTPUT_OFFSET))
		  NormSensor = (Sensor - DAISY_SENSOR_OUTPUT_OFFSET);
	  else
		  NormSensor = Sensor;

	  Result = DaisyBuffers[Layer][NormSensor][STATUS_POS];

	  //#define DEBUG
	  #undef DEBUG
	  #ifdef	DEBUG

	    if((Layer == 0) && (NormSensor == 1))
	    {
	      switch(Result)
	      {
	        case OK:		printf("Layer 0, Sensor 1, OK\n\r");
	                    break;

	        case BUSY:	printf("Layer 0, Sensor 1, BUSY\n\r");
	                    break;

	        case FAIL:	printf("Layer 0, Sensor 1, FAIL\n\r");
	                    break;

	        case STOP:	printf("Layer 0, Sensor 1, STOP\n\r");
	                    break;
	      }
	    }
	  #endif


	  *pMode = DaisyBuffers[Layer][NormSensor][MODE_POS];
		*pType = DaisyBuffers[Layer][NormSensor][TYPE_POS];

		// Nanny the length
		if(!(Length <= DEVICE_MAX_DATA)) Length = DEVICE_MAX_DATA;
		memcpy(pData, &(DaisyBuffers[Layer][NormSensor][DEVICE_DATA_POS]), Length);

		//#define DEBUG
		#undef DEBUG
	  #ifdef DEBUG
		  printf("L=%d, S=%d, T=%d\n\r", Layer, NormSensor, DaisyBuffers[Layer][NormSensor][TYPE_POS]);
    #endif
  }
  return Result;

}

void DaisyAsyncWriteCallBack(struct libusb_transfer *WriteUsbTransfer)
{
  BytesWritten = 0;           // Reset - only set if data written

  // Callback -> Asyncronous Write-job done

  if( WriteUsbTransfer->status == LIBUSB_TRANSFER_COMPLETED )
  {
    //#define DEBUG
	  #undef DEBUG
	  #ifdef DEBUG
	    printf("Did I reach callback?? 01\r\n");
	  #endif

	  LastWriteResult = DAISY_WR_COMPLETED; // Done with Success
    BytesWritten = WriteUsbTransfer->actual_length;
    DaisyReadyState = OK;
  }
  else  // Some thing went wrong....
  {
    switch( WriteUsbTransfer->status)
    {


      case LIBUSB_TRANSFER_TIMED_OUT:
    	  	  	  	  	  	  	  	    //#define DEBUG
    	    							              #undef DEBUG
    	  	  	  	  	  	  	  	    #ifdef DEBUG
    	  	  	  	  	  	  	  	      printf("Did I reach callback?? 02\r\n");
										                  #endif

    	  	  	  	  	  	  	  	    LastWriteResult = DAISY_WR_TIMEDOUT;
    	  	  	  	  	  	  	  	    DaisyReadyState = FAIL;

    	  	  	  	  	  	  	  	    break;

      case LIBUSB_TRANSFER_NO_DEVICE:
    	  	  	  	  	  	  	  	    //#define DEBUG
    	    							              #undef DEBUG
    	  	  	  	  	  	  	  	  	#ifdef DEBUG
    	  	  	  	  	  	  	  	  	  printf("Did I reach callback?? 03\r\n");
										                  #endif

    	  	  	  	  	  	  	  	  	DownConnectionState = DAISY_DOWN_DISCONNECTED;
    	  	  	  	  	  	  	  	  	LastWriteResult = DAISY_WR_DISCONNECTED;
    	  	  	  	  	  	  	  	  	DaisyReadyState = FAIL;

    	  	  	  	  	  	  	  	  	break;

      default:                        // Fall through
      case LIBUSB_TRANSFER_CANCELLED: // Fall through   Could be more detailed
      case LIBUSB_TRANSFER_STALL:     // Fall through
      case LIBUSB_TRANSFER_OVERFLOW:  // Fall through

      case LIBUSB_TRANSFER_ERROR:     LastWriteResult = DAISY_WR_ERROR;
      	  	  	  	  	  	  	  	  DaisyReadyState = FAIL;

      	  	  	  	  	  	  	  	  //#define DEBUG
        								              #undef DEBUG
										                  #ifdef DEBUG
      	  	  	  	  	  	  	  	  	printf("Did I reach callback?? 04\r\n");
										                  #endif

      	  	  	  	  	  	  	  	  break;
    }
  }

  WriteState = DAISY_WR_IDLE;

}

int cDaisyWriteDone(void)                               // Called for an asyncronous check on the actual write
{
  int ReturnValue = DAISY_WR_NOT_FINISHED;
  int EventResult = 0;

  EventResult = libusb_handle_events_timeout(0, &TV);         // Let the CPU get some job done - but don't let it
                                                              // use too much time.......
  switch(LastWriteResult)
  {
    case  DAISY_WR_NOT_FINISHED:                                        // Return Write in progress
                                                                        // ReturnValue already set...
    							                DaisyReadyState = OK;	                // Until something else proved

    							                break;

    case  DAISY_WR_IDLE:                                                // Hmmm We're idle
    							                DaisyReadyState = OK;
                                  ReturnValue = DAISY_WR_IDLE;

                                  break;

    case  DAISY_WR_COMPLETED:     ReturnValue = BytesWritten;           // Return a positive value i.e. NO error
                                  LastWriteResult = DAISY_WR_IDLE;
                                  DaisyReadyState = OK;
                                  WriteState = DAISY_WR_IDLE;           // Ready again

                                  break;

    case DAISY_WR_DISCONNECTED:   ReturnValue = DAISY_WR_DISCONNECTED;  // Device gone, but can be requested later
                                  WriteState = DAISY_WR_IDLE;           // But ready again if reconnected
                                  DaisyReadyState = FAIL;

                                  break;

    default:                      // FALL THROUGH
    case DAISY_WR_TIMED_OUT:      // FALL THROUGH   Could be more detailed

    case DAISY_WR_ERROR:          ReturnValue = DAISY_WR_ERROR;
                                  WriteState = DAISY_WR_IDLE;
                                  DaisyReadyState = FAIL;

                                  break;
  }
  return ReturnValue;
}

void cDaisyPollFromDownStream(void)  // Autonomous put into array etc. :-)
{
  int ReadResult = 0;
  int EventResult = 0;
  int InfoLength = 0;
  int TempStorePointer = 0;                             // Used for the offset of O/P sensors
                                                        // (i.e. 16 = 4, 17 = 5 index in array)

  EventResult = libusb_handle_events_timeout(0, &TV);   // Let the CPU get some job done - but don't let it
                                                        // use too much time.......

  	if(ReadState != DAISY_RD_IDLE)
  	{
  		// Some job in progress - wait for CallBack to give the async result

  	  switch(ReadState)
  	  {
  	    case DAISY_RD_DONE:       // We have a valid READ

    	  	  	  	  	  	  	    //#define DEBUG
    	    					              #undef DEBUG
                                  #ifdef DEBUG
                                    printf("\n\rcDaisyPollFromDownstream - DAISY_RD_DONE - length = %d\n\r", ReadLength);
                                  #endif

                                  // #define DEBUG
                                  #undef DEBUG
                                  #ifdef DEBUG

                                    int x;
                                    	printf("--DATAIN---\n\r");
                                    	for (x = 0; x < 16; x++)
                                    		printf("%d, ", DataIn[x]);

                                    	for (x = 16; x < 32; x++)
                                    		printf("%d, ", DataIn[x]);
                                    	printf("\n\r");

                                    // Smaller print for verify :-)
                                    //printf("Data rec\n\r");

								                  #endif

                                  // If we're the MASTER we keep the INFO in the buffer
                                  // and flags it's ready
                                  // else the INFO is pushed upwards in the cDaisyPushUpStream()

                                  // Switch on data type
                                  // DAISY_CHAIN_INFO     == 0xA2
                                  // DAISY_CHAIN_UPSTREAM == 0xA1

                                  ReadResult = ReadLength;

                                  switch(DataIn[SUBCMD])	  // Type of DAISY packet
                                  {
                                  case DAISY_CHAIN_INFO:
															                              #undef DEBUG
                                                            //#define DEBUG
                                	  	  	  	  	  	  	  #ifdef DEBUG
                                  	  	  	  	  	  	  	  printf("DaisyInfo from DownStream\n\r");
                                						                #endif

                                	  	  	  	  	  	  	  // Ready for update
                                	  	  	  	  	  	  	  if(DaisyInfo == EMPTY)	// Ready for new info stuff
                                	  	  	  	  	  	  	  {
                                	  	  	  	  	  	  		  if(ReadLength > MAX_DEVICE_INFOLENGTH)
                                	  	  	  	  	  	  			  InfoLength = MAX_DEVICE_INFOLENGTH;
                                	  	  	  	  	  	  		  else
                                	  	  	  	  	  	  			  InfoLength = ReadLength;

                                	  	  	  	  	  	  		  // Store the INFO
                                	  	  	  	  	  	  		  memcpy(DaisyInfoBuffer, &(DataIn[INFO_INFO]), InfoLength);
                                	  	  	  	  	  	  		  DaisyInfo = VALID;
                                	  	  	  	  	  	  	    ReadState = DAISY_RD_IDLE; // Ready for next job
                                	  	  	  	  	  	  	  }

                                	  	  	  	  	  	  	  // else wait for INFO polled and cleared by input
                                	  	  	  	  	  	  	  if(PushPriorityCounter > 0)
                                	  	  	          	  	  {
                                	  	  	  	  	  	  		  PushPriorityCounter--;

                                	  	  	  	  	  	  		  //#define DEBUG
                                	  	  	  	  	  	  		  #undef DEBUG
                                                              #ifdef DEBUG
                                	  	  	  	  	  	  		    printf("PushPriorityCounter, Time = %d , %d\n\r", PushPriorityCounter, BusyTimer);
                                                              #endif
                                	  	  	          	  	  }

                                	  	  	  	  	  	  	  break;

                                  case DAISY_CHAIN_UPSTREAM:

                                	  						            //#define DEBUG
                                                            #undef DEBUG
                                                            #ifdef DEBUG
                                	                            printf("DaisyData polled from DownStream\n\r");
                                	                          #endif

                                	  	  	  	  	  	  	  if(ReadLength < DAISY_MAX_EP_IN_SIZE) // Add trailing zeroes
                                                            {
                                	  	  	  	  	  	  		  memset((&(DataIn[ReadLength])), 0, DAISY_MAX_EP_IN_SIZE - ReadLength);
                                                            }

                                	  	  	  	  	  	  	  // Adjust the BUSY stuff
                                	  	  	  	  	  	  	  // If polled Cookies newer/same age as what's in the local array
                                	  	  	  	  	  	  	  // and the BUSY flag in the polled version is cleared - all the
                                	  	  	  	  	  	  	  // cookie is reset to 0

                                	  	  	  	  	  	  	  int k;
                                	  	  	  	  	  	  	  int l;

                                	  	  	  	  	  	  	  l = 0;

                                	  	  	  	  	  	  	  for(k = BUSYFLAGS_START_POS; k < (BUSYFLAGS_START_POS + 16); k++)
                                	  	  	  	  	  	  	  {
                                	  	  	  	  	  	  	    if((DataIn[k] & 0x7C) >= (DaisyBusyFlags[l] & 0x7C)) // Cookie upstream newer or same age
                                	  	  	  	  	  	  	    {
                                	  	  	  	  	  	  	    	if((DataIn[k] & 0x80) == 0x00)
                                	  	  	  	  	  	  	    	{
                                	  	  	  	  	  	  	    	  // Busy reset here but "age" and "from where" preserved for use upstream
                                	  	  	  	  	  	  	    	  DataIn[k] &= 0x7F;
                                	  	  	  	  	  	  	    		DaisyBusyFlags[l] &= 0x7F; //0x00; // local RESET too
                                	  	  	  	  	  	  	    	}
                                	  	  	  	  	  	  	    }

                                	  	  	  	  	  	  	    l++;
                                	  	  	  	  	  	  	  }

                                	  	  	  	  	  	  	  // Remember layer is 1 offset (i.e. Layer "One down" stored at index 0, "Two down" as 1 etc.
                                                            // So a Read Array Layer "One down" == read[1 - 1] - remember layer 0 (self) is read directly

                                	  	  	  	  	  	  	  TempStorePointer = GetSensorStorePointer();

                                	  	  	  	  	  	  	  if(TempStorePointer > 15)
                                	  	  	  	  	  	  	    TempStorePointer = (TempStorePointer - DAISY_SENSOR_OUTPUT_OFFSET);

                                                            memcpy((&(DaisyBuffers[GetLayerStorePointer()][TempStorePointer])), &(DataIn[8/*0*/]), DAISY_DEVICE_PAYLOAD_SIZE/*DAISY_MAX_EP_IN_SIZE*/);

                                                            //#define DEBUG
                                  							            #undef DEBUG
                                                            #ifdef DEBUG

                                                              int i;
                                                              int j;
                                                              int z;

                                                              	  printf("-----\n\r");
                                                              	  for (j = 0; j < 4; j++)
                                                              	  {
                                                              		  for (i = 0; i < 8; i++)
                                                              		  {
                                                              			  for (z = 0; z < 32; z++)
                                                              			  {
                                                              				  printf("%d, ",(UBYTE)(DaisyBuffers[j][i][z]));
                                                              			  }
                                                              			  printf("\n\r");
                                                              		  }
                                                              	  }

                                                              // Smaller print for verify :-)
                                                              //printf("Data rec\n\r");
                                                            #endif

                                                            if(PushPriorityCounter > 0)
                                                            {
                                                              PushPriorityCounter--;
                                                            }

                                                            ReadState = DAISY_RD_IDLE; // Ready for next job

                                                            break;

                                  default:					        ReadState = DAISY_RD_IDLE; // Ready for next job

                                                            break;
                                  }


                                  break;

      case DAISY_RD_REQUESTED:    // We still have a valid READ request
                                  // ReadState already set OK

    	  	  	  	  	  	  	    //#define DEBUG
    	    					              #undef DEBUG
                                  #ifdef DEBUG
                                    printf("\n\rcDaisyPollFromDownstream - DAISY_RD_REQUESTED\n\r");
                                  #endif

                                  break;

      case DAISY_RD_DISCONNECTED: // Device gone, but can be requested later
                                  // Ready again if reconnected

                                  DaisyZeroTheBuffers();

                                  //#define DEBUG
    	    					              #undef DEBUG
                                  #ifdef DEBUG
                                    printf("\n\rcDaisyPollFromDownstream - DAISY_RD_DISCONNECTED\n\r");
                                  #endif
                                  // FALL THROUGH

      case DAISY_RD_TIMEDOUT:     // Too lazy...

    	  	  	  	  	  	  	    //#define DEBUG
    	    					              #undef DEBUG
                                  #ifdef DEBUG
                                    printf("\n\rcDaisyPollFromDownstream - DAISY_RD_TIMEDOUT\n\r");
                                  #endif

                                  ReadState = DAISY_RD_IDLE;  // Ready again if it becomes more "energy"...

                                  break;

      default:                    // FALL THROUGH

      case DAISY_RD_ERROR:
    	  	  	  	  	  	  	    //#define DEBUG
                                  #undef DEBUG
    	  	  	  	  	  	  	    #ifdef DEBUG
                                    printf("\n\rcDaisyPollFromDownstream - DAISY_RD_ERROR\n\r");
                                  #endif

                                  ReadResult = DAISY_RD_ERROR;
                                  ReadState = DAISY_RD_IDLE; // Ready for next job

                                  break;
    }
  }
  else
  {
    // We have to start the READ job
    if(DeviceHandle == NULL)
    {
      #undef DEBUG
      //#define DEBUG
      #ifdef DEBUG
        printf("\n\rcDaisyPollFromDownstream - Device_handle = NULL\n\r");
      #endif
    }
    else
    {
      if(ReadTransfer == NULL)
      {
        #undef DEBUG
        //#define DEBUG
        #ifdef DEBUG
          printf("\n\rcDaisyPollFromDownstream - ReadTransfer = NULL\n\r");
        #endif
      }
      else
      {
        // Populate the transfer
        libusb_fill_interrupt_transfer( ReadTransfer,
                                    DeviceHandle,
                                    DAISY_INTERRUPT_EP_IN,
                                    DataIn,
                                    DAISY_MAX_EP_IN_SIZE,
                                    &DaisyAsyncReadCallBack,
                                    NO_USER_DATA, // No user data
                                    TimeOutMs);

        ReadResult = libusb_submit_transfer(ReadTransfer);
        switch(ReadResult)
        {
          case LIBUSB_SUCCESS:                                                  // Read initiated OK
                                        ReadState = DAISY_RD_REQUESTED;         // Ready for the callback to be called
                                        ReadLength = 0;                         // Nothing read yet

                                        //#define DEBUG
                                        #undef DEBUG
                                        #ifdef DEBUG
                                          printf("\n\rcDaisyPollFromDownstream REQ - LIBUSB_SUCCESS\n\r");
                                        #endif

                                        break;

          case LIBUSB_ERROR_NO_DEVICE:                                          // Device gone, but can be requested later
                                        ReadState = DAISY_RD_IDLE;              // No active job - Device is disconnected
                                        DownConnectionState = DAISY_DOWN_DISCONNECTED;
                                        DaisyZeroTheBuffers();

                                        //#define DEBUG
                                        #undef DEBUG
                                        #ifdef DEBUG
                                          printf("\n\rcDaisyPollFromDownstream - LIBUSB_ERROR_NO_DEVICE\n\r");
                                        #endif

                                        break;                                  // Try again later....

          default:                                                              // Some ERROR should not happen at start
                                        ReadState = DAISY_RD_IDLE;              // if device present - No active job

                                        //#define DEBUG
                                        #undef DEBUG
                                        #ifdef DEBUG
                                          printf("\n\rcDaisyPollFromDownstream - LIBUSB_DEFAULT\n\r");
                                        #endif

                                        break;                                // Try again later....
        }
      }
    }
  }
}

int cDaisyWrite()                     // Write DOWNSTREAM
{
  int ResValue;
  int ReturnValue = DAISY_WR_ERROR;

  if(WriteState == DAISY_WR_IDLE)     // We're ready for a (new) write
  {

	  //#define DEBUG
    #undef DEBUG
	  #ifdef DEBUG
	    printf("DAISY WriteState and IDLE\r\n");
	  #endif

	  // Do a start write
    libusb_fill_interrupt_transfer( WriteTransfer,
                                    DeviceHandle,
                                    DAISY_INTERRUPT_EP_OUT,
                                    DataOut,
                                    DAISY_DEFAULT_MAX_EP_SIZE,
                                    &DaisyAsyncWriteCallBack,
                                    NO_USER_DATA, // No user data
                                    TimeOutMs);

    ResValue = libusb_submit_transfer(WriteTransfer);

    //#define DEBUG
    #undef DEBUG
    #ifdef DEBUG
      printf("ResValue from write submit = %d\n\r", ResValue);
	  #endif

    switch(ResValue)                  // Could be returned as is,
    {                                 // but if some should be rearranged,
                                      // renamed or grouped in another way

      case LIBUSB_SUCCESS:
    	  	  	  	  	  	  	  	//#define DEBUG
    	    						            #undef DEBUG
    	  	  	  	  	  	  	    #ifdef DEBUG
    	  	  	  	  	  	  	    	printf("Write submit LIBUSB_SUCCESS\n\r");
									                #endif

    	  	  	  	  	  	  	    ReturnValue = DAISY_WR_NOT_FINISHED;                // Write started OK
                                  LastWriteResult = DAISY_WR_NOT_FINISHED;            // and NOT finished
                                  WriteState = DAISY_WR_REQUESTED;                    // Ready for for the callback to signal Write Done

                                  break;

      case LIBUSB_ERROR_NO_DEVICE:

                                  //#define DEBUG
    	    					              #undef DEBUG
    	  	  	  	  	  	  	  	#ifdef DEBUG
	  	    							            printf("Write submit LIBUSB_ERROR_NO_DEVICE\n\r");
									                #endif

	  	    							          ReturnValue = DAISY_WR_DISCONNECTED;                // Device gone, but can be requested later
	  	    							          LastWriteResult = DAISY_WR_DISCONNECTED;            // signal it
	  	    							          DownConnectionState = DAISY_DOWN_DISCONNECTED;
	  	    							          WriteState = DAISY_WR_IDLE;                         // We can be used if a device is connected

	  	    							          break;

      case LIBUSB_ERROR_BUSY:
    	  	  	  	  	  	  	    //#define DEBUG
    	    					              #undef DEBUG
									                #ifdef DEBUG
    	  	  	  	  	  	  	    	printf("Write submit LIBUSB_ERROR_BUSY\n\r");
									                #endif

    	  	  	  	  	  	  	    ReturnValue = DAISY_WR_NOT_FINISHED;                // Previous write NOT finished
                                  LastWriteResult = DAISY_WR_NOT_FINISHED;            // -
                                  WriteState = DAISY_WR_REQUESTED;                    // Still.... must be

                                  break;

      default:
    	  	  	  	  	  	  	    //#define DEBUG
    	    						            #undef DEBUG
    	  	  	  	  	  	  	  	#ifdef DEBUG
	  	    							            printf("Write submit DEFAULT in switch\n\r");	    // ERROR
									                #endif

	  	    							          break;
    }
  }
  return ReturnValue;
}

RESULT cDaisyUnlockOk(void)
{
  RESULT Result = FAIL;

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printf("cDaisyUnlockOk\n\r");
  #endif

  switch(ReadState)
  {
    case  DAISY_RD_DONE:      // We have to check the answer

    						              //#define DEBUG
    	  	  	  	  	  	    #undef DEBUG
                              #ifdef DEBUG
                                printf("Did we reach RD_DONE in UnLockOk?\n\r");
                              #endif

                              if((DataIn[SUBCMD] == DAISY_UNLOCK_SLAVE ) && (DataIn[STAT] == OK ))
                              {
                                Result = OK;
                                //#define DEBUG
                                #undef DEBUG
                                #ifdef DEBUG
                                  printf("cDaisyUnlockOk -> OK\n\r");
                                #endif
                              }
                              else
                              {
                                // else FAIL already set

                                //#define DEBUG
                                #undef DEBUG
                                #ifdef DEBUG
                                  printf("cDaisyUnlockOk -> FAIL\n\r");
                                #endif
                              }

                              break;

    case  DAISY_RD_REQUESTED: // Still busy

                              Result = BUSY;

                              break;

    case  DAISY_RD_DISCONNECTED:    // ALREADY set: Result = FAIL;
    case  DAISY_RD_TIMEDOUT:        // Fall through
    case  DAISY_RD_ERROR:           // Fall through

    default:                  break;
  }

  return Result;
}

RESULT cDaisyCheckForAttachedSlave(void)
{
  FILE *pIdVendor = NULL;
  FILE *pIdProduct = NULL;
  char VendorBuffer[64];
  char ProductBuffer[64];
  RESULT Result = FAIL;

  pIdVendor = fopen("/sys/bus/usb/devices/1-1/idVendor", "r");

  pIdProduct = fopen("/sys/bus/usb/devices/1-1/idProduct", "r");

  if((pIdVendor != NULL) && (pIdProduct != NULL))
  {
    if(fgets(VendorBuffer, sizeof (VendorBuffer), pIdVendor) != NULL)
    {
      if(fgets(ProductBuffer, sizeof (ProductBuffer), pIdProduct) != NULL)
      {
        if((strstr(ProductBuffer, SLAVE_PROD_ID) > 0) && (strstr(VendorBuffer, SLAVE_VENDOR_ID) > 0))
          Result = OK;
      }
    }
  }

  if(pIdVendor != NULL)
    fclose(pIdVendor);

  if(pIdProduct != NULL)
    fclose(pIdProduct);

  return Result;
}

void cDaisyControl(void)
{
  static  ULONG TimeOld = 0;
  ULONG   TimeNew;
  ULONG   TimeDiff;

  int WriteRes = 0;

  // Update busy timeout
  TimeNew  =  GetTimeUS();

  if (TimeOld == 0)
  {
    TimeOld  =  TimeNew;
  }

  TimeDiff   =  TimeNew - TimeOld;

  if (BusyTimer > TimeDiff)
  {
    BusyTimer -=  TimeDiff;
  }
  else
  {
    BusyTimer  =  0;
  }

  TimeOld     =  TimeNew;

  // Do we have an interface at all?

  int EventResult = 0;

  EventResult = libusb_handle_events_timeout(0, &TV);   // Let the CPU get some job done - but don't let it
                                                        // use too much time.......

  switch(DownConnectionState)       // What is downstream and what should we do---
  {
    case  DAISY_DOWN_DISCONNECTED:  // The "try and connect" is simple timed

    								                //#define DEBUG
                                    #undef DEBUG
    	                    		      #ifdef DEBUG
                                      printf("DAISY_DOWN_DISCONNECTED\r\n");
                                    #endif

                                    PlugAndPlay--;  // one more "loop"

                                    if(PlugAndPlay <= 0)
                                    {
                                      if(cDaisyCheckForAttachedSlave() == OK)
                                      {
                                        cDaisyOpen();     // Try to make a connection
                                      }
                                      else
                                      {
                                        PlugAndPlay = TIME_TO_CHECK_FOR_DAISY_DOWNSTREAM;  // Another period
                                        DaisyZeroTheBuffers();
                                        //#define DEBUG
                                        #undef DEBUG
                                        #ifdef DEBUG
                                          printf("DAISY disconnected DOWNSTREAM - No slave bricks!! \r\n");
                                        #endif
                                      }
                                    }
                                    break;

    case  DAISY_DOWN_CONNECTED:     // Slave connected but still locked

    								                //#define DEBUG
    	    	  	  	  	  	  	    #undef DEBUG
                                    #ifdef DEBUG
                                      printf("DAISY_DOWN_CONNECTED\r\n");
                                    #endif

                                    if(cDaisyWriteUnlockToSlave() == DAISY_WR_NOT_FINISHED)
                                    {
                                      // Write request started OK
                                    	DownConnectionState = DAISY_DOWN_UNLOCKING;
                                    }
                                    break;

    case  DAISY_DOWN_UNLOCKING:     // Write Request in progress

    								                //#define DEBUG
    	    	  	  	  	  	  	    #undef DEBUG
                                    #ifdef DEBUG
                                      printf("DAISY_DOWN_UNLOCKING\r\n");
                                    #endif

                                    if(cDaisyWriteDone() == DAISY_WR_ERROR)
                                    {
                                      DownConnectionState = DAISY_DOWN_CONNECTED;  // Try again
                                    }
                                    else
                                    {
                                      //#define DEBUG
                                      #undef DEBUG
                                      #ifdef DEBUG
                                        printf("Before cDaisyStartReadUnlockAnswer()\r\n");
                                      #endif

                                      cDaisyStartReadUnlockAnswer();
                                      DownConnectionState = DAISY_CHECK_UNLOCK;
                                    }
                                    break;

    case  DAISY_CHECK_UNLOCK:       // Check for a valid answer (DAISY_DOWN_UNLOCKED)

    								                //#define DEBUG
    	    	  	  	                #undef DEBUG
									                  #ifdef DEBUG
                                      printf("DAISY_CHECK_UNLOCK ReadState = %d\r\n", ReadState);
                                    #endif

                                    if(ReadState == DAISY_RD_DONE)
                                    {
                                      //#define DEBUG
                                      #undef DEBUG
                                      #ifdef DEBUG
                                        printf("DAISY_RD_DONE\n\r");
                                      #endif

                                      if(cDaisyUnlockOk() == OK)
                                      {
                                        //#define DEBUG
                                    	  #undef DEBUG
                                        #ifdef DEBUG
                                          printf("cDaisyUnlockOk() returned OK\n\r");
                                        #endif

                                        SetSlaveUnlocked(TRUE);
                                        DownConnectionState = DAISY_DOWN_UNLOCKED;
                                      }
                                      else
                                      {
                                        //#define DEBUG
                                    	  #undef DEBUG
                                        #ifdef DEBUG
                                          printf("cDaisyUnlockOk() failed :-( \n\r");
                                        #endif

                                        DownConnectionState = DAISY_DOWN_CONNECTED;  // Try again
                                      }
                                    }
                                    else
                                    {
                                      if(ReadState == DAISY_RD_ERROR)
                                      {
                                        DownConnectionState = DAISY_DOWN_CONNECTED;  // Try again
                                      }
                                    }
                                    break;

    case  DAISY_DOWN_UNLOCKED:      // Initialized i.e. UNLOCKed and ready for Transfer of INFO & DATA

    								                //#define DEBUG
    	    	  	  	  	  	  	    #undef DEBUG
                                    #ifdef DEBUG
                                      printf("DAISY_DOWN_UNLOCKED\r\n");
                                    #endif

                                    // Poll as fast as possible and even faster.....

                                    cDaisyPollFromDownStream();

                                    break;

    case DAISY_DOWN_SET_DEVICETYPE: // Write mode in progress

                                    WriteRes = cDaisyWriteDone();
                                    switch(WriteRes)
                                    {

                                      case	DAISY_WR_NOT_FINISHED: 	// Wait polite...
    																					                      break;

                                      case	DAISY_WR_IDLE:			    // Success :-)
                                                                    DownConnectionState = DAISY_DOWN_UNLOCKED;

                                                                    // Do the Polling Job again
                                                                    if(GetUnlocked() == TRUE) cDaisyPollFromDownStream();	// We have control :-)
                                                                    break;

                                      case	DAISY_WR_DISCONNECTED:	// Blame the user for falling in the wire
                                                                    DownConnectionState = DAISY_DOWN_DISCONNECTED;
                                                                    cDaisyFlagDisconnected(DaisyTempTypeLayer);
                                                                    break;

                                      case	DAISY_WR_ERROR:			    // Unhappy - error while setting the MODE
                                                                    cDaisyFlagFail(DaisyTempTypeLayer, DaisyTempTypeSensor);
                                                                    // FALL through! break;
                                      default:

                                                                    break;
                                    }

                                    break;

    case DAISY_DOWNSTREAM_CHECK_WRITE_DONE:	  // Generic Write in progress
											                        WriteRes = cDaisyWriteDone();

											                        switch(WriteRes)
											                        {
											                          case	DAISY_WR_NOT_FINISHED: 	// Wait polite...
											                                                        break;

											                          case	DAISY_WR_IDLE:			    // Success :-)
											                                                        DownConnectionState = DAISY_DOWN_UNLOCKED;
											                                                        // Do the Polling Job again
											                                                        if(GetUnlocked() == TRUE) cDaisyPollFromDownStream();	// We have control :-)
											                                                        break;

											                          case	DAISY_WR_DISCONNECTED:	// Blame the user for falling in the wire
											                                                        DownConnectionState = DAISY_DOWN_DISCONNECTED;
											                                                        cDaisyFlagDisconnected(DaisyTempDownLayer);
											                                                        break;

											                          case	DAISY_WR_ERROR:			    // Unhappy - error while writing
											                                                        // FALL through! break;
											                          default:

											                                                        break;
											                        }

											                        break;


	  default:                          break;
  }
}

RESULT cDaisyOpen(void)
{
  if(DeviceHandle != NULL)
  {
    libusb_close(DeviceHandle);   // Close for safe open (again)
    DeviceHandle = NULL;          // NO false pointer
  }
  return (cDaisyInit());
}

RESULT cDaisyClose(void)
{
  return (cDaisyExit());
}

RESULT cDaisyExit(void)
{
  RESULT  Result  =  OK;

  if(ReadTransfer)
    libusb_free_transfer(ReadTransfer);   // Cleanup
  if(WriteTransfer)
    libusb_free_transfer(WriteTransfer);  // Cleanup

  libusb_release_interface(DeviceHandle, 0);
  libusb_close(DeviceHandle);
  libusb_exit(NULL);

  DeviceHandle = NULL;
  return (Result);
}
