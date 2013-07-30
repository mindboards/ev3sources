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


/*! \page BluetoothPart Bluetooth Description
 *
 *- \subpage  InterfacetoComm
 *- \subpage  InternalBtFunctionality
 *
 */


/*! \page InterfacetoComm Comm Interface Description
 *
 *  <hr size="1"/>
 *
 *  Interfaces to C_COM
 *
 *
 */

/*! \page InternalBtFunctionality Bluetooth functionality description
 *
 *  <hr size="1"/>
 *
 *
 *  Support of up to 7 connection - either 7 outgoing connections or 1 incoming ->
 *  scatter-net not supported
 *
 *
 *  Bluetooth buffer setup
 *  ----------------------
 *
 *  RX side:
 *
 *  Bluetooth socket
 *       |
 *        --> Bluetooth RxBuf - Fragmented async. data bytes
 *                |
 *                 --> Bluetooth Msg buffer  - Collected as complete LEGO protocol messages
 *                            |
 *                             --> Com msg buffer - Transferred to com level for interpretation
 *
 *
 *  Tx side:
 *
 *  Com msg buffer
 *        |
 *         --> Bluetooth Tx Buffer
 *                     |
 *                      -->  Bluetooth socket
 *
 *
 *
 *
 *
 *  In mode2:
 *  ---------
 *
 *  RX:
 *
 *  Bluetooth socket
 *        |
 *         --> Mode2Buf - Fragmented data bytes
 *                 |
 *                  --> Mode2InBuf - Fragmented data bytes (c_i2c.c)
 *                           |
 *                            --> Transfer to Mode2 decode device
 *                                   |
 *                                    --> READBUF (return bytes from mode 2 device) -> for mode1 decoding
 *                                   |
 *                                   |
 *                                    --> WriteBuf (return bytes from mode 2 device) -> for tx to remote mode 2 device
 *
 *
 *  TX:
 *
 *  Mode2WriteBuf
 *      |
 *       --> Transfer to mode2 decoding
 *              |
 *               --> WriteBuf (return bytes from mode2 decoding) -> for tx
 *
 *
 */


#include  "lms2012.h"
#include  "c_com.h"
#include  "c_bt.h"
#include  "c_i2c.h"

#if (HARDWARE != SIMULATION)

  #include  <stdio.h>
  #include  <fcntl.h>
  #include  <stdlib.h>
  #include  <unistd.h>
  #include  <string.h>
  #include  <signal.h>
  #include  <sys/mman.h>
  #include  <time.h>
  #include  <errno.h>

  #include <bluetooth/bluetooth.h>
  #include <bluetooth/rfcomm.h>
  #include <bluetooth/hci.h>
  #include <bluetooth/hci_lib.h>
  #include <bluetooth/sdp.h>
  #include <bluetooth/sdp_lib.h>
  #include <sys/socket.h>

  #include <dbus/dbus.h>

  #include <sys/poll.h>
  #include <sys/ioctl.h>


  typedef struct
  {
    bdaddr_t  Addr;
    UBYTE     Port;
  }PAIREDDEVINFO;


  static PAIREDDEVINFO PairedDevInfo;

  BT_GLOBALS BtInstance;
  static  DBusConnection  *conn;
  static  DBusMessage     *reply;
  static  DBusMessage     *RejectReply;

  static  sdp_session_t   *session                   =   0;

  static  char            *adapter_path              =  NULL;
  char                    *agent_path                =  "/org/bluez/agent";
  static  UBYTE            SimplePairingDisable[]    = {0x06, 0x73, 0xFF};
  static  UBYTE            SimplePairingEnable[]     = {0x06, 0x7B, 0xFF};
  static  UBYTE            ExtendedFeaturesDisable[] = {0x07, 0x03, 0xFF};
  static  UBYTE            ExtendedFeaturesEnable[]  = {0x07, 0x83, 0xFF};

  static volatile sig_atomic_t __io_canceled         = 0;
  static volatile sig_atomic_t __io_terminated       = 0;

#else

  BT_GLOBALS * gBtInstance;

  void setOutputInstance(BT_GLOBALS * _Instance)
  {
    gBtInstance= _Instance;
  }

  BT_GLOBALS* getBtInstance()
  {
    return gBtInstance;
  }

#endif


#define   LEGO_BUNDLE_SEED_ID                     "9RNK8ZF528"
#define   LEGO_BUNDLE_ID                          "com.lego.lms"

enum
{
  I_AM_IN_IDLE    = 0,
  I_AM_MASTER     = 1,
  I_AM_SLAVE      = 2,
  I_AM_SCANNING   = 3,
  STOP_SCANNING   = 4,
  TURN_ON         = 5,
  TURN_OFF        = 6,
  RESTART         = 7,
  BLUETOOTH_OFF   = 8
};

enum
{
  MSG_BUF_EMPTY   = 0,
  MSG_BUF_LEN     = 1,
  MSG_BUF_BODY    = 2,
  MSG_BUF_FULL    = 3
};

/* Constants related to Decode mode */
enum
{
  MODE1 = 0,
  MODE2 = 1
};


UBYTE     BtConnectTo(UBYTE Port, bdaddr_t BtAddr);
void      BtTxMsgs(void);
void      BtSetup(UBYTE State);
UWORD     cBtHandleHCI(void);
void      BtDisconnectAll(void);
UBYTE     BtClearSearchListEntry(UBYTE Index);

UBYTE     cBtFindDevChNo(UBYTE ChNo, UBYTE *pIndex);
UBYTE     cBtFindDevConnHandle(UBYTE ConnHandle, UBYTE *pIndex);
UBYTE     cBtFindDevAdr(bdaddr_t *pAdr, UBYTE *pIndex);
void      cBtSetDevConnectedStatus(UBYTE Index);
void      cBtCloseDevConnection(UBYTE Index);
UBYTE     cBtFindSearchAdr(bdaddr_t *pAdr, UBYTE *pIndex);
void      cBtSetSearchConnectedStatus(UBYTE Index);
void      cBtClearSearchConnectedStatus(UBYTE Index);
void      BtCloseBtSocket(SLONG *pBtSocket);

static char               *get_adapter_path(DBusConnection *conn, const char *adapter);
static DBusHandlerResult   agent_message(DBusConnection *conn, DBusMessage *msg, void *data);
static int                 register_agent(DBusConnection *conn, const char *adapter_path, const char *agent_path, const char *capabilities);
static DBusHandlerResult   agent_filter(DBusConnection *conn, DBusMessage *msg, void *data);
static DBusHandlerResult   request_pincode_message(DBusConnection *conn, DBusMessage *msg, void *data);
static char               *get_default_adapter_path(DBusConnection *conn);
static void                sig_term(int sig);
static int                 unregister_agent(DBusConnection *conn, const char *adapter_path, const char *agent_path);

static int                 RemoveDevice(DBusConnection *conn, char *Device);
void                       cBtStrNoColonToBa(UBYTE *pBtStrAddr, bdaddr_t *pAddr);

void                       DecodeMode1(UBYTE BufNo);
void                       DecodeMode2(void);

void                       BtClose(void);
UBYTE                      BtIssueHciVisible(UBYTE Visible, UBYTE Page);



void    BtSetupHciSocket(void)
{
  int   dev_id;

  dev_id = hci_get_route(NULL);
  BtInstance.HciSocket.Socket = hci_open_dev( dev_id );
  if (dev_id < 0 || BtInstance.HciSocket.Socket < 0)
  {
    perror("Can't open socket");
    return;
  }
}

void      BtSetupPinEvent(void)
{
  struct  hci_filter flt;

  hci_filter_clear(&flt);
  hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
  hci_filter_set_event(EVT_CONN_COMPLETE, &flt);
  hci_filter_set_event(EVT_DISCONN_COMPLETE, &flt);
  hci_filter_set_event(EVT_LINK_KEY_NOTIFY, &flt);
  hci_filter_set_event(EVT_ENCRYPT_CHANGE, &flt);
  hci_filter_set_event(EVT_CMD_COMPLETE, &flt);
  hci_filter_set_event(EVT_CMD_STATUS, &flt);//

  hci_filter_set_event(EVT_INQUIRY_RESULT, &flt);
  hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &flt);
  hci_filter_set_event(EVT_INQUIRY_COMPLETE, &flt);
  hci_filter_set_event(EVT_REMOTE_NAME_REQ_COMPLETE, &flt);
  hci_filter_set_event(EVT_PIN_CODE_REQ, &flt);

  hci_filter_set_event(EVT_CONN_REQUEST, &flt);
  hci_filter_set_event(EVT_LINK_KEY_NOTIFY, &flt);

  if (setsockopt(BtInstance.HciSocket.Socket, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0)
  {
    perror("Can't set HCI filter");
    return;
  }
  BtInstance.HciSocket.p.fd      = BtInstance.HciSocket.Socket;
  BtInstance.HciSocket.p.events  = POLLIN | POLLERR | POLLHUP;


}


void    SetupListeningSocket(int *Socket)
{
  SLONG   TmpSockFlags;
  struct  bt_security sec;

  memset(&sec, 0, sizeof(sec));
  sec.level = 3;

  int lm_map[] =
  {
    0,
    RFCOMM_LM_AUTH,
    RFCOMM_LM_AUTH | RFCOMM_LM_ENCRYPT,
    RFCOMM_LM_AUTH | RFCOMM_LM_ENCRYPT | RFCOMM_LM_SECURE,
  }, opt = lm_map[sec.level];

  *Socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  BtInstance.ListenSocket.loc_addr.rc_family  = AF_BLUETOOTH;
  BtInstance.ListenSocket.loc_addr.rc_bdaddr  = *BDADDR_ANY;
  BtInstance.ListenSocket.loc_addr.rc_channel = 1;
  bind(*Socket, (struct sockaddr *)&(BtInstance.ListenSocket.loc_addr), sizeof(BtInstance.ListenSocket.loc_addr));

  setsockopt(*Socket, SOL_BLUETOOTH, BT_SECURITY, &sec, sizeof(sec));
  setsockopt(*Socket, SOL_RFCOMM, RFCOMM_LM, &opt, sizeof(opt));

  TmpSockFlags = fcntl(*Socket, F_GETFL, 0);
  fcntl(*Socket, F_SETFL, TmpSockFlags | O_NONBLOCK);

  listen(*Socket,1);
}


void      BtClearChBuf(UBYTE ChNo)
{
  BtInstance.BtCh[ChNo].Status           =  CH_FREE;
  BtInstance.BtCh[ChNo].ReadBuf.Status   =  READ_BUF_EMPTY;
  BtInstance.BtCh[ChNo].ReadBuf.InPtr    =  0;
  BtInstance.BtCh[ChNo].ReadBuf.OutPtr   =  0;

  if (0 == ChNo)
  {
    BtInstance.Mode2Buf.Status           =  MSG_BUF_EMPTY;
    BtInstance.Mode2Buf.InPtr            =  0;
    BtInstance.Mode2Buf.OutPtr           =  0;
  }

  BtInstance.BtCh[ChNo].MsgBuf.Status    =  MSG_BUF_EMPTY;
  BtInstance.BtCh[ChNo].MsgBuf.InPtr     =  0;
  BtInstance.BtCh[ChNo].MsgBuf.LargeMsg  =  FALSE;
  BtInstance.BtCh[ChNo].MsgBuf.MsgLen    =  0;
  BtInstance.BtCh[ChNo].MsgBuf.RemMsgLen =  0;
}


void      BtInit(char* pName)
{
  int     File;
  int     Tmp;
  FILE    *FSer;

  BtInstance.OnOffSeqCnt  =  0;
  BtInstance.NoOfConnDevs =  0;

  // Load all persistent variables
  FSer  =  fopen("./settings/BTser","r");
  if (FSer != NULL)
  {
    fgets((char*)&(BtInstance.Adr[0]), (int)13, FSer);
    fclose (FSer);
  }
  else
  {
    //Error - Fil with zeros
    strcpy((char*)&(BtInstance.Adr[0]), "000000000000");
  }

  File  =  open(NONVOL_BT_DATA,O_RDWR,0666);
  if (File >= MIN_HANDLE)
  {
    // Reads both Visible and ON/OFF status
    if (read(File,(UBYTE*)&BtInstance.NonVol, sizeof(BtInstance.NonVol)) != sizeof(BtInstance.NonVol))
    {
      close (File);
      File  =  -1;
    }
    else
    {
      close (File);
    }
  }
  else
  {
    // Default settings
    BtInstance.NonVol.Visible     =  TRUE;
    BtInstance.NonVol.On          =  FALSE;
    BtInstance.NonVol.DecodeMode  =  MODE1;

    snprintf((char*)&BtInstance.NonVol.BundleID[0], MAX_BUNDLE_ID_SIZE, "%s", (char*)LEGO_BUNDLE_ID);
    snprintf((char*)&BtInstance.NonVol.BundleSeedID[0], MAX_BUNDLE_SEED_ID_SIZE, "%s", (char*)LEGO_BUNDLE_SEED_ID);

    for(Tmp = 0; Tmp < MAX_DEV_TABLE_ENTRIES; Tmp++)
    {
      BtInstance.NonVol.DevList[Tmp].Status = DEV_EMPTY;
    }
    BtInstance.NonVol.DevListEntries = 0;
  }

  BtInstance.HciSocket.Socket = -1;

  if (TRUE == BtInstance.NonVol.On)
  {
    BtInstance.State =  TURN_ON;
    BtSetup(TURN_ON);
  }
  else
  {
    BtInstance.State =  TURN_OFF;
    BtSetup(TURN_OFF);
  }

  BtInstance.Events             =  0;
  BtInstance.TrustedDev.Status  =  FALSE;

  BtInstance.BtName[0] = 0;
  snprintf((char*)&(BtInstance.BtName[0]), vmBRICKNAMESIZE, "%s",(char*)pName);
  bacpy(&(BtInstance.TrustedDev.Adr), BDADDR_ANY);   // BDADDR_ANY = all zeros!

  I2cInit(&(BtInstance.BtCh[0].ReadBuf), &(BtInstance.Mode2WriteBuf), (char *)&(BtInstance.NonVol.BundleID[0]), (char*)&(BtInstance.NonVol.BundleSeedID[0]));
}


void      BtExit(void)
{
  int     File;

  BtClose();

  File  =  open(NONVOL_BT_DATA,O_CREAT | O_WRONLY | O_TRUNC,0666);
  if (File >= MIN_HANDLE)
  {
    write(File,(UBYTE*)&BtInstance.NonVol, sizeof(BtInstance.NonVol));
    close (File);
  }

  I2cExit();
}


void      BtCloseBtSocket(SLONG *pBtSocket)
{
  if (MIN_HANDLE <= *pBtSocket)
  {
    close(*pBtSocket);
    *pBtSocket = -1;
  }
}


void      BtCloseCh(UBYTE ChIndex)
{
  if (CH_FREE != BtInstance.BtCh[ChIndex].Status)
  {
    BtInstance.BtCh[ChIndex].Status = CH_FREE;
  }

  BtCloseBtSocket(&(BtInstance.BtCh[ChIndex].BtSocket.Socket));

  if (0 < BtInstance.NoOfConnDevs)
  {
    BtInstance.NoOfConnDevs--;
    if (0 == BtInstance.NoOfConnDevs)
    {
      BtSetup(I_AM_IN_IDLE);
    }
  }
  else
  {
    if ((I_AM_MASTER == BtInstance.State) || (I_AM_SLAVE == BtInstance.State))
    {
      //Ensure going back to idle if only pairing (no application running on remote device)
      BtSetup(I_AM_IN_IDLE);
    }
  }
}


void      BtDisconnectAll(void)
{
  UBYTE   Tmp;

  for(Tmp = 0; Tmp < MAX_DEV_TABLE_ENTRIES; Tmp++)
  {
    BtInstance.NonVol.DevList[Tmp].Connected = FALSE;
  }

  for(Tmp = 0; Tmp < NO_OF_BT_CHS; Tmp++)
  {
    if (CH_FREE != BtInstance.BtCh[Tmp].Status)
    {
      BtCloseCh(Tmp);
    }
  }
}


/*! \page ComModule
 *
 *  <hr size="1"/>
 *  <b>     write </b>
 */
/*! \brief    cBtSetup
 *
 *  Function that handles all state switching
 *
 *
 */
