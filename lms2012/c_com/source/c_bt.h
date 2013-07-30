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

#ifndef C_BT_H_
#define C_BT_H_


#include  "lms2012.h"
#include  <bluetooth/bluetooth.h>
#include  <bluetooth/rfcomm.h>
#include  <sys/poll.h>
#include  <sys/ioctl.h>
#include  <bluetooth/hci.h>


#define   NONVOL_BT_DATA                "settings/nonvolbt"


#define   MAX_DEV_TABLE_ENTRIES         30
#define   BT_CH_OFFSET                   2
#define   MAX_NAME_SIZE                 32
#define   MAX_BT_NAME_SIZE              248

#define   MAX_BUNDLE_ID_SIZE            24
#define   MAX_BUNDLE_SEED_ID_SIZE       11


enum
{
  BT_SLAVE_CH0,
  BT_HOST_CH0,
  BT_HOST_CH1,
  BT_HOST_CH2,
  BT_HOST_CH3,
  BT_HOST_CH4,
  BT_HOST_CH5,
  BT_HOST_CH6,
  NO_OF_BT_CHS
};


// Defines related to Device List
enum
{
  DEV_EMPTY       = 0x00,
  DEV_KNOWN       = 0x01,
};

enum
{
  READ_BUF_EMPTY,
  READ_BUF_FULL
};

enum
{
  SCAN_OFF,
  SCAN_INQ_STATE,
  SCAN_NAME_STATE
};

// Defines related to Channels
enum
{
  CH_CONNECTING,
  CH_FREE,
  CH_CONNECTED
};

// Defines related to BtInstance.HciSocket.Busy
enum
{
  HCI_IDLE        =  0x00,
  HCI_ONOFF       =  0x01,
  HCI_VISIBLE     =  0x02,
  HCI_NAME        =  0x04,
  HCI_SCAN        =  0x08,
  HCI_CONNECT     =  0x10,
  HCI_DISCONNECT  =  0x20,
  HCI_RESTART     =  0x40,
  HCI_FAIL        =  0x80
};


// Buffer to read into from the socket
typedef struct
{
  UBYTE Buf[1024];
  UWORD InPtr;
  UWORD OutPtr;
  UWORD Status;
}READBUF;


// Buffer to write into from the socket
typedef struct
{
  UBYTE Buf[1024];
  UWORD InPtr;
  UWORD OutPtr;
}WRITEBUF;


// Buffer to fill complete message into from READBUF
// only one Messages can fit into this buffer
typedef struct
{
  UBYTE Buf[1024];
  UWORD InPtr;
  UWORD MsgLen;
  UWORD RemMsgLen;
  UWORD Status;
  UBYTE LargeMsg;
}MSGBUF;


// Control socket
typedef   struct
{
  SLONG             Socket;
  struct  pollfd    p;
  UBYTE             WaitForEvent;
  UBYTE             Busy;
}HCISOCKET;


// Socket to listen for incoming requests
typedef   struct
{
  SLONG               Socket;
  struct  sockaddr_rc loc_addr;
  struct  sockaddr_rc rem_addr;
  ULONG               opt;
}LISTENSOCKET;


// Communication sockets
typedef   struct
{
  SLONG     Socket;
  bdaddr_t  Addr;
  struct    timeval     Cmdtv;
  fd_set    Cmdfds;
}BTSOCKET;


typedef struct
{
  char      Name[MAX_BT_NAME_SIZE];
  bdaddr_t  Adr;
  UWORD     ConnHandle;
  UBYTE     DevClass[3];
  UBYTE     Passkey[6];
  UBYTE     Connected;
  UBYTE     ChNo;
  UBYTE     Status;
}DEVICELIST;


typedef struct
{
  char      Name[MAX_BT_NAME_SIZE];
  bdaddr_t  Adr;
  UBYTE     DevClass[3];
  UBYTE     Connected;
  UBYTE     Parred;
  UBYTE     ChNo;
}SEARCHLIST;


typedef struct
{
  WRITEBUF  WriteBuf;
  READBUF   ReadBuf;
  MSGBUF    MsgBuf;
  BTSOCKET  BtSocket;
  UBYTE     Status;
}BTCH;


typedef   struct
{
  DEVICELIST  DevList[MAX_DEV_TABLE_ENTRIES];
  UBYTE       DevListEntries;
  UBYTE       Visible;
  UBYTE       On;
  UBYTE       DecodeMode;
  UBYTE       BundleID[MAX_BUNDLE_ID_SIZE];
  UBYTE       BundleSeedID[MAX_BUNDLE_SEED_ID_SIZE];
}NONVOLBT;

typedef   struct
{
  ULONG     Passkey;
  UWORD     ConnHandle;
  char      Name[MAX_BT_NAME_SIZE];
  bdaddr_t  Adr;
  UBYTE     DevClass[3];
}INCOMMING;

typedef   struct
{
  UBYTE     ChNo;
}OUTGOING;

typedef   struct
{
  bdaddr_t  Adr;
  UBYTE     PinLen;
  UBYTE     Pin[10];
  UBYTE     Status;
}TRUSTED_DEV;