void      BtSetup(UBYTE State)
{
  BtInstance.OldState = BtInstance.State;
  BtInstance.State    = State;
  switch(State)
  {
    case TURN_ON:
    {
      BtInstance.OnOffSeqCnt = 0;
    }
    break;

    case TURN_OFF:
    {
      BtInstance.OnOffSeqCnt = 0;
    }
    break;

    case I_AM_IN_IDLE:
    {
      // Stop Mode2 decoding - can be called even if not used
      I2cStop();
      SetupListeningSocket(&(BtInstance.ListenSocket.Socket));
      BtInstance.PageState = SCAN_PAGE;
      BtIssueHciVisible(BtInstance.NonVol.Visible, BtInstance.PageState);
      BtInstance.HciSocket.Busy  |=  HCI_VISIBLE;
    }
    break;

    case I_AM_MASTER:
    {
      // Cannot be accepting other incoming connections.. scatter-net not supported
      BtCloseBtSocket(&(BtInstance.ListenSocket.Socket));
    }
    break;

    case I_AM_SLAVE:
    {
      // Cannot be accepting other incoming connections.. scatter-net not supported
      BtCloseBtSocket(&(BtInstance.ListenSocket.Socket));
      BtClearChBuf(0);
    }
    break;

    case I_AM_SCANNING:
    {
      UBYTE                   TmpCnt;
      inquiry_cp              cp;
      write_inquiry_mode_cp   cmp;

      // Do not accept incoming connects during scan
      BtIssueHciVisible(BtInstance.NonVol.Visible, 0);
      BtCloseBtSocket(&(BtInstance.ListenSocket.Socket));

      cmp.mode = 0x01;
      if (hci_send_cmd(BtInstance.HciSocket.Socket, OGF_HOST_CTL, OCF_WRITE_INQUIRY_MODE, WRITE_INQUIRY_MODE_RP_SIZE, &cmp) < 0)
      {
        #ifdef DEBUG
          printf("Can't set inquiry mode");
        #endif

        return;
      }

      // This is general inquiry LAP
      memset (&cp, 0, sizeof(cp));
      cp.lap[2]  = 0x9e;
      cp.lap[1]  = 0x8b;
      cp.lap[0]  = 0x33;
      cp.num_rsp = 0;
      cp.length  = 0x0F;

      #ifdef DEBUG
        printf("Starting inquiry with RSSI...\n");
      #endif

      if (hci_send_cmd (BtInstance.HciSocket.Socket, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE, &cp) < 0)
      {
        #ifdef DEBUG
          printf("Can't start inquiry");
        #endif
        return;
      }

      // Clear the search list
      for(TmpCnt = 0; TmpCnt < MAX_DEV_TABLE_ENTRIES; TmpCnt++)
      {
        BtClearSearchListEntry(TmpCnt);
      }

      BtInstance.NoOfFoundNames     = 0;
      BtInstance.NoOfFoundDev       = 0;
      BtInstance.SearchIndex        = 0;
      BtInstance.State              = I_AM_SCANNING;
      BtInstance.ScanState          = SCAN_INQ_STATE;
    }
    break;

    case STOP_SCANNING:
    {
    }
    break;

    case BLUETOOTH_OFF:
    {
    }
    break;

    case RESTART:
    {
      BtInstance.OnOffSeqCnt    =  0;
      BtInstance.RestartSeqCnt  =  0;
    }
    break;
  }
}


UWORD     BtRequestName(void)
{
  UWORD               RtnVal;
  remote_name_req_cp  cp;

  RtnVal = TRUE;

  memset(&cp, 0, sizeof(cp));
  bacpy(&cp.bdaddr, &BtInstance.SearchList[BtInstance.SearchIndex].Adr);
  cp.pscan_rep_mode = 0x02;

  hci_send_cmd(BtInstance.HciSocket.Socket, OGF_LINK_CTL, OCF_REMOTE_NAME_REQ, REMOTE_NAME_REQ_CP_SIZE, &cp);

  return(RtnVal);
}


UBYTE     BtStartScan(void)
{
  UBYTE   RtnVal;

  if(((HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy)) && (BLUETOOTH_OFF != BtInstance.State))
  {
    BtInstance.HciSocket.Busy = HCI_SCAN;
    BtSetup(I_AM_SCANNING);
    RtnVal = OK;
  }
  else
  {
    RtnVal = FAIL;
  }
  return(RtnVal);
}


UBYTE      BtStopScan(void)
{
  UBYTE    RtnVal;
  if (HCI_SCAN == BtInstance.HciSocket.Busy)
  {
    switch(BtInstance.ScanState)
    {
      case SCAN_OFF:
      {
        BtInstance.HciSocket.Busy  = HCI_IDLE;
        BtIssueHciVisible(BtInstance.NonVol.Visible, BtInstance.PageState);
      }
      break;
      case SCAN_INQ_STATE:
      {
        hci_send_cmd (BtInstance.HciSocket.Socket, OGF_LINK_CTL, OCF_INQUIRY_CANCEL, 0, NULL);
      }
      break;
      case SCAN_NAME_STATE:
      {
        BtInstance.ScanState       = SCAN_OFF;
      }
      break;
    }
    RtnVal = OK;
  }
  else
  {
    if((HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy))
    {
      BtInstance.HciSocket.Busy = HCI_IDLE;
      RtnVal = OK;
    }
    else
    {
      RtnVal = FAIL;
    }
  }
  return(RtnVal);
}


UWORD     cBtReadCh0(UBYTE *pBuf, UWORD Length)
{
  MSGBUF  *pMsgBuf;
  UWORD   RtnLen = 0;

  pMsgBuf = &(BtInstance.BtCh[0].MsgBuf);

  if (MSG_BUF_FULL == pMsgBuf->Status)
  {
    #ifdef DEBUG
      printf("\r\n MSG_BUF_FULL - Reading from cCom on Bt Channel 0 number of bytes = %d\r\n",pMsgBuf->InPtr);
    #endif

    memcpy(pBuf, pMsgBuf->Buf, (pMsgBuf->InPtr));
    RtnLen           =  pMsgBuf->InPtr;
    pMsgBuf->Status  =  MSG_BUF_EMPTY;
  }
  return(RtnLen);
}


UWORD     cBtReadCh1(UBYTE *pBuf, UWORD Length)
{
  MSGBUF  *pMsgBuf;
  UWORD   RtnLen = 0;

  pMsgBuf = &(BtInstance.BtCh[1].MsgBuf);

  if (MSG_BUF_FULL == pMsgBuf->Status)
  {

    #ifdef DEBUG
      printf("\r\n MSG_BUF_FULL on Bt Channel 1 %d bytes copied\r\n",pMsgBuf->InPtr);
    #endif

    memcpy(pBuf, pMsgBuf->Buf, (pMsgBuf->InPtr));
    RtnLen           =  pMsgBuf->InPtr;
    pMsgBuf->Status  =  MSG_BUF_EMPTY;
  }
  return(RtnLen);
}


UWORD     cBtReadCh2(UBYTE *pBuf, UWORD Length)
{
  MSGBUF  *pMsgBuf;
  UWORD   RtnLen = 0;

  pMsgBuf = &(BtInstance.BtCh[2].MsgBuf);

  if (MSG_BUF_FULL == pMsgBuf->Status)
  {

    #ifdef DEBUG
      printf("\r\n MSG_BUF_FULL on Bt Channel 1\r\n");
    #endif

    memcpy(pBuf, pMsgBuf->Buf, (pMsgBuf->InPtr));
    RtnLen           =  pMsgBuf->InPtr;
    pMsgBuf->Status  =  MSG_BUF_EMPTY;
  }
  return(RtnLen);
}


UWORD      cBtReadCh3(UBYTE *pBuf, UWORD Length)
{
  MSGBUF  *pMsgBuf;
  UWORD   RtnLen = 0;

  pMsgBuf = &(BtInstance.BtCh[3].MsgBuf);

  if (MSG_BUF_FULL == pMsgBuf->Status)
  {

    #ifdef DEBUG
      printf("\r\n MSG_BUF_FULL on Bt Channel 1\r\n");
    #endif

    memcpy(pBuf, pMsgBuf->Buf, (pMsgBuf->InPtr));
    RtnLen           =  pMsgBuf->InPtr;
    pMsgBuf->Status  =  MSG_BUF_EMPTY;
  }
  return(RtnLen);
}


UWORD      cBtReadCh4(UBYTE *pBuf, UWORD Length)
{
  MSGBUF  *pMsgBuf;
  UWORD   RtnLen = 0;

  pMsgBuf = &(BtInstance.BtCh[4].MsgBuf);

  if (MSG_BUF_FULL == pMsgBuf->Status)
  {

    #ifdef DEBUG
      printf("\r\n MSG_BUF_FULL on Bt Channel 1\r\n");
    #endif

    memcpy(pBuf, pMsgBuf->Buf, (pMsgBuf->InPtr));
    RtnLen           =  pMsgBuf->InPtr;
    pMsgBuf->Status  =  MSG_BUF_EMPTY;
  }
  return(RtnLen);
}


UWORD      cBtReadCh5(UBYTE *pBuf, UWORD Length)
{
  MSGBUF  *pMsgBuf;
  UWORD   RtnLen = 0;

  pMsgBuf = &(BtInstance.BtCh[5].MsgBuf);

  if (MSG_BUF_FULL == pMsgBuf->Status)
  {

    #ifdef DEBUG
      printf("\r\n MSG_BUF_FULL on Bt Channel 1\r\n");
    #endif

    memcpy(pBuf, pMsgBuf->Buf, (pMsgBuf->InPtr));
    RtnLen           =  pMsgBuf->InPtr;
    pMsgBuf->Status  =  MSG_BUF_EMPTY;
  }
  return(RtnLen);
}


UWORD      cBtReadCh6(UBYTE *pBuf, UWORD Length)
{
  MSGBUF  *pMsgBuf;
  UWORD   RtnLen = 0;

  pMsgBuf = &(BtInstance.BtCh[6].MsgBuf);

  if (MSG_BUF_FULL == pMsgBuf->Status)
  {

    #ifdef DEBUG
      printf("\r\n MSG_BUF_FULL on Bt Channel 1\r\n");
    #endif

    memcpy(pBuf, pMsgBuf->Buf, (pMsgBuf->InPtr));
    RtnLen           =  pMsgBuf->InPtr;
    pMsgBuf->Status  =  MSG_BUF_EMPTY;
  }
  return(RtnLen);
}

UWORD      cBtReadCh7(UBYTE *pBuf, UWORD Length)
{
  MSGBUF  *pMsgBuf;
  UWORD   RtnLen = 0;

  pMsgBuf = &(BtInstance.BtCh[7].MsgBuf);

  if (MSG_BUF_FULL == pMsgBuf->Status)
  {

    #ifdef DEBUG
      printf("\r\n MSG_BUF_FULL on Bt Channel 1\r\n");
    #endif

    memcpy(pBuf, pMsgBuf->Buf, (pMsgBuf->InPtr));
    RtnLen           =  pMsgBuf->InPtr;
    pMsgBuf->Status  =  MSG_BUF_EMPTY;
  }
  return(RtnLen);
}

void      DecodeBtStream(UBYTE BufNo)
{
  if (MODE1 == BtInstance.NonVol.DecodeMode)
  {
    DecodeMode1(BufNo);
  }
  else
  {
    // Need to decode mode2 before decoding mode1
    DecodeMode2();
  }
}


void      DecodeMode2(void)
{
  UWORD   BytesAccepted;
  SLONG   AvailBytes;

  // Buffer is dedicated to mode2 only
  // Only one bluetooth connection is valid at a time

  AvailBytes = (BtInstance.Mode2Buf.InPtr - BtInstance.Mode2Buf.OutPtr);             /* How many bytes is ready to be read */
  if (AvailBytes)
  {
    BytesAccepted = DataToMode2Decoding(&(BtInstance.Mode2Buf.Buf[0]), AvailBytes);  /* Transfer bytes to mode2 decoding   */

    if (BytesAccepted == AvailBytes)
    {
      BtInstance.Mode2Buf.OutPtr = 0;
      BtInstance.Mode2Buf.InPtr  = 0;
      BtInstance.Mode2Buf.Status = READ_BUF_EMPTY;
    }
    else
    {
      BtInstance.Mode2Buf.OutPtr += BytesAccepted;
    }
  }
}


void      DecodeMode1(UBYTE BufNo)
{
  SLONG   AvailBytes;
  READBUF *pReadBuf;
  MSGBUF  *pMsgBuf;

  #ifdef  DEBUG
    SLONG   Test;
  #endif

  /* 1. Check if there is more data to interpret */
  /* 2. Check the status of the active buffer    */
  pReadBuf = &(BtInstance.BtCh[BufNo].ReadBuf); // Source buffer
  pMsgBuf  = &(BtInstance.BtCh[BufNo].MsgBuf);  // Destination Buffer

  AvailBytes = (pReadBuf->InPtr - pReadBuf->OutPtr);          /* How many bytes is ready to be read */
  
  #ifdef DEBUG
    printf("\r\nDecode mode 1: Avail bytes = %d MsgBuf status = %d\r\n",AvailBytes,pMsgBuf->Status);
  #endif

  switch(pMsgBuf->Status)
  {
    case MSG_BUF_EMPTY:
    {
      // Message buffer is empty
      pMsgBuf->InPtr   =  0;

      if(TRUE == pMsgBuf->LargeMsg)
      {
        pMsgBuf->Status = MSG_BUF_BODY;
      }
      else
      {
        if (2 <= AvailBytes)
        {
          memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), 2);
          pMsgBuf->InPtr     += 2;
          pReadBuf->OutPtr   += 2;
          AvailBytes         -= 2;

          pMsgBuf->RemMsgLen  = (int)(pMsgBuf->Buf[0]) + ((int)(pMsgBuf->Buf[1]) * 256);
          pMsgBuf->MsgLen     = pMsgBuf->RemMsgLen;

          if (0 != pMsgBuf->RemMsgLen)
          {

            if (pMsgBuf->RemMsgLen <= AvailBytes)
            {
              // Rest of message is received move it to the message buffer
              memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), (pMsgBuf->RemMsgLen));

              AvailBytes       -=  (pMsgBuf->RemMsgLen);
              pReadBuf->OutPtr +=  (pMsgBuf->RemMsgLen);
              pMsgBuf->InPtr   +=  (pMsgBuf->RemMsgLen);
              pMsgBuf->Status   =  MSG_BUF_FULL;

              #ifdef DEBUG
                printf(" Message is received from MSG_BUF_EMPTY: ");
                for (Test = 0; Test < ((pMsgBuf->MsgLen) + 2); Test++)
                {
                  printf("%02X ", pMsgBuf->Buf[Test]);
                }
                printf(" \r\n");
              #endif

              if (0 == AvailBytes)
              {
                // Read buffer is empty
                pReadBuf->OutPtr = 0;
                pReadBuf->InPtr  = 0;
                pReadBuf->Status = READ_BUF_EMPTY;
              }
            }
            else
            {
              // Still some bytes needed to be received
              // So Read buffer is emptied
              memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), AvailBytes);

              pMsgBuf->Status     = MSG_BUF_BODY;
              pMsgBuf->RemMsgLen -= AvailBytes;
              pMsgBuf->InPtr     += AvailBytes;

              pReadBuf->OutPtr    = 0;
              pReadBuf->InPtr     = 0;
              pReadBuf->Status    = READ_BUF_EMPTY;
            }
          }
          else
          {
            if (0 == AvailBytes)
            {
              // Read buffer is empty
              pReadBuf->OutPtr = 0;
              pReadBuf->InPtr  = 0;
              pReadBuf->Status = READ_BUF_EMPTY;
            }
          }
        }
        else
        {
          // Only one byte has been received - first byte of length info
          memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), 1);
          pReadBuf->OutPtr++;
          pMsgBuf->InPtr++;

          pMsgBuf->RemMsgLen = (int)(pMsgBuf->Buf[0]);
          pMsgBuf->Status    = MSG_BUF_LEN;

          pReadBuf->OutPtr = 0;
          pReadBuf->InPtr  = 0;
          pReadBuf->Status = READ_BUF_EMPTY;
        }
      }
    }
    break;

    case MSG_BUF_LEN:
    {
      // Read the last length bytes
      memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), 1);
      pMsgBuf->InPtr++;
      pReadBuf->OutPtr++;
      AvailBytes--;

      pMsgBuf->RemMsgLen = (int)(pMsgBuf->Buf[0]) + ((int)(pMsgBuf->Buf[1]) * 256);
      pMsgBuf->MsgLen    = pMsgBuf->RemMsgLen;

      if (0 != pMsgBuf->RemMsgLen)
      {

        if ((pMsgBuf->RemMsgLen) <= AvailBytes)
        {
          // rest of message is received move it to the message buffer
          memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), (pMsgBuf->RemMsgLen));

          AvailBytes       -= (pMsgBuf->RemMsgLen);
          pReadBuf->OutPtr += (pMsgBuf->RemMsgLen);
          pMsgBuf->InPtr   += (pMsgBuf->RemMsgLen);
          pMsgBuf->Status   = MSG_BUF_FULL;

          #ifdef DEBUG
            printf(" Message is received from MSG_BUF_EMPTY: ");
            for (Test = 0; Test < ((pMsgBuf->MsgLen) + 2); Test++)
            {
              printf("%02X ", pMsgBuf->Buf[Test]);
            }
            printf(" \r\n");
          #endif

          if (0 == AvailBytes)
          {
            // Read buffer is empty
            pReadBuf->OutPtr = 0;
            pReadBuf->InPtr  = 0;
            pReadBuf->Status = READ_BUF_EMPTY;
          }
        }
        else
        {
          // Still some bytes needed to be received
          // So receive buffer is emptied
          memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), AvailBytes);

          pMsgBuf->Status     = MSG_BUF_BODY;
          pMsgBuf->RemMsgLen -= AvailBytes;
          pMsgBuf->InPtr     += AvailBytes;

          pReadBuf->OutPtr    = 0;
          pReadBuf->InPtr     = 0;
          pReadBuf->Status    = READ_BUF_EMPTY;
        }
      }
      else
      {
        if (0 == AvailBytes)
        {
          // Read buffer is empty
          pReadBuf->OutPtr = 0;
          pReadBuf->InPtr  = 0;
          pReadBuf->Status = READ_BUF_EMPTY;

          pMsgBuf->Status  = MSG_BUF_EMPTY;
        }
      }
    }
    break;

    case MSG_BUF_BODY:
    {
      ULONG  BufFree;

      BufFree = (sizeof(pMsgBuf->Buf) - (pMsgBuf->InPtr));
      if (BufFree < (pMsgBuf->RemMsgLen))
      {

        pMsgBuf->LargeMsg = TRUE;

        //This is large message
        if (BufFree >= AvailBytes)
        {
          //The available bytes can be included in this buffer
          memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), AvailBytes);

          //Buffer is still not full
          pMsgBuf->Status     = MSG_BUF_BODY;
          pMsgBuf->RemMsgLen -= AvailBytes;
          pMsgBuf->InPtr     += AvailBytes;

          //Readbuffer has been completely emptied
          pReadBuf->OutPtr    = 0;
          pReadBuf->InPtr     = 0;
          pReadBuf->Status    = READ_BUF_EMPTY;
        }
        else
        {
          //The available bytes cannot all be included in the buffer
          memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), BufFree);
          pReadBuf->OutPtr   +=  BufFree;
          pMsgBuf->InPtr     +=  BufFree;
          pMsgBuf->RemMsgLen -=  BufFree;
          pMsgBuf->Status     =  MSG_BUF_FULL;
        }
      }
      else
      {
        pMsgBuf->LargeMsg = FALSE;

        if ((pMsgBuf->RemMsgLen) <= AvailBytes)
        {
          // rest of message is received move it to the message buffer
          memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), (pMsgBuf->RemMsgLen));

          AvailBytes       -= (pMsgBuf->RemMsgLen);
          pReadBuf->OutPtr += (pMsgBuf->RemMsgLen);
          pMsgBuf->InPtr   += (pMsgBuf->RemMsgLen);
          pMsgBuf->Status   = MSG_BUF_FULL;

          #ifdef DEBUG
            printf(" Message is received from MSG_BUF_EMPTY: ");
            for (Test = 0; Test < ((pMsgBuf->MsgLen) + 2); Test++)
            {
              printf("%02X ", pMsgBuf->Buf[Test]);
            }
            printf(" \r\n");
          #endif

          if (0 == AvailBytes)
          {
            // Read buffer is empty
            pReadBuf->OutPtr = 0;
            pReadBuf->InPtr  = 0;
            pReadBuf->Status = READ_BUF_EMPTY;
          }
        }
        else
        {
          // Still some bytes needed to be received
          // So receive buffer is emptied
          memcpy(&(pMsgBuf->Buf[pMsgBuf->InPtr]), &(pReadBuf->Buf[pReadBuf->OutPtr]), AvailBytes);

          pMsgBuf->Status     = MSG_BUF_BODY;
          pMsgBuf->RemMsgLen -= AvailBytes;
          pMsgBuf->InPtr     += AvailBytes;

          pReadBuf->OutPtr    = 0;
          pReadBuf->InPtr     = 0;
          pReadBuf->Status    = READ_BUF_EMPTY;
        }
      }
    }
    break;

    case MSG_BUF_FULL:
    {
    }
    break;

    default:
    {
    }
    break;
  }
}


void      BtTurnOnSeq(void)
{
  switch(BtInstance.OnOffSeqCnt)
  {
    case 0:
    {
      UBYTE   Tmp;
      struct  timespec  TimerData;

      // Clear all channel buffers
      memset(&(BtInstance.BtCh), 0, sizeof(BtInstance.BtCh));
      for (Tmp = 0; Tmp < NO_OF_BT_CHS; Tmp++)
      {
        BtClearChBuf(Tmp);
        BtInstance.BtCh[Tmp].BtSocket.Socket = -1;
      }
      for (Tmp = 0; Tmp < MAX_DEV_TABLE_ENTRIES; Tmp++)
      {
        BtInstance.NonVol.DevList[Tmp].ChNo = NO_OF_BT_CHS;
      }

      BtInstance.Mode2WriteBuf.InPtr  =  0;
      BtInstance.Mode2WriteBuf.OutPtr =  0;

      BtInstance.ScanState            =  SCAN_OFF;
      BtInstance.PageState            =  TRUE;
      BtInstance.ListenSocket.Socket  =  -1;
      BtInstance.NoOfFoundNames       =   0;
      BtInstance.NoOfConnDevs         =   0;

      BtInstance.HciSocket.Socket = hci_open_dev( 0 );
      ioctl(BtInstance.HciSocket.Socket, HCIDEVDOWN, 0);
      ioctl(BtInstance.HciSocket.Socket, HCIDEVUP, 0);

      clock_gettime(CLOCK_MONOTONIC,&TimerData);
      BtInstance.Delay  =  (ULONG)TimerData.tv_nsec;

      BtInstance.OnOffSeqCnt++ ;
    }
    break;

    case 1:
    {
      struct  timespec  TimerData;
      ULONG   Time;

      cBtHandleHCI();

      clock_gettime(CLOCK_MONOTONIC,&TimerData);
      Time = (ULONG)TimerData.tv_nsec;
      if (400000000 < (Time - BtInstance.Delay))
      {
        // Wait min. 400mS
        // Events are setup here to be ready for coming hci_send_cmd
        BtSetupPinEvent();
        BtInstance.OnOffSeqCnt++;
      }
    }
    break;

    case 2:
    {
      if (MODE2 == BtInstance.NonVol.DecodeMode)
      {
        hci_send_cmd(BtInstance.HciSocket.Socket, OGF_VENDOR_CMD, 0xFF26, 0x03, &SimplePairingEnable);
      }
      else
      {
        hci_send_cmd(BtInstance.HciSocket.Socket, OGF_VENDOR_CMD, 0xFF26, 0x03, &SimplePairingDisable);
      }
      BtInstance.HciSocket.WaitForEvent = TRUE;
      BtInstance.OnOffSeqCnt++;
    }
    break;

    case 3:
    {
      cBtHandleHCI();
      if (FALSE == BtInstance.HciSocket.WaitForEvent)
      {
        if (MODE2 == BtInstance.NonVol.DecodeMode)
        {
          hci_send_cmd(BtInstance.HciSocket.Socket, OGF_VENDOR_CMD, 0xFF26, 0x03, &ExtendedFeaturesEnable);
        }
        else
        {
          hci_send_cmd(BtInstance.HciSocket.Socket, OGF_VENDOR_CMD, 0xFF26, 0x03, &ExtendedFeaturesDisable);
        }
        BtInstance.HciSocket.WaitForEvent = TRUE;
        BtInstance.OnOffSeqCnt++;
      }
    }
    break;

    case 4:
    {
      cBtHandleHCI();
      if (FALSE == BtInstance.HciSocket.WaitForEvent)
      {
        UBYTE Auth;
        if (MODE2 == BtInstance.NonVol.DecodeMode)
        {
          Auth = AUTH_DISABLED;
        }
        else
        {
          Auth = AUTH_ENABLED;
        }

        hci_send_cmd(BtInstance.HciSocket.Socket, OGF_HOST_CTL, OCF_WRITE_AUTH_ENABLE, 1, &Auth);
        BtInstance.HciSocket.WaitForEvent = TRUE;
        BtInstance.OnOffSeqCnt++;
      }
    }
    break;

    case 5:
    {
      struct  timespec  TimerData;

      cBtHandleHCI();
      if (FALSE == BtInstance.HciSocket.WaitForEvent)
      {
        ioctl(BtInstance.HciSocket.Socket, HCIDEVDOWN, 0);
        ioctl(BtInstance.HciSocket.Socket, HCIDEVUP, 0);

        clock_gettime(CLOCK_MONOTONIC,&TimerData);
        BtInstance.Delay  =  (ULONG)TimerData.tv_nsec;

        BtInstance.OnOffSeqCnt++;
      }
    }
    break;

    case 6:
    {
      struct  timespec  TimerData;
      ULONG   Time;

      cBtHandleHCI();

      clock_gettime(CLOCK_MONOTONIC,&TimerData);
      Time = (ULONG)TimerData.tv_nsec;
      if (400000000 < (Time - BtInstance.Delay))
      {

        // Update Name
        BtIssueHciVisible(BtInstance.NonVol.Visible, BtInstance.PageState);

        BtInstance.HciSocket.WaitForEvent = TRUE;
        BtInstance.OnOffSeqCnt++;
      }
    }
    break;

    case 7:
    {
      cBtHandleHCI();
      if (FALSE == BtInstance.HciSocket.WaitForEvent)
      {
        change_local_name_cp  cp;

        snprintf((char *)&(cp.name[0]), vmBRICKNAMESIZE, "%s", (char *)BtInstance.BtName);
        hci_send_cmd(BtInstance.HciSocket.Socket, OGF_HOST_CTL, OCF_CHANGE_LOCAL_NAME, CHANGE_LOCAL_NAME_CP_SIZE, &cp);
        BtInstance.HciSocket.WaitForEvent = TRUE;
        BtInstance.OnOffSeqCnt++;
      }
    }
    break;

    case 8:
    {
      cBtHandleHCI();
      if (FALSE == BtInstance.HciSocket.WaitForEvent)
      {
        /* Setup Mode2 if it is enabled */
        if (MODE2 == BtInstance.NonVol.DecodeMode)
        {
          uint32_t svc_uuid_int[]       =   { 0, 0xDEFACADE, 0xAFDECADE, 0xFFCACADE };
          uint8_t rfcomm_channel        =   1;
          const char *service_name      =   "Wireless EV3";
          const char *service_dsc       =   "MINDSTORMS EV3";
          const char *service_prov      =   "LEGO";

          int err = 0;

          uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid;//, svc_class_uuid;
          sdp_list_t *l2cap_list        =   0,
                     *rfcomm_list       =   0,
                     *root_list         =   0,
                     *proto_list        =   0,
                     *access_proto_list =   0,
                     *svc_class_list    =   0,
                     *profile_list      =   0;
          sdp_data_t *channel           =   0;                //, *psm = 0;
          sdp_profile_desc_t profile;

          UBYTE *pUUID;
          sdp_record_t *record = sdp_record_alloc();

          // set the general service ID
          sdp_uuid128_create( &svc_uuid, &svc_uuid_int );

          pUUID = &(svc_uuid.value.uuid128.data[0]);

          sdp_set_service_id( record, svc_uuid );

          pUUID = &(record->svclass.value.uuid128.data[0]);

          // set the service class
          svc_class_list = sdp_list_append(0, &svc_uuid);
          sdp_set_service_classes(record, svc_class_list);

          pUUID = &(record->svclass.value.uuid128.data[0]);

          // set the Bluetooth profile information
          sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
          profile.version = 0x0100;
          profile_list = sdp_list_append(0, &profile);
          sdp_set_profile_descs(record, profile_list);

          // make the service record publicly browsable
          sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
          root_list = sdp_list_append(0, &root_uuid);
          sdp_set_browse_groups( record, root_list );

          // set l2cap information
          sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
          l2cap_list = sdp_list_append( 0, &l2cap_uuid );
          proto_list = sdp_list_append( 0, l2cap_list );

          // register the RFCOMM channel for RFCOMM sockets
          sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
          channel     = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
          rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
          sdp_list_append( rfcomm_list, channel );
          sdp_list_append( proto_list, rfcomm_list );
          access_proto_list = sdp_list_append( 0, proto_list );
          sdp_set_access_protos( record, access_proto_list );

          // set the name, provider, and description
          sdp_set_info_attr(record, service_name, service_prov, service_dsc);

          session = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY );

          pUUID = &(record->svclass.value.uuid128.data[0]);

          err = sdp_record_register(session, record, 0);

          // cleanup
          sdp_data_free( channel );
          sdp_list_free( l2cap_list, 0 );
          sdp_list_free( rfcomm_list, 0 );
          sdp_list_free( root_list, 0 );
          sdp_list_free( access_proto_list, 0 );

          sdp_list_free( proto_list, 0 );
          sdp_list_free( svc_class_list, 0 );
          sdp_list_free( profile_list, 0 );

        }

        BtInstance.OnOffSeqCnt++;
      }
    }
    break;

    case 9:
    {
      struct   agent;
      char     match_string[128], *adapter_id = NULL;
      struct   sigaction sa;

      static const DBusObjectPathVTable agent_table =
      {
        .message_function = agent_message,
      };
      const char *capabilities = "DisplayYesNo";

      cBtHandleHCI();
      if (FALSE == BtInstance.HciSocket.WaitForEvent)
      {
        BtInstance.OnOffSeqCnt      =  0;
        BtInstance.NonVol.On        =  TRUE;
        BtInstance.HciSocket.Busy  &=  ~HCI_ONOFF;
        BtSetup(I_AM_IN_IDLE);

        conn          =  dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
        adapter_path  =  get_adapter_path(conn, adapter_id);

        if (!dbus_connection_register_object_path(conn, agent_path, &agent_table, NULL))
        {
          fprintf(stderr, "Can't register object path for agent\n");
        }

        if (register_agent(conn, adapter_path, agent_path, capabilities) < 0)
        {
          dbus_connection_unref(conn);
          exit(1);
        }

        if (!dbus_connection_add_filter(conn, agent_filter, NULL, NULL))
          fprintf(stderr, "Can't add signal filter");

        snprintf(match_string, sizeof(match_string),
            "interface=%s,member=NameOwnerChanged,arg0=%s",
            DBUS_INTERFACE_DBUS, "org.bluez");

        dbus_bus_add_match(conn, match_string, NULL);

        memset(&sa, 0, sizeof(sa));
        sa.sa_flags   = SA_NOCLDSTOP;
        sa.sa_handler = sig_term;
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGINT,  &sa, NULL);
      }
    }
    break;
  }
}


void      BtClose(void)
{
  BtDisconnectAll();

  if (TRUE == BtInstance.NonVol.On)
  {
    // Unregister agent can only be done if already registered
    unregister_agent(conn, adapter_path, agent_path);
  }

  if (MODE2 == BtInstance.NonVol.DecodeMode)
  {
    if (0 != session)
    {
      sdp_close(session);
      session = 0;
    }
  }

  BtCloseBtSocket(&(BtInstance.ListenSocket.Socket));

  if (MIN_HANDLE <= BtInstance.HciSocket.Socket)
  {
    ioctl(BtInstance.HciSocket.Socket, HCIDEVDOWN, 0);
    hci_close_dev(BtInstance.HciSocket.Socket);
    BtInstance.HciSocket.Socket = -1;
  }

  I2cStop();
}


void      BtTurnOffSeq(void)
{
  BtClose();

  BtSetup(BLUETOOTH_OFF);
  BtInstance.NonVol.On       =  FALSE;
  BtInstance.HciSocket.Busy  =  HCI_IDLE;
  BtInstance.OnOffSeqCnt     =  0;
}