typedef struct
{
  //*****************************************************************************
  // Bluetooth Global variables
  //*****************************************************************************

  HCISOCKET     HciSocket;            // Control socket
  LISTENSOCKET  ListenSocket;         // Socket to listen for incoming requests
  BTCH          BtCh[NO_OF_BT_CHS];   // Communication sockets
  READBUF       Mode2Buf;
  WRITEBUF      Mode2WriteBuf;

  SEARCHLIST    SearchList[MAX_DEV_TABLE_ENTRIES];
  INCOMMING     Incoming;
  OUTGOING      OutGoing;
  TRUSTED_DEV   TrustedDev;
  char          Adr[13];
  UBYTE         SearchIndex;
  UBYTE         NoOfFoundDev;
  UBYTE         NoOfFoundNames;
  UBYTE         PageState;
  UBYTE         ScanState;
  UBYTE         NoOfConnDevs;

  SLONG         State;
  SLONG         OldState;
  ULONG         Delay;
  NONVOLBT      NonVol;
  UBYTE         OnOffSeqCnt;
  UBYTE         RestartSeqCnt;
  UBYTE         Events;
  UBYTE         DecodeMode;
  UBYTE         SspPairingMethod;
  char          BtName[vmBRICKNAMESIZE];
}BT_GLOBALS;


void      IncommingConnect(void);
void      BtInit(char *pName);
void      BtExit(void);
void      BtUpdate(void);
void      BtTxMsgs(void);
UBYTE     cBtConnect(UBYTE *pName);
UBYTE     cBtDisconnect(UBYTE *pName);
UBYTE     cBtDiscChNo(UBYTE ChNo);

UWORD     cBtReadCh0(UBYTE *pBuf, UWORD Length);
UWORD     cBtReadCh1(UBYTE *pBuf, UWORD Length);
UWORD     cBtReadCh2(UBYTE *pBuf, UWORD Length);
UWORD     cBtReadCh3(UBYTE *pBuf, UWORD Length);
UWORD     cBtReadCh4(UBYTE *pBuf, UWORD Length);
UWORD     cBtReadCh5(UBYTE *pBuf, UWORD Length);
UWORD     cBtReadCh6(UBYTE *pBuf, UWORD Length);
UWORD     cBtReadCh7(UBYTE *pBuf, UWORD Length);

UWORD     cBtDevWriteBuf(UBYTE *pBuf, UWORD Size);
UWORD     cBtDevWriteBuf1(UBYTE *pBuf, UWORD Size);
UWORD     cBtDevWriteBuf2(UBYTE *pBuf, UWORD Size);
UWORD     cBtDevWriteBuf3(UBYTE *pBuf, UWORD Size);
UWORD     cBtDevWriteBuf4(UBYTE *pBuf, UWORD Size);
UWORD     cBtDevWriteBuf5(UBYTE *pBuf, UWORD Size);
UWORD     cBtDevWriteBuf6(UBYTE *pBuf, UWORD Size);
UWORD     cBtDevWriteBuf7(UBYTE *pBuf, UWORD Size);

UBYTE     cBtI2cBufReady(void);
UWORD     cBtI2cToBtBuf(UBYTE *pBuf, UWORD Size);

// Generic Bluetooth commands
UBYTE     BtSetVisibility(UBYTE State);
UBYTE     BtGetVisibility(void);
UBYTE     BtSetOnOff(UBYTE On);
UBYTE     BtGetOnOff(UBYTE *On);
UBYTE     BtSetMode2(UBYTE Mode2);
UBYTE     BtGetMode2(UBYTE *pMode2);
UBYTE     BtStartScan(void);
UBYTE     BtStopScan(void);
UBYTE     cBtGetNoOfConnListEntries(void);
UBYTE     cBtGetConnListEntry(UBYTE Item, UBYTE *pName, SBYTE Length, UBYTE* pType);
UBYTE     cBtGetNoOfDevListEntries(void);
UBYTE     cBtGetDevListEntry(UBYTE Item, SBYTE *pConnected, SBYTE *pType, UBYTE *pName, SBYTE Length);
UBYTE     cBtDeleteFavourItem(UBYTE *pName);
UBYTE     cBtGetNoOfSearchListEntries(void);
UBYTE     cBtGetSearchListEntry(UBYTE Item, SBYTE *pConnected, SBYTE *pType, SBYTE *pParred, UBYTE *pName, SBYTE Length);
UBYTE     cBtGetHciBusyFlag(void);
void      DecodeMode1(UBYTE BufNo);

UBYTE     cBtGetStatus(void);
void      cBtGetId(UBYTE *pId, UBYTE Length);
UBYTE     cBtSetName(UBYTE *pName, UBYTE Length);
UBYTE     cBtGetChNo(UBYTE *pName, UBYTE *pChNo);
void      cBtGetIncoming(UBYTE *pName, UBYTE *pCod, UBYTE Len);
UBYTE     cBtGetEvent(void);
UBYTE     cBtSetPin(UBYTE *pPin);
UBYTE     cBtSetPasskey(UBYTE Accept);

void      cBtSetTrustedDev(UBYTE *pBtAddr, UBYTE *pPin, UBYTE PinSize);

UWORD     cBtSetBundleId(UBYTE *pId);
UWORD     cBtSetBundleSeedId(UBYTE *pSeedId);


#endif /* C_BT_H_ */