void      BtUpdate(void)
{

  switch(BtInstance.State)
  {
    case I_AM_IN_IDLE:
    {
      BTSOCKET  *pBtSocket;
      UBYTE     Index;

      cBtHandleHCI();
      dbus_connection_read_write_dispatch(conn, 0);

      pBtSocket = &(BtInstance.BtCh[0].BtSocket); // BtCh = 0 is the slave port

      // Check for incoming connection from known device
      BtInstance.ListenSocket.opt   = sizeof(struct sockaddr_rc );
      pBtSocket->Socket = accept(BtInstance.ListenSocket.Socket, (struct sockaddr *)&(BtInstance.ListenSocket.rem_addr), &(BtInstance.ListenSocket.opt));
      if (0 <= pBtSocket->Socket)
      {

        #ifdef DEBUG
          char tmpbuf[50];
        #endif

        bacpy(&(pBtSocket->Addr),(bdaddr_t*)(&(BtInstance.ListenSocket.rem_addr.rc_bdaddr)));

        #ifdef DEBUG
          ba2str((const bdaddr_t*)(&(BtInstance.ListenSocket.rem_addr.rc_bdaddr)), tmpbuf);
          printf("Remote connection accepted, address: %s \r\n",tmpbuf);
        #endif

        if (OK == cBtFindDevAdr(&(pBtSocket->Addr), &Index))
        {
          cBtSetDevConnectedStatus(Index);
        }
        if (OK == cBtFindSearchAdr(&(BtInstance.NonVol.DevList[Index].Adr), &Index))
        {
          cBtSetSearchConnectedStatus(Index);
        }

        BtSetup(I_AM_SLAVE);
        BtInstance.BtCh[0].Status = CH_CONNECTED;

        if (MODE2 == BtInstance.NonVol.DecodeMode)
        {
          I2cStart();
        }
      }
    }
    break;

    case I_AM_SLAVE:
    {
      int       BytesRead;
      READBUF   *pReadBuf;
      BTSOCKET  *pBtSocket;

      cBtHandleHCI();

      if (MODE1 == BtInstance.NonVol.DecodeMode)
      {
        pReadBuf  = &(BtInstance.BtCh[0].ReadBuf);   // BtCh = 0 is the slave port
      }
      else
      {
        pReadBuf  = &(BtInstance.Mode2Buf);          // dedicated buffer used for Mode2
      }

      pBtSocket = &(BtInstance.BtCh[0].BtSocket);

      if ((pBtSocket->Socket != -1))
      {
        /* Check if it is possible to read more byte from the BT stream */
        if (READ_BUF_EMPTY  ==  pReadBuf->Status)
        {
          pBtSocket->Cmdtv.tv_sec    =  0;
          pBtSocket->Cmdtv.tv_usec   =  0;
          FD_ZERO(&(pBtSocket->Cmdfds));
          FD_SET(pBtSocket->Socket, &(pBtSocket->Cmdfds));
          if (select(pBtSocket->Socket + 1, &(pBtSocket->Cmdfds), NULL, NULL, &(pBtSocket->Cmdtv)))
          {
            BytesRead = read(pBtSocket->Socket, pReadBuf->Buf, sizeof(pReadBuf->Buf));

            if (0 <= BytesRead)
            {
              if (0 < BytesRead)
              {

                #ifdef DEBUG
                  printf("\r\nData received on BT in Slave mode");
                #endif

                pReadBuf->OutPtr = 0;
                pReadBuf->InPtr  = BytesRead;
                pReadBuf->Status = READ_BUF_FULL;

                #ifdef DEBUG
                  for (Cnt = 0; Cnt < BytesRead; Cnt++)
                  {
                    printf("\r\n Rx byte nr %02d = %02X",Cnt,pReadBuf->Buf[Cnt]);
                  }
                  printf("\r\n");
                #endif

                DecodeBtStream((UBYTE) 0);
              }
            }
            else
            {
              // Socket closed at the other end -> close it
              BtCloseBtSocket(&(BtInstance.BtCh[0].BtSocket.Socket));
            }
          }
        }
        else
        {
          DecodeBtStream((UBYTE) 0);
        }
      }
      usleep(20);
    }
    break;

    case I_AM_MASTER:
    {
      // Can have up till 7 slaves - that is having up to 7 sockets to serve...
      // All packets go into the same buffer for the communication module and VM
      // and all packets needs to be wrapped with the channel number
      UBYTE     Cnt;
      UBYTE     Index;
      int       BytesRead;
      READBUF   *pReadBuf;
      BTSOCKET  *pBtSocket;
      int       SocketReturn;

      cBtHandleHCI();
      dbus_connection_read_write_dispatch(conn, 0);

      // Check all 7 channels
      for(Cnt = 1; Cnt < NO_OF_BT_CHS; Cnt++)
      {

        pReadBuf  = &(BtInstance.BtCh[Cnt].ReadBuf);
        pBtSocket = &(BtInstance.BtCh[Cnt].BtSocket);

        if ((CH_FREE != BtInstance.BtCh[Cnt].Status) && (pBtSocket->Socket != -1))
        {
          if ((READ_BUF_EMPTY  ==  pReadBuf->Status))
          {
            pBtSocket->Cmdtv.tv_sec    =  0;
            pBtSocket->Cmdtv.tv_usec   =  0;
            FD_ZERO(&(pBtSocket->Cmdfds));
            FD_SET(pBtSocket->Socket, &(pBtSocket->Cmdfds));

            if (CH_CONNECTING == BtInstance.BtCh[Cnt].Status)
            {
              // When writing becomes possible then socket is up running
              SocketReturn = select(pBtSocket->Socket + 1, NULL, &(pBtSocket->Cmdfds), NULL, &(pBtSocket->Cmdtv));

              if (0 < SocketReturn)
              {
                // This is acceptance from the slave
                if (OK == cBtFindDevChNo(Cnt, &Index))
                {
                  cBtSetDevConnectedStatus(Index);
                }
                if (OK == cBtFindSearchAdr(&(BtInstance.NonVol.DevList[Index].Adr), &Index))
                {
                  cBtSetSearchConnectedStatus(Index);
                }

                BtInstance.HciSocket.Busy   &= ~HCI_CONNECT;
                BtInstance.BtCh[Cnt].Status  =  CH_CONNECTED;
              }
            }
            else
            {
              if (0 < select(pBtSocket->Socket + 1, &(pBtSocket->Cmdfds), NULL, NULL, &(pBtSocket->Cmdtv)))
              {
                BytesRead = read(pBtSocket->Socket, pReadBuf->Buf, sizeof(pReadBuf->Buf));
                if (0 <= BytesRead)
                {
                  if (0 < BytesRead)
                  {
                    pReadBuf->OutPtr = 0;
                    pReadBuf->InPtr  = BytesRead;
                    pReadBuf->Status = READ_BUF_FULL;

                    DecodeBtStream(Cnt);
                  }
                }
                else
                {
                  BtCloseBtSocket(&(pBtSocket->Socket));
                }
              }
            }
          }
          else
          {
           DecodeBtStream(Cnt);
          }
        }
      }
      usleep(20);
    }
    break;

    case I_AM_SCANNING:
    {
      cBtHandleHCI();
    }
    break;

    case TURN_ON:
    {
      BtTurnOnSeq();
    }
    break;

    case TURN_OFF:
    {
      BtTurnOffSeq();
    }
    break;

    case RESTART:
    {
      if (0 == BtInstance.RestartSeqCnt)
      {
        BtTurnOffSeq();

        // Check if power down is done
        if (0 == (HCI_ONOFF & BtInstance.HciSocket.Busy))
        {
          BtInstance.RestartSeqCnt++;
          BtInstance.HciSocket.Busy |= HCI_ONOFF;

          //Stay in this state - not an elegant way to do this!
          //But the BtTurnOffSeq sets state to OFF when done.
          BtInstance.State = RESTART;
        }
      }
      else
      {
        BtTurnOnSeq();
      }
    }
    break;

    case BLUETOOTH_OFF:
    {
    }
    break;

    default:
    {
    }
    break;
  }
  BtTxMsgs();
}


void    create_paired_device_reply(DBusPendingCall *pending, void *user_data)
{
  PAIREDDEVINFO *pPairedDevInfo;
  struct         sockaddr_rc  addr;
  int            sock_flags, status;


  // Get the device info from the user data (device info is bluetooth addr and port number)
  pPairedDevInfo = (PAIREDDEVINFO*)user_data;

  BtInstance.BtCh[pPairedDevInfo->Port].BtSocket.Socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  addr.rc_family  = AF_BLUETOOTH;
  addr.rc_channel = 1;

  bacpy(&(addr.rc_bdaddr), &(pPairedDevInfo->Addr));

  sock_flags = fcntl(BtInstance.BtCh[pPairedDevInfo->Port].BtSocket.Socket, F_GETFL, 0);
  fcntl(BtInstance.BtCh[pPairedDevInfo->Port].BtSocket.Socket, F_SETFL, sock_flags | O_NONBLOCK);

  status = connect(BtInstance.BtCh[pPairedDevInfo->Port].BtSocket.Socket,(struct sockaddr *) &addr, sizeof(addr));
  BtClearChBuf(pPairedDevInfo->Port);
  BtInstance.BtCh[pPairedDevInfo->Port].Status = CH_CONNECTING;

  usleep(20);
}


UBYTE   create_paired_device(DBusConnection *conn, const char *adapter_path,
                             const char     *agent_path,
                             const char     *capabilities,
                             const char     *device,
                             UBYTE           Port)
{
  dbus_bool_t      success;
  DBusMessage     *msg;
  DBusPendingCall *pending;

  //Copy the device info to be used when paired reply is issued
  str2ba(device, &(PairedDevInfo.Addr));
  PairedDevInfo.Port = Port;

  msg = dbus_message_new_method_call("org.bluez", adapter_path, "org.bluez.Adapter", "CreatePairedDevice");
  if (!msg)
  {
    fprintf(stderr, "Can't allocate new method call\n");
    return(0);
  }

  dbus_message_append_args(msg, DBUS_TYPE_STRING, &device, DBUS_TYPE_OBJECT_PATH, &agent_path, DBUS_TYPE_STRING, &capabilities, DBUS_TYPE_INVALID);

  success = dbus_connection_send_with_reply(conn, msg, &pending, -1);

  if (pending)
  {
    dbus_pending_call_set_notify(pending, create_paired_device_reply, (void*)&PairedDevInfo, NULL);
  }

  dbus_message_unref(msg);

  if (!success)
  {
    fprintf(stderr, "Not enough memory for message send\n");
    return(0);
  }

  dbus_connection_flush(conn);
  usleep(20);

  return(1);
}


UBYTE   BtConnectTo(UBYTE Port, bdaddr_t BtAddr)
{
  const char   *capabilities = "DisplayYesNo";
  char          Addr[20];
  char          Agent[128];
  UBYTE         success;
  UBYTE         RtnVal;

  snprintf(Agent, sizeof(Agent), "/org/bluez/agent_%d", getpid());

  ba2str((bdaddr_t *)(&(BtAddr)), Addr);

  success = create_paired_device(conn, adapter_path, Agent, capabilities, Addr, Port);

  if (!success)
  {
    RtnVal = FALSE;
  }
  else
  {
    RtnVal = TRUE;
  }
  return(RtnVal);
}


UBYTE   BtSetMode2(UBYTE Mode2)
{
  UBYTE   RtnVal = OK;

  if ((HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy))
  {
    if (Mode2)
    {
      if (0 == BtInstance.NonVol.DecodeMode)
      {
        // If not already mode2 then set mode2
        BtInstance.NonVol.DecodeMode = MODE2;

        // Recycle on/off on BT to enable the changes
        if (BLUETOOTH_OFF != BtInstance.State)
        {
          BtInstance.HciSocket.Busy = HCI_RESTART;
          BtSetup(RESTART);
        }
      }
      else
      {
        BtInstance.HciSocket.Busy = HCI_IDLE;
      }
    }
    else
    {
      if (BtInstance.NonVol.DecodeMode)
      {
        // If in mode2 then set mode1
        BtInstance.NonVol.DecodeMode = MODE1;

        // Recycle on/off on BT to enable the changes
        if (BLUETOOTH_OFF != BtInstance.State)
        {
          BtInstance.HciSocket.Busy  = HCI_RESTART;
          BtSetup(RESTART);
        }
      }
      else
      {
        BtInstance.HciSocket.Busy = HCI_IDLE;
      }
    }
  }
  else
  {
    RtnVal = FAIL;
  }

  return(RtnVal);
}


UBYTE     BtGetMode2(UBYTE *pMode2)
{
  UBYTE   RtnVal = OK;

  *pMode2 = BtInstance.NonVol.DecodeMode;

  return(RtnVal);
}


UBYTE     BtSetOnOff(UBYTE On)
{
  UBYTE   RtnVal = OK;

  if ((HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy))
  {
    if (On)
    {
      if(BLUETOOTH_OFF == BtInstance.State)
      {
        BtSetup(TURN_ON);
        BtInstance.HciSocket.Busy = HCI_ONOFF;
      }
      else
      {
        // Already ON
        BtInstance.HciSocket.Busy = HCI_IDLE;
      }
    }
    else
    {
      if (BLUETOOTH_OFF != BtInstance.State)
      {
        BtSetup(TURN_OFF);
        BtInstance.HciSocket.Busy = HCI_ONOFF;
      }
      else
      {
        // Already off
        BtInstance.HciSocket.Busy = HCI_IDLE;
      }
    }
  }
  else
  {
    RtnVal = FAIL;
  }

  return(RtnVal);
}


UBYTE     BtGetOnOff(UBYTE *On)
{
  UBYTE   RtnVal = OK;

  *On    = BtInstance.NonVol.On;

  return(RtnVal);
}


UBYTE     BtIssueHciVisible(UBYTE Visible, UBYTE Page)
{
  UBYTE   Visibility;
  UBYTE   Status;

  Visibility = 0;

  if (Page)
  {
    Visibility |= SCAN_PAGE;
  }

  if (Visible)
  {
    Visibility |= SCAN_INQUIRY;
  }

  Status =  hci_send_cmd(BtInstance.HciSocket.Socket, OGF_HOST_CTL, OCF_WRITE_SCAN_ENABLE, 1, &Visibility);

  return(Status);
}


/* Set visibility ON/OFF - which is the capability to respond to scanning */
UBYTE     BtSetVisibility(UBYTE State)
{
  UBYTE   Status;
  UBYTE   RtnVal;

  RtnVal = OK;

  if ((HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy))
  {
    if (BLUETOOTH_OFF != BtInstance.State)
    {
      Status = BtIssueHciVisible(State, BtInstance.PageState);
      if (0 == Status)
      {
        BtInstance.HciSocket.Busy  =  HCI_VISIBLE;
        BtInstance.NonVol.Visible  =  State;
      }
      else
      {
        BtInstance.HciSocket.Busy  =  HCI_FAIL;
      }
    }
    else
    {
      BtInstance.HciSocket.Busy = HCI_IDLE;
      BtInstance.NonVol.Visible = State;
    }
  }
  else
  {
    RtnVal = FAIL;
  }

  return(RtnVal);
}


/* Get visibility */
UBYTE     BtGetVisibility(void)
{
  return(BtInstance.NonVol.Visible);
}


UBYTE     cBtFindDevName(UBYTE *pItem, UBYTE *pName, UBYTE StartIndex)
{
  UBYTE   RtnVal = FALSE;
  UBYTE   Index;

  Index = StartIndex;
  if (0 == pName[0])
  {
    // If search name is 0 then it is wildcard *
    while((0 == BtInstance.NonVol.DevList[Index].Name[0]) && (Index < MAX_DEV_TABLE_ENTRIES))
    {
      Index++;
    }
  }
  else
  {
    while((0 != strcmp((char*)pName,(char*)BtInstance.NonVol.DevList[Index].Name)) && (Index < MAX_DEV_TABLE_ENTRIES))
    {
      Index++;
    }
  }

  if (Index < MAX_DEV_TABLE_ENTRIES)
  {
    RtnVal = TRUE;
    *pItem = Index;
  }
  return(RtnVal);
}


UBYTE     BtClearSearchListEntry(UBYTE Index)
{
  UBYTE   RtnVal;

  RtnVal = FAIL;
  if (Index < MAX_DEV_TABLE_ENTRIES)
  {
    memset(&(BtInstance.SearchList[Index].Adr.b[0]), 0, sizeof(bdaddr_t));
    BtInstance.SearchList[Index].Name[0]    =  0;
    BtInstance.SearchList[Index].Connected  =  FALSE;
    BtInstance.SearchList[Index].Parred     =  FALSE;
    RtnVal = OK;
  }
  return(RtnVal);
}


UBYTE     cBtFindSearchName(UBYTE *pItem, UBYTE *pName)
{
  UBYTE   RtnVal = FALSE;
  UBYTE   Index;

  Index = 0;
  while((0 != strcmp((char*)pName,(char*)BtInstance.SearchList[Index].Name)) && (Index < MAX_DEV_TABLE_ENTRIES))
  {
    Index++;
  }
  if (Index < MAX_DEV_TABLE_ENTRIES)
  {
    RtnVal = TRUE;
    *pItem = Index;
  }
  return(RtnVal);
}


UBYTE     Connect(bdaddr_t BdAddr, UBYTE PortNo)
{
  UBYTE   RtnVal;
  UBYTE   Tmp;

  #ifdef DEBUG
    char    Addr[20];
  #endif

  RtnVal = FALSE;
  if (I_AM_IN_IDLE == BtInstance.State)
  {
    // State allows to connect to other devices
    #ifdef DEBUG
      printf("Connecting from IDLE... \r\n");
    #endif

    BtSetup(I_AM_MASTER);
    if (TRUE == BtConnectTo(PortNo, BdAddr))
    {
      RtnVal = TRUE;
    }
    else
    {
      //Connection attempt failed return to idle
      BtSetup(I_AM_IN_IDLE);
      RtnVal = FALSE;
    }
  }
  else
  {
    if (I_AM_MASTER == BtInstance.State)
    {
      // Can be the master of up to 7 units
      #ifdef DEBUG
        ba2str(&(BtInstance.NonVol.DevList[Item].Adr), Addr);
        printf("Connecting from MASTER PortIndex = %d, Addr = %s \r\n",PortIndex,Addr);
      #endif

      if (FALSE == BtConnectTo(PortNo, BdAddr))
      {
        RtnVal = TRUE;
      }
      else
      {
        RtnVal = FALSE;
      }
    }
    else
    {
      if (I_AM_SLAVE == BtInstance.State)
      {
        // An outgoing connection will always disconnect Ch 0
        // if already connected
        for(Tmp = 0; Tmp < MAX_DEV_TABLE_ENTRIES; Tmp++)
        {
          if ((TRUE == BtInstance.NonVol.DevList[Tmp].Connected) && (0 == BtInstance.NonVol.DevList[Tmp].ChNo))
          {
            cBtDisconnect((UBYTE *)BtInstance.NonVol.DevList[Tmp].Name);
            Tmp = MAX_DEV_TABLE_ENTRIES;
          }
        }
        BtSetup(I_AM_MASTER);

        if (TRUE == BtConnectTo(PortNo, BdAddr))
        {
          RtnVal = TRUE;
        }
        else
        {
          //Incoming connection has been closed and connection attempt failed -> return to idle
          BtSetup(I_AM_IN_IDLE);
          RtnVal = FALSE;
        }
      }
      else
      {
        // Mode does not allow connecting
        #ifdef DEBUG
          printf("Wrong Mode... = %d \r\n", (int)BtInstance.State);
        #endif
      }
    }
  }
  return(RtnVal);
}


/* This is the entry level to connecting */
UBYTE     cBtConnect(UBYTE *pDevName)
{
  UBYTE     RtnVal;
  UBYTE     Index;
  UBYTE     PortIndex;
  bdaddr_t  *pBdAddr;
  UBYTE     AllReadyConn;

  RtnVal       =  OK;
  AllReadyConn = FALSE;
  if (((HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy)) && (BLUETOOTH_OFF != BtInstance.State))
  {
    if (MODE1 == BtInstance.NonVol.DecodeMode)
    {
      //First check if already connected (can only be done if NOT using UI)
      if (TRUE == cBtFindDevName(&Index, pDevName, 0))
      {
        if (TRUE == BtInstance.NonVol.DevList[Index].Connected)
        {
          AllReadyConn              = TRUE;
          BtInstance.HciSocket.Busy = HCI_IDLE;
        }
      }

      if (FALSE == AllReadyConn)
      {
        // Find free port to use for connection
        PortIndex = BT_HOST_CH0;
        while((PortIndex < NO_OF_BT_CHS) && (CH_FREE != BtInstance.BtCh[PortIndex].Status))
        {
          PortIndex++;
        }

        if (NO_OF_BT_CHS > PortIndex)
        {
          // Set Busy flag to signal upwards
          BtInstance.HciSocket.Busy = HCI_CONNECT;
          BtInstance.OutGoing.ChNo  = PortIndex;

          #ifdef DEBUG
            printf("\r\n ..... c_bt_connect....Name = %s\r\n",pDevName);
          #endif

          // First look in the known list
          if (TRUE == cBtFindDevName(&Index, pDevName, 0))
          {
            #ifdef DEBUG
              printf("\r\nFound in dev table: index = %d, name = %s, Port no = %d\r\n", Index, pDevName, PortIndex);
            #endif

            pBdAddr = &(BtInstance.NonVol.DevList[Index].Adr);
          }
          else
          {
            // Then look in the search list
            if (TRUE == cBtFindSearchName(&Index, pDevName))
            {
              #ifdef DEBUG
                printf("\r\n ..... Found in search table index = %d.... name = %s\r\n", Index, pDevName);
              #endif

              pBdAddr = &(BtInstance.SearchList[Index].Adr);
            }
            else
            {
              RtnVal = FAIL;
              BtInstance.HciSocket.Busy = HCI_FAIL;
            }
          }

          if (0 == (HCI_FAIL & BtInstance.HciSocket.Busy))
          {
            // All above went OK -> Connect
            // Put name in incoming so it can be viewed in pin code pop up screen
            snprintf((char*)&(BtInstance.Incoming.Name[0]), vmBRICKNAMESIZE, "%s", (char*)pDevName);
            if (FALSE == Connect(*pBdAddr, PortIndex))
            {
              RtnVal = FAIL;
              BtInstance.HciSocket.Busy = HCI_FAIL;
            }
          }
        }
        else
        {
          // No more free ports
          RtnVal = FAIL;
          BtInstance.HciSocket.Busy = HCI_FAIL;
        }
      }
    }
    else
    {
      RtnVal = FAIL;
      BtInstance.HciSocket.Busy = HCI_FAIL;
    }
  }
  else
  {
    RtnVal = FAIL;
    BtInstance.HciSocket.Busy = HCI_FAIL;
  }
  return(RtnVal);
}


UBYTE     cBtDiscDevIndex(UBYTE Index)
{
  UBYTE   RtnVal;

  RtnVal = FALSE;
  if(TRUE == BtInstance.NonVol.DevList[Index].Connected)
  {
    BtInstance.HciSocket.Busy  =  HCI_DISCONNECT;
    BtCloseBtSocket(&(BtInstance.BtCh[BtInstance.NonVol.DevList[Index].ChNo].BtSocket.Socket));
    RtnVal = TRUE;
  }
  return(RtnVal);
}


UBYTE     cBtDiscChNo(UBYTE ChNo)
{
  UBYTE   RtnVal;
  UBYTE   TmpCnt;

  RtnVal = FALSE;
  for(TmpCnt = 0; TmpCnt < MAX_DEV_TABLE_ENTRIES; TmpCnt++)
  {
    if ((TRUE == BtInstance.NonVol.DevList[TmpCnt].Connected) && (ChNo == BtInstance.NonVol.DevList[TmpCnt].ChNo))
    {
      cBtDiscDevIndex(TmpCnt);
      TmpCnt = MAX_DEV_TABLE_ENTRIES;
      RtnVal = TRUE;
    }
  }
  return(RtnVal);
}


UBYTE     cBtDisconnect(UBYTE *pName)
{
  UBYTE   Index;
  UBYTE   RtnVal;
  UBYTE   TmpCnt;

  // Disconnect by name - find first index with the name which is connected
  if (((HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy)) && (BLUETOOTH_OFF != BtInstance.State))
  {
    for(TmpCnt = 0; TmpCnt < MAX_DEV_TABLE_ENTRIES; TmpCnt++)
    {
      BtInstance.HciSocket.Busy = HCI_IDLE;
      if (TRUE == cBtFindDevName(&Index, pName, TmpCnt))
      {
        if (cBtDiscDevIndex(Index))
        {
          TmpCnt = MAX_DEV_TABLE_ENTRIES;
        }
        else
        {
          // The device was found but not connected -
          // Keep looking for a connected device
          TmpCnt = Index;
        }
      }
      else
      {
        TmpCnt = MAX_DEV_TABLE_ENTRIES;
      }
    }

    #ifdef DEBUG
      printf("\r\nDisconnecting Index = %d, Socketno = %d, Name = %s",Index,BtInstance.BtCh[BtInstance.NonVol.DevList[Index].ChNo].BtSocket.Socket,pName);
    #endif

    RtnVal = OK;
  }
  else
  {
    BtInstance.HciSocket.Busy = HCI_FAIL;
    RtnVal = FAIL;
  }
  return(RtnVal);
}


void      BtTxMsgs(void)
{
  UBYTE     Cnt;
  UWORD     ByteCnt;
  SWORD     NoWritten;
  WRITEBUF  *pWriteBuf;
  BTSOCKET  *pBtSocket;
  int       SocketReturn;

  for (Cnt = 0; Cnt < NO_OF_BT_CHS; Cnt++)
  {
    pWriteBuf = &(BtInstance.BtCh[Cnt].WriteBuf);
    pBtSocket = &(BtInstance.BtCh[Cnt].BtSocket);

    ByteCnt   = pWriteBuf->InPtr - pWriteBuf->OutPtr;

    if(ByteCnt)
    {
      if (-1 != pBtSocket->Socket)
      {

        pBtSocket->Cmdtv.tv_sec    =  0;
        pBtSocket->Cmdtv.tv_usec   =  0;
        FD_ZERO(&(pBtSocket->Cmdfds));
        FD_SET(pBtSocket->Socket, &(pBtSocket->Cmdfds));

        SocketReturn = select(pBtSocket->Socket + 1, NULL, &(pBtSocket->Cmdfds), NULL, &(pBtSocket->Cmdtv));

        if (0 < SocketReturn)
        {
          // Still bytes to transmit on the requested channel
          NoWritten = send(pBtSocket->Socket, &(pWriteBuf->Buf[pWriteBuf->OutPtr]), ByteCnt, (int)0);

          if (0 < NoWritten)
          {
            pWriteBuf->OutPtr += NoWritten;

            #ifdef DEBUG
              printf("\r\n.... transmitted to socket = %d on chno = %d, Bytes to send %d, Bytes written = %d \r\n",pBtSocket->Socket, Cnt, ByteCnt, NoWritten);
              printf("\r\n errno = %d\r\n", errno);
            #endif

            if (pWriteBuf->OutPtr == pWriteBuf->InPtr)
            {
              // All bytes has been written - clear the buffer
              pWriteBuf->InPtr  = 0;
              pWriteBuf->OutPtr = 0;
            }
          }
        }
      }
      else
      {
        // Socket is closed (illegal handle) -> flush buffer
        pWriteBuf->InPtr  = 0;
        pWriteBuf->OutPtr = 0;
      }
    }
  }
}


UBYTE     cBtI2cBufReady(void)
{
  UBYTE   RtnVal;

  RtnVal = 1;
  if(0 != BtInstance.BtCh[0].WriteBuf.InPtr)
  {
    RtnVal = 0;
  }
  return(RtnVal);
}


UWORD     cBtI2cToBtBuf(UBYTE *pBuf, UWORD Size)
{
  if(0 == BtInstance.BtCh[0].WriteBuf.InPtr)
  {
    memcpy(BtInstance.BtCh[0].WriteBuf.Buf, pBuf, Size);
    BtInstance.BtCh[0].WriteBuf.InPtr = Size;
  }
  return(Size);
}


UWORD     cBtDevWriteBuf(UBYTE *pBuf, UWORD Size)
{
  if (MODE2 == BtInstance.NonVol.DecodeMode)
  {
    if(0 == BtInstance.Mode2WriteBuf.InPtr)
    {
      memcpy(BtInstance.Mode2WriteBuf.Buf, pBuf, Size);
      BtInstance.Mode2WriteBuf.InPtr = Size;
    }
    else
    {
      Size = 0;
    }
  }
  else
  {
    if(0 == BtInstance.BtCh[0].WriteBuf.InPtr)
    {
      memcpy(BtInstance.BtCh[0].WriteBuf.Buf, pBuf, Size);
      BtInstance.BtCh[0].WriteBuf.InPtr = Size;
    }
    else
    {
      Size = 0;
    }
  }
  return(Size);
}


UWORD     cBtDevWriteBuf1(UBYTE *pBuf, UWORD Size)
{
  if(0 == BtInstance.BtCh[1].WriteBuf.InPtr)
  {
    memcpy(BtInstance.BtCh[1].WriteBuf.Buf, pBuf, Size);
    BtInstance.BtCh[1].WriteBuf.InPtr = Size;
  }
  else
  {
    Size = 0;
  }
  return(Size);
}


UWORD     cBtDevWriteBuf2(UBYTE *pBuf, UWORD Size)
{
  if(0 == BtInstance.BtCh[2].WriteBuf.InPtr)
  {
    memcpy(BtInstance.BtCh[2].WriteBuf.Buf, pBuf, Size);
    BtInstance.BtCh[2].WriteBuf.InPtr = Size;
  }
  else
  {
    Size = 0;
  }
  return(Size);
}


UWORD     cBtDevWriteBuf3(UBYTE *pBuf, UWORD Size)
{
  if(0 == BtInstance.BtCh[3].WriteBuf.InPtr)
  {
    memcpy(BtInstance.BtCh[3].WriteBuf.Buf, pBuf, Size);
    BtInstance.BtCh[3].WriteBuf.InPtr = Size;
  }
  else
  {
    Size = 0;
  }
  return(Size);
}


UWORD     cBtDevWriteBuf4(UBYTE *pBuf, UWORD Size)
{
  if(0 == BtInstance.BtCh[4].WriteBuf.InPtr)
  {
    memcpy(BtInstance.BtCh[4].WriteBuf.Buf, pBuf, Size);
    BtInstance.BtCh[4].WriteBuf.InPtr = Size;
  }
  else
  {
    Size = 0;
  }
  return(Size);
}


UWORD     cBtDevWriteBuf5(UBYTE *pBuf, UWORD Size)
{
  if(0 == BtInstance.BtCh[5].WriteBuf.InPtr)
  {
    memcpy(BtInstance.BtCh[5].WriteBuf.Buf, pBuf, Size);
    BtInstance.BtCh[5].WriteBuf.InPtr = Size;
  }
  else
  {
    Size = 0;
  }
  return(Size);
}


UWORD     cBtDevWriteBuf6(UBYTE *pBuf, UWORD Size)
{
  if(0 == BtInstance.BtCh[6].WriteBuf.InPtr)
  {
    memcpy(BtInstance.BtCh[6].WriteBuf.Buf, pBuf, Size);
    BtInstance.BtCh[6].WriteBuf.InPtr = Size;
  }
  else
  {
    Size = 0;
  }
  return(Size);
}


UWORD     cBtDevWriteBuf7(UBYTE *pBuf, UWORD Size)
{
  if(0 == BtInstance.BtCh[7].WriteBuf.InPtr)
  {
    memcpy(BtInstance.BtCh[7].WriteBuf.Buf, pBuf, Size);
    BtInstance.BtCh[7].WriteBuf.InPtr = Size;
  }
  else
  {
    Size = 0;
  }
  return(Size);
}


UBYTE     cBtFindSearchAdr(bdaddr_t *pAdr, UBYTE *pIndex)
{
  UBYTE   Cnt;
  UBYTE   RtnVal;

  RtnVal = FAIL;
  for (Cnt = 0; ((Cnt < MAX_DEV_TABLE_ENTRIES) && (FAIL == RtnVal)); Cnt++)
  {
    if (0 == bacmp(pAdr, &(BtInstance.SearchList[Cnt].Adr)))
    {
      *pIndex = Cnt;
      RtnVal  = OK;
    }
  }
  return(RtnVal);
}


UBYTE     cBtFindDevAdr(bdaddr_t *pAdr, UBYTE *pIndex)
{
  UBYTE   Cnt;
  UBYTE   RtnVal;

  RtnVal = FAIL;
  for (Cnt = 0; ((Cnt < MAX_DEV_TABLE_ENTRIES) && (FAIL == RtnVal)); Cnt++)
  {
    if ((DEV_EMPTY != BtInstance.NonVol.DevList[Cnt].Status) &&
        (0 == bacmp(pAdr, &(BtInstance.NonVol.DevList[Cnt].Adr))))
    {
      *pIndex = Cnt;
      RtnVal  = OK;
    }
  }
  return(RtnVal);
}


UBYTE     cBtFindDevConnHandle(UBYTE ConnHandle, UBYTE *pIndex)
{
  UBYTE   Cnt;
  UBYTE   RtnVal;

  RtnVal = FAIL;
  for (Cnt = 0; ((Cnt < MAX_DEV_TABLE_ENTRIES) && (FAIL == RtnVal)); Cnt++)
  {
    if ((DEV_EMPTY != BtInstance.NonVol.DevList[Cnt].Status) &&
        (ConnHandle == BtInstance.NonVol.DevList[Cnt].ConnHandle) &&
        (TRUE == BtInstance.NonVol.DevList[Cnt].Connected))
    {
      *pIndex = Cnt;
      RtnVal  = OK;
    }
  }
  return(RtnVal);
}


UBYTE     cBtFindDevChNo(UBYTE ChNo, UBYTE *pIndex)
{
  UBYTE   Cnt;
  UBYTE   RtnVal;

  RtnVal = FAIL;
  for (Cnt = 0; ((Cnt < MAX_DEV_TABLE_ENTRIES) && (FAIL == RtnVal)); Cnt++)
  {
    if ((DEV_EMPTY != BtInstance.NonVol.DevList[Cnt].Status)    &&
        (ChNo      == BtInstance.NonVol.DevList[Cnt].ChNo)      &&
        (CH_FREE   != BtInstance.BtCh[ChNo].Status))
    {
      *pIndex = Cnt;
      RtnVal  = OK;
    }
  }
  return(RtnVal);
}


UBYTE     cBtInsertInDeviceList(bdaddr_t *pBtAdr, UBYTE *pIndex)
{
  UBYTE   Cnt;
  UBYTE   Exit;
  UBYTE   RtnVal;

  RtnVal = OK;

  //Check if already present
  Exit = FALSE;
  for (Cnt = 0; ((Cnt < MAX_DEV_TABLE_ENTRIES) && (FALSE == Exit)); Cnt++)
  {
    if (DEV_EMPTY != BtInstance.NonVol.DevList[Cnt].Status)
    {
      if (0 == bacmp(&(BtInstance.NonVol.DevList[Cnt].Adr), pBtAdr))
      {
        *pIndex = Cnt;
        Exit    = TRUE;
      }
    }
  }

  if (FALSE == Exit)
  {
    // Device not already present -> Create one
    for (Cnt = 0; ((Cnt < MAX_DEV_TABLE_ENTRIES) && (FALSE == Exit)); Cnt++)
    {
      if (DEV_EMPTY == BtInstance.NonVol.DevList[Cnt].Status)
      {
        BtInstance.NonVol.DevListEntries++;
        BtInstance.NonVol.DevList[Cnt].Name[0]    =  0;
        BtInstance.NonVol.DevList[Cnt].Status     =  DEV_KNOWN;
        bacpy(&(BtInstance.NonVol.DevList[Cnt].Adr), pBtAdr);
        *pIndex = Cnt;
        Exit    = TRUE;
      }
    }
  }

  if (FALSE == Exit)
  {
    RtnVal = FAIL;
  }
  return(RtnVal);
}


void      cBtSetDevConnectedStatus(UBYTE Index)
{
  BtInstance.NonVol.DevList[Index].Connected  = TRUE;
  BtInstance.NoOfConnDevs++;
}


void      cBtCloseDevConnection(UBYTE Index)
{
  BtInstance.NonVol.DevList[Index].Connected  = FALSE;

  // BtInstance.NoOfConnDevs is updated in BtCloseCh function
  BtCloseCh(BtInstance.NonVol.DevList[Index].ChNo);
  BtInstance.NonVol.DevList[Index].ChNo  =  NO_OF_BT_CHS;
}


void      cBtSetSearchConnectedStatus(UBYTE Index)
{
  BtInstance.SearchList[Index].Connected  = TRUE;
  BtInstance.SearchList[Index].Parred     = TRUE;
}


void      cBtClearSearchConnectedStatus(UBYTE Index)
{
  BtInstance.SearchList[Index].Connected  = FALSE;
}


void      cBtInsertDevConnHandle(UBYTE Index, UWORD ConnHandle)
{
  BtInstance.NonVol.DevList[Index].ConnHandle =  ConnHandle;
}


/*! \page ComModule
 *
 *  <hr size="1"/>
 *  <b>     write </b>
 */
/*! \brief    cBtHandleHCI
 *
 *
 *  CONNECTION MANAGEMENT
 *  ---------------------
 *
 *
 *    Scenarious :
 *    ------------
 *
 *    CONNECTING:
 *
 *
 *          Connecting brick:                                  Remote brick:
 *          (IDLE state)                                       (SLAVE state)
 *
 *
 *
 *    Connect to brick (Issued from byte codes)     |
 *    - Set busy flag                               |
 *    - Disable page inquiry                        |
 *    - Open socket                           --->  |  --->    EVT_CONN_REQUEST
 *                                                  |          - Issue remote name request (might not be known)
 *                                                  |
 *                                                  |
 *    EVT_CONN_COMPLETE                       <---  |  --->    EVT_CONN_COMPLETE
 *    - Update Device list                          |          - Disable page inquiry
 *    - Update Search list                          |          - Insert all info in dev list (Except connected)
 *                                                  |
 *                                                  |
 *    Socket write ready (Remote socket open) <---  |  --->    Success on accept listen socket (Socket gives remote address)
 *    - NoOfConnDevices++                           |          - Set slave mode
 *    - Update Search list to connected             |          - NoOfConnDevices++
 *    - Update Device list to connected             |          - Update Device list to connected
 *                                                  |          - Update Search list to connected
 *                                                  |
 *                                                  |
 *                                                  |
 *
 *
 *    DISCONNECTING:
 *
 *
 *    Disconnecting brick:                          |          Remote brick:
 *                                                  |
 *    Disconnect  (Issued from byte codes)          |
 *    - Close bluetooth socket                      |  --->    Socket indicates remote socket closed
 *                                                  |          - Close socket
 *                                                  |
 *                                                  |
 *    EVT_DISCONN_COMPLETE                    <---  |  --->    EVT_DISCONN_COMPLETE
 *    - Update Search list to disconnected          |          - Update Search list to disconnected
 *    - Update Device list to disconnected          |          - Update Device list to disconnected
 *    - NoofConnDevices--                           |          - NoofConnDevices--
 *    - If NoofConnDevices = 0 -> set idle mode     |          - If NoofConnDevices = 0 -> set idle mode
 *                                                  |
 *                                                  |
 *
 */

UWORD     cBtHandleHCI(void)
{
  UWORD           RtnVal;
  int             len;
  int             results;
  unsigned char   buf[HCI_MAX_EVENT_SIZE];
  unsigned char   *ptr;
  hci_event_hdr   *hdr;

  RtnVal = TRUE;

  // Poll the HCI socket for pin key request
  BtInstance.HciSocket.p.revents = 0;

  if (poll(&(BtInstance.HciSocket.p), 1, 0) > 0)
  {
    len = read(BtInstance.HciSocket.Socket, buf, sizeof(buf));

    if (len > 0)
    {

      hdr = (void *) (buf + 1);
      ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
      results = ptr[0];

      switch (hdr->evt)
      {

        case EVT_PIN_CODE_REQ:
        {

          if (0 == bacmp(&(BtInstance.TrustedDev.Adr),  &((evt_pin_code_req*)ptr)->bdaddr))
          {
            // If pincode request is from a trusted dev. then signal to the DBUS handle
            // that it is trusted
            BtInstance.TrustedDev.Status = TRUE;
            bacpy(&(BtInstance.TrustedDev.Adr), BDADDR_ANY); //Address can only be used once
          }
        }
        break;

        case EVT_CONN_REQUEST:
        {
          if (I_AM_IN_IDLE == BtInstance.State)
          {
            // Connection from the outside - coming from the listen socket
            // Need to add it to the dev list and search list if possible
            remote_name_req_cp  cp;

            #ifdef DEBUG
              printf("EVT_CONN_REQUEST from outside this is slave..... \r\n");
            #endif

            // Save information regarding incoming connection
            bacpy(&(BtInstance.Incoming.Adr), &((evt_conn_request*)ptr)->bdaddr);
            memcpy(&(BtInstance.Incoming.DevClass[0]), &(((evt_conn_request*)ptr)->dev_class[0]), 3);
            BtInstance.Incoming.Name[0] = 0; //Clear name, it is not available yet

            // Issue name request because it is coming from the outside
            memset(&cp, 0, sizeof(cp));
            bacpy(&cp.bdaddr, &((evt_conn_request*)ptr)->bdaddr);
            cp.pscan_rep_mode = 0x02;
            hci_send_cmd(BtInstance.HciSocket.Socket, OGF_LINK_CTL, OCF_REMOTE_NAME_REQ, REMOTE_NAME_REQ_CP_SIZE, &cp);
          }
        }
        break;

        case EVT_CONN_COMPLETE:
        {
          UBYTE DevIndex;
          UBYTE SearchIndex;

          if (0 != ((evt_conn_complete*)ptr)->status)
          {
            if (I_AM_MASTER == BtInstance.State)
            {
              if (OK == cBtFindDevAdr(&((evt_conn_complete*)ptr)->bdaddr, &DevIndex))
              {
                //Need to insert the correct channel number to close it
                BtInstance.NonVol.DevList[DevIndex].ChNo = BtInstance.OutGoing.ChNo;
                cBtCloseDevConnection(DevIndex);
              }
            }

            #ifdef DEBUG
              printf("EVT_CONN_COMPLETE - FAIL !!!!! Status = %d \r\n",((evt_conn_complete*)ptr)->status);
            #endif
            BtInstance.HciSocket.Busy &= ~HCI_CONNECT;
          }
          else
          {
            // Now connected to one device -> Disable incoming connect requests
            // This command repeated for each connected device in master mode
            BtInstance.PageState = 0;
            BtIssueHciVisible(BtInstance.NonVol.Visible, BtInstance.PageState);
            BtInstance.HciSocket.Busy  |=  HCI_VISIBLE;

            if (FAIL != cBtFindDevAdr(&((evt_conn_complete*)ptr)->bdaddr, &DevIndex))
            {
              cBtInsertDevConnHandle(DevIndex, ((evt_conn_complete*)ptr)->handle);

              if (I_AM_MASTER == BtInstance.State)
              {
                BtInstance.NonVol.DevList[DevIndex].ChNo = BtInstance.OutGoing.ChNo;

                if (OK == cBtFindSearchAdr(&((evt_conn_complete*)ptr)->bdaddr, &SearchIndex))
                {
                  // Insert COD from search list
                  memcpy(&(BtInstance.NonVol.DevList[DevIndex].DevClass[0]),&(BtInstance.SearchList[SearchIndex].DevClass[0]),3);
                }
              }
              else
              {
                // Connections from the outside (I am a slave) always uses ch 0
                BtInstance.NonVol.DevList[DevIndex].ChNo = 0;
                memcpy(&(BtInstance.NonVol.DevList[DevIndex].DevClass[0]),&(BtInstance.Incoming.DevClass[0]),3);
              }
            }

            BtInstance.Incoming.ConnHandle = ((evt_conn_complete*)ptr)->handle;
          }
        }
        break;

        case EVT_INQUIRY_RESULT_WITH_RSSI:
        {
          int                     i;
          inquiry_info_with_rssi  *info_rssi;
          UBYTE                   DevStatus;
          UBYTE                   Tmp;

          for (i = 0; i < results; i++)
          {
            info_rssi = (void *)ptr + (sizeof(*info_rssi) * i) + 1;

            bacpy(&(BtInstance.SearchList[BtInstance.SearchIndex].Adr), &(info_rssi->bdaddr));
            memcpy(&(BtInstance.SearchList[BtInstance.SearchIndex].DevClass[0]), &(info_rssi->dev_class[0]), 3);

            DevStatus = FALSE;
            for (Tmp = 0; ((Tmp < MAX_DEV_TABLE_ENTRIES) && (DevStatus == FALSE)); Tmp++)
            {

              if (DEV_EMPTY != BtInstance.NonVol.DevList[Tmp].Status)
              {

                if (0 == bacmp(&(BtInstance.NonVol.DevList[Tmp].Adr), &(info_rssi->bdaddr)))
                {
                  // The device was in the dev list -> it is paired -> Check for connected...
                  BtInstance.SearchList[BtInstance.SearchIndex].Parred    = TRUE;
                  BtInstance.SearchList[BtInstance.SearchIndex].Connected = BtInstance.NonVol.DevList[Tmp].Connected;

                  // Update Class of Device
                  memcpy(&(BtInstance.NonVol.DevList[Tmp].DevClass[0]), &(info_rssi->dev_class[0]), 3);

                  DevStatus  =  TRUE;
                }
              }
            }

            if (FALSE == DevStatus)
            {
              BtInstance.SearchList[BtInstance.SearchIndex].Parred    = FALSE;
              BtInstance.SearchList[BtInstance.SearchIndex].Connected = FALSE;
            }

            (BtInstance.SearchIndex)++;
          }
        }
        break;

        case EVT_INQUIRY_COMPLETE:
        {
          BtInstance.NoOfFoundDev  =  BtInstance.SearchIndex;
          BtInstance.SearchIndex   =  0;

          if (0 == BtInstance.NoOfFoundDev)
          {
            if (SCAN_INQ_STATE == BtInstance.ScanState)
            {

              // No devices found
              #ifdef DEBUG
                printf("inquiry done.... exiting \r\n");
              #endif

              BtSetup(BtInstance.OldState);               // Return to previous state
              BtInstance.HciSocket.Busy   &= ~HCI_SCAN;
              BtInstance.ScanState         =  SCAN_OFF;
              BtIssueHciVisible(BtInstance.NonVol.Visible, BtInstance.PageState);
            }
          }
          else
          {
            if (SCAN_INQ_STATE == BtInstance.ScanState)
            {
              BtInstance.ScanState = SCAN_NAME_STATE;
              BtRequestName();
            }
            else
            {

              // Scan has been terminated
              BtSetup(BtInstance.OldState);               // Return to previous state
              BtInstance.ScanState        =   SCAN_OFF;
              BtInstance.HciSocket.Busy  &=  ~HCI_SCAN;
              BtIssueHciVisible(BtInstance.NonVol.Visible, BtInstance.PageState);
            }
          }
        }
        break;

        case EVT_REMOTE_NAME_REQ_COMPLETE:
        {
          // Remote name complete will occur during connect!!
          evt_remote_name_req_complete  *rn;
          UBYTE                         Cnt;
          UBYTE                         Exit;

          #ifdef DEBUG
            printf("EVT_REMOTE_NAME_REQ_COMPLETE\r\n");
            char    TmpAddr[16];
          #endif

          rn = (void *)ptr;
          if (I_AM_SCANNING == BtInstance.State)
          {
            if(rn->status)
            {
              strcpy(BtInstance.SearchList[BtInstance.SearchIndex].Name, "????");
            }
            else
            {
              strcpy(BtInstance.SearchList[BtInstance.SearchIndex].Name, (char*)rn->name);

              // Update favourite list with the name received
              for (Cnt = 0, Exit = FALSE; ((Cnt < MAX_DEV_TABLE_ENTRIES) && (FALSE == Exit)); Cnt++)
              {
                if (DEV_EMPTY != BtInstance.NonVol.DevList[Cnt].Status)
                {
                  if (0 == bacmp(&(BtInstance.NonVol.DevList[Cnt].Adr), (bdaddr_t*)&(rn->bdaddr)))
                  {

                    #ifdef DEBUG
                      printf("\r\nUpdated name in favourite list \r\n");
                    #endif

                    strcpy(BtInstance.NonVol.DevList[Cnt].Name, (char*)rn->name);

                    Exit  =  TRUE;
                  }
                }
              }
            }

            #ifdef DEBUG
              ba2str((const bdaddr_t *)(&BtInstance.SearchList[BtInstance.SearchIndex].Adr), TmpAddr);
              printf("Dev Table[%d]: Addr = %s, Name = %s \r\n", BtInstance.SearchIndex, TmpAddr, BtInstance.SearchList[BtInstance.SearchIndex].Name);
            #endif

            if (BtInstance.SearchIndex >= (BtInstance.NoOfFoundDev - 1))
            {
              // All devices found
              #ifdef DEBUG
                printf("inquiry done.... exiting \r\n");
              #endif

              BtSetup(BtInstance.OldState);     // Return to previous state
              BtInstance.ScanState        =   SCAN_OFF;
              BtInstance.HciSocket.Busy  &=  ~HCI_SCAN;
              BtIssueHciVisible(BtInstance.NonVol.Visible, BtInstance.PageState);
            }
            else
            {

              if (SCAN_NAME_STATE == BtInstance.ScanState)
              {
                BtInstance.SearchIndex++;
                BtRequestName();
              }
              else
              {
                BtSetup(BtInstance.OldState);     // Return to previous state
                BtInstance.ScanState        =   SCAN_OFF;
                BtInstance.HciSocket.Busy  &=  ~HCI_SCAN;
                BtIssueHciVisible(BtInstance.NonVol.Visible, BtInstance.PageState);
              }
            }
            BtInstance.NoOfFoundNames++;
          }
          else
          {
            // In all other cases that scanning -> try to update the favourite list with the name
            if(!(rn->status))
            {
              for (Cnt = 0, Exit = FALSE; ((Cnt < MAX_DEV_TABLE_ENTRIES) && (FALSE == Exit)); Cnt++)
              {
                if (DEV_EMPTY != BtInstance.NonVol.DevList[Cnt].Status)
                {
                  if (0 == bacmp(&(BtInstance.NonVol.DevList[Cnt].Adr), (bdaddr_t*)&(rn->bdaddr)))
                  {

                    #ifdef DEBUG
                      printf("\r\nUpdated name in favourite list \r\n");
                    #endif
                    strcpy(BtInstance.NonVol.DevList[Cnt].Name, (char*)rn->name);

                    Exit  =  TRUE;
                  }
                }
              }

              // also update incoming data if it matches
              if (0 == bacmp(&(BtInstance.Incoming.Adr), (bdaddr_t*)&(rn->bdaddr)))
              {

                #ifdef DEBUG
                  printf("\r\nUpdated name in favourite list \r\n");
                #endif
                strcpy(BtInstance.Incoming.Name, (char*)rn->name);
              }
            }
          }
        }
        break;

        case EVT_DISCONN_COMPLETE:
        {
          UWORD   ConnHandle;
          UBYTE   Index;

          if (0 == ((evt_disconn_complete*)ptr)->status)
          {
            ConnHandle = ((evt_disconn_complete*)ptr)->handle;

            if (OK == cBtFindDevConnHandle(ConnHandle, &Index))
            {
              cBtCloseDevConnection(Index);
              if (OK == cBtFindSearchAdr(&(BtInstance.NonVol.DevList[Index].Adr), &Index))
              {
                cBtClearSearchConnectedStatus(Index);
              }
            }
            else
            {
              if (0 == BtInstance.NoOfConnDevs)
              {
                // Ensure going back to idle if only pairing (no application running on remote device)
                // Socket is not going to be established.
                BtInstance.PageState = SCAN_PAGE;
                BtIssueHciVisible(BtInstance.NonVol.Visible, BtInstance.PageState);
                BtInstance.HciSocket.Busy  |=  HCI_VISIBLE;
              }
            }
            BtInstance.HciSocket.Busy  &= ~HCI_DISCONNECT;
          }
        }
        break;

        case EVT_LINK_KEY_NOTIFY:
        {
          UBYTE DevIndex;
          UBYTE SearchIndex;

          if ((0 == ((evt_link_key_notify*)ptr)->key_type) || (5 == ((evt_link_key_notify*)ptr)->key_type))
          {
            //Only authenticated link keys will result in device table update
            if (OK == cBtInsertInDeviceList(&((evt_link_key_notify*)ptr)->bdaddr, &DevIndex))
            {
              if (I_AM_MASTER == BtInstance.State)
              {
                #ifdef DEBUG
                  printf("\r\nEVT_CONN_COMPLETE in master mode: DevIndex = %d, ChNo = %d\r\n",DevIndex, BtInstance.OutGoing.ChNo);
                #endif

                BtInstance.NonVol.DevList[DevIndex].ChNo = BtInstance.OutGoing.ChNo;

                if (OK == cBtFindSearchAdr(&((evt_link_key_notify*)ptr)->bdaddr, &SearchIndex))
                {
                  // Insert COD from search list
                  memcpy(&(BtInstance.NonVol.DevList[DevIndex].DevClass[0]),&(BtInstance.SearchList[SearchIndex].DevClass[0]),3);
                }

              }
              else
              {
                // This is I_AM_IN_IDLE
                // Connections from the outside (I am a slave) always uses ch 0
                BtInstance.NonVol.DevList[DevIndex].ChNo = 0;
                memcpy(&(BtInstance.NonVol.DevList[DevIndex].DevClass[0]),&(BtInstance.Incoming.DevClass[0]),3);
                strcpy(&(BtInstance.NonVol.DevList[DevIndex].Name[0]),&(BtInstance.Incoming.Name[0]));
                BtInstance.NonVol.DevList[DevIndex].ConnHandle = BtInstance.Incoming.ConnHandle;
              }
            }
          }
        }
        break;

        case EVT_CMD_COMPLETE:
        {
          switch(((evt_cmd_complete*)ptr)->opcode)
          {
            case cmd_opcode_pack(OGF_HOST_CTL, OCF_WRITE_SCAN_ENABLE):
            {
              // This is the complete event for visible setting
              BtInstance.HciSocket.WaitForEvent  =  FALSE;
              BtInstance.HciSocket.Busy         &= ~HCI_VISIBLE;
            }
            break;

            case cmd_opcode_pack(OGF_HOST_CTL, OCF_WRITE_CLASS_OF_DEV):
            {
              // This is the complete event for changing class of device
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OGF_HOST_CTL, OCF_DELETE_STORED_LINK_KEY):
            {
              // This is the complete event for deleting link keys
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OGF_LINK_CTL, OCF_INQUIRY_CANCEL):
            {
              // Inquiry has been cancelled
              BtSetup(BtInstance.OldState);     // Return to previous state
              BtInstance.HciSocket.Busy  &=  ~HCI_SCAN;
              BtInstance.ScanState        =   SCAN_OFF;
            }
            break;

            case cmd_opcode_pack(OGF_HOST_CTL, OCF_CHANGE_LOCAL_NAME):
            {
              BtInstance.HciSocket.WaitForEvent  =  FALSE;
              BtInstance.HciSocket.Busy         &= ~HCI_NAME;
            }
            break;

            case cmd_opcode_pack(OGF_HOST_CTL, OCF_WRITE_AUTH_ENABLE):
            {
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OGF_INFO_PARAM, OCF_READ_BD_ADDR):
            {
              // Copy own bluetooth address in place.
              // Removed - address now set in init
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OCF_WRITE_SIMPLE_PAIRING_MODE, WRITE_SIMPLE_PAIRING_MODE_CP_SIZE):
            {
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OGF_HOST_CTL, OCF_READ_SIMPLE_PAIRING_MODE):
            {
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OGF_HOST_CTL, OCF_RESET):
            {
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OGF_HOST_CTL, OCF_SET_EVENT_MASK):
            {
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OGF_HOST_CTL, OCF_WRITE_LE_HOST_SUPPORTED):
            {
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OGF_HOST_CTL, OCF_READ_EXT_INQUIRY_RESPONSE):
            {
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OGF_HOST_CTL, OCF_WRITE_EXT_INQUIRY_RESPONSE):
            {
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            case cmd_opcode_pack(OGF_VENDOR_CMD, 0xFF26):
            {
              BtInstance.HciSocket.WaitForEvent = FALSE;
            }
            break;

            default:
            {
              // Looking for other cmd complete events
            }
          }
        }
        break;

        case EVT_ENCRYPT_CHANGE:
        {
        }
        break;

        default:
        {
          // If we must look for other events....
        }
        break;
      }
    }
  }
  return(RtnVal);
}


UBYTE     cBtGetHciBusyFlag(void)
{
  UBYTE   RtnVal;

  if (HCI_IDLE == BtInstance.HciSocket.Busy)
  {
    RtnVal = OK;
  }
  else
  {
    if (HCI_FAIL & BtInstance.HciSocket.Busy)
    {
      RtnVal = FAIL;
    }
    else
    {
      RtnVal = BUSY;
    }
  }
  return(RtnVal);
}


UBYTE     cBtGetBtType(UBYTE *pCod)
{
  UBYTE   Type;

  Type = BTTYPE_UNKNOWN;
  if ((0x08 == pCod[1]) && (0x04 == pCod[0]))
  {
    Type = BTTYPE_BRICK;
  }
  else
  {
    if (0x01 == pCod[1])
    {
      Type = BTTYPE_PC;
    }
    else
    {
      if (0x02 == pCod[1])
      {
        Type = BTTYPE_PHONE;
      }
    }
  }
  return(Type);
}


UBYTE     cBtGetNoOfConnListEntries(void)
{
  return(BtInstance.NoOfConnDevs);
}


UBYTE     cBtGetConnListEntry(UBYTE Item, UBYTE *pName, SBYTE Length, UBYTE *pType)
{
  UBYTE   ConnCnt = 0;
  UBYTE   TmpCnt;
  UBYTE   RtnVal;

  RtnVal = TRUE;
  for(TmpCnt = 0; TmpCnt < MAX_DEV_TABLE_ENTRIES; TmpCnt++)
  {
    if (TRUE == BtInstance.NonVol.DevList[TmpCnt].Connected)
    {
      if (ConnCnt == Item)
      {
        snprintf((char*)pName, Length, "%s", (char *)&(BtInstance.NonVol.DevList[TmpCnt].Name[0]));

        *pType = cBtGetBtType(&(BtInstance.NonVol.DevList[TmpCnt].DevClass[0]));
        TmpCnt = MAX_DEV_TABLE_ENTRIES;
      }
      else
      {
        ConnCnt++;
      }
    }
  }
  return(RtnVal);
}


UBYTE     cBtGetNoOfDevListEntries(void)
{
  return(BtInstance.NonVol.DevListEntries);
}


UBYTE     cBtGetDevListEntry(UBYTE Item, SBYTE *pConnected, SBYTE *pType, UBYTE *pName, SBYTE Length)
{
  UBYTE   RtnVal;

  snprintf((char*)pName,Length, "%s", (char *)&(BtInstance.NonVol.DevList[Item].Name[0]));
  *pConnected  =  BtInstance.NonVol.DevList[Item].Connected;
  *pType       =  cBtGetBtType(&(BtInstance.NonVol.DevList[Item].DevClass[0]));

  RtnVal = TRUE;
  return(RtnVal);
}


UBYTE     cBtGetNoOfSearchListEntries(void)
{
  return(BtInstance.NoOfFoundNames);
}


UBYTE     cBtGetSearchListEntry(UBYTE Item, SBYTE *pConnected, SBYTE *pType, SBYTE *pParred, UBYTE *pName, SBYTE Length)
{
  UBYTE   RtnVal;

  snprintf((char*)pName,Length, "%s", (char *)&(BtInstance.SearchList[Item].Name[0]));
  *pConnected  =  BtInstance.SearchList[Item].Connected;
  *pParred     =  BtInstance.SearchList[Item].Parred;
  *pType       =  cBtGetBtType(&(BtInstance.SearchList[Item].DevClass[0]));

  RtnVal = TRUE;
  return(RtnVal);
}


UBYTE     cBtDeleteFavourItem(UBYTE *pName)
{
  UBYTE   RtnVal;
  UBYTE   Cnt, Item;
  char    Addr[20];

  RtnVal = FAIL;
  if (((HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy)) && (BLUETOOTH_OFF != BtInstance.State))
  {
    // Check device list
    if (TRUE == cBtFindDevName(&Item, pName, 0))
    {
      if (DEV_EMPTY != BtInstance.NonVol.DevList[Item].Status)
      {
        //If item is connected then disconnect it
        if(TRUE == BtInstance.NonVol.DevList[Item].Connected)
        {
          BtInstance.NonVol.DevList[Item].Connected   =  FALSE;
          BtCloseCh(BtInstance.NonVol.DevList[Item].ChNo);
        }

        ba2str((bdaddr_t *)(&(BtInstance.NonVol.DevList[Item].Adr.b[0])), Addr);
        RemoveDevice(conn, Addr);

        for(Cnt = Item; Cnt < BtInstance.NonVol.DevListEntries; Cnt++)
        {
          memcpy(&(BtInstance.NonVol.DevList[Cnt].Name[0]), &(BtInstance.NonVol.DevList[Cnt + 1].Name[0]), sizeof(DEVICELIST));
        }
        if (BtInstance.NonVol.DevListEntries)
        {
          BtInstance.NonVol.DevListEntries--;
        }
        BtInstance.NonVol.DevList[Cnt].Status     =  DEV_EMPTY;
        BtInstance.NonVol.DevList[Cnt].Name[0]    =  0;
        BtInstance.NonVol.DevList[Cnt].Connected  =  FALSE;
        BtInstance.HciSocket.Busy                 =  HCI_IDLE;
        RtnVal = TRUE;
      }
      else
      {
        // Device list empty
        BtInstance.HciSocket.Busy =  HCI_FAIL;
      }
      RtnVal = OK;
    }

    // Check search list
    if (TRUE == cBtFindSearchName(&Item, pName))
    {
      for(Cnt = Item; Cnt < BtInstance.NoOfFoundNames; Cnt++)
      {
        memcpy(&(BtInstance.SearchList[Cnt].Name[0]), &(BtInstance.SearchList[Cnt + 1].Name[0]), sizeof(SEARCHLIST));
      }
      if (BtInstance.NoOfFoundNames)
      {
        BtInstance.NoOfFoundNames--;
      }
      BtClearSearchListEntry(Cnt);
      RtnVal = OK;
    }
  }
  return(RtnVal);
}


UBYTE     cBtGetStatus(void)
{
  UBYTE   RtnVal;

  RtnVal = 0;
  if (BtInstance.NonVol.On)
  {
    RtnVal = 0x01;

    if (BtInstance.NonVol.Visible)
    {
      RtnVal |= 0x02;
    }

    if (0 < BtInstance.NoOfConnDevs)
    {
      RtnVal |= 0x04;
    }
  }

  return(RtnVal);
}


void      cBtGetId(UBYTE *pId, UBYTE Length)
{
  strncpy((char*)pId, &(BtInstance.Adr[0]), Length);
}


UBYTE     cBtSetName(UBYTE *pName, UBYTE Length)
{
  UBYTE   RtnVal = OK;
  change_local_name_cp  cp;

  if (BLUETOOTH_OFF != BtInstance.State)
  {
    if ((HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy))
    {
      snprintf((char *)&(BtInstance.BtName[0]), Length, "%s", (char *)pName);

      snprintf((char *)&(cp.name[0]), Length, "%s", (char *)pName);
      hci_send_cmd(BtInstance.HciSocket.Socket, OGF_HOST_CTL, OCF_CHANGE_LOCAL_NAME, CHANGE_LOCAL_NAME_CP_SIZE, &cp);
      BtInstance.HciSocket.Busy = HCI_NAME;
    }
    else
    {
      RtnVal = FAIL;
    }
  }
  else
  {
    BtInstance.HciSocket.Busy = HCI_IDLE;
    snprintf((char *)&(BtInstance.BtName[0]), Length, "%s", (char *)pName);
  }
  return(RtnVal);
}


UBYTE     cBtGetChNo(UBYTE *pName, UBYTE *pChNos)
{
  UBYTE   RtnVal = 0;
  UBYTE   Index;
  UBYTE   Cnt;

  pChNos[RtnVal] = 0;
  for(Cnt = 0; Cnt < MAX_DEV_TABLE_ENTRIES; Cnt++)
  {
    if (cBtFindDevName(&Index, pName, Cnt))
    {
      if (TRUE == BtInstance.NonVol.DevList[Index].Connected)
      {
        pChNos[RtnVal] = BtInstance.NonVol.DevList[Index].ChNo;
        RtnVal++;
        pChNos[RtnVal] = 0;
      }
      Cnt = Index;
    }
    else
    {
      Cnt = MAX_DEV_TABLE_ENTRIES;
    }
  }
  return(RtnVal);
}


static char *get_adapter_path(DBusConnection *conn, const char *adapter)
{
  DBusMessage *msg, *reply;
  DBusError err;
  const char *reply_path;
  char *path;

  if (!adapter)
    return get_default_adapter_path(conn);

  msg = dbus_message_new_method_call("org.bluez", "/",
          "org.bluez.Manager", "FindAdapter");

  if (!msg)
  {
    fprintf(stderr, "Can't allocate new method call\n");
    return NULL;
  }

  dbus_message_append_args(msg, DBUS_TYPE_STRING, &adapter,
          DBUS_TYPE_INVALID);

  dbus_error_init(&err);

  // Next dbus message is blocking
  reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

  dbus_message_unref(msg);

  if (!reply)
  {
    fprintf(stderr, "Can't find adapter %s\n", adapter);
    if (dbus_error_is_set(&err))
    {
      fprintf(stderr, "%s\n", err.message);
      dbus_error_free(&err);
    }
    return NULL;
  }

  if (!dbus_message_get_args(reply, &err, DBUS_TYPE_OBJECT_PATH, &reply_path, DBUS_TYPE_INVALID))
  {
    fprintf(stderr, "Can't get reply arguments\n");
    if (dbus_error_is_set(&err))
    {
      fprintf(stderr, "%s\n", err.message);
      dbus_error_free(&err);
    }
    return NULL;
  }

  path = strdup(reply_path);

  dbus_message_unref(reply);

  dbus_connection_flush(conn);

  return path;
}


static DBusHandlerResult request_confirmation_message(DBusConnection *L_conn, DBusMessage *msg, void *data)
{
  const char   *path;

  conn = L_conn;

  if (!dbus_message_get_args(msg, NULL, DBUS_TYPE_OBJECT_PATH, &path,
       DBUS_TYPE_UINT32, &(BtInstance.Incoming.Passkey), DBUS_TYPE_INVALID))
  {
    fprintf(stderr, "Invalid arguments for RequestPasskey method");
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  // Create both positive and negative reply - user decides later what to use
  RejectReply = dbus_message_new_error(msg, "org.bluez.Error.Rejected", "");
  reply       = dbus_message_new_method_return(msg);
  if (!reply)
  {
    fprintf(stderr, "Can't create reply message\n");
    return DBUS_HANDLER_RESULT_NEED_MEMORY;
  }

  BtInstance.Events  =  0x02;

  return DBUS_HANDLER_RESULT_HANDLED;
}


static DBusHandlerResult agent_message(DBusConnection *L_conn, DBusMessage *msg, void *data)
{
  if (dbus_message_is_method_call(msg, "org.bluez.Agent", "RequestPinCode"))
  {
    BtInstance.SspPairingMethod = FALSE;
    return request_pincode_message(L_conn, msg, data);
  }

  if (dbus_message_is_method_call(msg, "org.bluez.Agent", "RequestPasskey"))
  {
   // return request_passkey_message(conn, msg, data);
  }

  if (dbus_message_is_method_call(msg, "org.bluez.Agent", "RequestConfirmation"))
  {
    BtInstance.SspPairingMethod = TRUE;
    return request_confirmation_message(L_conn, msg, data);
  }

  if (dbus_message_is_method_call(msg, "org.bluez.Agent", "Authorize"))
  {
  // return authorize_message(conn, msg, data);
  }

  if (dbus_message_is_method_call(msg, "org.bluez.Agent", "Cancel"))
  {
//    return cancel_message(conn, msg, data);
  }

  if (dbus_message_is_method_call(msg, "org.bluez.Agent", "Release"))
  {
//    return release_message(conn, msg, data);
  }

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


static int register_agent(DBusConnection *conn, const char *adapter_path, const char *agent_path, const char *capabilities)
{
  DBusMessage *msg, *reply;
  DBusError    err;
  int          RtnVal;

  RtnVal = 0;

  msg = dbus_message_new_method_call("org.bluez", adapter_path, "org.bluez.Adapter", "RegisterAgent");
  if (!msg)
  {
    #ifdef DEBUG
      fprintf(stderr, "Can't allocate new method call\n");
    #endif
    RtnVal = -1;
  }
  else
  {
    dbus_message_append_args(msg, DBUS_TYPE_OBJECT_PATH, &agent_path, DBUS_TYPE_STRING, &capabilities, DBUS_TYPE_INVALID);
    dbus_error_init(&err);

    // Next dbus message is blocking
    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    dbus_message_unref(msg);

    if (!reply)
    {
      #ifdef DEBUG
        fprintf(stderr, "Can't register agent\n");
      #endif
      if (dbus_error_is_set(&err))
      {
        fprintf(stderr, "%s\n", err.message);
        dbus_error_free(&err);
      }
      RtnVal = -1;
    }
    else
    {
      dbus_message_unref(reply);
      dbus_connection_flush(conn);
    }
  }
  return(RtnVal);
}


static DBusHandlerResult agent_filter(DBusConnection *conn, DBusMessage *msg, void *data)
{
  const char *name, *old, *new;

  if (!dbus_message_is_signal(msg, DBUS_INTERFACE_DBUS, "NameOwnerChanged"))
  {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  if (!dbus_message_get_args(msg, NULL,
          DBUS_TYPE_STRING, &name,
          DBUS_TYPE_STRING, &old,
          DBUS_TYPE_STRING, &new,
          DBUS_TYPE_INVALID))
  {
    fprintf(stderr, "Invalid arguments for NameOwnerChanged signal");
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  if (!strcmp(name, "org.bluez") && *new == '\0')
  {
    fprintf(stderr, "Agent has been terminated\n");
    __io_terminated = 1;
  }

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


static DBusHandlerResult request_pincode_message(DBusConnection *conn, DBusMessage *msg, void *data)
{
  const char         *path;
  DBusHandlerResult   Result;
  UBYTE               *pPin;

  if (!dbus_message_get_args(msg, NULL, DBUS_TYPE_OBJECT_PATH, &path, DBUS_TYPE_INVALID))
  {
    #ifdef DEBUG
      fprintf(stderr, "Invalid arguments for RequestPinCode method");
    #endif
    Result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }
  else
  {
    reply = dbus_message_new_method_return(msg);
    if (!reply)
    {
      #ifdef DEBUG
        fprintf(stderr, "Can't create reply message\r\n");
      #endif
      Result = DBUS_HANDLER_RESULT_NEED_MEMORY;
    }
    else
    {

      // If remote device is trusted then automatically
      // add the pin code
      if (TRUE == BtInstance.TrustedDev.Status)
      {
        // This request is the trusted one - evaluated by the hci pin request event
        pPin = BtInstance.TrustedDev.Pin;

        dbus_message_append_args(reply, DBUS_TYPE_STRING,  &pPin, DBUS_TYPE_INVALID);
        dbus_connection_send(conn, reply, NULL);
        dbus_connection_flush(conn);
        dbus_message_unref(reply);

        BtInstance.TrustedDev.Status = FALSE;  //Only one trusted pin per pin event
      }
      else
      {
        BtInstance.Events  =  1;
        Result             =  DBUS_HANDLER_RESULT_HANDLED;
      }
    }
  }
  return(Result);
}


static char *get_default_adapter_path(DBusConnection *conn)
{
  DBusMessage *msg, *reply;
  DBusError err;
  const char *reply_path;
  char *path;

  msg = dbus_message_new_method_call("org.bluez", "/", "org.bluez.Manager", "DefaultAdapter");

  if (!msg)
  {
    fprintf(stderr, "Can't allocate new method call\n");
    return NULL;
  }

  dbus_error_init(&err);

  reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

  dbus_message_unref(msg);

  if (!reply)
  {
    fprintf(stderr, "Can't get default adapter\n");
    if (dbus_error_is_set(&err))
    {
      fprintf(stderr, "%s\n", err.message);
      dbus_error_free(&err);
    }
    return NULL;
  }

  if (!dbus_message_get_args(reply, &err, DBUS_TYPE_OBJECT_PATH, &reply_path, DBUS_TYPE_INVALID))
  {
    fprintf(stderr, "Can't get reply arguments\n");
    if (dbus_error_is_set(&err))
    {
      fprintf(stderr, "%s\n", err.message);
      dbus_error_free(&err);
    }
    return NULL;
  }

  path = strdup(reply_path);

  dbus_message_unref(reply);

  dbus_connection_flush(conn);

  return path;
}


static int RemoveDevice(DBusConnection *conn, char *Device)
{
  DBusMessage *msg, *reply;
  DBusError    err;
  char        *Path;
  const char  *reply_path;
  int         RtnVal;

  RtnVal = 0;

  msg = dbus_message_new_method_call("org.bluez", adapter_path, "org.bluez.Adapter", "FindDevice");
  if (!msg)
  {
    fprintf(stderr, "Can't allocate new method call\n");
    RtnVal = -1;
  }
  else
  {

    dbus_message_append_args(msg, DBUS_TYPE_STRING, &Device, DBUS_TYPE_INVALID);
    dbus_error_init(&err);

    // Next dbus message is a blocking call
    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    dbus_message_unref(msg);

    if (!reply)
    {
      #ifdef DEBUG
        fprintf(stderr, "Can't register agent\r\n");
      #endif
      if (dbus_error_is_set(&err))
      {
        #ifdef DEBUG
          fprintf(stderr, "%s \r\n", err.message);
        #endif
        dbus_error_free(&err);
      }
      RtnVal = -1;
    }
    else
    {

      if (!dbus_message_get_args(reply, &err, DBUS_TYPE_OBJECT_PATH, &reply_path, DBUS_TYPE_INVALID))
      {
        #ifdef DEBUG
          fprintf(stderr, "Can't get reply arguments\n");
        #endif

        if (dbus_error_is_set(&err))
        {
          #ifdef DEBUG
            fprintf(stderr, "%s\n", err.message);
          #endif

          dbus_error_free(&err);
        }
        RtnVal = -1;
      }
      else
      {
        dbus_connection_flush(conn);

        Path = strdup(reply_path);
        dbus_message_unref(reply);

        msg = dbus_message_new_method_call("org.bluez", adapter_path, "org.bluez.Adapter", "RemoveDevice");
        if (!msg)
        {
          #ifdef DEBUG
            fprintf(stderr, "Can't allocate new method call\n");
          #endif
          RtnVal =  -1;
        }
        else
        {

          dbus_message_append_args(msg, DBUS_TYPE_OBJECT_PATH, &Path, DBUS_TYPE_INVALID);
          dbus_error_init(&err);

          // Next dbus message is blocking
          reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
          dbus_message_unref(msg);

          if (!reply)
          {
            #ifdef DEBUG
              fprintf(stderr, "Can't register agent\n");
            #endif
            if (dbus_error_is_set(&err))
            {
              #ifdef DEBUG
                fprintf(stderr, "%s\n", err.message);
              #endif
              dbus_error_free(&err);
            }
            RtnVal = -1;
          }
          else
          {
            dbus_message_unref(reply);
            dbus_connection_flush(conn);
          }
        }
        free(Path);
      }
    }
  }

  return(RtnVal);
}


static void sig_term(int sig)
{
  __io_canceled = 1;
}


static int unregister_agent(DBusConnection *conn, const char *adapter_path, const char *agent_path)
{
  DBusMessage *msg, *reply;
  DBusError    err;

  msg = dbus_message_new_method_call("org.bluez", adapter_path, "org.bluez.Adapter", "UnregisterAgent");
  if (!msg)
  {
    #ifdef DEBUG
      fprintf(stderr, "Can't allocate new method call\n");
    #endif
    return -1;
  }

  dbus_message_append_args(msg, DBUS_TYPE_OBJECT_PATH, &agent_path, DBUS_TYPE_INVALID);
  dbus_error_init(&err);
  reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
  dbus_message_unref(msg);

  if (!reply)
  {
    #ifdef DEBUG
      fprintf(stderr, "Can't unregister agent\n");
    #endif
    if (dbus_error_is_set(&err))
    {
      fprintf(stderr, "%s\n", err.message);
      dbus_error_free(&err);
    }
    return -1;
  }

  dbus_message_unref(reply);
  dbus_connection_flush(conn);
  dbus_connection_unregister_object_path(conn, agent_path);

  return 0;
}


UBYTE     cBtGetEvent(void)
{
  UBYTE   Evt;

  Evt = BtInstance.Events;
  BtInstance.Events = 0;
  return(Evt);
}


void      cBtGetIncoming(UBYTE *pName, UBYTE *pCod, UBYTE Len)
{
  snprintf((char*)pName, Len, "%s", &(BtInstance.Incoming.Name[0]));
  *pCod  =  cBtGetBtType(&(BtInstance.Incoming.DevClass[0]));
}


UBYTE     cBtSetPin(UBYTE *pPin)
{
  UBYTE   RtnVal = OK;

  // Pin can be set both when connecting as master or slave
  if (((HCI_CONNECT == BtInstance.HciSocket.Busy) || (HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy)) &&
       (BLUETOOTH_OFF != BtInstance.State))
  {
    dbus_message_append_args(reply, DBUS_TYPE_STRING, &pPin, DBUS_TYPE_INVALID);
    dbus_connection_send(conn, reply, NULL);
    dbus_connection_flush(conn);
    dbus_message_unref(reply);
  }
  else
  {
    RtnVal = FAIL;
  }
  return(RtnVal);
}


UBYTE   cBtSetPasskey(UBYTE Accept)
{
  UBYTE   RtnVal = OK;

  // Pin can be set both when connecting as master or slave
  if (((HCI_CONNECT == BtInstance.HciSocket.Busy) || (HCI_IDLE == BtInstance.HciSocket.Busy) || (HCI_FAIL == BtInstance.HciSocket.Busy)) &&
       (BLUETOOTH_OFF != BtInstance.State))
  {
    if (Accept)
    {
      dbus_connection_send(conn, reply, NULL);
      dbus_connection_flush(conn);
      dbus_message_unref(reply);
    }
    else
    {
      dbus_connection_send(conn, RejectReply, NULL);
      dbus_connection_flush(conn);
      dbus_message_unref(RejectReply);
    }
  }
  else
  {
    RtnVal = FAIL;
  }
  return(RtnVal);
}

void    cBtSetTrustedDev(UBYTE *pBtAddr, UBYTE *pPin, UBYTE PinSize)
{

  BtInstance.TrustedDev.PinLen = PinSize;
  snprintf((char*)&(BtInstance.TrustedDev.Pin[0]), 7, "%s", pPin);
  cBtStrNoColonToBa(pBtAddr, &(BtInstance.TrustedDev.Adr));
}


void      cBtStrNoColonToBa(UBYTE *pBtStrAddr, bdaddr_t *pAddr)
{
  ULONG   Ba[6];

  sscanf((char *)pBtStrAddr,"%2X%2X%2X%2X%2X%2X", &(Ba[5]), &(Ba[4]), &(Ba[3]), &(Ba[2]), &(Ba[1]), &(Ba[0]));

  // To avoid alignment faults casting to lower size is necessary
  pAddr->b[5] = (uint8_t)Ba[5];
  pAddr->b[4] = (uint8_t)Ba[4];
  pAddr->b[3] = (uint8_t)Ba[3];
  pAddr->b[2] = (uint8_t)Ba[2];
  pAddr->b[1] = (uint8_t)Ba[1];
  pAddr->b[0] = (uint8_t)Ba[0];
}


UWORD     cBtSetBundleId(UBYTE *pId)
{
  UWORD   RtnVal;

  RtnVal = FALSE;
  if (MAX_BUNDLE_ID_SIZE > strlen((char*)pId))
  {
    snprintf((char*)&BtInstance.NonVol.BundleID[0], MAX_BUNDLE_ID_SIZE, "%s", (char*)pId);
    RtnVal = TRUE;
  }

  return(RtnVal);
}

UWORD     cBtSetBundleSeedId(UBYTE *pSeedId)
{
  UWORD   RtnVal;

  RtnVal = FALSE;
  if (MAX_BUNDLE_SEED_ID_SIZE > strlen((char*)pSeedId))
  {
    snprintf((char*)&BtInstance.NonVol.BundleSeedID[0], MAX_BUNDLE_SEED_ID_SIZE, "%s", (char*)pSeedId);
    RtnVal = TRUE;
  }

  return(RtnVal);
}




