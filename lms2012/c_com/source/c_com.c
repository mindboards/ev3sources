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


/*! \page CommunicationLibrary Communication Library
 *
 *- \subpage  CommunicationLibraryDescription
 *- \subpage  CommunicationLibraryCodes
 */


/*! \page CommunicationLibraryDescription Description
 *
 *
 */


/*! \page CommunicationLibraryCodes Byte Code Summary
 *
 *
 */


#include  "c_com.h"
#include  "lms2012.h"
#include  "c_bt.h"
#include  "c_wifi.h"
#include  "c_daisy.h"
#include  "c_md5.h"
#include  "c_i2c.h"
#include  "c_output.h"
#include  "c_memory.h"

#ifdef    DEBUG_C_COM
#define   DEBUG
#endif

#if (HARDWARE != SIMULATION)
  #include  <stdlib.h>
  #include  <stdio.h>
  #include  <sys/types.h>
  #include  <sys/stat.h>
  #include  <fcntl.h>
  #include  <unistd.h>
  #include  <string.h>
  #include  <errno.h>
  #include  <dirent.h>
  COM_GLOBALS ComInstance;
#else

COM_GLOBALS * gComInstance;

void setComInstance(COM_GLOBALS * _Instance)
{
  gComInstance= _Instance;
}

COM_GLOBALS* getComInstance()
{
  return gComInstance;
}
#endif

#define USB_CABLE_DETECT_RATE 15000   // ?? Approx 5 sec. on a good day. Cable detection is a NON-critical
                                      // event
UWORD UsbConUpdate = 0;               // Timing of USB device side cable detection;
UBYTE cComUsbDeviceConnected = FALSE;

UWORD     cComReadBuffer(UBYTE *pBuffer,UWORD Size);
UWORD     cComWriteBuffer(UBYTE *pBuffer,UWORD Size);
UBYTE     cComFindMailbox(UBYTE *pName, UBYTE *pNo);

void  cComSetMusbHdrcMode(void);

#ifdef    DEBUG
  void      cComPrintTxMsg(TXBUF *pTxBuf);
#endif


#define   SIZEOFFILESIZE                  8

enum
{
  FS_IDLE,
};

RESULT    cComInit(void)
{
  RESULT  Result = FAIL;
  UWORD   TmpFileHandle;
  UBYTE   Cnt;
  FILE    *File;


#if (HARDWARE != SIMULATION)

  ComInstance.CommandReady      =  0;
  ComInstance.Cmdfd             =  open(COM_CMD_DEVICE_NAME, O_RDWR, 0666);

  if (ComInstance.Cmdfd >= 0)
  {
    memset(ComInstance.TxBuf[0].Buf,0,sizeof(ComInstance.TxBuf[0].Buf));

    Result  =  OK;
  }
  for (TmpFileHandle = 0;TmpFileHandle < MAX_FILE_HANDLES;TmpFileHandle++)
  {
    ComInstance.Files[TmpFileHandle].State   =  FS_IDLE;
    ComInstance.Files[TmpFileHandle].Name[0] =  0;
    ComInstance.Files[TmpFileHandle].File    = -1;
  }

  for(Cnt = 0; Cnt < NO_OF_CHS; Cnt++)
  {
    ComInstance.TxBuf[Cnt].BufSize = 1024;
    ComInstance.TxBuf[Cnt].Writing = 0;
    ComInstance.RxBuf[Cnt].BufSize = 1024;
    ComInstance.RxBuf[Cnt].State   = RXIDLE;
  }

  ComInstance.ReadChannel[0]   = cComReadBuffer;
  ComInstance.ReadChannel[1]   = NULL;
  ComInstance.ReadChannel[2]   = cBtReadCh0;
  ComInstance.ReadChannel[3]   = cBtReadCh1;
  ComInstance.ReadChannel[4]   = cBtReadCh2;
  ComInstance.ReadChannel[5]   = cBtReadCh3;
  ComInstance.ReadChannel[6]   = cBtReadCh4;
  ComInstance.ReadChannel[7]   = cBtReadCh5;
  ComInstance.ReadChannel[8]   = cBtReadCh6;
  ComInstance.ReadChannel[9]   = cBtReadCh7;
  ComInstance.ReadChannel[10]  = cWiFiReadTcp;

  ComInstance.WriteChannel[0]  = cComWriteBuffer;
  ComInstance.WriteChannel[1]  = NULL;
  ComInstance.WriteChannel[2]  = cBtDevWriteBuf;
  ComInstance.WriteChannel[3]  = cBtDevWriteBuf1;
  ComInstance.WriteChannel[4]  = cBtDevWriteBuf2;
  ComInstance.WriteChannel[5]  = cBtDevWriteBuf3;
  ComInstance.WriteChannel[6]  = cBtDevWriteBuf4;
  ComInstance.WriteChannel[7]  = cBtDevWriteBuf5;
  ComInstance.WriteChannel[8]  = cBtDevWriteBuf6;
  ComInstance.WriteChannel[9]  = cBtDevWriteBuf7;
  ComInstance.WriteChannel[10] = cWiFiWriteTcp;

  for(Cnt = 0; Cnt < NO_OF_MAILBOXES; Cnt++)
  {
    ComInstance.MailBox[Cnt].Status   =  FAIL;
    ComInstance.MailBox[Cnt].DataSize =  0;
    ComInstance.MailBox[Cnt].ReadCnt  =  0;
    ComInstance.MailBox[Cnt].WriteCnt =  0;
    ComInstance.MailBox[Cnt].Name[0]  =  0;
  }

  ComInstance.ComResult = OK;

  ComInstance.BrickName[0] = 0;
  File  =  fopen("./settings/BrickName","r");
  if (File != NULL)
  {

    fgets((char*)&(ComInstance.BrickName[0]), (int)vmBRICKNAMESIZE, File);
    fclose (File);
  }

  // USB cable stuff init
  UsbConUpdate = 0;               // Reset timing of USB device side cable detection;
  cComUsbDeviceConnected = FALSE; // Until we believe in something else ;-)
  cComSetMusbHdrcMode();

  BtInit((char*)&(ComInstance.BrickName[0]));
  cDaisyInit();
  cWiFiInit();

  ComInstance.VmReady      =  1;
  ComInstance.ReplyStatus  =  0;

  #else
    return OK;
  #endif

  return (Result);

}


RESULT    cComOpen(void)
{
  RESULT  Result = FAIL;

  Result  =  OK;

  return (Result);
}


RESULT    cComClose(void)
{
  RESULT  Result = FAIL;
  UBYTE   Cnt;

  for(Cnt = 0; Cnt < NO_OF_MAILBOXES; Cnt++)
  {
    ComInstance.MailBox[Cnt].Status   =  FAIL;
    ComInstance.MailBox[Cnt].DataSize =  0;
    ComInstance.MailBox[Cnt].ReadCnt  =  0;
    ComInstance.MailBox[Cnt].WriteCnt =  0;
    ComInstance.MailBox[Cnt].Name[0]  =  0;
  }

  Result  =  OK;

  return (Result);
}


RESULT    cComExit(void)
{
  RESULT  Result = FAIL;

  close(ComInstance.Cmdfd);

  Result  =  OK;

  cWiFiExit();
  BtExit();

  return (Result);
}

// DAISY chain
// Write type data to chain
RESULT    cComSetDeviceInfo(DATA8 Length,UBYTE *pInfo)
{
  return cDaisySetDeviceInfo(Length, pInfo);
  //return FAIL;
}

// Read type data from chain
RESULT    cComGetDeviceInfo(DATA8 Length,UBYTE *pInfo)
{
  return cDaisyGetDeviceInfo(Length, pInfo);
}

// Write mode to chain
RESULT    cComSetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode)
{
  return cDaisySetDeviceType(Layer, Port, Type, Mode);
}

// Read device data from chain
RESULT    cComGetDeviceData(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
{
  return cDaisyGetDownstreamData(Layer, Port, Length, pType, pMode, pData);
}

void  cComSetMusbHdrcMode(void)
{
  // Force OTG into mode
  char Musb_Cmd[64];
  // Build the command string for setting the OTG mode
  strcpy(Musb_Cmd, "echo otg > /sys/devices/platform/musb_hdrc/mode");
  system(Musb_Cmd);
}

UBYTE cComCheckUsbCable(void)
{
  // Get mode from MUSB_HDRC
  UBYTE Result = FALSE;
  char buffer[21];
  FILE *f;

  f = fopen("/sys/devices/platform/musb_hdrc/mode", "r");
  if(f)
  {
    fread(buffer, 20, 1, f);
    fclose(f);
  }

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printf("BUFFER = %s\n\r", buffer);
  #endif

  if(strstr(buffer, "b_peripheral") != 0)
    Result = TRUE;

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    if(Result == TRUE)
      printf("CABLE connected\n\r");
    else
      printf("CABLE dis-connected :-(\n\r");
  #endif
  // Just to be sure - we disables further prinf "polution"
  #undef DEBUG

  return Result;
}

#ifdef DEBUG
void      cComShow(UBYTE *pB)
{
  UWORD   Lng;

  Lng  =  (UWORD)pB[0];
  Lng +=  (UWORD)pB[1] << 8;

  printf("[%02X%02X",pB[0],pB[1]);
  pB++;
  pB++;

  while (Lng)
  {
    printf("%02X",*pB);
    pB++;
    Lng--;
  }
  printf("]\r\n");
}

#endif


#ifdef    DEBUG
  void      cComPrintTxMsg(TXBUF *pTxBuf)
  {
    ULONG   Cnt, Cnt2;
    COMRPL  *pComRpl;

    pComRpl = (COMRPL*) pTxBuf->Buf;

    printf("Tx Buf content: \r\n");
    for(Cnt = 0; ((Cnt < ((*pComRpl).CmdSize + 2)) && (Cnt < 1024)); Cnt++)
    {
      for(Cnt2 = Cnt; Cnt2 < (Cnt + 16); Cnt2++)
      {
        printf("0x%02X, ",((UBYTE*)(&((*pComRpl).CmdSize)))[Cnt2]);
      }
      Cnt = (Cnt2-1);
      printf("\r\n");
    }
    printf("\r\n");
  }
#endif



UWORD     cComReadBuffer(UBYTE *pBuffer,UWORD Size)
{
  UWORD   Length = 0;
#if (HARDWARE != SIMULATION)

  struct  timeval Cmdtv;
  fd_set  Cmdfds;

  Cmdtv.tv_sec   =  0;
  Cmdtv.tv_usec  =  0;
  FD_ZERO(&Cmdfds);
  FD_SET(ComInstance.Cmdfd, &Cmdfds);
  if (select(ComInstance.Cmdfd + 1, &Cmdfds, NULL, NULL, &Cmdtv))
  {
    Length  =  read(ComInstance.Cmdfd,pBuffer,(size_t)Size);

    #undef DEBUG
    //#define DEBUG
	  #ifdef DEBUG
    	printf("cComReadBuffer Length = %d\r\n", Length);
	  #endif

  }
#endif
  return (Length);
}


UWORD     cComWriteBuffer(UBYTE *pBuffer,UWORD Size)
{
  UWORD   Length = 0;
#if (HARDWARE != SIMULATION)



  	  if(FULL_SPEED == cDaisyGetUsbUpStreamSpeed())
  	  {
  	    Length  =  write(ComInstance.Cmdfd,pBuffer,64);
  	  }
  	  else
  	    Length  =  write(ComInstance.Cmdfd,pBuffer,1024);

	  #undef DEBUG
	  //#define DEBUG
	  #ifdef DEBUG
	  	  printf("cComWriteBuffer %d\n\r", Length);
	  #endif

#endif
  return (Length);
}


UBYTE     cComDirectCommand(UBYTE *pBuffer,UBYTE *pReply)
{
  UBYTE   Result = 0;
  COMCMD  *pComCmd;
  COMRPL  *pComRpl;
  DIRCMD  *pDirCmd;
  CMDSIZE CmdSize;
  CMDSIZE HeadSize;
  UWORD   Tmp;
  UWORD   Globals;
  UWORD   Locals;
  IMGHEAD *pImgHead;
  OBJHEAD *pObjHead;
  CMDSIZE Length;

  ComInstance.VmReady = 0;
  pComCmd   =  (COMCMD*)pBuffer;
  pDirCmd   =  (DIRCMD*)(*pComCmd).PayLoad;

  CmdSize   =  (*pComCmd).CmdSize;
  HeadSize  =  ((*pDirCmd).Code - pBuffer) - sizeof(CMDSIZE);
  Length    =  CmdSize - HeadSize;

  pComRpl             =  (COMRPL*)pReply;
  (*pComRpl).CmdSize  =  3;
  (*pComRpl).MsgCnt   =  (*pComCmd).MsgCnt;
  (*pComRpl).Cmd      =  DIRECT_REPLY_ERROR;

  if ((CmdSize > HeadSize) && ((CmdSize - HeadSize) < (sizeof(ComInstance.Image) - (sizeof(IMGHEAD) + sizeof(OBJHEAD)))))
  {

    Tmp  =  (UWORD)(*pDirCmd).Globals + ((UWORD)(*pDirCmd).Locals << 8);

    Globals  =  (Tmp & 0x3FF);
    Locals   =  ((Tmp >> 10) & 0x3F);

    if ((Locals <= MAX_COMMAND_LOCALS) && (Globals <= MAX_COMMAND_GLOBALS))
    {

      pImgHead                            =  (IMGHEAD*)ComInstance.Image;
      pObjHead                            =  (OBJHEAD*)(ComInstance.Image + sizeof(IMGHEAD));

      (*pImgHead).Sign[0]                 =  'l';
      (*pImgHead).Sign[1]                 =  'e';
      (*pImgHead).Sign[2]                 =  'g';
      (*pImgHead).Sign[3]                 =  'o';
      (*pImgHead).VersionInfo             =  (UWORD)(VERS * 100.0);
      (*pImgHead).NumberOfObjects         =  (OBJID)1;
      (*pImgHead).GlobalBytes             =  (GBINDEX)Globals;

      (*pObjHead).OffsetToInstructions    =  (IP)(sizeof(IMGHEAD) + sizeof(OBJHEAD));
      (*pObjHead).OwnerObjectId           =  0;
      (*pObjHead).TriggerCount            =  0;
      (*pObjHead).LocalBytes              =  (OBJID)Locals;

      memcpy(&ComInstance.Image[sizeof(IMGHEAD) + sizeof(OBJHEAD)],(*pDirCmd).Code,Length);
      Length       +=  sizeof(IMGHEAD) + sizeof(OBJHEAD);

      ComInstance.Image[Length]  =  opOBJECT_END;
      (*pImgHead).ImageSize      =  Length;

      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
        printf("\r\n");
        for (Tmp = 0;Tmp <= Length;Tmp++)
        {
          printf("%02X ",ComInstance.Image[Tmp]);
          if ((Tmp & 0x0F) == 0x0F)
          {
            printf("\r\n");
          }
        }
        printf("\r\n");
      #endif

      Result  =  1;
    }
    else
    {
      (*pComRpl).Cmd      =  DIRECT_REPLY_ERROR;
    }
  }
  else
  {
    (*pComRpl).Cmd      =  DIRECT_REPLY_ERROR;
  }

  return (Result);
}


void      cComCloseFileHandle(SLONG *pHandle)
{
  if (*pHandle >= MIN_HANDLE)
  {
    close(*pHandle);
    *pHandle = -1;
  }
}


UBYTE     cComFreeHandle(DATA8 Handle)
{
  UBYTE   RtnVal = TRUE;

  if (0 != ComInstance.Files[Handle].Name[0])
  {
    if ((Handle >= 0) && (Handle < MAX_FILE_HANDLES))
    {
      ComInstance.Files[Handle].Name[0]  =  0;
    }
  }
  else
  {
    // Handle is unused
    RtnVal = FALSE;
  }
  return(RtnVal);
}


DATA8     cComGetHandle(char *pName)
{
  DATA8   Result = 0;

  while ((ComInstance.Files[Result].Name[0]) && (Result < MAX_FILE_HANDLES))
  {
    Result++;
  }

  if (Result < MAX_FILE_HANDLES)
  {
    if (OK == cMemoryCheckFilename(pName, NULL, NULL, NULL))
    {
      sprintf(ComInstance.Files[Result].Name, "%s", (char*)pName);
    }
    else
    {
      Result = -1;
    }
  }
  else
  {
    Result = -1;
  }
  return (Result);
}


UBYTE     cComGetNxtFile(DIR *pDir, UBYTE *pName)
{
  UBYTE     RtnVal = 0;
  struct    dirent   *pDirPtr;

  pDirPtr = readdir(pDir);
  while((NULL != pDirPtr) && (DT_REG != pDirPtr->d_type))
  {
    pDirPtr = readdir(pDir);
  }

  if(NULL != pDirPtr)
  {
    snprintf((char*)pName, FILENAME_SIZE,"%s",pDirPtr->d_name);
    RtnVal = 1;
  }
  else
  {
    closedir(pDir);
  }

  return(RtnVal);
}


void      cComCreateBeginDl( TXBUF *pTxBuf, UBYTE *pName)
{
  UWORD     Index;
  UWORD     ReadBytes;
  BEGIN_DL  *pDlMsg;
  SBYTE     FileHandle;
  char      TmpFileName[vmFILENAMESIZE];

  FileHandle = -1;
  pDlMsg    =  (BEGIN_DL*) &(pTxBuf->Buf[0]);

  if ((strlen((char *)pTxBuf->Folder) + strlen((char *)pName) + 1) <= vmFILENAMESIZE)
  {
    sprintf(TmpFileName, "%s", (char*)pTxBuf->Folder);
    strcat(TmpFileName, (char*)pName);

    FileHandle  =  cComGetHandle(TmpFileName);
  }

  if (-1 != FileHandle)
  {

    pTxBuf->pFile        =  (FIL*)&(ComInstance.Files[FileHandle]);
    pTxBuf->pFile->File  =  open(pTxBuf->pFile->Name,O_RDONLY,0x444);

    // Get file length
    pTxBuf->pFile->Size = lseek(pTxBuf->pFile->File, 0L, SEEK_END);
    lseek(pTxBuf->pFile->File, 0L, SEEK_SET);

    pDlMsg->CmdSize       =  0x00;                               // Msg Len
    pDlMsg->MsgCount      =  0x00;                               // Msg Count
    pDlMsg->CmdType       =  SYSTEM_COMMAND_REPLY;               // Cmd Type
    pDlMsg->Cmd           =  BEGIN_DOWNLOAD;                     // Cmd

    pDlMsg->FileSizeLsb   =  (UBYTE)(pTxBuf->pFile->Size);       // File Size Lsb
    pDlMsg->FileSizeNsb1  =  (UBYTE)(pTxBuf->pFile->Size >>  8); // File Size
    pDlMsg->FileSizeNsb2  =  (UBYTE)(pTxBuf->pFile->Size >> 16); // File Size
    pDlMsg->FileSizeMsb   =  (UBYTE)(pTxBuf->pFile->Size >> 24); // File Size Msb

    Index = strlen((char*)pTxBuf->pFile->Name) + 1;
    snprintf((char*)(&(pTxBuf->Buf[SIZEOF_BEGINDL])), Index, "%s", (char*)pTxBuf->pFile->Name);

    Index += SIZEOF_BEGINDL;

    // Find the number of bytes to go into this message
    if ((MAX_MSG_SIZE - Index) < pTxBuf->pFile->Size)
    {
      // Msg cannot hold complete file
      pTxBuf->Buf[0] = (UBYTE)(MAX_MSG_SIZE - 2);
      pTxBuf->Buf[1] = (UBYTE)((MAX_MSG_SIZE - 2) >> 8);

      ReadBytes         =  read(pTxBuf->pFile->File, &(pTxBuf->Buf[Index]), (pTxBuf->BufSize - Index));
      pTxBuf->BlockLen  =  pTxBuf->BufSize;

      pTxBuf->SubState        =  FILE_IN_PROGRESS_WAIT_FOR_REPLY;
      pTxBuf->SendBytes       =  ReadBytes;                       // No of bytes send in message
      pTxBuf->pFile->Pointer  =  ReadBytes;                       // No of bytes read from file
      pTxBuf->MsgLen          =  (pDlMsg->CmdSize - Index) + 2;   // Index = full header size
      pTxBuf->Writing         =  1;
    }
    else
    {
      // Msg can hold complete file
      pTxBuf->Buf[0] = (UBYTE) (pTxBuf->pFile->Size + Index - 2);
      pTxBuf->Buf[1] = (UBYTE)((pTxBuf->pFile->Size + Index - 2) >> 8);

      if ((pTxBuf->BufSize - Index) < pTxBuf->pFile->Size)
      {
        // Complete msg exceeds buffer size
        ReadBytes         =  read(pTxBuf->pFile->File, &(pTxBuf->Buf[Index]), (pTxBuf->BufSize - Index));
        pTxBuf->BlockLen  =  pTxBuf->BufSize;
      }
      else
      {
        // Complete msg can fit in buffer
        ReadBytes         =  read(pTxBuf->pFile->File, &(pTxBuf->Buf[Index]), pTxBuf->pFile->Size);
        pTxBuf->BlockLen  =  pTxBuf->pFile->Size + Index;

        // Close handles
        cComCloseFileHandle(&(pTxBuf->pFile->File));
        cComFreeHandle(FileHandle);
      }

      pTxBuf->SubState        =  FILE_COMPLETE_WAIT_FOR_REPLY;
      pTxBuf->SendBytes       =  ReadBytes;                       // No of bytes send in message
      pTxBuf->pFile->Pointer  =  ReadBytes;                       // No of bytes read from file
      pTxBuf->MsgLen          =  (pDlMsg->CmdSize - Index) + 2;   // Index = full header size
      pTxBuf->Writing         =  1;
    }
  }
}

void      cComCreateContinueDl(RXBUF *pRxBuf, TXBUF *pTxBuf)
{
  UWORD         ReadBytes;
  CONTINUE_DL   *pContinueDl;
  RPLY_BEGIN_DL *pRplyBeginDl;

  pRplyBeginDl =  (RPLY_BEGIN_DL*)&(pRxBuf->Buf[0]);
  pContinueDl  =  (CONTINUE_DL*)&(pTxBuf->Buf[0]);

  pContinueDl->CmdSize   =  SIZEOF_CONTINUEDL - sizeof(CMDSIZE);
  pContinueDl->MsgCount  =  (pRplyBeginDl->MsgCount)++;
  pContinueDl->CmdType   =  SYSTEM_COMMAND_REPLY;
  pContinueDl->Cmd       =  CONTINUE_DOWNLOAD;
  pContinueDl->Handle    =  pTxBuf->RemoteFileHandle;

  if ((MAX_MSG_SIZE - SIZEOF_CONTINUEDL) < (pTxBuf->pFile->Size - pTxBuf->pFile->Pointer))
  {
    // Msg cannot hold complete file
    ReadBytes         =  read(pTxBuf->pFile->File, &(pTxBuf->Buf[SIZEOF_CONTINUEDL]), (pTxBuf->BufSize - SIZEOF_CONTINUEDL));
    pTxBuf->BlockLen  =  pTxBuf->BufSize;

    pContinueDl->CmdSize   +=  ReadBytes;
    pTxBuf->SubState        =  FILE_IN_PROGRESS_WAIT_FOR_REPLY;
    pTxBuf->SendBytes       =  ReadBytes;                                       // No of bytes send in message
    pTxBuf->pFile->Pointer +=  ReadBytes;                                       // No of bytes read from file
    pTxBuf->MsgLen          =  (pContinueDl->CmdSize - SIZEOF_CONTINUEDL) + 2;  // Index = full header size
    pTxBuf->Writing         =  1;
  }
  else
  {
    // Msg can hold complete file
    if ((pTxBuf->BufSize - SIZEOF_CONTINUEDL) < (pTxBuf->pFile->Size - pTxBuf->pFile->Pointer))
    {
      // Complete msg exceeds buffer size
      ReadBytes              =  read(pTxBuf->pFile->File, &(pTxBuf->Buf[SIZEOF_CONTINUEDL]), (pTxBuf->BufSize - SIZEOF_CONTINUEDL));
    }
    else
    {
      // Complete msg can fit in buffer
      ReadBytes              =  read(pTxBuf->pFile->File, &(pTxBuf->Buf[SIZEOF_CONTINUEDL]), (pTxBuf->pFile->Size - pTxBuf->pFile->Pointer));

      // Close handles
      cComCloseFileHandle(&(pTxBuf->pFile->File));
      cComFreeHandle(pTxBuf->FileHandle);
    }

    pTxBuf->BlockLen        =  ReadBytes + SIZEOF_CONTINUEDL;
    pContinueDl->CmdSize   +=  ReadBytes;
    pTxBuf->SubState        =  FILE_COMPLETE_WAIT_FOR_REPLY;
    pTxBuf->SendBytes       =  ReadBytes;                       // No of bytes send in message
    pTxBuf->pFile->Pointer +=  ReadBytes;                       // No of bytes read from file
    pTxBuf->MsgLen          =  (pContinueDl->CmdSize - SIZEOF_CONTINUEDL) + 2;   // Index = full header size
    pTxBuf->Writing         =  1;
  }
}


void      cComSystemReply(RXBUF *pRxBuf, TXBUF *pTxBuf)
{
  COMCMD   *pComCmd;
  SYSCMDC  *pSysCmdC;
  CMDSIZE   CmdSize;
  UBYTE     FileName[MAX_FILENAME_SIZE];

  pComCmd   =  (COMCMD*)pRxBuf->Buf;
  pSysCmdC  =  (SYSCMDC*)(*pComCmd).PayLoad;
  CmdSize   =  (*pComCmd).CmdSize;

  switch ((*pSysCmdC).Sys)
  {
    case BEGIN_DOWNLOAD:
    {
      // This is the reply from the begin download command
      // This is part of the file transfer sequence
      // Check for single file or Folder transfer

      RPLY_BEGIN_DL   *pRplyBeginDl;

      pRplyBeginDl  =  (RPLY_BEGIN_DL*)&(pRxBuf->Buf[0]);

      if ((SUCCESS == pRplyBeginDl->Status) || (END_OF_FILE == pRplyBeginDl->Status))
      {
        pTxBuf->RemoteFileHandle = pRplyBeginDl->Handle;

        if (FILE_IN_PROGRESS_WAIT_FOR_REPLY == pTxBuf->SubState)
        {
          // Issue continue write as file is not
          // completely downloaded
          cComCreateContinueDl(pRxBuf, pTxBuf);
        }
        else
        {
          if (FILE_COMPLETE_WAIT_FOR_REPLY == pTxBuf->SubState)
          {
            // Complete file downloaded check for more files
            // to download
            if (TXFOLDER == pTxBuf->State)
            {
              if (cComGetNxtFile(pTxBuf->pDir, FileName))
              {
                cComCreateBeginDl(pTxBuf, FileName);
              }
              else
              {
                // No More files
                pTxBuf->State          =  TXIDLE;
                pTxBuf->SubState       =  SUBSTATE_IDLE;
                ComInstance.ComResult  =  OK;
              }
            }
            else
            {
              // Only one file to send
              pTxBuf->State          =  TXIDLE;
              pTxBuf->SubState       =  SUBSTATE_IDLE;
              ComInstance.ComResult  =  OK;
            }
          }
        }
      }
    }
    break;

    case CONTINUE_DOWNLOAD:
    {

      RPLY_CONTINUE_DL   *pRplyContinueDl;
      pRplyContinueDl  =  (RPLY_CONTINUE_DL*)&(pRxBuf->Buf[0]);

      if ((SUCCESS == pRplyContinueDl->Status) || (END_OF_FILE == pRplyContinueDl->Status))
      {
        if (FILE_IN_PROGRESS_WAIT_FOR_REPLY == pTxBuf->SubState)
        {
          // Issue continue write as file is not
          // completely downloaded
          cComCreateContinueDl(pRxBuf, pTxBuf);
        }
        else
        {
          if (FILE_COMPLETE_WAIT_FOR_REPLY == pTxBuf->SubState)
          {
            // Complete file downloaded check for more files
            // to download
            if (TXFOLDER == pTxBuf->State)
            {
              if (cComGetNxtFile(pTxBuf->pDir, FileName))
              {
                cComCreateBeginDl(pTxBuf, FileName);
              }
              else
              {
                pTxBuf->State          =  TXIDLE;
                pTxBuf->SubState       =  SUBSTATE_IDLE;
                ComInstance.ComResult  =  OK;
              }
            }
            else
            {
              pTxBuf->State          =  TXIDLE;
              pTxBuf->SubState       =  SUBSTATE_IDLE;
              ComInstance.ComResult  =  OK;
            }
          }
        }
      }
    }
    break;

    default:
    {
    }
    break;
  }
}


void      cComGetNameFromScandirList(struct  dirent *NameList, char *pBuffer, ULONG *pNameLen, UBYTE *pFolder)
{
  char    FileName[MD5LEN + 1 + FILENAMESIZE];
  struct  stat FileInfo;
  ULONG   Md5Sum[4];

  *pNameLen = 0;

  //If type is a directory the add "/" at the end
  if ((DT_LNK == NameList->d_type) || (DT_DIR == NameList->d_type))
  {
    strncpy(pBuffer, NameList->d_name, FILENAMESIZE);   // Need to copy to tmp var to be able to add "/" for folders
    *pNameLen = strlen(NameList->d_name);               // + 1 is the new line character

    strcat(pBuffer,"/");
    (*pNameLen)++;

    strcat(pBuffer,"\n");
    (*pNameLen)++;

  }
  else
  {

    if (DT_REG == NameList->d_type)
    {
      /* If it is a file then add 16 bytes MD5SUM + space */
      strcpy(FileName, (char*)pFolder);
      strcat(FileName, "/");
      strcat(FileName,NameList->d_name);

      /* Get the MD5sum and put in the buffer */
      md5_file(FileName, 0, (unsigned char *)Md5Sum);
      *pNameLen  = sprintf(pBuffer, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X ",
                          ((UBYTE*)Md5Sum)[0] , ((UBYTE*)Md5Sum)[1] , ((UBYTE*)Md5Sum)[2] , ((UBYTE*)Md5Sum)[3] ,
                          ((UBYTE*)Md5Sum)[4] , ((UBYTE*)Md5Sum)[5] , ((UBYTE*)Md5Sum)[6] , ((UBYTE*)Md5Sum)[7] ,
                          ((UBYTE*)Md5Sum)[8] , ((UBYTE*)Md5Sum)[9] , ((UBYTE*)Md5Sum)[10], ((UBYTE*)Md5Sum)[11],
                          ((UBYTE*)Md5Sum)[12], ((UBYTE*)Md5Sum)[13], ((UBYTE*)Md5Sum)[14], ((UBYTE*)Md5Sum)[15]);

      /* Insert file size */
      stat(FileName,&FileInfo);
      *pNameLen += sprintf(&(pBuffer[MD5LEN + 1]), "%08X ",(ULONG)FileInfo.st_size);

      /* Insert Filename */
      strcat(pBuffer, NameList->d_name);
      *pNameLen += strlen(NameList->d_name);

      strcat(pBuffer,"\n");
      (*pNameLen)++;
    }
  }
}


UBYTE     cComCheckForSpace(char *pFullName, ULONG Size)
{
  UBYTE  RtnVal;
  DATA32 TotalSize;
  DATA32 FreeSize;
  DATA8  Changed;

  RtnVal = FAIL;

  //Make it KB and round up
  if (Size & (ULONG)(KB-1))
  {
    Size /= KB;
    Size += 1;
  }
  else
  {
    Size /= KB;
  }

  if (strstr(pFullName, SDCARD_FOLDER) != NULL)
  {
    if (CheckSdcard(&Changed, &TotalSize, &FreeSize, 1))
    {
      if (FreeSize >= Size)
      {
        RtnVal = OK;
      }
    }
  }
  else
  {
    if (strstr(pFullName, USBSTICK_FOLDER) != NULL)
    {
      if (CheckUsbstick(&Changed, &TotalSize, &FreeSize, 1))
      {
        if (FreeSize >= Size)
        {
          RtnVal = OK;
        }
      }
    }
    else
    {
      cMemoryGetUsage(&TotalSize, &FreeSize, 1);
      if (FreeSize >= Size + 300)
      {
        RtnVal = OK;
      }
    }
  }
  return(RtnVal);
}


/*! \page ComModule
 *
 *  <hr size="1"/>
 *  <b>     write </b>
 */
/*! \brief    cSystemCommand
 *
 *
 *  cComSystemCommand
 *  -----------------
 *
 *
 *
 *    One message at a time principles
 *    --------------------------------
 *
 *     - The system is only able to process one command at a time (both SYS and DIR commands).
 *     - ComInstance.ReplyStatus holds the information about command processing.
 *     - Writing flag indicated that the reply is ready to be transmitted
 *     - Direct commands are commands to be interpreted by VM - Direct commands are not interpreted until process is
 *       returned to the VM
 *     - System commands are usually processed when received except for large messages (messages larger that buffersize)
 *
 *    Direct command              -> When received     ->  if reply required        ->  ComInstance.ReplyStatus = DIR_CMD_REPLY
 *                                         |
 *                                          ------------>  if no reply required     ->  ComInstance.ReplyStatus = DIR_CMD_NOREPLY
 *
 *    VM reply to direct command  -> if (ComInstance.ReplyStatus = DIR_CMD_REPLY)   ->  pTxBuf->Writing         = 1
 *    (VM always replies, after interp.)                                                ComInstance.ReplyStatus = 0
 *               |
 *                -----------------> if (ComInstance.ReplyStatus = DIR_CMD_NOREPLY) ->  pTxBuf->Writing         = 0
 *                                                                                      ComInstance.ReplyStatus = 0
 *
 *
 *    System command              -> if reply required ->  ComInstance.ReplyStatus = SYS_CMD_REPLY   -> if (pRxBuf->State  =  RXFILEDL) -> Do nothing
 *         |                                                                 |
 *         |                                                                  ------------------------> if (pRxBuf->State  != RXFILEDL) -> pTxBuf->Writing         = 1
 *         |                                                                                                                               ComInstance.ReplyStatus = 0
 *         |
 *          -----------------------> If reply not req. ->  ComInstance.ReplyStatus = SYS_CMD_NOREPLY -> if (pRxBuf->State  =  RXFILEDL) -> Do nothing
 *                                                                           |
 *                                                                            ------------------------> if (pRxBuf->State  != RXFILEDL) -> ComInstance.ReplyStatus = 0
 *
 *
 *
 *    File Download - Large messages
 *    ------------------------------
 *
 *    CONTINUE_DOWNLOAD
 *    BEGIN_DOWNLOAD    --->   (Message size <= Buffer size)  -->  Write bytes to file
 *         |                                                  -->  pRxBuf->State  =  RXIDLE
 *         |
 *         |
 *          --------------->   (Message size  > Buffer Size)  -->  Write bytes from buffer
 *                                                            -->  pRxBuf->State  =  RXFILEDL
 *
 *          Buffer full --->   if pRxBuf->State  =  RXFILEDL  -->  Yes -> write into buffer
 *                                           |                                   |
 *                                           |                                    ------------------> if Remainig msg = 0  ->  pRxBuf->State  =  RXIDLE
 *                                           |
 *                                            ------------------>  No  ->  Interprete as new command
 *
 *
 *
 */
void      cComSystemCommand(RXBUF *pRxBuf, TXBUF *pTxBuf)
{
  COMCMD   *pComCmd;
  SYSCMDC  *pSysCmdC;
  CMDSIZE  CmdSize;
  int      Tmp;
  char     Folder[60];
  ULONG    BytesToWrite;
  DATA8    FileHandle;

  pComCmd   =  (COMCMD*)pRxBuf->Buf;
  pSysCmdC  =  (SYSCMDC*)(*pComCmd).PayLoad;
  CmdSize   =  (*pComCmd).CmdSize;

  switch ((*pSysCmdC).Sys)
  {
    case BEGIN_DOWNLOAD :
    {
      BEGIN_DL        *pBeginDl;
      RPLY_BEGIN_DL   *pReplyDl;
      ULONG           MsgHeaderSize;

      //Setup pointers
      pBeginDl           =  (BEGIN_DL*)pRxBuf->Buf;
      pReplyDl           =  (RPLY_BEGIN_DL*)pTxBuf->Buf;

      // Get file handle
      FileHandle         =  cComGetHandle((char*)pBeginDl->Path);
      pRxBuf->pFile      =  &(ComInstance.Files[FileHandle]);

      // Fill the reply
      pReplyDl->CmdSize  =  SIZEOF_RPLYBEGINDL - sizeof(CMDSIZE);
      pReplyDl->MsgCount =  pBeginDl->MsgCount;
      pReplyDl->CmdType  =  SYSTEM_REPLY;
      pReplyDl->Cmd      =  BEGIN_DOWNLOAD;
      pReplyDl->Status   =  SUCCESS;
      pReplyDl->Handle   =  FileHandle;

      if (FileHandle >= 0)
      {
        pRxBuf->pFile->Size     =  (ULONG)(pBeginDl->FileSizeLsb);
        pRxBuf->pFile->Size    +=  (ULONG)(pBeginDl->FileSizeNsb1) <<  8;
        pRxBuf->pFile->Size    +=  (ULONG)(pBeginDl->FileSizeNsb2) << 16;
        pRxBuf->pFile->Size    +=  (ULONG)(pBeginDl->FileSizeMsb)  << 24;

        if (OK == cComCheckForSpace(&(ComInstance.Files[FileHandle].Name[0]), pRxBuf->pFile->Size))
        {

          pRxBuf->pFile->Length   =  (ULONG)0;
          pRxBuf->pFile->Pointer  =  (ULONG)0;

          Tmp  =  0;
          while ((ComInstance.Files[FileHandle].Name[Tmp]) && (Tmp < (MAX_FILENAME_SIZE - 1)))
          {
            Folder[Tmp]  =  ComInstance.Files[FileHandle].Name[Tmp];
            if (Folder[Tmp] == '/')
            {
              Folder[Tmp + 1]  =  0;
              if (strcmp("~/",Folder) != 0)
              {
                if (mkdir(Folder,S_IRWXU | S_IRWXG | S_IRWXO) == 0)
                {
                  chmod(Folder,S_IRWXU | S_IRWXG | S_IRWXO);
                  #ifdef DEBUG
                    printf("Folder %s created\r\n",Folder);
                  #endif
                }
                else
                {
                  #ifdef DEBUG
                    printf("Folder %s not created (%s)\r\n",Folder,strerror(errno));
                  #endif
                }
              }
            }
            Tmp++;
          }

          pRxBuf->pFile->File  =  open(pRxBuf->pFile->Name, O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, 0x666);

          if (pRxBuf->pFile->File >= 0)
          {
            MsgHeaderSize   =  (strlen((char*)pBeginDl->Path) + 1 + SIZEOF_BEGINDL); // +1 = zero termination
            pRxBuf->MsgLen  =  CmdSize + sizeof(CMDSIZE) - MsgHeaderSize;

            if (CmdSize <= (pRxBuf->BufSize - sizeof(CMDSIZE)))
            {
              // All data included in this buffer
              BytesToWrite     =  pRxBuf->MsgLen;
              pRxBuf->State    =  RXIDLE;
            }
            else
            {
              // Rx buffer must be read more that one to receive the complete message
              BytesToWrite   =  pRxBuf->BufSize - MsgHeaderSize;
              pRxBuf->State  =  RXFILEDL;
            }
            pTxBuf->BlockLen =  SIZEOF_RPLYBEGINDL;

            if (BytesToWrite > (ComInstance.Files[FileHandle].Size))
            {
              // If Bytes to write into the file is bigger than file size -> Error message
              pReplyDl->Status  =  SIZE_ERROR;
              BytesToWrite      =  ComInstance.Files[FileHandle].Size;
            }

            write(pRxBuf->pFile->File, &(pRxBuf->Buf[MsgHeaderSize]), (size_t)BytesToWrite);
            ComInstance.Files[FileHandle].Length  +=  (ULONG)BytesToWrite;
            pRxBuf->RxBytes                        =  (ULONG)BytesToWrite;
            pRxBuf->pFile->Pointer                 =  (ULONG)BytesToWrite;

            if (pRxBuf->pFile->Pointer >= pRxBuf->pFile->Size)
            {
              cComCloseFileHandle(&(pRxBuf->pFile->File));
              chmod(ComInstance.Files[FileHandle].Name,S_IRWXU | S_IRWXG | S_IRWXO);
              cComFreeHandle(FileHandle);

              pReplyDl->Status  =  END_OF_FILE;
              pRxBuf->State     =  RXIDLE;
            }
          }
          else
          {
            // Error in opening file
            #ifdef DEBUG
              printf("File %s not created\r\n",ComInstance.Files[FileHandle].Name);
            #endif
            cComFreeHandle(FileHandle);
            pReplyDl->CmdType  =  SYSTEM_REPLY_ERROR;
            pReplyDl->Status   =  UNKNOWN_HANDLE;
            pReplyDl->Handle   =  -1;
            pTxBuf->BlockLen   =  SIZEOF_RPLYBEGINDL;

          }
        }
        else
        {
          //Not enough space for the file
          #ifdef DEBUG
            printf("File %s is too big\r\n",ComInstance.Files[FileHandle].Name);
          #endif

          cComFreeHandle(FileHandle);
          pReplyDl->CmdType  =  SYSTEM_REPLY_ERROR;
          pReplyDl->Status   =  SIZE_ERROR;
          pReplyDl->Handle   =  -1;
          pTxBuf->BlockLen   =  SIZEOF_RPLYBEGINDL;
        }
      }
      else
      {
        // Error in getting a handle
        pReplyDl->CmdType  =  SYSTEM_REPLY_ERROR;
        pReplyDl->Status   =  NO_HANDLES_AVAILABLE;
        pReplyDl->Handle   =  -1;
        pTxBuf->BlockLen   =  SIZEOF_RPLYBEGINDL;

        #ifdef DEBUG
          printf("No more handles\r\n");
        #endif
      }
    }
    break;

    case CONTINUE_DOWNLOAD :
    {
      CONTINUE_DL       *pContiDl;
      RPLY_CONTINUE_DL  *pRplyContiDl;
      ULONG             BytesToWrite;

      //Setup pointers
      pContiDl       =  (CONTINUE_DL*)pComCmd;
      pRplyContiDl   =  (RPLY_CONTINUE_DL*)pTxBuf->Buf;

      // Get handle
      FileHandle     =  pContiDl->Handle;

      // Fill the reply
      pRplyContiDl->CmdSize  =  SIZEOF_RPLYCONTINUEDL - sizeof(CMDSIZE);
      pRplyContiDl->MsgCount =  pContiDl->MsgCount;
      pRplyContiDl->CmdType  =  SYSTEM_REPLY;
      pRplyContiDl->Cmd      =  CONTINUE_DOWNLOAD;
      pRplyContiDl->Status   =  SUCCESS;
      pRplyContiDl->Handle   =  FileHandle;

      if ((FileHandle >= 0) && (ComInstance.Files[FileHandle].Name[0]))
      {

        pRxBuf->FileHandle = FileHandle;
        pRxBuf->pFile      = &(ComInstance.Files[FileHandle]);

        if (pRxBuf->pFile->File >= 0)
        {

          pRxBuf->MsgLen   =  (pContiDl->CmdSize  + sizeof(CMDSIZE)) - SIZEOF_CONTINUEDL;
          pRxBuf->RxBytes  =  0;

          if (pContiDl->CmdSize <= (pRxBuf->BufSize - sizeof(CMDSIZE)))
          {
            // All data included in this buffer
            BytesToWrite   =  pRxBuf->MsgLen;
            pRxBuf->State  =  RXIDLE;
          }
          else
          {
            // Rx buffer must be read more that one to receive the complete message
            BytesToWrite   =  pRxBuf->BufSize - SIZEOF_CONTINUEDL;
            pRxBuf->State  =  RXFILEDL;
          }
          pTxBuf->BlockLen = SIZEOF_RPLYCONTINUEDL;

          if ((pRxBuf->pFile->Pointer + pRxBuf->MsgLen) > pRxBuf->pFile->Size)
          {
            BytesToWrite  =  pRxBuf->pFile->Size - pRxBuf->pFile->Pointer;

            #ifdef DEBUG
              printf("File size limited\r\n");
            #endif
          }

          write(ComInstance.Files[FileHandle].File, (pContiDl->PayLoad), (size_t)BytesToWrite);
          pRxBuf->pFile->Pointer  +=  BytesToWrite;
          pRxBuf->RxBytes          =  BytesToWrite;

          #ifdef DEBUG
            printf("Size %8lu - Loaded %8lu\r\n",(unsigned long)ComInstance.Files[FileHandle].Size,(unsigned long)ComInstance.Files[FileHandle].Length);
          #endif

          if (pRxBuf->pFile->Pointer >= ComInstance.Files[FileHandle].Size)
          {
            #ifdef DEBUG
              printf("%s %lu bytes downloaded\r\n",ComInstance.Files[FileHandle].Name,(unsigned long)ComInstance.Files[FileHandle].Length);
            #endif

            cComCloseFileHandle(&(ComInstance.Files[FileHandle].File));
            chmod(ComInstance.Files[FileHandle].Name,S_IRWXU | S_IRWXG | S_IRWXO);
            cComFreeHandle(FileHandle);
            pRplyContiDl->Status  =  END_OF_FILE;
          }
        }
        else
        {
          // Illegal file
          pRplyContiDl->CmdType  =  SYSTEM_REPLY_ERROR;
          pRplyContiDl->Status   =  UNKNOWN_ERROR;
          pRplyContiDl->Handle   =  -1;
          pTxBuf->BlockLen       =  SIZEOF_RPLYCONTINUEDL;
          cComFreeHandle(FileHandle);

          #ifdef DEBUG
            printf("Data not appended\r\n");
          #endif
        }
      }
      else
      {
        // Illegal file handle
        pRplyContiDl->CmdType  =  SYSTEM_REPLY_ERROR;
        pRplyContiDl->Status   =  UNKNOWN_HANDLE;
        pRplyContiDl->Handle   =  -1;
        pTxBuf->BlockLen       =  SIZEOF_RPLYCONTINUEDL;

        #ifdef DEBUG
          printf("Invalid handle\r\n");
        #endif
      }
    }
    break;

    case BEGIN_UPLOAD:
    {
      ULONG       BytesToRead;
      ULONG       ReadBytes;

      BEGIN_READ      *pBeginRead;;
      RPLY_BEGIN_READ *pReplyBeginRead;

      //Setup pointers
      pBeginRead        =  (BEGIN_READ*)pRxBuf->Buf;
      pReplyBeginRead   =  (RPLY_BEGIN_READ*)pTxBuf->Buf;

      FileHandle    =  cComGetHandle((char*)pBeginRead->Path);

      pTxBuf->pFile      =  &ComInstance.Files[FileHandle];  // Insert the file pointer into the ch struct
      pTxBuf->FileHandle =  FileHandle;                      // Also save the File handle number

      pReplyBeginRead->CmdSize  =  SIZEOF_RPLYBEGINREAD - sizeof(CMDSIZE);
      pReplyBeginRead->MsgCount =  pBeginRead->MsgCount;
      pReplyBeginRead->CmdType  =  SYSTEM_REPLY;
      pReplyBeginRead->Cmd      =  BEGIN_UPLOAD;
      pReplyBeginRead->Status   =  SUCCESS;
      pReplyBeginRead->Handle   =  FileHandle;

      if (FileHandle >= 0)
      {
        BytesToRead   = (ULONG)(pBeginRead->BytesToReadLsb);
        BytesToRead  += (ULONG)(pBeginRead->BytesToReadMsb) << 8;

        pTxBuf->pFile->File  =  open(pTxBuf->pFile->Name,O_RDONLY,0x444);

        if (pTxBuf->pFile->File >= 0)
        {
          // Get file length
          pTxBuf->pFile->Size = lseek(pTxBuf->pFile->File, 0L, SEEK_END);
          lseek(pTxBuf->pFile->File, 0L, SEEK_SET);

          pTxBuf->MsgLen  =  BytesToRead;

          if (BytesToRead > pTxBuf->pFile->Size)
          {
            // if message length  > filesize then crop message
            pTxBuf->MsgLen  =  pTxBuf->pFile->Size;
          }

          if ((pTxBuf->MsgLen + SIZEOF_RPLYBEGINREAD) <= pTxBuf->BufSize)
          {
            // Read all requested bytes as they can fit into the buffer
            ReadBytes        =  read(pTxBuf->pFile->File, pReplyBeginRead->PayLoad, pTxBuf->MsgLen);
            pTxBuf->BlockLen =  pTxBuf->MsgLen + SIZEOF_RPLYBEGINREAD;
            pTxBuf->State    =  TXIDLE;
          }
          else
          {
            // Read only up to full buffer size
            ReadBytes        = read(pTxBuf->pFile->File, pReplyBeginRead->PayLoad, (pTxBuf->BufSize - SIZEOF_RPLYBEGINREAD));
            pTxBuf->BlockLen =  pTxBuf->BufSize - SIZEOF_RPLYBEGINREAD;
            pTxBuf->State    = TXFILEUPLOAD;
          }

          pTxBuf->SendBytes      = ReadBytes;  // No of bytes send in message
          pTxBuf->pFile->Pointer = ReadBytes;  // No of bytes read from file

          #ifdef DEBUG
            printf("Bytes to read %d bytes read %d\r\n",BytesToRead, ReadBytes);
            printf("FileSize: %d \r\n",pTxBuf->pFile->Size);
          #endif

          pReplyBeginRead->CmdSize      +=   (CMDSIZE)pTxBuf->MsgLen;
          pReplyBeginRead->FileSizeLsb   =   (UBYTE)(pTxBuf->pFile->Size);
          pReplyBeginRead->FileSizeNsb1  =   (UBYTE)(pTxBuf->pFile->Size >>  8);
          pReplyBeginRead->FileSizeNsb2  =   (UBYTE)(pTxBuf->pFile->Size >> 16);
          pReplyBeginRead->FileSizeMsb   =   (UBYTE)(pTxBuf->pFile->Size >> 24);

          if (pTxBuf->pFile->Pointer >= pTxBuf->pFile->Size)
          {
            #ifdef DEBUG
              printf("%s %lu bytes UpLoaded\r\n",pTxBuf->pFile->Name,(unsigned long)pTxBuf->pFile->Length);
            #endif

            pReplyBeginRead->Status  =  END_OF_FILE;
            cComCloseFileHandle(&(pTxBuf->pFile->File));
            cComFreeHandle(FileHandle);
          }

          #ifdef DEBUG
            cComPrintTxMsg(pTxBuf);
          #endif

        }
        else
        {
          pReplyBeginRead->CmdType  =  SYSTEM_REPLY_ERROR;
          pReplyBeginRead->Status   =  UNKNOWN_HANDLE;
          pReplyBeginRead->Handle   =  -1;
          pTxBuf->BlockLen          =  SIZEOF_RPLYBEGINREAD;
          cComFreeHandle(FileHandle);

          #ifdef DEBUG
            printf("File %s is not present \r\n",ComInstance.Files[FileHandle].Name);
          #endif
        }
      }
      else
      {
        pReplyBeginRead->CmdType  =  SYSTEM_REPLY_ERROR;
        pReplyBeginRead->Status   =  NO_HANDLES_AVAILABLE;
        pReplyBeginRead->Handle   =  -1;
        pTxBuf->BlockLen          =  SIZEOF_RPLYBEGINREAD;

        #ifdef DEBUG
          printf("No more handles\r\n");
        #endif
      }

    }
    break;

    case CONTINUE_UPLOAD:
    {
      ULONG       BytesToRead;
      ULONG       ReadBytes;

      CONTINUE_READ      *pContinueRead;
      RPLY_CONTINUE_READ *pReplyContinueRead;

      //Setup pointers
      pContinueRead        =  (CONTINUE_READ*)pRxBuf->Buf;
      pReplyContinueRead   =  (RPLY_CONTINUE_READ*)pTxBuf->Buf;

      FileHandle  =  pContinueRead->Handle;

       /* Fill out the default settings */
      pReplyContinueRead->CmdSize  =  SIZEOF_RPLYCONTINUEREAD - sizeof(CMDSIZE);
      pReplyContinueRead->MsgCount =  pContinueRead->MsgCount;
      pReplyContinueRead->CmdType  =  SYSTEM_REPLY;
      pReplyContinueRead->Cmd      =  CONTINUE_UPLOAD;
      pReplyContinueRead->Status   =  SUCCESS;
      pReplyContinueRead->Handle   =  FileHandle;

      if ((FileHandle >= 0) && (pTxBuf->pFile->Name[0]))
      {

        if (pTxBuf->pFile->File >= 0)
        {
          BytesToRead     =  (ULONG)(pContinueRead->BytesToReadLsb);
          BytesToRead    +=  (ULONG)(pContinueRead->BytesToReadMsb) << 8;

          // If host is asking for more bytes than remaining file size then
          // message length needs to adjusted accordingly
          if ((pTxBuf->pFile->Size - pTxBuf->pFile->Pointer) > BytesToRead)
          {
            pTxBuf->MsgLen  =  BytesToRead;
          }
          else
          {
            pTxBuf->MsgLen  =  (pTxBuf->pFile->Size - pTxBuf->pFile->Pointer);
            BytesToRead     =  pTxBuf->MsgLen;
          }

          if ((BytesToRead + SIZEOF_RPLYCONTINUEREAD) <= pTxBuf->BufSize)
          {
            // Read all requested bytes as they can fit into the buffer
            ReadBytes         =  read(pTxBuf->pFile->File,pReplyContinueRead->PayLoad,(size_t)BytesToRead);
            pTxBuf->BlockLen  =  BytesToRead + SIZEOF_RPLYCONTINUEREAD;
            pTxBuf->State     =  TXIDLE;
          }
          else
          {
            ReadBytes         =  read(pTxBuf->pFile->File,pReplyContinueRead->PayLoad,(size_t)(pTxBuf->BufSize - SIZEOF_RPLYCONTINUEREAD));
            pTxBuf->BlockLen  =  pTxBuf->BufSize - SIZEOF_RPLYCONTINUEREAD;
            pTxBuf->State     =  TXFILEUPLOAD;
          }

          pTxBuf->SendBytes             = (ULONG)ReadBytes;
          pTxBuf->pFile->Pointer       += (ULONG)ReadBytes;
          pReplyContinueRead->CmdSize  += pTxBuf->MsgLen;

          #ifdef DEBUG
            cComPrintTxMsg(pTxBuf);
          #endif

          #ifdef DEBUG
            printf("Size %8lu - Loaded %8lu\r\n",(unsigned long)pTxBuf->pFile->Size,(unsigned long)pTxBuf->pFile->Length);
          #endif

          if (ComInstance.Files[FileHandle].Pointer >= ComInstance.Files[FileHandle].Size)
          {
            #ifdef DEBUG
              printf("%s %lu bytes UpLoaded\r\n",ComInstance.Files[FileHandle].Name,(unsigned long)pTxBuf->pFile->Length);
            #endif

            pReplyContinueRead->Status = END_OF_FILE;
            cComCloseFileHandle(&(pTxBuf->pFile->File));
            cComFreeHandle(FileHandle);
          }
        }
        else
        {
          pReplyContinueRead->CmdType  =  SYSTEM_REPLY_ERROR;
          pReplyContinueRead->Status   =  HANDLE_NOT_READY;
          pReplyContinueRead->Handle   =  -1;
          pTxBuf->BlockLen             =  SIZEOF_RPLYCONTINUEREAD;
          cComFreeHandle(FileHandle);
          #ifdef DEBUG
            printf("Data not read\r\n");
          #endif
        }
      }
      else
      {
        pReplyContinueRead->CmdType  =  SYSTEM_REPLY_ERROR;
        pReplyContinueRead->Status   =  UNKNOWN_HANDLE;
        pReplyContinueRead->Handle   =  -1;
        pTxBuf->BlockLen             =  SIZEOF_RPLYCONTINUEREAD;
        #ifdef DEBUG
          printf("Invalid handle\r\n");
        #endif
      }
    }
    break;

    case BEGIN_GETFILE:
    {

      ULONG                 BytesToRead;
      ULONG                 ReadBytes;
      BEGIN_GET_FILE        *pBeginGetFile;;
      RPLY_BEGIN_GET_FILE   *pReplyBeginGetFile;

      //Setup pointers
      pBeginGetFile        =  (BEGIN_GET_FILE*)pRxBuf->Buf;
      pReplyBeginGetFile   =  (RPLY_BEGIN_GET_FILE*)pTxBuf->Buf;

      FileHandle           =  cComGetHandle((char*)pBeginGetFile->Path);
      pTxBuf->pFile        =  &ComInstance.Files[FileHandle];  // Insert the file pointer into the ch struct
      pTxBuf->FileHandle   =  FileHandle;                      // Also save the File handle number

      // Fill out the reply
      pReplyBeginGetFile->CmdSize  =  SIZEOF_RPLYBEGINGETFILE - sizeof(CMDSIZE);
      pReplyBeginGetFile->MsgCount =  pBeginGetFile->MsgCount;
      pReplyBeginGetFile->CmdType  =  SYSTEM_REPLY;
      pReplyBeginGetFile->Cmd      =  BEGIN_GETFILE;
      pReplyBeginGetFile->Handle   =  FileHandle;
      pReplyBeginGetFile->Status   =  SUCCESS;

      if (FileHandle >= 0)
      {
        /* How many bytes to be returned the in the reply for BEGIN_UPLOAD    */
        /* Should actually only be 2 bytes as only we can hold into 2 length  */
        /* bytes in the protocol                                              */
        BytesToRead    =  (ULONG)(pBeginGetFile->BytesToReadLsb);
        BytesToRead   +=  (ULONG)(pBeginGetFile->BytesToReadMsb) << 8;

        #ifdef DEBUG
          printf("File to get:  %s \r\n",ComInstance.Files[FileHandle].Name);
        #endif

        pTxBuf->pFile->File  =  open(pTxBuf->pFile->Name,O_RDONLY,0x444);
        if (pTxBuf->pFile->File >= 0)
        {
          // Get file length
          pTxBuf->pFile->Size = lseek(pTxBuf->pFile->File, 0L, SEEK_END);
          lseek(pTxBuf->pFile->File, 0L, SEEK_SET);

          pTxBuf->MsgLen  =  BytesToRead;
          if (BytesToRead > pTxBuf->pFile->Size)
          {
            // if message length  > filesize then crop message
            pTxBuf->MsgLen  =  pTxBuf->pFile->Size;
          }

          if ((pTxBuf->MsgLen + SIZEOF_RPLYBEGINGETFILE) <= pTxBuf->BufSize)
          {
            // Read all requested bytes as they can fit into the buffer
            ReadBytes         =  read(pTxBuf->pFile->File, pReplyBeginGetFile->PayLoad, pTxBuf->MsgLen);
            pTxBuf->BlockLen  =  ReadBytes + SIZEOF_RPLYBEGINGETFILE;
            pTxBuf->State     =  TXIDLE;
          }
          else
          {
            // Read only up to full buffer size
            ReadBytes         =  read(pTxBuf->pFile->File, pReplyBeginGetFile->PayLoad, (pTxBuf->BufSize - SIZEOF_RPLYBEGINGETFILE));
            pTxBuf->BlockLen  =  pTxBuf->BufSize - SIZEOF_RPLYBEGINGETFILE;
            pTxBuf->State     =  TXGETFILE;
          }

          pTxBuf->SendBytes      = ReadBytes;  // No of bytes send in message
          pTxBuf->pFile->Pointer = ReadBytes;  // No of bytes read from file

          #ifdef DEBUG
            printf("Bytes to read %d vs. bytes read %d\r\n",BytesToRead, ReadBytes);
            printf("FileSize: %d \r\n",pTxBuf->pFile->Size);
          #endif

          pReplyBeginGetFile->CmdSize       +=  pTxBuf->MsgLen;
          pReplyBeginGetFile->FileSizeLsb    =  (UBYTE)pTxBuf->pFile->Size;
          pReplyBeginGetFile->FileSizeNsb1   =  (UBYTE)(pTxBuf->pFile->Size >>  8);
          pReplyBeginGetFile->FileSizeNsb2   =  (UBYTE)(pTxBuf->pFile->Size >> 16);
          pReplyBeginGetFile->FileSizeMsb    =  (UBYTE)(pTxBuf->pFile->Size >> 24);

          if (pTxBuf->pFile->Pointer >= pTxBuf->pFile->Size)
          {
            #ifdef DEBUG
              printf("%s %lu bytes UpLoaded\r\n",pTxBuf->pFile->Name,(unsigned long)pTxBuf->pFile->Length);
            #endif

            // If last bytes has been returned and file is not open for writing
            if (FAIL == cMemoryCheckOpenWrite(pTxBuf->pFile->Name))
            {
              pReplyBeginGetFile->Status   =  END_OF_FILE;
              cComCloseFileHandle(&(pTxBuf->pFile->File));
              cComFreeHandle(FileHandle);
            }
          }

          #ifdef DEBUG
            cComPrintTxMsg(pTxBuf);
          #endif
        }
        else
        {
          pReplyBeginGetFile->CmdType  =  SYSTEM_REPLY_ERROR;
          pReplyBeginGetFile->Status   =  HANDLE_NOT_READY;
          pReplyBeginGetFile->Handle   =  -1;
          pTxBuf->BlockLen             =  SIZEOF_RPLYBEGINGETFILE;
          cComFreeHandle(FileHandle);

          #ifdef DEBUG
            printf("File %s is not present \r\n",ComInstance.Files[FileHandle].Name);
          #endif
        }
      }
      else
      {
        pReplyBeginGetFile->CmdType  =  SYSTEM_REPLY_ERROR;
        pReplyBeginGetFile->Status   =  UNKNOWN_HANDLE;
        pReplyBeginGetFile->Handle   =  -1;
        pTxBuf->BlockLen             =  SIZEOF_RPLYBEGINGETFILE;

        #ifdef DEBUG
          printf("No more handles\r\n");
        #endif
      }
    }
    break;

    case CONTINUE_GETFILE:
    {
      ULONG                     BytesToRead;
      ULONG                     ReadBytes;
      CONTINUE_GET_FILE         *pContinueGetFile;
      RPLY_CONTINUE_GET_FILE    *pReplyContinueGetFile;

      //Setup pointers
      pContinueGetFile       =  (CONTINUE_GET_FILE*)pRxBuf->Buf;
      pReplyContinueGetFile  =  (RPLY_CONTINUE_GET_FILE*)pTxBuf->Buf;

      FileHandle  =  pContinueGetFile->Handle;

       /* Fill out the default settings */
      pReplyContinueGetFile->CmdSize  =  SIZEOF_RPLYCONTINUEGETFILE - sizeof(CMDSIZE);
      pReplyContinueGetFile->MsgCount =  pContinueGetFile->MsgCount;
      pReplyContinueGetFile->CmdType  =  SYSTEM_REPLY;
      pReplyContinueGetFile->Cmd      =  CONTINUE_GETFILE;
      pReplyContinueGetFile->Status   =  SUCCESS;
      pReplyContinueGetFile->Handle   =  FileHandle;

      if ((FileHandle >= 0) && (pTxBuf->pFile->Name[0]))
      {

        if (pTxBuf->pFile->File >= 0)
        {

          BytesToRead     =  (ULONG)(pContinueGetFile->BytesToReadLsb);
          BytesToRead    +=  (ULONG)(pContinueGetFile->BytesToReadMsb) << 8;

          // Get new file length: Set pointer to 0 -> find end -> set where to read from
          lseek(pTxBuf->pFile->File, 0L, SEEK_SET);
          pTxBuf->pFile->Size = lseek(pTxBuf->pFile->File, 0L, SEEK_END);
          lseek(pTxBuf->pFile->File, pTxBuf->pFile->Pointer, SEEK_SET);

          // If host is asking for more bytes than remaining file size then
          // message length needs to adjusted accordingly
          if ((pTxBuf->pFile->Size - pTxBuf->pFile->Pointer) > BytesToRead)
          {
            pTxBuf->MsgLen  =  BytesToRead;
          }
          else
          {
            pTxBuf->MsgLen  =  (pTxBuf->pFile->Size - pTxBuf->pFile->Pointer);
            BytesToRead     =  pTxBuf->MsgLen;
          }

          if ((BytesToRead + SIZEOF_RPLYCONTINUEGETFILE) <= pTxBuf->BufSize)
          {
            // Read all requested bytes as they can fit into the buffer
            ReadBytes        = read(pTxBuf->pFile->File,pReplyContinueGetFile->PayLoad,(size_t)BytesToRead);
            pTxBuf->BlockLen = ReadBytes + SIZEOF_RPLYCONTINUEGETFILE;
            pTxBuf->State    = TXIDLE;
          }
          else
          {
            ReadBytes        = read(pTxBuf->pFile->File,pReplyContinueGetFile->PayLoad,(size_t)(pTxBuf->BufSize - SIZEOF_RPLYCONTINUEGETFILE));
            pTxBuf->BlockLen = ReadBytes + SIZEOF_RPLYCONTINUEGETFILE;
            pTxBuf->State    = TXGETFILE;
          }

          pTxBuf->SendBytes        = (ULONG)ReadBytes;
          pTxBuf->pFile->Pointer  += (ULONG)ReadBytes;

          pReplyContinueGetFile->CmdSize      +=  pTxBuf->MsgLen;
          pReplyContinueGetFile->FileSizeLsb   =  (UBYTE)(pTxBuf->pFile->Size);
          pReplyContinueGetFile->FileSizeNsb1  =  (UBYTE)(pTxBuf->pFile->Size >>  8);
          pReplyContinueGetFile->FileSizeNsb2  =  (UBYTE)(pTxBuf->pFile->Size >> 16);
          pReplyContinueGetFile->FileSizeMsb   =  (UBYTE)(pTxBuf->pFile->Size >> 24);

          if (pTxBuf->pFile->Pointer >= pTxBuf->pFile->Size)
          {
            #ifdef DEBUG
              printf("%s %lu bytes UpLoaded\r\n",pTxBuf->pFile->Name,(unsigned long)pTxBuf->pFile->Length);
            #endif

            // If last bytes has been returned and file is not open for writing
            if (FAIL == cMemoryCheckOpenWrite(pTxBuf->pFile->Name))
            {
              pReplyContinueGetFile->Status   =  END_OF_FILE;
              cComCloseFileHandle(&(pTxBuf->pFile->File));
              cComFreeHandle(FileHandle);
            }
          }

          #ifdef DEBUG
            printf("Size %8lu - Loaded %8lu\r\n",(unsigned long)pTxBuf->pFile->Size,(unsigned long)pTxBuf->pFile->Length);
          #endif

          #ifdef DEBUG
            cComPrintTxMsg(pTxBuf);
          #endif

        }
        else
        {
          pReplyContinueGetFile->CmdType  =  SYSTEM_REPLY_ERROR;
          pReplyContinueGetFile->Status   =  HANDLE_NOT_READY;
          pReplyContinueGetFile->Handle   =  -1;
          pTxBuf->BlockLen                =  SIZEOF_RPLYCONTINUEGETFILE;
          cComFreeHandle(FileHandle);

          #ifdef DEBUG
            printf("Data not read\r\n");
          #endif
        }
      }
      else
      {
        pReplyContinueGetFile->CmdType  =  SYSTEM_REPLY_ERROR;
        pReplyContinueGetFile->Status   =  UNKNOWN_HANDLE;
        pReplyContinueGetFile->Handle   =  -1;
        pTxBuf->BlockLen                =  SIZEOF_RPLYCONTINUEGETFILE;

        #ifdef DEBUG
          printf("Invalid handle\r\n");
        #endif
      }
    }
    break;

    case LIST_FILES:
    {
      ULONG           BytesToRead;
      ULONG           Len;
      char            TmpFileName[MD5LEN + 1 + SIZEOFFILESIZE + 1+ FILENAMESIZE];
      BEGIN_LIST      *pBeginList;
      RPLY_BEGIN_LIST *pReplyBeginList;

      //Setup pointers
      pBeginList        =  (BEGIN_LIST*)pRxBuf->Buf;
      pReplyBeginList   =  (RPLY_BEGIN_LIST*)pTxBuf->Buf;

      FileHandle          =  cComGetHandle("ListFileHandle");
      pTxBuf->pFile       =  &ComInstance.Files[FileHandle];   // Insert the file pointer into the ch struct
      pTxBuf->FileHandle  =  FileHandle;                       // Also save the File handle number

      pReplyBeginList->CmdSize  =  SIZEOF_RPLYBEGINLIST - sizeof(CMDSIZE);
      pReplyBeginList->MsgCount =  pBeginList->MsgCount;
      pReplyBeginList->CmdType  =  SYSTEM_REPLY;
      pReplyBeginList->Cmd      =  LIST_FILES;
      pReplyBeginList->Status   =  SUCCESS;
      pReplyBeginList->Handle   =  FileHandle;
      pTxBuf->MsgLen            =  0;

      if (0 <= FileHandle)
      {
        BytesToRead   = (ULONG)pBeginList->BytesToReadLsb;
        BytesToRead  += (ULONG)pBeginList->BytesToReadMsb << 8;

        snprintf((char*)pTxBuf->Folder, FILENAMESIZE, "%s",(char*)pBeginList->Path);
        pTxBuf->pFile->File  =  scandir((char*)pTxBuf->Folder, &(pTxBuf->pFile->namelist), 0, NULL);

        if (pTxBuf->pFile->File <= 0)
        {
          if (pTxBuf->pFile->File == 0)
          {
            // here if no files found, equal to error as "./" and "../" normally
            // always found
            pReplyBeginList->ListSizeLsb   =  0;
            pReplyBeginList->ListSizeNsb1  =  0;
            pReplyBeginList->ListSizeNsb2  =  0;
            pReplyBeginList->ListSizeMsb   =  0;
            pReplyBeginList->Handle        =  -1;

            pTxBuf->BlockLen               =  SIZEOF_RPLYBEGINLIST;
            cComFreeHandle(FileHandle);
          }
          else
          {
            // If error occurs
            pReplyBeginList->CmdType       =  SYSTEM_REPLY_ERROR;
            pReplyBeginList->Status        =  UNKNOWN_ERROR;
            pReplyBeginList->ListSizeLsb   =  0;
            pReplyBeginList->ListSizeNsb1  =  0;
            pReplyBeginList->ListSizeNsb2  =  0;
            pReplyBeginList->ListSizeMsb   =  0;
            pReplyBeginList->Handle        =  -1;

            pTxBuf->BlockLen               =  SIZEOF_RPLYBEGINLIST;
            cComFreeHandle(FileHandle);
          }
        }
        else
        {
          SLONG   TmpN;
          ULONG   NameLen;
          ULONG   BytesToSend;
          UBYTE   Repeat;
          UBYTE   *pDstAdr;
          UBYTE   *pSrcAdr;

          TmpN = pTxBuf->pFile->File;     // Make a copy of number of entries
          Len = 0;

          // Start by calculating the length of the entire list
          while(TmpN--)
          {
            if ((DT_REG == pTxBuf->pFile->namelist[TmpN]->d_type) || (DT_DIR == pTxBuf->pFile->namelist[TmpN]->d_type) || (DT_LNK == pTxBuf->pFile->namelist[TmpN]->d_type))
            {
              Len += strlen(pTxBuf->pFile->namelist[TmpN]->d_name);
              if (DT_REG != pTxBuf->pFile->namelist[TmpN]->d_type)
              {
                // Make room room for ending "/"
                Len++;
              }
              else
              {
                // Make room for md5sum + space + Filelength (fixed to 8 chars) + space
                Len += MD5LEN + 1 + SIZEOFFILESIZE + 1;
              }

              // Add one new line character per file/folder/link -name
              Len++;
            }
          }

          pTxBuf->pFile->Size = Len;

          // Total list length has been calculated
          pReplyBeginList->ListSizeLsb   =  (UBYTE) Len;
          pReplyBeginList->ListSizeNsb1  =  (UBYTE)(Len >>  8);
          pReplyBeginList->ListSizeNsb2  =  (UBYTE)(Len >> 16);
          pReplyBeginList->ListSizeMsb   =  (UBYTE)(Len >> 24);

          pTxBuf->MsgLen          =  BytesToRead;
          pTxBuf->SendBytes       =  0;
          pTxBuf->pFile->Pointer  =  0;
          pTxBuf->pFile->Length   =  0;

          if (BytesToRead > Len)
          {
            // if message length  > file size then crop message
            pTxBuf->MsgLen            =  Len;
            pReplyBeginList->Status   =  END_OF_FILE; // Complete file in first message
          }

          TmpN = pTxBuf->pFile->File ;          // Make a copy of number of entries

          if ((pTxBuf->MsgLen + SIZEOF_RPLYBEGINLIST) <= pTxBuf->BufSize)
          {
            //All requested bytes can be inside the buffer
            BytesToSend      =  pTxBuf->MsgLen;
            pTxBuf->BlockLen =  pTxBuf->MsgLen + SIZEOF_RPLYBEGINLIST;
            pTxBuf->State    =  TXIDLE;
          }
          else
          {
            BytesToSend       =  (pTxBuf->BufSize - SIZEOF_RPLYBEGINLIST);
            pTxBuf->BlockLen  =  pTxBuf->BufSize;
            pTxBuf->State     =  TXLISTFILES;
          }

          Repeat  =  1;
          Len     =  0;
          pDstAdr =  pReplyBeginList->PayLoad;
          while(Repeat)
          {
            TmpN--;

            cComGetNameFromScandirList((pTxBuf->pFile->namelist[TmpN]), (char *)TmpFileName, &NameLen, (UBYTE *)pTxBuf->Folder);

            if (0 != NameLen)
            {
              if ((NameLen + Len) <=  BytesToSend)                         // Does the next name fit into the buffer?
              {

                pSrcAdr     =  (UBYTE*)(TmpFileName);
                while (*pSrcAdr) *pDstAdr++ = *pSrcAdr++;
                Len        +=  NameLen;

                free(pTxBuf->pFile->namelist[TmpN]);

                if (BytesToSend == Len)
                {
                  // buffer is filled up now exit
                  pTxBuf->pFile->Length    =  0;
                  pTxBuf->pFile->File      =  TmpN;
                  Repeat                   =  0;
                }
              }
              else
              {
                // No - Now fill up to exact size
                pTxBuf->pFile->Length    =  (BytesToSend - Len);
                memcpy((char*)pDstAdr, TmpFileName, pTxBuf->pFile->Length);
                Len                     +=  pTxBuf->pFile->Length;
                pTxBuf->pFile->File      =  TmpN;                         // Adjust Iteration number
                Repeat                   =  0;
              }
            }
          }

          // Update pointers
          pTxBuf->SendBytes        =  Len;
          pTxBuf->pFile->Pointer   =  Len;

          if (pTxBuf->pFile->Pointer >= pTxBuf->pFile->Size)
          {
            #ifdef DEBUG
              printf("Complete list of %lu Bytes uploaded \r\n",(unsigned long)pTxBuf->pFile->Length);
            #endif

            pReplyBeginList->Status   =  END_OF_FILE;
            cComFreeHandle(FileHandle);
          }
        }
      }
      else
      {
        // No more handle or illegal file name
        pReplyBeginList->CmdType       =  SYSTEM_REPLY_ERROR;
        pReplyBeginList->Status        =  UNKNOWN_ERROR;
        pReplyBeginList->ListSizeLsb   =  0;
        pReplyBeginList->ListSizeNsb1  =  0;
        pReplyBeginList->ListSizeNsb2  =  0;
        pReplyBeginList->ListSizeMsb   =  0;
        pReplyBeginList->Handle        =  -1;

        pTxBuf->BlockLen               =  SIZEOF_RPLYBEGINLIST;
      }
      pReplyBeginList->CmdSize +=  pTxBuf->MsgLen;

      #ifdef DEBUG
        cComPrintTxMsg(pTxBuf);
      #endif

    }
    break;

    case CONTINUE_LIST_FILES:
    {
      SLONG           TmpN;
      ULONG           BytesToRead;
      ULONG           Len;
      ULONG           BytesToSend;
      ULONG           NameLen;
      ULONG           RemCharCnt;
      UBYTE           Repeat;
      char            TmpFileName[FILENAMESIZE];

      CONTINUE_LIST       *pContinueList;
      RPLY_CONTINUE_LIST  *pReplyContinueList;

      //Setup pointers
      pContinueList         =  (CONTINUE_LIST*)pRxBuf->Buf;
      pReplyContinueList    =  (RPLY_CONTINUE_LIST*)pTxBuf->Buf;

      pReplyContinueList->CmdSize  =  SIZEOF_RPLYCONTINUELIST - sizeof(CMDSIZE);
      pReplyContinueList->MsgCount =  pContinueList->MsgCount;
      pReplyContinueList->CmdType  =  SYSTEM_REPLY;
      pReplyContinueList->Cmd      =  CONTINUE_LIST_FILES;
      pReplyContinueList->Handle   =  pContinueList->Handle;
      pReplyContinueList->Status   =  SUCCESS;

      BytesToRead   =  (ULONG)pContinueList->BytesToReadLsb;
      BytesToRead  +=  (ULONG)pContinueList->BytesToReadMsb << 8;

      if (((pTxBuf->pFile->File == 0) && (0 > pTxBuf->pFile->Length)) || (pTxBuf->pFile->File < 0))
      {
        // here if nothing is to be returned
        pReplyContinueList->CmdType    =  SYSTEM_REPLY_ERROR;
        pReplyContinueList->Status     =  UNKNOWN_ERROR;
        pReplyContinueList->Handle     =  -1;

        pTxBuf->MsgLen                 =  0;
        pTxBuf->BlockLen               =  SIZEOF_RPLYCONTINUELIST;
        cComFreeHandle(FileHandle);
      }
      else
      {
        TmpN = pTxBuf->pFile->File;       // Make a copy of number of entries

        pTxBuf->MsgLen    = BytesToRead;
        pTxBuf->SendBytes = 0;

        if (BytesToRead  >=  (pTxBuf->pFile->Size - pTxBuf->pFile->Pointer))
        {
          // if message length  > file size then crop message
          pTxBuf->MsgLen              =  (pTxBuf->pFile->Size - pTxBuf->pFile->Pointer);
          pReplyContinueList->Status  =  END_OF_FILE;           // Remaining file included in this message
        }

        if ((pTxBuf->MsgLen + SIZEOF_RPLYCONTINUELIST) <= pTxBuf->BufSize)
        {
          BytesToSend       =  pTxBuf->MsgLen;
          pTxBuf->BlockLen  =  pTxBuf->MsgLen + SIZEOF_RPLYCONTINUELIST;
          pTxBuf->State     =  TXIDLE;
        }
        else
        {
          BytesToSend       =  pTxBuf->BufSize - SIZEOF_RPLYCONTINUELIST;
          pTxBuf->BlockLen  =  pTxBuf->BufSize;
          pTxBuf->State     =  TXLISTFILES;
        }

        Len    =  0;
        Repeat =  1;
        if (pTxBuf->pFile->Length)
        {

          // Only here if filename has been divided in 2 pieces
          cComGetNameFromScandirList((pTxBuf->pFile->namelist[TmpN]), (char *)TmpFileName, &NameLen, (UBYTE *)pTxBuf->Folder);

          if (0 != NameLen)
          {
            // First transfer the remaining part of the last filename
            RemCharCnt  =  NameLen - pTxBuf->pFile->Length;

            if (RemCharCnt <= BytesToSend)
            {
              // this will fit into the message length
              memcpy((char*)(&(pReplyContinueList->PayLoad[Len])), &(TmpFileName[pTxBuf->pFile->Length]), RemCharCnt);
              Len += RemCharCnt;

              free(pTxBuf->pFile->namelist[TmpN]);

              if (RemCharCnt == BytesToSend)
              {
                //if If all bytes is already occupied not more to go
                //for this message
                Repeat                 =  0;
                pTxBuf->pFile->Length  =  0;
              }
              else
              {
                Repeat  =  1;
              }
            }
            else
            {
              // This is the rare condition if remaining msg len and buf size are almost equal
              memcpy((char*)(&(pReplyContinueList->PayLoad[Len])), &(TmpFileName[pTxBuf->pFile->Length]), BytesToSend);
              Len += BytesToSend;

              pTxBuf->pFile->File    =  TmpN;                         // Adjust Iteration number
              pTxBuf->pFile->Length +=  Len;
              Repeat                 =  0;
            }
          }
        }
        if (TmpN)
        {
          while(Repeat)
          {
            TmpN--;

            cComGetNameFromScandirList((pTxBuf->pFile->namelist[TmpN]), (char *)TmpFileName, &NameLen, (UBYTE *)pTxBuf->Folder);

            if ((NameLen + Len) <=  BytesToSend)                         // Does the next name fit into the buffer?
            {
              memcpy((char*)(&(pReplyContinueList->PayLoad[Len])), TmpFileName, NameLen);
              Len  +=  NameLen;

              free(pTxBuf->pFile->namelist[TmpN]);

              if (BytesToSend == Len)
              {
                // buffer is filled up now exit
                pTxBuf->pFile->Length    =  0;
                pTxBuf->pFile->File      =  TmpN;
                Repeat                   =  0;
              }
            }
            else
            {
              // Now fill up to complete buffer size
              pTxBuf->pFile->Length    =  (BytesToSend - Len);
              memcpy((char*)(&(pReplyContinueList->PayLoad[Len])), TmpFileName, pTxBuf->pFile->Length);
              Len                     +=  pTxBuf->pFile->Length;
              pTxBuf->pFile->File      =  TmpN;         // Adjust Iteration number
              Repeat = 0;
            }
          }
        }
      }

      // Update pointers
      pTxBuf->SendBytes        =  Len;
      pTxBuf->pFile->Pointer  +=  Len;

      if (pTxBuf->pFile->Pointer >= pTxBuf->pFile->Size)
      {
        #ifdef DEBUG
          printf("Complete list of %lu Bytes uploaded \r\n",(unsigned long)pTxBuf->pFile->Length);
        #endif

        pReplyContinueList->Status   =  END_OF_FILE;
        cComFreeHandle(pContinueList->Handle);
      }

      pReplyContinueList->CmdSize +=  pTxBuf->MsgLen;

      #ifdef DEBUG
        cComPrintTxMsg(pTxBuf);
      #endif
    }
    break;

    case CLOSE_FILEHANDLE:
    {
      CLOSE_HANDLE        *pCloseHandle;
      RPLY_CLOSE_HANDLE   *pReplyCloseHandle;

      //Setup pointers
      pCloseHandle        =  (CLOSE_HANDLE*)pRxBuf->Buf;
      pReplyCloseHandle   =  (RPLY_CLOSE_HANDLE*)pTxBuf->Buf;

      FileHandle  =  pCloseHandle->Handle;

      #ifdef DEBUG
        printf("FileHandle to close = %d, Linux Handle = %d\r\n",FileHandle, ComInstance.Files[FileHandle].File);
      #endif

      pReplyCloseHandle->CmdSize  =  SIZEOF_RPLYCLOSEHANDLE - sizeof(CMDSIZE);
      pReplyCloseHandle->MsgCount =  pCloseHandle->MsgCount;
      pReplyCloseHandle->CmdType  =  SYSTEM_REPLY;
      pReplyCloseHandle->Cmd      =  CLOSE_FILEHANDLE;
      pReplyCloseHandle->Handle   =  pCloseHandle->Handle;
      pReplyCloseHandle->Status   =  SUCCESS;

      if (TRUE == cComFreeHandle(FileHandle))
      {
        cComCloseFileHandle(&(ComInstance.Files[FileHandle].File));
      }
      else
      {
        pReplyCloseHandle->CmdType  =  SYSTEM_REPLY_ERROR;
        pReplyCloseHandle->Status   =  UNKNOWN_HANDLE;
      }
      pTxBuf->BlockLen = SIZEOF_RPLYCLOSEHANDLE;

      #ifdef DEBUG
        cComPrintTxMsg(pTxBuf);
      #endif

    }
    break;

    case CREATE_DIR:
    {
      MAKE_DIR        *pMakeDir;
      RPLY_MAKE_DIR   *pReplyMakeDir;
      char            Folder[sizeof(ComInstance.Files[FileHandle].Name)];

      //Setup pointers
      pMakeDir        =  (MAKE_DIR*)pRxBuf->Buf;
      pReplyMakeDir   =  (RPLY_MAKE_DIR*)pTxBuf->Buf;

      pReplyMakeDir->CmdSize  =  SIZEOF_RPLYMAKEDIR - sizeof(CMDSIZE);
      pReplyMakeDir->MsgCount =  pMakeDir->MsgCount;
      pReplyMakeDir->CmdType  =  SYSTEM_REPLY;
      pReplyMakeDir->Cmd      =  CREATE_DIR;
      pReplyMakeDir->Status   =  SUCCESS;

      snprintf(Folder,sizeof(Folder),"%s",(char*)(pMakeDir->Dir));

      if (0 == mkdir(Folder,S_IRWXU | S_IRWXG | S_IRWXO))
      {
        chmod(Folder,S_IRWXU | S_IRWXG | S_IRWXO);
        #ifdef DEBUG
          printf("Folder %s created\r\n",Folder);
        #endif
        SetUiUpdate();
      }
      else
      {
        pReplyMakeDir->CmdType =  SYSTEM_REPLY_ERROR;
        pReplyMakeDir->Status  =  NO_PERMISSION;

        #ifdef DEBUG
          printf("Folder %s not created (%s)\r\n",Folder,strerror(errno));
        #endif
      }
      pTxBuf->BlockLen = SIZEOF_RPLYMAKEDIR;
    }
    break;

    case DELETE_FILE:
    {
      REMOVE_FILE        *pRemove;
      RPLY_REMOVE_FILE   *pReplyRemove;
      char               Name[60];

      //Setup pointers
      pRemove        =  (REMOVE_FILE*)pRxBuf->Buf;
      pReplyRemove   =  (RPLY_REMOVE_FILE*)pTxBuf->Buf;

      pReplyRemove->CmdSize  =  SIZEOF_RPLYREMOVEFILE - sizeof(CMDSIZE);
      pReplyRemove->MsgCount =  pRemove->MsgCount;
      pReplyRemove->CmdType  =  SYSTEM_REPLY;
      pReplyRemove->Cmd      =  DELETE_FILE;
      pReplyRemove->Status   =  SUCCESS;

      snprintf(Name, 60,"%s", (char*)(pRemove->Name));

      #ifdef DEBUG
        printf("File to delete %s\r\n", Name);
      #endif

      if (0 == remove(Name))
      {
        SetUiUpdate();
      }
      else
      {
        pReplyRemove->CmdType  =  SYSTEM_REPLY_ERROR;
        pReplyRemove->Status   =  NO_PERMISSION;

        #ifdef DEBUG
          printf("Folder %s not deleted (%s)\r\n",Folder,strerror(errno));
        #endif
      }
      pTxBuf->BlockLen = SIZEOF_RPLYREMOVEFILE;
    }
    break;

    case LIST_OPEN_HANDLES:
    {
      UBYTE HCnt1, HCnt2;

      LIST_HANDLES       *pListHandles;
      RPLY_LIST_HANDLES  *pReplyListHandles;

      //Setup pointers
      pListHandles        =  (LIST_HANDLES*)pRxBuf->Buf;
      pReplyListHandles   =  (RPLY_LIST_HANDLES*)pTxBuf->Buf;

      pReplyListHandles->CmdSize  =  SIZEOF_RPLYLISTHANDLES - sizeof(CMDSIZE);
      pReplyListHandles->MsgCount =  pListHandles->MsgCount;
      pReplyListHandles->CmdType  =  SYSTEM_REPLY;
      pReplyListHandles->Cmd      =  LIST_OPEN_HANDLES;
      pReplyListHandles->Status   =  SUCCESS;

      for (HCnt1 = 0; HCnt1 < ((MAX_FILE_HANDLES/8) + 1); HCnt1++)
      {

        pReplyListHandles->PayLoad[HCnt1 + 2] = 0;

        for(HCnt2 = 0; HCnt2 < 8; HCnt2++)
        {

          if (0 != ComInstance.Files[HCnt2 * HCnt1].State)
          { // Filehandle is in use

            pReplyListHandles->PayLoad[HCnt1 + 2] |= (0x01 << HCnt2) ;
          }
        }
      }
      pReplyListHandles->CmdSize  +=  HCnt1;
      pTxBuf->BlockLen             =  SIZEOF_RPLYLISTHANDLES + HCnt1;
    }
    break;

    case WRITEMAILBOX:
    {
      UBYTE                 No;
      UWORD                 PayloadSize;
      WRITE_MAILBOX         *pWriteMailbox;
      WRITE_MAILBOX_PAYLOAD *pWriteMailboxPayload;

      pWriteMailbox   =  (WRITE_MAILBOX*)pRxBuf->Buf;

      if(1 == cComFindMailbox(&(pWriteMailbox->Name[0]), &No))
      {
        pWriteMailboxPayload  =  (WRITE_MAILBOX_PAYLOAD*)&(pWriteMailbox->Name[(pWriteMailbox->NameSize)]);
        PayloadSize           =  (UWORD)(pWriteMailboxPayload->SizeLsb);
        PayloadSize          +=  ((UWORD)(pWriteMailboxPayload->SizeMsb)) << 8;
        memcpy(ComInstance.MailBox[No].Content, pWriteMailboxPayload->Payload, PayloadSize);
        ComInstance.MailBox[No].DataSize  =  PayloadSize;
        ComInstance.MailBox[No].WriteCnt++;
      }
    }
    break;

    case BLUETOOTHPIN:
    {
      // Both MAC and Pin are zero terminated string type
      UBYTE                BtAddr[vmBTADRSIZE];
      UBYTE                Pin[vmBTPASSKEYSIZE];
      UBYTE                PinSize;
      BLUETOOTH_PIN       *pBtPin;
      RPLY_BLUETOOTH_PIN  *pReplyBtPin;

      pBtPin       =  (BLUETOOTH_PIN*)pRxBuf->Buf;
      pReplyBtPin  =  (RPLY_BLUETOOTH_PIN*)pTxBuf->Buf;

      snprintf((char*)BtAddr, pBtPin->MacSize, "%s", (char*)pBtPin->Mac);
      PinSize = pBtPin->PinSize;
      snprintf((char*)Pin, PinSize, "%s", (char*)pBtPin->Pin);

      // This command can for safety reasons only be handled by USB
      if (USBDEV == ComInstance.ActiveComCh)
      {
        cBtSetTrustedDev(BtAddr, Pin, PinSize);
        pReplyBtPin->Status    =  SUCCESS;
      }
      else
      {
        pReplyBtPin->Status    =  ILLEGAL_CONNECTION;
      }

      pReplyBtPin->CmdSize   =  0x00;
      pReplyBtPin->MsgCount  =  pBtPin->MsgCount;
      pReplyBtPin->CmdType   =  SYSTEM_REPLY;
      pReplyBtPin->Cmd       =  BLUETOOTHPIN;
      pReplyBtPin->MacSize   =  vmBTADRSIZE;

      cBtGetId(pReplyBtPin->Mac, vmBTADRSIZE);
      pReplyBtPin->PinSize = PinSize;
      memcpy(pReplyBtPin->Pin, pBtPin->Pin, PinSize);

      pReplyBtPin->CmdSize  =  (SIZEOF_RPLYBLUETOOTHPIN + pReplyBtPin->MacSize + pReplyBtPin->PinSize + sizeof(pReplyBtPin->MacSize) + sizeof(pReplyBtPin->PinSize)) - sizeof(CMDSIZE);
      pTxBuf->BlockLen      =  (pReplyBtPin->CmdSize) + sizeof(CMDSIZE);
    }
    break;

    case ENTERFWUPDATE:
    {
      ULONG  UpdateFile;
      UBYTE  Dummy;

      if (USBDEV == ComInstance.ActiveComCh)
      {
        UpdateFile  =  open(UPDATE_DEVICE_NAME,O_RDWR);

        if (UpdateFile >= 0)
        {
          write(UpdateFile,&Dummy,1);
          close(UpdateFile);
          system("reboot -d -f -i");
        }
      }
    }
    break;
    
   case SETBUNDLEID:
    {
      BUNDLE_ID        *pBundleId;
      RPLY_BUNDLE_ID   *pReplyBundleId;

      pBundleId       =  (BUNDLE_ID*)pRxBuf->Buf;
      pReplyBundleId  =  (RPLY_BUNDLE_ID*)pTxBuf->Buf;

      pReplyBundleId->CmdSize   =  0x05;
      pReplyBundleId->MsgCount  =  pBundleId->MsgCount;
      pReplyBundleId->Cmd       =  SETBUNDLEID;

      if (TRUE == cBtSetBundleId(pBundleId->BundleId))
      {
        // Success
        pReplyBundleId->CmdType   =  SYSTEM_REPLY;
        pReplyBundleId->Status    =  SUCCESS;
      }
      else
      {
        // Error
        pReplyBundleId->CmdType   =  SYSTEM_REPLY_ERROR;
        pReplyBundleId->Status    =  SIZE_ERROR;
      }
      pTxBuf->BlockLen = SIZEOF_RPLYBUNDLEID;
    }
    break;

    case SETBUNDLESEEDID:
    {
      BUNDLE_SEED_ID          *pBundleSeedId;
      RPLY_BUNDLE_SEED_ID     *pReplyBundleSeedId;

      pBundleSeedId       =  (BUNDLE_SEED_ID*)pRxBuf->Buf;
      pReplyBundleSeedId  =  (RPLY_BUNDLE_SEED_ID*)pTxBuf->Buf;

      pReplyBundleSeedId->CmdSize   =  0x05;
      pReplyBundleSeedId->MsgCount  =  pBundleSeedId->MsgCount;
      pReplyBundleSeedId->Cmd       =  SETBUNDLESEEDID;

      if (TRUE == cBtSetBundleSeedId(pBundleSeedId->BundleSeedId))
      {
        // Success
        pReplyBundleSeedId->CmdType   =  SYSTEM_REPLY;
        pReplyBundleSeedId->Status    =  SUCCESS;
      }
      else
      {
        // Error
        pReplyBundleSeedId->CmdType   =  SYSTEM_REPLY_ERROR;
        pReplyBundleSeedId->Status    =  SIZE_ERROR;
      }
      pTxBuf->BlockLen = SIZEOF_RPLYBUNDLESEEDID;
    }
    break;
  }
}


void      cComUpdate(void)
{
  COMCMD  *pComCmd;
  IMGHEAD *pImgHead;
  TXBUF   *pTxBuf;
  RXBUF   *pRxBuf;
  UBYTE   ChNo;
  UWORD   BytesRead;

  UBYTE		ThisMagicCookie;
  UBYTE		HelperByte;
  uint		Iterator;
  UBYTE		MotorBusySignal;
  UBYTE		MotorBusySignalPointer;

  ChNo    =  0;

  cDaisyControl();  // Keep the HOST part going

  for(ChNo = 0; ChNo < NO_OF_CHS; ChNo++)
  {

    pTxBuf    =  &(ComInstance.TxBuf[ChNo]);
    pRxBuf    =  &(ComInstance.RxBuf[ChNo]);
    BytesRead =  0;

    if (!pTxBuf->Writing)
    {
      if(NULL != ComInstance.ReadChannel[ChNo])
      {
    	  if(ComInstance.VmReady == 1)
    	  {
    	    BytesRead = ComInstance.ReadChannel[ChNo](pRxBuf->Buf, pRxBuf->BufSize);
    	  }
      }

      #undef DEBUG
      //#define DEBUG
      #ifdef DEBUG
        // start DEBUG
        if(ChNo == USBDEV)
        {
          printf("Writing NOT set @ USBDEV - BytesRead = %d\r\n", BytesRead);
        }
        // end DEBUG
	    #endif


      if(BytesRead)
      {
        // Temporary fix until full implementation of com channels is ready
        ComInstance.ActiveComCh = ChNo;

        #ifdef DEBUG
          //          cComShow(ComInstance.UsbCmdInRep);
        #endif

        if (RXIDLE == pRxBuf->State)
        {
          // Not file down-loading
          memset(pTxBuf->Buf,0,sizeof(pTxBuf->Buf));

          pComCmd  =  (COMCMD*)pRxBuf->Buf;

          if ((*pComCmd).CmdSize)
          {
            // message received
            switch ((*pComCmd).Cmd)
            {
                                                    // NEW MOTOR/DAISY
              case	DIR_CMD_NO_REPLY_WITH_BUSY:	{
                                                    #undef DEBUG
                                                    //#define DEBUG
                                                    #ifdef DEBUG
                                                      printf("Did we reach a *BUSY* DIRECT_COMMAND_NO_REPLY pComCmd.Size = %d\n\r", ((*pComCmd).CmdSize));
                                                    #endif

                                                    Iterator = ((*pComCmd).CmdSize) - 7; // Incl. LEN bytes

                                                    HelperByte = 0x00;

                                                    for(Iterator = (((*pComCmd).CmdSize) - 7); Iterator < (((*pComCmd).CmdSize) - 3); Iterator++ )
                                                    {
                                                      HelperByte |= ((*pComCmd).PayLoad[Iterator] & 0x03);
                                                    }

            	  	  	  	  	  	  	  	  	  	 cDaisySetOwnLayer(HelperByte);

            	  	  	  	  	  	  	  	  	  	 // Setup the Cookie stuff get one by one and store

            	  	  	  	  	  	  	  	  	  	 HelperByte = 0; // used as Index

            	  	  	  	  	  	  	  	  	  	 // New MotorSignal
            	  	  	  	  	  	  	  	  	  	 MotorBusySignal = 0;
            	  	  	  	  	  	  	  	  	  	 MotorBusySignalPointer = 1;

            	  	  	  	  	  	  	  	  	  	 for(Iterator = (((*pComCmd).CmdSize) - 7); Iterator < (((*pComCmd).CmdSize) - 3); Iterator++ )
            	  	  	  	  	  	  	             {
														                         #undef DEBUG
            	  	  	  	  	  	  	  	  	  		 //#define DEBUG
														                         #ifdef DEBUG
            	  	  	  	  	  	  	  	  	  		 	 printf("Iterator = %d\n\r", Iterator);
														                         #endif

            	  	  	  	  	  	  	  	  	  		 ThisMagicCookie = (*pComCmd).PayLoad[Iterator];

            	  	  	  	  	  	  	  	  	  		 // New MotorSignal
            	  	  	  	  	  	  	  	  	  		 if(ThisMagicCookie & 0x80)	// BUSY signalled
            	  	  	  	  	  	  	  	  	  		 {
            	  	  	  	  	  	  	  	  	  			 MotorBusySignal = (UBYTE)(MotorBusySignal | MotorBusySignalPointer);
            	  	  	  	  	  	  	  	  	  		 }

            	  	  	  	  	  	  	  	  	  		 // New MotorSignal
            	  	  	  	  	  	  	  	  	  		 MotorBusySignalPointer <<= 1;

            	  	  	  	  	  	  	  	  	  		 #undef DEBUG
            	  	  	  	  	  	  	  	  	  		 //#define DEBUG
														                         #ifdef DEBUG
            	  	  	  	  	  	  	  	  	  		 	 printf("ThisMagicCookie = %d\n\r", ThisMagicCookie);
														                         #endif

            	  	  	  	  	  	  	  	  	  		 cDaisySetBusyFlags(cDaisyGetOwnLayer(), HelperByte, ThisMagicCookie);
            	  	  	  	  	  	  	  	  	  		 HelperByte++;
            	  	  	  	  	  	  	             }

            	  	  	  	  	  	  	  	  	  	 ResetDelayCounter(MotorBusySignal);

            	  	  	  	  	  	  	  	  	  	 #undef DEBUG
            	  	  	  	  	  	  	  	  	  	 //#define DEBUG
            	  	  	  	  	  	  	  	  	  	 #ifdef DEBUG
            	  	  	  	  	  	  	  	  	  	   printf("cMotorSetBusyFlags(%X)\n\r", MotorBusySignal);
            	  	  	  	  	  	  	  	  	  	 #endif

            	  	  	  	  	  	  	  	  	  	 // New MotorSignal
            	  	  	  	  	  	  	  	  	  	 cMotorSetBusyFlags(MotorBusySignal);

            	  	  	  	  	  	  	  	  	  	 // Adjust length

            	  	  	  	  	  	  	             ((*pComCmd).CmdSize) -= 4;	// NO VM use of cookies (or sweets ;-))

            	  	  	  	  	  	  	             // Now as "normal" direct command with NO reply

            	  	  	  	  	  	  	             ComInstance.CommandReady  =  cComDirectCommand(pRxBuf->Buf,pTxBuf->Buf);
              	  	  	  	  	  	  	  	  	}
              	  	  	  	  	  	  	  	  	break;

              case DIRECT_COMMAND_REPLY :
              {
            	  // direct command

                #undef DEBUG
            	  //#define DEBUG
               	#ifdef DEBUG
           	      printf("Did we reach a DIRECT_COMMAND_REPLY\n\r");
   				      #endif

                if(0 == ComInstance.ReplyStatus)
                {
           	      // If ReplyStstus = 0 then no commands is currently being
                  // processed -> new command can be processed
                  ComInstance.ReplyStatus  |=  DIR_CMD_REPLY;
                  ComInstance.CommandReady  =  cComDirectCommand(pRxBuf->Buf,pTxBuf->Buf);
                  if (!ComInstance.CommandReady)
                  {
                    // some error
                    pTxBuf->Writing         =  1;
                    ComInstance.ReplyStatus =  0;
                  }
                }
                else
                {
                  // Else VM is currently processing direct commands
                  // send error command, only if a DIR_CMD_NOREPLY
                  // is being executed. Not if a DIR_CMD_NOREPLY is
                  // being executed as replies can collide.
                  if(DIR_CMD_NOREPLY & ComInstance.ReplyStatus)
                  {
                    COMCMD  *pComCmd;
                    COMRPL  *pComRpl;

                    pComCmd             =  (COMCMD*)pRxBuf->Buf;
                    pComRpl             =  (COMRPL*)pTxBuf->Buf;

                    (*pComRpl).CmdSize  =  3;
                    (*pComRpl).MsgCnt   =  (*pComCmd).MsgCnt;
                    (*pComRpl).Cmd      =  DIRECT_REPLY_ERROR;

                    pTxBuf->Writing  =  1;
                  }
                }
              }
              break;

              case DIRECT_COMMAND_NO_REPLY :
              {
                // direct command

                #undef DEBUG
                //#define DEBUG
            	  #ifdef DEBUG
            	    printf("Did we reach a DIRECT_COMMAND_NO_REPLY\n\r");
				        #endif

                //Do not reply even if error
            	  if(0 == ComInstance.ReplyStatus)
                {
                  // If ReplyStstus = 0 then no commands is currently being
                  // processed -> new command can be processed
                  ComInstance.ReplyStatus  |=  DIR_CMD_NOREPLY;
                  ComInstance.CommandReady  =  cComDirectCommand(pRxBuf->Buf,pTxBuf->Buf);
                }
              }
              break;

              case SYSTEM_COMMAND_REPLY :
              {
                if(0 == ComInstance.ReplyStatus)
                {
                  ComInstance.ReplyStatus  |=  SYS_CMD_REPLY;
                  cComSystemCommand(pRxBuf,pTxBuf);
                  if (RXFILEDL != pRxBuf->State)
                  {
                    // Only here if command has been completed
                    pTxBuf->Writing          =  1;
                    ComInstance.ReplyStatus  =  0;
                  }
                }
              }
              break;

              case SYSTEM_COMMAND_NO_REPLY :
              {
                if(0 == ComInstance.ReplyStatus)
                {
                  ComInstance.ReplyStatus  |= SYS_CMD_NOREPLY;
                  cComSystemCommand(pRxBuf,pTxBuf);
                  if (RXFILEDL != pRxBuf->State)
                  {
                    // Only here if command has been completed
                    ComInstance.ReplyStatus  =  0;
                  }
                }
              }
              break;

              case SYSTEM_REPLY:
              {
                cComSystemReply(pRxBuf, pTxBuf);
              }
              break;

              case SYSTEM_REPLY_ERROR:
              {
                #ifdef DEBUG
                  printf("\r\nsystem reply error\r\n");
                #endif
              }
              break;

              case DAISY_COMMAND_REPLY:
              {
                if(ChNo == USBDEV)
                {
                  #undef DEBUG
                	//#define DEBUG
					        #ifdef DEBUG
                		printf("Did we reach c_COM @ DAISY_COMMAND_REPLY?\n\r");
					        #endif

                  cDaisyCmd(pRxBuf, pTxBuf);

                }
                else
                {
                  // Some ERROR handling
                }
              }
              break;

              case DAISY_COMMAND_NO_REPLY:
              {

                #undef DEBUG
                //#define DEBUG
					      #ifdef DEBUG
                  printf("Did we reach c_COM @ DAISY_COMMAND_NO_REPLY?\n\r");
					      #endif

                // A Daisy command without any reply
                if(ChNo == USBDEV)
                {
                  // Do something
                	cDaisyCmd(pRxBuf, pTxBuf);
                }
                else
                {
                  // Some ERROR handling
                }
              }
              break;

              default :
              {
              }
              break;

            }
          }
          else
          { // poll received
            // send response

            pImgHead              =  (IMGHEAD*)ComInstance.Image;
            pComCmd               =  (COMCMD*)pTxBuf->Buf;

            (*pComCmd).CmdSize    =  (CMDSIZE)(*pImgHead).GlobalBytes + 1;
            (*pComCmd).Cmd        =  DIRECT_REPLY;
            memcpy((*pComCmd).PayLoad,ComInstance.Globals,(*pImgHead).GlobalBytes);

            pTxBuf->Writing = 1;
          }
        }
        else
        { // in the middle of a write file command
          ULONG RemBytes;
          ULONG BytesToWrite;

          RemBytes = pRxBuf->MsgLen - pRxBuf->RxBytes;

          if (RemBytes <= pRxBuf->BufSize)
          {
            // Remaining bytes to write
            BytesToWrite      =  RemBytes;

            // Send the reply if requested
            if (ComInstance.ReplyStatus  &  SYS_CMD_REPLY)
            {
              pTxBuf->Writing   =  1;
            }

            // Clear to receive next msg header
            pRxBuf->State            =  RXIDLE;
            ComInstance.ReplyStatus  =  0;
          }
          else
          {
            BytesToWrite  =  pRxBuf->BufSize;
          }

          write(pRxBuf->pFile->File, pRxBuf->Buf, (size_t)BytesToWrite);
          pRxBuf->pFile->Pointer  +=  (ULONG)BytesToWrite;
          pRxBuf->RxBytes         +=  (ULONG)BytesToWrite;

          if (pRxBuf->pFile->Pointer >= pRxBuf->pFile->Size)
          {
            cComCloseFileHandle(&(pRxBuf->pFile->File));
            chmod(pRxBuf->pFile->Name, S_IRWXU | S_IRWXG | S_IRWXO);
            cComFreeHandle(pRxBuf->FileHandle);
          }
        }
      }
    }
    cComTxUpdate(ChNo);

    // Time for USB unplug detect?
    UsbConUpdate++;
    if(UsbConUpdate >= USB_CABLE_DETECT_RATE)
    {
      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
        printf("ready to check\n\r");
      #endif
      UsbConUpdate = 0;
      cComUsbDeviceConnected = cComCheckUsbCable();
    }

  }

  BtUpdate();
  cWiFiControl();
}

DATA8 cComGetUsbStatus(void)
 {
   // Returns true if the USB port is connected
   return cComUsbDeviceConnected;
 }

void      cComTxUpdate(UBYTE ChNo)
{
  TXBUF   *pTxBuf;
  ULONG   ReadBytes;

  pTxBuf  = &(ComInstance.TxBuf[ChNo]);

  if((OK == cDaisyChained()) && (ChNo == USBDEV)) // We're part of a chain && USBDEV
    {
      // Do "special handling" - I.e. no conflict between pushed DaisyData and non-syncronized
      // returns from Commands (answers/errors).

      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
        printf("\n\r001\n\r");
      #endif

      if(GetDaisyPushCounter() == DAISY_PUSH_NOT_UNLOCKED)  // It's the very first
      {                                                     // (or one of the first ;-)) transmission(s)
        //#define DEBUG
        #undef DEBUG
        #ifdef DEBUG
          printf("Not unlocked 001\n\r");
        #endif

        if (pTxBuf->Writing)  // Anything Pending?
        {
          //#define DEBUG
          #undef DEBUG
          #ifdef DEBUG
            printf("Not unlocked 002\n\r");
          #endif

          if(NULL != ComInstance.WriteChannel[ChNo])  // Valid channel?
          {
            //#define DEBUG
            #undef DEBUG
            #ifdef DEBUG
              printf("Not unlocked 003\n\r");
            #endif

            if((ComInstance.WriteChannel[USBDEV](pTxBuf->Buf, pTxBuf->BlockLen)) != 0)
            {
              //#define DEBUG
              #undef DEBUG
              #ifdef DEBUG
                printf("Not (OR should be) unlocked 004\n\r");
              #endif

              pTxBuf->Writing = 0;

              ResetDaisyPushCounter();  // Ready for normal run

            }
          }
        }
      }
      else
      {
        // We're unlocked

        if(GetDaisyPushCounter() == 0)  // It's a NON DaisyChain time-slice
        {
          //#define DEBUG
          #undef DEBUG
          #ifdef DEBUG
            printf("Unlocked 001\n\r");
          #endif

          if (pTxBuf->Writing)
          {
            //#define DEBUG
            #undef DEBUG
            #ifdef DEBUG
              printf("Unlocked 002\n\r");
            #endif

            if(NULL != ComInstance.WriteChannel[ChNo])
            {
              //#define DEBUG
              #undef DEBUG
              #ifdef DEBUG
                printf("Unlocked 003\n\r");
              #endif

              if(ComInstance.WriteChannel[ChNo](pTxBuf->Buf, pTxBuf->BlockLen))
              {
                //#define DEBUG
                #undef DEBUG
                #ifdef DEBUG
                  printf("Unlocked 004\n\r");
                #endif

                pTxBuf->Writing = 0;
                ResetDaisyPushCounter();  // Done/or we'll wait - we can allow more Daisy stuff
              }

            }
          }
          else
          {
            //#define DEBUG
            #undef DEBUG
            #ifdef DEBUG
              printf("Unlocked 005\n\r");
            #endif

            ResetDaisyPushCounter();      // Skip this "master" slice - use time with more benefit ;-)
          }
        }
        else
        {
          // We have a prioritised Daisy transfer/time-slice
          // DaisyPushCounter == either 3, 2 or 1

          if(NULL != ComInstance.WriteChannel[USBDEV])
          {
            UBYTE *pData;     // Pointer to dedicated Daisy Upstream Buffer (INFO or Data)
            UWORD Len = 0;

            Len = cDaisyData(&pData);

            //#define DEBUG
            #undef DEBUG
            #ifdef DEBUG
              printf("Daisy Len = %d, Counter = %d\n\r", Len, GetDaisyPushCounter());
            #endif

            if(Len > 0)
            {

              if((ComInstance.WriteChannel[USBDEV](pData, Len)) != 0)
              {
                //#define DEBUG
                #undef DEBUG
                #ifdef DEBUG
                  printf("Daisy OK tx%d\n\r", GetDaisyPushCounter());
                #endif

                DecrementDaisyPushCounter();
                cDaisyPushUpStream(); // Flood upward
                cDaisyPrepareNext();  // Ready for the next sensor in the array
              }
              else
              {
                //#define DEBUG
                #undef DEBUG
                #ifdef DEBUG
                  printf("Daisy FAIL in txing %d\n\r", GetDaisyPushCounter());  // TX upstream called
                #endif
              }
            }

        }
      }
    }
  }
  else
  {
	  if (pTxBuf->Writing)
	  {
      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
		    printf("007\n\r");
      #endif

		  if(NULL != ComInstance.WriteChannel[ChNo])
		  {
		    //#define DEBUG
		    #undef DEBUG
        #ifdef DEBUG
			    printf("Tx Writing true in the bottom ChNo = %d - PushCounter = %d\n\r", ChNo, GetDaisyPushCounter());
        #endif

		    if(ComInstance.WriteChannel[ChNo](pTxBuf->Buf, pTxBuf->BlockLen))
			  {
		      //#define DEBUG
		      #undef DEBUG
		      #ifdef DEBUG
		        printf("008\n\r");
          #endif

				  pTxBuf->Writing = 0;
			  }
		  }

	  }
  }

  if (0 == pTxBuf->Writing)
  {
    // Tx buffer needs to be empty to fill new data into it....
    switch(pTxBuf->State)
    {
      case TXFILEUPLOAD:
      {
        ULONG   MsgLeft;

        MsgLeft  = pTxBuf->MsgLen - pTxBuf->SendBytes;

        if( MsgLeft > pTxBuf->BufSize)
        {
          ReadBytes               = read(pTxBuf->pFile->File ,pTxBuf->Buf,(size_t)pTxBuf->BufSize);
          pTxBuf->pFile->Pointer += ReadBytes;
          pTxBuf->SendBytes      += ReadBytes;
          pTxBuf->State           = TXFILEUPLOAD;
        }
        else
        {
          ReadBytes               = read(pTxBuf->pFile->File ,pTxBuf->Buf,(size_t)MsgLeft);
          pTxBuf->pFile->Pointer += ReadBytes;
          pTxBuf->SendBytes      += ReadBytes;

          if (pTxBuf->MsgLen == pTxBuf->SendBytes)
          {
            pTxBuf->State = TXIDLE;

            if (pTxBuf->pFile->Pointer >= pTxBuf->pFile->Size)
            { //All Bytes has been read in the file - close handles (it is not GetFile command)

              #ifdef DEBUG
                printf("%s %lu bytes UpLoaded\r\n",pTxBuf->pFile->Name,(unsigned long)pTxBuf->pFile->Length);
              #endif

              cComCloseFileHandle(&(pTxBuf->pFile->File));
              cComFreeHandle(pTxBuf->FileHandle);
            }
          }
        }
        if (ReadBytes)
        {
          pTxBuf->Writing = 1;
        }
      }
      break;

      case TXGETFILE:
      {
        ULONG   MsgLeft;

        MsgLeft  = pTxBuf->MsgLen - pTxBuf->SendBytes;

        if( MsgLeft > pTxBuf->BufSize)
        {
          ReadBytes               = read(pTxBuf->pFile->File ,pTxBuf->Buf,(size_t)pTxBuf->BufSize);
          pTxBuf->pFile->Pointer += ReadBytes;
          pTxBuf->SendBytes      += ReadBytes;
          pTxBuf->State           = TXGETFILE;
        }
        else
        {
          ReadBytes               = read(pTxBuf->pFile->File ,pTxBuf->Buf,(size_t)MsgLeft);
          pTxBuf->pFile->Pointer += ReadBytes;
          pTxBuf->SendBytes      += ReadBytes;

          if (pTxBuf->MsgLen == pTxBuf->SendBytes)
          {
            pTxBuf->State = TXIDLE;

            if (pTxBuf->pFile->Pointer >= pTxBuf->pFile->Size)
            {

              #ifdef DEBUG
                printf("%s %lu bytes UpLoaded\r\n",pTxBuf->pFile->Name,(unsigned long)pTxBuf->pFile->Length);
              #endif
            }
          }
        }
        if (ReadBytes)
        {
          pTxBuf->Writing = 1;
        }
      }
      break;

      case TXLISTFILES:
      {
        ULONG   TmpN;
        ULONG   Len;
        ULONG   NameLen;
        ULONG   RemCharCnt;
        ULONG   BytesToSend;
        UBYTE   Repeat;
        char    TmpFileName[FILENAMESIZE];

        TmpN        =  pTxBuf->pFile->File;
        Len         =  0;
        Repeat      =  1;

        if ((pTxBuf->MsgLen - pTxBuf->SendBytes) <= pTxBuf->BufSize)
        {
          //All requested bytes can be inside the buffer
          BytesToSend      =  (pTxBuf->MsgLen - pTxBuf->SendBytes);
          pTxBuf->State    =  TXIDLE;
        }
        else
        {
          BytesToSend      =  pTxBuf->BufSize;
          pTxBuf->State    =  TXLISTFILES;
        }
        pTxBuf->BlockLen =  BytesToSend;

        if (pTxBuf->pFile->Length)
        {
          // Only here if filename has been divided in 2 pieces
          // First transfer the remaining part of the last filename
          cComGetNameFromScandirList((pTxBuf->pFile->namelist[TmpN]), (char *)TmpFileName, &NameLen, (UBYTE *)pTxBuf->Folder);
          RemCharCnt  =  NameLen - pTxBuf->pFile->Length;

          if (RemCharCnt <= BytesToSend)
          {
            // this will fit into the message length
            memcpy((char*)(&(pTxBuf->Buf[Len])), &(TmpFileName[pTxBuf->pFile->Length]), RemCharCnt);
            Len    +=  RemCharCnt;

            free(pTxBuf->pFile->namelist[TmpN]);

            if (RemCharCnt == BytesToSend)
            {
              //if If all bytes is already occupied not more to go right now
              Repeat                 =  0;
              pTxBuf->pFile->Length  =  0;
            }
            else
            {
              Repeat  =  1;
            }
          }
          else
          {
            // This is the rare condition if remaining msg len and buf size are almost equal
            memcpy((char*)(&(pTxBuf->Buf[Len])), &(TmpFileName[pTxBuf->pFile->Length]), BytesToSend);
            Len                    +=  BytesToSend;
            pTxBuf->pFile->File     =  TmpN;                         // Adjust Iteration number
            pTxBuf->pFile->Length  +=  Len;
            Repeat                  =  0;
          }
        }

        while(Repeat)
        {

          TmpN--;

          cComGetNameFromScandirList((pTxBuf->pFile->namelist[TmpN]), (char *)TmpFileName, &NameLen, (UBYTE *)pTxBuf->Folder);

          if ((NameLen + Len) <=  BytesToSend)                         // Does the next name fit into the buffer?
          {
            memcpy((char*)(&(pTxBuf->Buf[Len])), TmpFileName, NameLen);
            Len += NameLen;

            #ifdef DEBUG
              printf("List entry no = %d; File name = %s \r\n", TmpN, pTxBuf->pFile->namelist[TmpN]->d_name);
            #endif

            free(pTxBuf->pFile->namelist[TmpN]);

            if (BytesToSend == Len)
            { // buffer is filled up now exit

              pTxBuf->pFile->Length    =  0;
              pTxBuf->pFile->File      =  TmpN;
              Repeat                   =  0;
            }
          }
          else
          {
            // No, now fill up to complete buffer size
            ULONG RemCnt;

            RemCnt                 =  BytesToSend - Len;
            memcpy((char*)(&(pTxBuf->Buf[Len])), TmpFileName, RemCnt);
            Len += RemCnt;
            pTxBuf->pFile->Length  =  RemCnt;
            pTxBuf->pFile->File    =  TmpN;
            Repeat                 =  0;
          }
        }

        // Update pointers
        pTxBuf->pFile->Pointer  +=  Len;
        pTxBuf->SendBytes       +=  Len;

        if (pTxBuf->pFile->Pointer == pTxBuf->pFile->Size)
        {
          // Complete list has been tx'ed
          free(pTxBuf->pFile->namelist);
          cComFreeHandle(pTxBuf->FileHandle);
        }

        pTxBuf->Writing = 1;
      }
      break;

      default:
      {
        // this is idle state
      }
      break;
    }
  }
}


UBYTE     cComFindMailbox(UBYTE *pName, UBYTE *pNo)
{
  UBYTE   RtnVal = 0;
  UBYTE   Index;

  Index = 0;
  while((0 != strcmp((char*)pName,(char*)ComInstance.MailBox[Index].Name)) && (Index < NO_OF_MAILBOXES))
  {
    Index++;
  }

  if (Index < NO_OF_MAILBOXES)
  {
    RtnVal = 1;
    *pNo   = Index;
  }
  return(RtnVal);
}


//******* BYTE CODE SNIPPETS **************************************************


/*! \page cCom Communication
 *  <hr size="1"/>
 *  <b>     opCOM_READY     </b>
 *
 *- Test if communication is busy                                     \n
 *- Dispatch status may be set to BUSYBREAK                           \n
 *- If name is 0 then own adapter status is evaluated                 \n
 *
 *  \param  (DATA8)  HARDWARE  - \ref transportlayers                 \n
 *  \param  (DATA8*) *pNAME    - Name of the remote/own device        \n
 *
 */
/*! \brief  opCOM_READY byte code
 *
 */
void      cComReady(void)
{
  DSPSTAT DspStat = NOBREAK;
  DATA8   Hardware;
  DATA8   *pName;
  IP      TmpIp;
  UBYTE   Status;
  UBYTE   ChNos;
  UBYTE   ChNoArr[NO_OF_BT_CHS];


  TmpIp     =  GetObjectIp();
  Hardware  =  *(DATA8*)PrimParPointer();
  pName     =   (DATA8*)PrimParPointer();

  if(0 == pName[0])
  {
    //Checking if own bt adapter is busy
    if (OK == ComInstance.ComResult)
    {
      Status  =  cBtGetHciBusyFlag();
    }
    else
    {
      Status  =  ComInstance.ComResult;
    }

    if (BUSY == Status)
    {
      DspStat = BUSYBREAK;
    }
  }
  else
  {
    ChNos =  cBtGetChNo((UBYTE*)pName, ChNoArr);
    if(ChNos)
    {
      if (1 == ComInstance.TxBuf[ChNoArr[ChNos - 1] + BTSLAVE].Writing)
      {
        DspStat = BUSYBREAK;
      }
    }
  }

  if (DspStat == BUSYBREAK)
  {
    // Rewind IP
    SetObjectIp(TmpIp - 1);
  }
  SetDispatchStatus(DspStat);
}


/*! \page cCom Communication
 *  <hr size="1"/>
 *  <b>     opCOM_TEST     </b>
 *
 *- Test if communication is busy                                     \n
 *- Dispatch status is set to NOBREAK                                 \n
 *- If name is 0 then own adapter busy status is returned
 *
 *  \param  (DATA8)  HARDWARE  - \ref transportlayers                 \n
 *  \param  (DATA8*) *pNAME    - Name of the remote/own device        \n
 *  \return (DATA8)  Busy      - busy flag (0 = Ready, 1 = Busy)      \n
 *
 */
/*! \brief  opCOM_TEST byte code
 *
 */
void      cComTest(void)
{
  DSPSTAT DspStat = FAILBREAK;
  DATA8   Busy    = 0;
  DATA8   Hardware;
  DATA8   *pName;
  UBYTE   Status;
  UBYTE   ChNos;
  UBYTE   ChNoArr[NO_OF_BT_CHS];

  Hardware  =  *(DATA8*)PrimParPointer();
  pName     =   (DATA8*)PrimParPointer();

  if(0 == pName[0])
  {
    //Checking if own bt adapter is busy
    if (OK == ComInstance.ComResult)
    {
      Status  =  cBtGetHciBusyFlag();
    }
    else
    {
      Status  =  ComInstance.ComResult;
    }
    if (BUSY == Status)
    {
      Busy = 1;
    }
  }
  else
  {
    ChNos = cBtGetChNo((UBYTE*)pName, ChNoArr);
    if(ChNos)
    {
      if (1 == ComInstance.TxBuf[ChNoArr[ChNos - 1] + BTSLAVE].Writing)
      {
        Busy = 1;
      }
    }
  }

  *(DATA8*)PrimParPointer() = Busy;
  DspStat = NOBREAK;
  SetDispatchStatus(DspStat);
}


/*! \page cCom Communication
 *  <hr size="1"/>
 *  <b>     opCOM_READ (CMD, ....)  </b>
 *
 *- Communication read                                                \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   CMD     - Specific command \ref comreadsubcode  \n
 *
 *  - CMD = COMMAND
 *  \param  (DATA32)  LENGTH   - Maximal code stream length           \n
 *  \return (DATA32)  *IMAGE   - Address of image                     \n
 *  \return (DATA32)  *GLOBAL  - Address of global variables          \n
 *  \return (DATA8)   FLAG     - Flag that tells if image is ready    \n
 *
 */
/*! \brief  opCOM_READ byte code
 *
 */
void      cComRead(void)
{
  DATA8   Cmd;
  DATA32  pImage;
  DATA32  pGlobal;
  DATA8   Flag;

  if (ComInstance.Cmdfd >= 0)
  {
    // Moved To lms2012.c
        //cComUpdate();
  }

  Cmd     =  *(DATA8*)PrimParPointer();

  switch (Cmd)
  { // Function

    case COMMAND :
    {
      // pImage used as temp var
      pImage                      = *(DATA32*)PrimParPointer();

      pImage                      =  (DATA32)ComInstance.Image;
      pGlobal                     =  (DATA32)ComInstance.Globals;

      Flag                        =  ComInstance.CommandReady;
      ComInstance.CommandReady    =  0;

      *(DATA32*)PrimParPointer()  =  pImage;
      *(DATA32*)PrimParPointer()  =  pGlobal;
      *(DATA8*)PrimParPointer()   =  Flag;

    }
    break;
  }
}


/*! \page cCom
 *  <hr size="1"/>
 *  <b>     opCOM_WRITE (CMD, ....)  </b>
 *
 *- Communication write\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   CMD     - Specific command \ref comwritesubcode  \n
 *
 *  - CMD = REPLY
 *  \return (DATA32)  *IMAGE   - Address of image                      \n
 *  \return (DATA32)  *GLOBAL  - Address of global variables           \n
 *
 */
/*! \brief  opCOM_WRITE byte code
 *
 */
void      cComWrite(void)
{
  DATA8   Cmd;
  DATA8   Status;
  DATA32  pImage;
  DATA32  pGlobal;
  COMCMD  *pComCmd;
  IMGHEAD *pImgHead;

  Cmd     =  *(DATA8*)PrimParPointer();

  switch (Cmd)
  { // Function

    case REPLY :
    {
      ComInstance.VmReady   =  1;
      pImage                = *(DATA32*)PrimParPointer();
      pGlobal               = *(DATA32*)PrimParPointer();
      Status                = *(DATA8*)PrimParPointer();
      pImgHead              =  (IMGHEAD*)pImage;

      if (DIR_CMD_REPLY & ComInstance.ReplyStatus)
      {
        pComCmd               =  (COMCMD*)ComInstance.TxBuf[ComInstance.ActiveComCh].Buf;
        (*pComCmd).CmdSize   +=  (CMDSIZE)(*pImgHead).GlobalBytes; // Has been pre filled with the default length

        if (OK == Status)
        {
          (*pComCmd).Cmd  =  DIRECT_REPLY;
        }
        else
        {
          (*pComCmd).Cmd  =  DIRECT_REPLY_ERROR;
        }

        memcpy((*pComCmd).PayLoad,(UBYTE*)pGlobal,(*pImgHead).GlobalBytes);

        ComInstance.TxBuf[ComInstance.ActiveComCh].Writing   =  1;
        ComInstance.TxBuf[ComInstance.ActiveComCh].BlockLen  =  (*pComCmd).CmdSize + sizeof(CMDSIZE);
      }

      //Clear ReplyStatus both for DIR_CMD_REPLY and DIR_CMD_NOREPLY
      ComInstance.ReplyStatus = 0;
    }
    break;
  }
}


void      cComReadData(void)
{
  DATA8   Hardware;
  DATA8   *pName;
  DATA8   Size;
  DATA8   *pData;

  Hardware  =  *(DATA8*)PrimParPointer();
  pName     =   (DATA8*)PrimParPointer();
  Size      =  *(DATA8*)PrimParPointer();
  pData     =   (DATA8*)PrimParPointer();
}


void      cComWriteData(void)
{
  DATA8   Hardware;
  DATA8   *pName;
  DATA8   Size;
  DATA8   *pData;

  Hardware  =  *(DATA8*)PrimParPointer();
  pName     =   (DATA8*)PrimParPointer();
  Size      =  *(DATA8*)PrimParPointer();
  pData     =   (DATA8*)PrimParPointer();
}


/*! \page cCom
 *  <hr size="1"/>
 *  <b>     opMAILBOX_OPEN   </b>
 *
 *- Open a mail box on the brick                                                                        \n
 *- Dispatch status can return FAILBREAK
 *
 *  \param  (DATA8)    NO        - Reference ID for the mailbox. Maximum number of mailboxes is 30      \n
 *  \param  (DATA8)    BOXNAME   - Zero terminated string with the mailbox name                         \n
 *  \param  (DATA8)    TYPE      - Data type of the content of the mailbox \ref formats "TYPE enum"     \n
 *  \param  (DATA8)    FIFOSIZE  - Not used                                                             \n
 *  \param  (DATA8)    VALUES    - Number of values of the type (specified by TYPE).                    \n
 *
 *
 *  If data type DATA_S is selected then it requires that a zero terminated string is send.             \n
 *
 *  Maximum mailbox size is 250 bytes. I.e. if type is string (DATA_S) then there can only be 1 string  \n
 *  of maximum 250 bytes (incl. zero termination), or if array (DATA_A), then array size cannot be      \n
 *  larger than 250 bytes                                                                               \n
 */
/*! \brief  opMAILBOX_OPEN byte code
 *
 */
void      cComOpenMailBox(void)
{
  DSPSTAT DspStat = FAILBREAK;
  DATA8   No;
  DATA8   *pBoxName;
  DATA8   Type;
  DATA8   Values;
  DATA8   FifoSize;

  No          =  *(DATA8*)PrimParPointer();
  pBoxName    =   (DATA8*)PrimParPointer();
  Type        =  *(DATA8*)PrimParPointer();
  FifoSize    =  *(DATA8*)PrimParPointer();
  Values      =  *(DATA8*)PrimParPointer();

  if (OK != ComInstance.MailBox[No].Status)
  {
    snprintf((char*)(&(ComInstance.MailBox[No].Name[0])), 50,"%s",(char*)pBoxName);
    memset(ComInstance.MailBox[No].Content, 0, MAILBOX_CONTENT_SIZE);
    ComInstance.MailBox[No].Type      =  Type;
    ComInstance.MailBox[No].Status    =  OK;
    ComInstance.MailBox[No].ReadCnt   =  0;
    ComInstance.MailBox[No].WriteCnt  =  0;
    DspStat                           =  NOBREAK;
  }
  SetDispatchStatus(DspStat);
}


/*! \page cCom
 *  <hr size="1"/>
 *  <b>     opMAILBOX_WRITE   </b>
 *
 *- Write to mailbox in remote brick                                                      \n
 *- Dispatch status can return FAILBREAK
 *
 *  If Brick name is left empty (0) then all connected devices will                       \n
 *  receive the mailbox message
 *
 *  \param  (DATA8)    BRICKNAME  - Zero terminated string name of the receiving brick    \n
 *  \param  (DATA8)    HARDWARE   - Transportation media                                  \n
 *  \param  (DATA8)    BOXNAME    - Zero terminated string name of the receiving mailbox  \n
 *  \param  (DATA8)    TYPE       - Data type of the values \ref formats "TYPE enum"      \n
 *  \param  (DATA8)    VALUES     - Number of values of the specified type to send        \n
 *
 *  If string type (DATA_S) data is to be transmitted then a zero terminated string is    \n
 *  expected.
 *
 *  If array type data (DATA_A) is to be transmitted then the number of bytes to be send  \n
 *  is equal to the array size
 *
 */
/*! \brief  opMAILBOX_WRITE byte code
 *
 */
void      cComWriteMailBox(void)
{
  DSPSTAT DspStat = FAILBREAK;
  DATA8   *pBrickName;
  DATA8   Hardware;
  DATA8   *pBoxName;
  DATA8   Type;
  DATA8   Values;
  DATA32  Payload[(MAILBOX_CONTENT_SIZE/4) + 1];
  UBYTE   ChNos;
  UBYTE   ComChNo;
  UBYTE   ChNoArr[NO_OF_BT_CHS];
  UBYTE   Cnt;
  UWORD   PayloadSize = 0;

  WRITE_MAILBOX          *pComMbx;
  WRITE_MAILBOX_PAYLOAD  *pComMbxPayload;

  pBrickName  =   (DATA8*)PrimParPointer();
  Hardware    =  *(DATA8*)PrimParPointer();
  pBoxName    =   (DATA8*)PrimParPointer();
  Type        =  *(DATA8*)PrimParPointer();
  Values      =  *(DATA8*)PrimParPointer();

  // It is needed that all parameters are pop'ed off regardless
  for(Cnt = 0; Cnt < Values; Cnt++)
  {
    switch(Type)
    {
      case DATA_8:
      {
        ((DATA8*)(Payload))[Cnt] = *(DATA8*)PrimParPointer();
        PayloadSize++;
      }
      break;
      case DATA_16:
      {
        ((DATA16*)(Payload))[Cnt] = *(DATA16*)PrimParPointer();
        PayloadSize += 2;
      }
      break;
      case DATA_32:
      {
        ((DATA32*)(Payload))[Cnt] = *(DATA32*)PrimParPointer();
        PayloadSize += 4;
      }
      break;
      case DATA_F:
      {
        ((DATAF*)(Payload))[Cnt]  = *(DATAF*)PrimParPointer();
        PayloadSize += 4;
      }
      break;
      case DATA_S:
      {
        // Supports only one string
        DATA8   *pName;

        pName = (DATA8*)PrimParPointer();

        PayloadSize = snprintf((char*)&Payload[0], MAILBOX_CONTENT_SIZE, "%s",(char*)pName);
        PayloadSize++; // Include zero termination
      }
      break;
      case DATA_A:
      {
        DESCR   *pDescr;
        PRGID   TmpPrgId;
        DATA32  Size;
        void    *pTmp;
        HANDLER TmpHandle;

        TmpPrgId     =  CurrentProgramId();
        TmpHandle    =  *(HANDLER*)PrimParPointer();

        if (OK == cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp))
        {
          pDescr = (DESCR*)pTmp;
          Size = (pDescr->Elements);
        }
        else
        {
          Size = 0;
        }

        if (MAILBOX_CONTENT_SIZE < Size)
        {
          //Truncate if larger than buffer size
          Size = MAILBOX_CONTENT_SIZE;
        }

        memcpy((char*)&Payload[0],(DATA8*)(pDescr->pArray), Size);
        PayloadSize = Size;
      }
      break;
    }
  }

  ChNos = cBtGetChNo((UBYTE*)pBrickName, ChNoArr);

  for(Cnt = 0; Cnt < ChNos; Cnt++)
  {
    ComChNo = ChNoArr[Cnt] + BTSLAVE;                               // Ch nos offset from BT module

    // Valid channel found
    if ((0 == ComInstance.TxBuf[ComChNo].Writing) && (TXIDLE == ComInstance.TxBuf[ComChNo].State))
    {
      // Buffer is empty
      pComMbx             =  (WRITE_MAILBOX*)ComInstance.TxBuf[ComChNo].Buf;

      // First part of message
      (*pComMbx).CmdSize  =  SIZEOF_WRITEMAILBOX - sizeof(CMDSIZE);
      (*pComMbx).MsgCount =  1;
      (*pComMbx).CmdType  =  SYSTEM_COMMAND_NO_REPLY;
      (*pComMbx).Cmd      =  WRITEMAILBOX;
      (*pComMbx).NameSize =  strlen((char*)pBoxName) + 1;
      snprintf((char*)(*pComMbx).Name,(*pComMbx).NameSize,"%s",(char*)pBoxName);

      (*pComMbx).CmdSize += (*pComMbx).NameSize;

      // Payload part of message
      pComMbxPayload            = (WRITE_MAILBOX_PAYLOAD*) &(ComInstance.TxBuf[ComChNo].Buf[(*pComMbx).CmdSize + sizeof(CMDSIZE)]);
      (*pComMbxPayload).SizeLsb = (UBYTE) (PayloadSize & 0x00FF);
      (*pComMbxPayload).SizeMsb = (UBYTE)((PayloadSize >> 8) & 0x00FF);
      memcpy((*pComMbxPayload).Payload, Payload, PayloadSize);
      (*pComMbx).CmdSize += (PayloadSize + SIZEOF_WRITETOMAILBOXPAYLOAD);

      ComInstance.TxBuf[ComChNo].BlockLen  =  (*pComMbx).CmdSize + sizeof(CMDSIZE);
      ComInstance.TxBuf[ComChNo].Writing   =  1;
    }
  }

  DspStat = NOBREAK;
  SetDispatchStatus(DspStat);
}

/*! \page cCom
 *  <hr size="1"/>
 *  <b>     opMAILBOX_READ  </b>
 *
 *- Read data from mailbox specified by NO                                           \n
 *- Dispatch status can return FAILBREAK
 *
 *  \param  (DATA8)    NO         - Messagebox ID of the message box you want to read\n
 *  \param  (DATA16)   LENGTH     - Maximum bytes to be read                         \n
 *  \param  (DATA8)    VALUES     - Number of value to read                          \n
 *  \return (Type specified in open)  VALUE      - Data from the message box         \n
 *
 *  The type of Value is specified by mailbox open byte code.                        \n
 */
 /*! \brief  opMAILBOX_READ byte code
 *
 */
void      cComReadMailBox(void)
{
  DSPSTAT DspStat = FAILBREAK;
  DATA8   No;
  DATA8   Values;
  DATA16  Len;
  UBYTE   Cnt;

  No        =  *(DATA8*)PrimParPointer();
  Len       =  *(DATA16*)PrimParPointer();
  Values    =  *(DATA8*)PrimParPointer();

  if (OK == ComInstance.MailBox[No].Status)
  {
    for(Cnt = 0; Cnt < Values; Cnt++)
    {
      switch(ComInstance.MailBox[No].Type)
      {
        case DATA_8:
        {
          *(DATA8*)PrimParPointer()  =  ((DATA8*)(ComInstance.MailBox[No].Content))[Cnt];
        }
        break;
        case DATA_16:
        {
          *(DATA16*)PrimParPointer() =  ((DATA16*)(ComInstance.MailBox[No].Content))[Cnt];
        }
        break;
        case DATA_32:
        {
          *(DATA32*)PrimParPointer() =  ((DATA32*)(ComInstance.MailBox[No].Content))[Cnt];
        }
        break;
        case DATA_F:
        {
          *(DATAF*)PrimParPointer()  =  ((DATAF*)(ComInstance.MailBox[No].Content))[Cnt];
        }
        break;
        case DATA_S:
        {
          // Supports only one string
          DATA8  *pData;
          DATA32  Data32;

          pData = (DATA8*)PrimParPointer();

          if (VMInstance.Handle >= 0)
          {

            Data32   =  ComInstance.MailBox[No].DataSize;
            if (Data32 > MIN_ARRAY_ELEMENTS)
            {
              pData  =  (DATA8*)VmMemoryResize(VMInstance.Handle, Data32);
            }
          }

          if (pData != NULL)
          {
            snprintf((char*)pData, MAILBOX_CONTENT_SIZE, "%s",(char*)ComInstance.MailBox[No].Content);
          }
        }
        break;
        case DATA_A:
        {
          HANDLER   TmpHandle;
          DATA8     *pData;
          DATA32    Data32;

          TmpHandle  =  *(HANDLER*)PrimParPointer();
          Data32     =  ComInstance.MailBox[No].DataSize;

          if (Data32 > MIN_ARRAY_ELEMENTS)
          {
            pData  =  (DATA8*)VmMemoryResize(TmpHandle, Data32);
          }

          if (NULL != pData)
          {
            memcpy(pData, (char*)&((DATA8*)(ComInstance.MailBox[No].Content))[Cnt], Data32);
          }
          ComInstance.MailBox[No].DataSize = 0;
        }
        break;
      }
    }

    if (ComInstance.MailBox[No].WriteCnt != ComInstance.MailBox[No].ReadCnt)
    {
      ComInstance.MailBox[No].ReadCnt++;
    }
  }

  DspStat = NOBREAK;
  SetDispatchStatus(DspStat);
}


/*! \page cCom
 *  <hr size="1"/>
 *  <b>     opMAILBOX_TEST  </b>
 *
 *- Tests if new message has been read                                              \n
 *- Dispatch status can return FAILBREAK
 *
 *  \param  (DATA8)    NO         - Reference ID mailbox number                     \n
 *  \return (DATA8)    BUSY       - If Busy = TRUE then no new messages are received\n
 */
 /*! \brief  opMAILBOX_TEST byte code
 *
 */
void      cComTestMailBox(void)
{
  DSPSTAT DspStat = FAILBREAK;
  DATA8   Busy    = 0;
  DATA8   No;

  No   =  *(DATA8*)PrimParPointer();

  if (ComInstance.MailBox[No].WriteCnt == ComInstance.MailBox[No].ReadCnt)
  {
    Busy = 1;
  }

  *(DATA8*)PrimParPointer() = Busy;

  DspStat = NOBREAK;
  SetDispatchStatus(DspStat);
}


/*! \page cCom
 *  <hr size="1"/>
 *  <b>     opMAILBOX_READY  </b>
 *
 *- Waiting from message to be read                            \n
 *- Dispatch status can return FAILBREAK
 *
 *  \param  (DATA8)    NO         - Reference ID mailbox number\n
 */
 /*! \brief  opMAILBOX_READY byte code
 *
 */
void      cComReadyMailBox(void)
{
  DSPSTAT DspStat = NOBREAK;
  DATA8   No;
  IP      TmpIp;

  TmpIp  =  GetObjectIp();
  No     =  *(DATA8*)PrimParPointer();

  if (ComInstance.MailBox[No].WriteCnt == ComInstance.MailBox[No].ReadCnt)
  {
    DspStat = BUSYBREAK;;
  }

  if (DspStat == BUSYBREAK)
  { // Rewind IP

    SetObjectIp(TmpIp - 1);
  }
  SetDispatchStatus(DspStat);
}


/*! \page cCom
 *  <hr size="1"/>
 *  <b>     opMAILBOX_CLOSE  </b>
 *
 *- Closes mailbox indicated by NO                                  \n
 *- Dispatch status can return FAILBREAK
 *
 *    -  \param  (DATA8)    NO         - Reference ID mailbox number\n
 */
/*! \brief  opMAILBOX_CLOSE byte code
*
*/
void      cComCloseMailBox(void)
{
  DSPSTAT DspStat = NOBREAK;
  DATA8 No;

  No  =  *(DATA8*)PrimParPointer();

  ComInstance.MailBox[No].Status = FAIL;
  SetDispatchStatus(DspStat);
}


 void      cComWriteFile(void)
{
  TXBUF           *pTxBuf;
  DSPSTAT DspStat = FAILBREAK;

  DATA8   Hardware;
  DATA8   *pDeviceName;
  DATA8   *pFileName;
  DATA8   FileType;

  Hardware      =  *(DATA8*)PrimParPointer();
  pDeviceName   =   (DATA8*)PrimParPointer();
  pFileName     =   (DATA8*)PrimParPointer();
  FileType      =  *(DATA8*)PrimParPointer();

  #ifdef DEBUG
    printf("%d [%s] ->> %d [%s]\r\n",FileType,(char*)pFileName,Hardware,(char*)pDeviceName);
  #endif

  switch(Hardware)
  {
    case HW_USB:
    {
    }
    break;

    case HW_BT:
    {
      UBYTE       ChNo[NO_OF_BT_CHS];
      UBYTE       pName[MAX_FILENAME_SIZE];
      BEGIN_DL    *pDlMsg;

      cBtGetChNo((UBYTE*)pDeviceName, ChNo);

      ChNo[0] += BTSLAVE;                           // Add Com module offset
      pTxBuf   =  &(ComInstance.TxBuf[ChNo[0]]);
      pDlMsg   =  (BEGIN_DL*)(&(pTxBuf->Buf[0]));

      if (TYPE_FOLDER == FileType)
      {
        // Sending folder
        pTxBuf->State  =  TXFOLDER;

        // Make copy of Foldername including a "/" at the end
        snprintf((char*)pTxBuf->Folder, MAX_FILENAME_SIZE, "%s",pFileName);
        strcat((char*)pTxBuf->Folder, "/");

        pTxBuf->pDir  =  (DIR*)opendir((char*)pFileName);

        if (cComGetNxtFile(pTxBuf->pDir, pName))
        {
          // File has been located
          ComInstance.ComResult = BUSY;
          cComCreateBeginDl(pTxBuf, pName);
        }
        else
        {
          // No files in directory
          ComInstance.ComResult = OK;
        }
        DspStat  =  NOBREAK;
      }
      else
      {
        // Sending single file
        pTxBuf->State          =  TXFILE;
        pTxBuf->Folder[0]      =  0;           // Make sure that folder is empty
        ComInstance.ComResult  =  BUSY;
        cComCreateBeginDl(pTxBuf, (UBYTE*)pFileName);
        DspStat  =  NOBREAK;
      }
    }
    break;

    case HW_WIFI:
    {
    }
    break;
  }
  SetDispatchStatus(DspStat);
}


//static    DATA8 BtParred      =  0;

/*! \page cCom
 *  <hr size="1"/>
 *  <b>     opCOM_GET (CMD, ....)  </b>
 *
 *- Communication get entry\n
 *- Dispatch status can return FAILBREAK
 *
 *  \param  (DATA8)   CMD               - \ref comgetsetsubcode
 *
 *\n
 *  - CMD = GET_ON_OFF
 *\n  Get active state                                                         \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \return (DATA8)    ACTIVE      - Active [0,1]                         \n
 *
 *\n
 *
 *\n
 *  - CMD = GET_VISIBLE
 *\n  Get visibility state                                                     \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \return (DATA8)    VISIBLE     - Visible [0,1]                        \n
 *
 *\n
 *
 *\n
 *  - CMD = GET_RESULT
 *\n  Get status. This command gets the result of the command that             \n
 *    is being executed. This could be a search or a connection                \n
 *    request.                                                                 \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    ITEM        - Name index                           \n
 *    -  \return (DATA8)    RESULT      - \ref results                         \n
 *
 *\n
 *
 *\n
 *  - CMD = GET_PIN
 *\n  Get pin code. For now "1234" is returned                                 \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    NAME        - First character in character string  \n
 *    -  \param  (DATA8)    LENGTH      - Max length of returned string        \n
 *    -  \return (DATA8)    PINCODE     - First character in character string  \n
 *
 *\n
 *
 *\n
 *  - CMD = SEARCH_ITEMS
 *\n  Get number of item from search. After a search has been completed,       \n
 *    SEARCH ITEMS will return the number of remote devices found.             \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \return (DATA8)    ITEMS       - No of items in seach list            \n
 *
 *\n
 *
 *\n
 *  - CMD = SEARCH_ITEM
 *\n  Get search item informations. Used to retrieve the item information      \n
 *    in the search list                                                       \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    ITEM        - Item - index in search list          \n
 *    -  \param  (DATA8)    LENGTH      - Max length of returned string        \n
 *    -  \return (DATA8)    NAME        - First character in character string  \n
 *    -  \return (DATA8)    PARRED      - Parred [0,1]                         \n
 *    -  \return (DATA8)    CONNECTED   - Connected [0,1]                      \n
 *    -  \return (DATA8)    TYPE        - \ref bttypes                         \n
 *    -  \return (DATA8)    VISIBLE     - Visible [0,1]                        \n
 *
 *\n
 *
 *\n
 *  - CMD = FAVOUR_ITEMS
 *\n  Get no of item in favourite list. The number of paired devices, not      \n
 *    necessarily visible or present devices                                   \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \return (DATA8)    ITEMS       - No of items in list                  \n
 *
 *\n
 *
 *\n
 *  - CMD = FAVOUR_ITEM
 *\n  Get favourite item informations. Used to retrieve the item information   \n
 *    in the favourite list. All items in the favourite list are paired devices\n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    ITEM        - Item - index in favourite list       \n
 *    -  \param  (DATA8)    LENGTH      - Max length of returned string        \n
 *    -  \return (DATA8)    NAME        - First character in character string  \n
 *    -  \return (DATA8)    PARRED      - Parred [0,1]                         \n
 *    -  \return (DATA8)    CONNECTED   - Connected [0,1]                      \n
 *    -  \return (DATA8)    TYPE        - \ref bttypes                         \n
 *
 *\n
 *
 *\n
 *  - CMD = GET_ID
 *\n  Get bluetooth address information                                        \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    LENGTH      - Max length of returned string        \n
 *    -  \return (DATA8)    STRING      - First character in BT adr string     \n
 *
 *\n
 *
 *\n
 *  - CMD = GET_BRICKNAME
 *\n  Gets the name of the brick                                               \n
 *    -  \param  (DATA8)    LENGTH      - Max length of returned string        \n
 *    -  \return (DATA8)    NAME        - First character in brick name        \n
 *
 *\n
 *
 *\n
 *  - CMD = GET_NETWORK
 *\n  Gets the network information. WIFI only                                  \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    LENGTH      - Max length of returned string        \n
 *    -  \return (DATA8)    NAME        - First character in AP name           \n
 *    -  \return (DATA8)    MAC         - First character in MAC adr string    \n
 *    -  \return (DATA8)    IP          - First character in IP no string      \n
 *
 *\n
 *
 *\n
 *  - CMD = GET_PRESENT
 *\n  Return if hardare is present. WIFI only                                  \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \return (DATA8)    OK          - Present [0,1]                        \n
 *
 *\n
 *
 *\n
 *  - CMD = GET_ENCRYPT
 *\n  Returns the encryption mode of the hardware. WIFI only                   \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    ITEM        - Item - index in favourite list       \n
 *    -  \param  (DATA8)    TYPE        - Encryption type                      \n
 *
 *\n
 *
 *\n
 *  - CMD = GET_INCOMMING
 *\n  Returns the encryption mode of the hardware. WIFI only                   \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    LENGTH      - Max length of returned string        \n
 *    -  \param  (DATA8)    NAME        - First character in name              \n
 *
 *\n
 *
 */
/*! \brief  opCOM_GET byte code
 *
 */


void      cComGet(void)
{
  DSPSTAT DspStat = FAILBREAK;
  DATA8   Cmd;
  DATA8   Hardware;
  DATA8   OnOff;
  DATA8   Mode2;
  DATA8   Item;
  DATA8   Status;
  DATA8   *pName;
  DATA8   Length;
  DATA8   *pPin;
  DATA8   Items;
  DATA8   Parred;
  DATA8   Connected;
  DATA8   Visible;
  DATA8   Type;
  DATA8   *pMac;
  DATA8   *pIp;

  Cmd  =  *(DATA8*)PrimParPointer();

  switch (Cmd)
  { // Function
  
    case GET_MODE2 :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      Mode2       =  0;

      switch(Hardware)
      {
        case HW_BT:
        {

          if (FAIL != BtGetMode2((UBYTE*)&Mode2))
          {
            DspStat = NOBREAK;
          }
        }
        break;
        default:
        {
        }
        break;
      }
      *(DATA8*)PrimParPointer()  =  Mode2;
    }
    break;
    
    case GET_ON_OFF :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      OnOff       =  0;

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          if (FAIL != BtGetOnOff((UBYTE*)&OnOff))
          {
            DspStat = NOBREAK;
          }
        }
        break;
        case HW_WIFI:
        {
          if (OK == cWiFiGetOnStatus())
          {
            OnOff   =  1;
          }
          DspStat =  NOBREAK;
        }
        break;
        default:
        {
        }
        break;
      }

      *(DATA8*)PrimParPointer()  =  OnOff;
    }
    break;

    case GET_VISIBLE :
    {
      DATA8   Visible;

      Hardware    =  *(DATA8*)PrimParPointer();

      if (Hardware < HWTYPES)
      {
        // Fill in code here
        Visible   =  BtGetVisibility();

        DspStat   =  NOBREAK;
      }
      *(DATA8*)PrimParPointer()  =  Visible;
    }
    break;

    case GET_RESULT :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      Item        =  *(DATA8*)PrimParPointer();
      Status      =  0;

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          if (OK == ComInstance.ComResult)
          {
            Status  =  cBtGetHciBusyFlag();
          }
          else
          {
            Status  =  ComInstance.ComResult;
          }
          DspStat = NOBREAK;
        }
        break;
        case HW_WIFI:
        {
          Status  = cWiFiGetStatus();
          DspStat = NOBREAK;
        }
        break;
        default:
        {
        }
        break;
      }

      *(DATA8*)PrimParPointer()  =  Status;
    }
    break;

    case GET_PIN :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      pName       =   (DATA8*)PrimParPointer();
      Length      =  *(DATA8*)PrimParPointer();
      pPin        =   (DATA8*)PrimParPointer();

      // Fill in code here
      snprintf((char*)pPin,Length, "%s","1234");

      DspStat   =  NOBREAK;

    }
    break;

    case CONNEC_ITEMS:
    {
      Hardware  =  *(DATA8*)PrimParPointer();
      Items     =  0;

      switch(Hardware)
      {
        case HW_USB:
        {
          DspStat =  NOBREAK;
        }
        break;
        case HW_BT:
        {
          Items   =  cBtGetNoOfConnListEntries();
          DspStat =  NOBREAK;
        }
        break;
        case HW_WIFI:
        {
          DspStat =  NOBREAK;
        }
        break;
      }
      *(DATA8*)PrimParPointer() = Items;
    }
    break;

    case CONNEC_ITEM:
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      Item        =  *(DATA8*)PrimParPointer();
      Length      =  *(DATA8*)PrimParPointer();
      pName       =   (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        if (-1 == Length)
        {
          Length = vmBRICKNAMESIZE;
        }
        pName  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
      }

      switch(Hardware)
      {
        case HW_USB:
        {
          DspStat =  NOBREAK;
        }
        break;
        case HW_BT:
        {
          if (NULL != pName)
          {
            cBtGetConnListEntry(Item, (UBYTE *)pName, Length, (UBYTE*) &Type);
            *(DATA8*)PrimParPointer() = Type;
          }
          DspStat =  NOBREAK;
        }
        break;
        case HW_WIFI:
        {
          DspStat =  NOBREAK;
        }
        break;
      }
    }
    break;

    case SEARCH_ITEMS :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      Items       =  0;

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          Items   =  cBtGetNoOfSearchListEntries();
          DspStat =  NOBREAK;
        }
        break;
        case HW_WIFI:
        {
          Items   =  cWiFiGetApListSize();
          DspStat =  NOBREAK;
        }
        break;
      }
      *(DATA8*)PrimParPointer() = Items;
    }
    break;

    case SEARCH_ITEM :
    {
      UBYTE   Flags;

      Hardware    =  *(DATA8*)PrimParPointer();
      Item        =  *(DATA8*)PrimParPointer();
      Length      =  *(DATA8*)PrimParPointer();
      pName       =   (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        if (-1 == Length)
        {
          Length = vmBRICKNAMESIZE;
        }
        pName  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
      }

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          Parred      =  0;
          Connected   =  0;
          Type        =  ICON_UNKNOWN;
          Visible     =  1;

          if (NULL != pName)
          {
            cBtGetSearchListEntry(Item, &Connected, &Type, &Parred, (UBYTE*)pName, Length);
          }

          DspStat   =  NOBREAK;
        }
        break;
        case HW_WIFI:
        {
          Parred      =  0;
          Connected   =  0;
          Type        =  0;
          Visible     =  0;

          if (NULL != pName)
          {
            cWiFiGetName((char *)pName, (int)Item, Length);
            Flags  =  cWiFiGetFlags(Item);

            if (Flags & VISIBLE)
            {
              Visible = 1;
            }
            if (Flags & CONNECTED)
            {
              Connected = 1;
            }
            if (Flags & KNOWN)
            {
              Parred = 1;
            }
            if (Flags & WPA2)
            {
              Type = 1;
            }
          }

          DspStat  =  NOBREAK;
        }
        break;
      }
      *(DATA8*)PrimParPointer()  =  Parred;
      *(DATA8*)PrimParPointer()  =  Connected;
      *(DATA8*)PrimParPointer()  =  Type;
      *(DATA8*)PrimParPointer()  =  Visible;
    }
    break;

    case FAVOUR_ITEMS :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      Items       =  0;

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          Items   =  cBtGetNoOfDevListEntries();
          DspStat =  NOBREAK;
        }
        break;
        case HW_WIFI:
        {
        }
        break;
        default:
        {
        }
        break;
      }
      *(DATA8*)PrimParPointer()  =  Items;
    }
    break;

    case FAVOUR_ITEM :
    {
      UBYTE  Flags;

      Hardware    =  *(DATA8*)PrimParPointer();
      Item        =  *(DATA8*)PrimParPointer();
      Length      =  *(DATA8*)PrimParPointer();
      pName       =   (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        if (-1 == Length)
        {
          Length = vmBRICKNAMESIZE;
        }
        pName  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
      }

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          Parred      =  1;                      // Only in favour list if parred
          Connected   =  0;
          Type        =  ICON_UNKNOWN;

          if (NULL != pName)
          {
            cBtGetDevListEntry(Item, &Connected, &Type,(UBYTE*)pName, Length);
          }
          DspStat  =  NOBREAK;
        }
        break;
        case HW_WIFI:
        {
          Parred      =  0;                      // Only in favour list if parred
          Connected   =  0;
          Type        =  0;

          if (NULL != pName)
          {
            cWiFiGetName((char *)pName, (int)Item, Length);
            Flags    =  cWiFiGetFlags(Item);

            if (Flags & CONNECTED)
            {
              Connected = 1;
            }
            if (Flags & KNOWN)
            {
              Parred = 1;
            }
            if (Flags & WPA2)
            {
              Type = 1;
            }
          }

          DspStat  =  NOBREAK;
        }
        break;
        default:
        {
        }
        break;
      }

      *(DATA8*)PrimParPointer()  =  Parred;
      *(DATA8*)PrimParPointer()  =  Connected;
      *(DATA8*)PrimParPointer()  =  Type;
    }
    break;

    case GET_ID:
    {
      Hardware  =  *(DATA8*)PrimParPointer();
      Length    =  *(DATA8*)PrimParPointer();
      pName     =   (DATA8*)PrimParPointer();

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          if (VMInstance.Handle >= 0)
          {
            if (-1 == Length)
            {
              Length = vmBTADRSIZE;
            }
            pName  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
          }

          if (pName != NULL)
          {
            cBtGetId((UBYTE*)pName, Length);
          }
          DspStat =  NOBREAK;

        }
        break;
        case HW_WIFI:
        {
        }
        break;
        default:
        {
        }
        break;
      }

    }
    break;

    case GET_BRICKNAME :
    {
      Length =  *(DATA8*)PrimParPointer();
      pName  =  (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        if (-1 == Length)
        {
          Length = vmBRICKNAMESIZE;
        }
        pName  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
      }

      if (NULL != pName)
      {
        snprintf((char*)pName,Length,"%s",(char*)&(ComInstance.BrickName[0]));
      }

      DspStat =  NOBREAK;
    }
    break;

    case GET_NETWORK:
    {
      UBYTE   Flags;
      UBYTE   MaxStrLen;

      Hardware  =  *(DATA8*)PrimParPointer();
      Length    =  *(DATA8*)PrimParPointer();

      // Lenght are the maximum length for all
      // 3 strings returned (pName, pMac and pIp)
      if ((vmBRICKNAMESIZE >= vmMACSIZE) && (vmBRICKNAMESIZE >= vmIPSIZE))
      {
        MaxStrLen = vmBRICKNAMESIZE;
      }
      else
      {
        if (vmMACSIZE >= vmIPSIZE)
        {
          MaxStrLen = vmMACSIZE;
        }
        else
        {
          MaxStrLen = vmIPSIZE;
        }
      }

      pName     =   (DATA8*)PrimParPointer();
      if (VMInstance.Handle >= 0)
      {
        if (-1 == Length)
        {
          Length = MaxStrLen;
        }
        pName  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
      }

      pMac      =   (DATA8*)PrimParPointer();
      if (VMInstance.Handle >= 0)
      {
        if (-1 == Length)
        {
          Length = MaxStrLen;
        }
        pMac  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
      }

      pIp       =   (DATA8*)PrimParPointer();
      if (VMInstance.Handle >= 0)
      {
        if (-1 == Length)
        {
          Length = MaxStrLen;
        }
        pIp  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
      }

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
        }
        break;
        case HW_WIFI:
        {
          if ((NULL != pName) && (NULL != pMac) && (NULL != pIp))
          {
            if(OK == cWiFiGetOnStatus())
            {
              Flags = cWiFiGetFlags((int)0);
              if (Flags & CONNECTED)
              {
                cWiFiGetIpAddr((char*)pIp);
                cWiFiGetMyMacAddr((char*)pMac);
                cWiFiGetName((char*)pName, 0, Length);
              }
              else
              {
                cWiFiGetMyMacAddr((char*)pMac);
                snprintf((char*)pName, Length, "%s","NONE");
                snprintf((char*)pIp, 3, "%s","??");
              }
            }
            else
            {
              snprintf((char*)pMac, 3, "%s","??");
              snprintf((char*)pName, Length, "%s","NONE");
              snprintf((char*)pIp, 3, "%s", "??");
            }
          }
        }
        break;
      }

      DspStat   =  NOBREAK;
    }
    break;

    case GET_PRESENT:
    {
      Hardware  =  *(DATA8*)PrimParPointer();
      Status    =  0;

      switch(Hardware)
      {
        case HW_USB:
        {
          Status = 1;
        }
        break;
        case HW_BT:
        {
          Status = 1;
        }
        break;
        case HW_WIFI:
        {
          if(OK == cWiFiKnownDongleAttached())
          {
        	Status = 1;
          }
        }
        break;
      }
      *(DATA8*)PrimParPointer() = Status;
      DspStat   =  NOBREAK;
    }
    break;

    case GET_ENCRYPT:
    {

      Hardware   =  *(DATA8*)PrimParPointer();
      Item       =  *(DATA8*)PrimParPointer();

      Type       =  0;
      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
        }
        break;
        case HW_WIFI:
        {
          DspStat   =  NOBREAK;
        }
        break;
      }

      *(DATA8*)PrimParPointer() = Type;
    }
    break;

    case GET_INCOMING:
    {
      Hardware   =  *(DATA8*)PrimParPointer();
      Length     =  *(DATA8*)PrimParPointer();
      pName      =   (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        if (-1 == Length)
        {
          Length = vmBRICKNAMESIZE;
        }
        pName  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
      }

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          if (NULL != pName)
          {
            cBtGetIncoming((UBYTE*)pName,(UBYTE*)&Type, Length);
          }

          *(DATA8*)PrimParPointer() = Type;
          DspStat   =  NOBREAK;
        }
        break;
        case HW_WIFI:
        {
        }
        break;
      }
    }
    break;

  }
  SetDispatchStatus(DspStat);
}


/*! \page cCom
 *  <hr size="1"/>
 *  <b>     opCOM_SET (CMD, ....)  </b>
 *
 *- Communication set entry\n
 *- Dispatch status can return FAILBREAK
 *
 *  \param  (DATA8)   CMD               - \ref comgetsetsubcode
 *
 *
 *\n
 *  - CMD = SET_MODE2
 *\n  Set active mode state, either active or not\n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    ACTIVE      - Active [0,1], 1 = on, 0 = off        \n
 *
 *\n
 *
 *\n
 *  - CMD = SET_ON_OFF
 *\n  Set active state, either on or off\n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    ACTIVE      - Active [0,1], 1 = on, 0 = off        \n
 *
 *\n
 *
 *\n
 *  - CMD = SET_VISIBLE
 *\n  Set visibility state - Only available for bluetooth                      \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    VISIBLE     - Visible [0,1], 1 = visible, 0 = invisible\n
 *
 *\n
 *
 *\n
 *  - CMD = SET_SEARCH
 *\n  Control search. Starts or or stops the search for remote devices         \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    SEARCH      - Search [0,1] 0 = stop search, 1 = start search\n
 *
 *\n
 *
 *\n
 *  - CMD = SET_PIN
 *\n  Set pin code. Set the pincode for a remote device.                       \n
 *    Used when requested by bluetooth.                                        \n
 *    not at this point possible by user program                               \n
 *    -  \param  *(DATA8)   HARDWARE    - \ref transportlayers                 \n
 *    -  \param   (DATA8)   NAME        - First character in character string  \n
 *    -  \param   (DATA8)   PINCODE     - First character in character string  \n
 *
 *\n
 *
 *\n
 *  - CMD = SET_PASSKEY
 *\n  Set pin code. Set the pincode for a remote device.                       \n
 *    Used when requested by bluetooth.                                        \n
 *    not at this point possible by user program                               \n
 *    -  \param  *(DATA8)   Hardware    - \ref transportlayers                 \n
 *    -  \param  *(DATA8)   ACCEPT      - Acceptance [0,1] 0 = reject 1 = accept \n
 *
 *\n
 *
 *\n
 *  - CMD = SET_CONNECTION
 *\n  Control connection. Initiate or closes the connection request to a       \n
 *    remote device by the specified name                                      \n
 *    -  \param  (DATA8)    Hardware    - \ref transportlayers                 \n
 *    -  \param *(DATA8)    NAME        - First character in Name              \n
 *    -  \param  (DATA8)    CONNECTION  - Connect [0,1], 1 = Connect, 0 = Disconnect\n
 *
 *\n
 *
 *\n
 *  - CMD = SET_BRICKNAME
 *\n  Sets the name of the brick\n
 *    -  \param  (DATA8)    NAME        - First character in character string  \n
 *
 *\n
 *
 *\n
 *  - CMD = SET_MOVEUP
 *\n  Moves the index in list one step up. Used to re-arrange WIFI list        \n
 *    Only used for WIFI                                                       \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    ITEM        - Index in table                       \n
 *
 *\n
 *
 *\n
 *  - CMD = SET_MOVEDOWN
 *\n  Moves the index in list one step down. Used to re-arrange WIFI list      \n
 *    Only used for WIFI                                                       \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    ITEM        - Index in table                       \n
 *
 *\n
 *
 *\n
 *  - CMD = SET_ENCRYPT
 *\n  Moves the index in list one step down. Only used for WIFI                \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param  (DATA8)    ITEM        - Index in table                       \n
 *    -  \param  (DATA8)    ENCRYPT     - Encryption type                      \n
 *
 *\n
 *
 *\n
 *  - CMD = SET_SSID
 *\n  Sets the SSID name. Only used for WIFI                                   \n
 *    -  \param  (DATA8)    HARDWARE    - \ref transportlayers                 \n
 *    -  \param *(DATA8)    NAME        - Index in table                       \n
 *
 *\n
 *
 */
/*! \brief  opCOM_SET byte code
 *
 */
void      cComSet(void)
{
  DSPSTAT DspStat = FAILBREAK;
  DATA8   Cmd;
  DATA8   Hardware;
  DATA8   OnOff;
  DATA8   Mode2;
  DATA8   Visible;
  DATA8   Search;
  DATA8   *pName;
  DATA8   *pPin;
  DATA8   Connection;
  DATA8   Item;
  DATA8   Type;

  Cmd  =  *(DATA8*)PrimParPointer();

  switch (Cmd)
  { // Function
    case SET_MODE2 :
    {
      Hardware   =  *(DATA8*)PrimParPointer();
      Mode2      =  *(DATA8*)PrimParPointer();

      switch(Hardware)
      {
        case HW_BT:
        {
          if (FAIL != BtSetMode2(Mode2))
          {
            DspStat = NOBREAK;
          }
        }
        break;
        default:
        {
        }
        break;
      }
    }
    break;
    
    case SET_ON_OFF :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      OnOff       =  *(DATA8*)PrimParPointer();

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          if(FAIL != BtSetOnOff(OnOff))
          {
            DspStat =  NOBREAK;
          }
        }
        break;
        case HW_WIFI:
        {
          if (OnOff)
          {
            if (FAIL != cWiFiTurnOn())
            {
              DspStat =  NOBREAK;
            }
          }
          else
          {
            if (FAIL != cWiFiTurnOff())
            {
              DspStat =  NOBREAK;
            }
          }
        }
        break;
        default:
        {
        }
        break;
      }
    }
    break;

    case SET_VISIBLE :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      Visible     =  *(DATA8*)PrimParPointer();
      DspStat     =  NOBREAK;

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          // Fill in code here
          if (FAIL != BtSetVisibility(Visible))
          {
            DspStat =  NOBREAK;
          }
        }
        break;
        case HW_WIFI:
        {
          // N_A
        }
        break;
        default:
        {
        }
        break;
      }
    }
    break;

    case SET_SEARCH :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      Search      =  *(DATA8*)PrimParPointer();

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          if (Search)
          {
            if (FAIL != BtStartScan())
            {
              DspStat =  NOBREAK;
            }
          }
          else
          {
            if (FAIL != BtStopScan())
            {
              DspStat  =  NOBREAK;
            }
          }
        }
        break;
        case HW_WIFI:
        {
          if (Search)
          {
            if (FAIL != cWiFiScanForAPs())
            {
              DspStat  =  NOBREAK;
            }
          }
          else
          {
            //NA in Wifi
          }
        }
        break;
        default:
        {
        }
        break;
      }
    }
    break;

    case SET_PIN :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      pName       =   (DATA8*)PrimParPointer();
      pPin        =   (DATA8*)PrimParPointer();

      switch (Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          if (FAIL != cBtSetPin((UBYTE*)pPin))
          {
            DspStat  =  NOBREAK;
          }
        }
        break;
        case HW_WIFI:
        {
          #ifdef DEBUG
            printf("\n\rSSID = %s\r\n", pName);
          #endif
          if (OK == cWiFiGetIndexFromName((char*)pName, (UBYTE*)&Item))
          {
            #ifdef DEBUG
              printf("\r\nGot Index = %d\r\n", Item);
              printf("\r\nGot Index from name = %s\r\n", pName);
            #endif
            cWiFiMakePsk((char*)pName, (char*)pPin, (int)Item);
            #ifdef DEBUG
              printf("\r\nPSK made\r\n");
            #endif
            DspStat  =  NOBREAK;
          }
        }
        break;
      }
    }
    break;

    case SET_PASSKEY:
    {
      UBYTE  Accept;

      Hardware  =  *(DATA8*)PrimParPointer();
      Accept    =  *(DATA8*)PrimParPointer();

      switch (Hardware)
      {
        case HW_BT:
        {
          if (FAIL != cBtSetPasskey(Accept))
          {
            DspStat  =  NOBREAK;
          }
        }
        break;
        default:
        {
          //Only used in bluetooth
        }
        break;
      }
    }
    break;

    case SET_CONNECTION :
    {
      Hardware    =  *(DATA8*)PrimParPointer();
      pName       =   (DATA8*)PrimParPointer();
      Connection  =  *(DATA8*)PrimParPointer();

      switch (Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
          if (Connection)
          {
            if (FAIL != cBtConnect((UBYTE *)pName))
            {
              DspStat = NOBREAK;
            }

          }
          else
          {
            if (FAIL != cBtDisconnect((UBYTE *)pName))
            {
              DspStat = NOBREAK;
            }
          }
        }
        break;
        case HW_WIFI:
        {
          if (Connection)
          {
            if (OK == cWiFiGetIndexFromName((char*)pName, (UBYTE*)&Item))
            {
              //#define DEBUG
              #undef DEBUG
              #ifdef DEBUG
                printf("cWiFiConnect => index: %d, Name: %s\n\r", Item, pName);
              #endif

              cWiFiConnectToAp((int)Item);

              //#define DEBUG
              #undef DEBUG
              #ifdef DEBUG
                printf("We have tried to connect....\n\r");
              #endif

              DspStat  =  NOBREAK;
            }
            else
            {
            }
          }
          else
          {
            // Not implemented
            // implicite in connect
          }
        }
        break;
      }
    }
    break;

    case SET_BRICKNAME:
    {
      UBYTE    Len;
      FILE     *File;
      char     nl[1];

      nl[0] = 0x0A;                                 // Insert new line

      pName  =  (DATA8*)PrimParPointer();

      Len = strlen((char*)pName);

      if (OK == ValidateString(pName,vmCHARSET_NAME) && ((vmBRICKNAMESIZE - 1) > strlen((char*)pName)))
      {
        if (FAIL != cBtSetName((UBYTE*)pName, Len + 1))
        {
          File  =  fopen("./settings/BrickName","w");
          if (File != NULL)
          {
            fwrite(pName, 1, (int)Len + 1, File);
            fwrite(nl, 1, (int)1, File);                // Insert new line
            fclose (File);
          }
          snprintf((char*)&(ComInstance.BrickName[0]), vmBRICKNAMESIZE, "%s",(char*)pName);
          sethostname((char*)pName, Len + 1);
          DspStat =  NOBREAK;
        }
      }
    }
    break;

    case SET_MOVEUP:
    {
      Hardware   =  *(DATA8*)PrimParPointer();
      Item       =  *(DATA8*)PrimParPointer();

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
        }
        break;
        case HW_WIFI:
        {
          cWiFiMoveUpInList((int)Item);
          DspStat =  NOBREAK;
        }
        break;
      }
    }
    break;

    case SET_MOVEDOWN:
    {
      Hardware   =  *(DATA8*)PrimParPointer();
      Item       =  *(DATA8*)PrimParPointer();

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
        }
        break;
        case HW_WIFI:
        {
          cWiFiMoveDownInList((int)Item);
          DspStat  =  NOBREAK;
        }
        break;
      }
    }
    break;

    case SET_ENCRYPT:
    {
      Hardware   =  *(DATA8*)PrimParPointer();
      pName      =   (DATA8*)PrimParPointer();
      Type       =  *(DATA8*)PrimParPointer();

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
        }
        break;
        case HW_WIFI:
        {
          if (Type)
          {
            //#define DEBUG
            #undef DEBUG
            #ifdef DEBUG
              printf("\r\nWPA encrypt called\r\n");
            #endif

            uint LocalIndex = 0;
            cWiFiGetIndexFromName((char*)pName, (UBYTE*)&LocalIndex);
            cWiFiSetEncryptToWpa2(LocalIndex);
          }
          else
          {
            //#define DEBUG
            #undef DEBUG
            #ifdef DEBUG
              printf("\r\nNONE encrypt called\r\n");
            #endif

            cWiFiSetEncryptToNone(cWiFiGetApListSize());
          }
          DspStat =  NOBREAK;
        }
        break;
      }
    }
    break;

    case SET_SSID:
    {
      UBYTE   Index;

      Hardware   =  *(DATA8*)PrimParPointer();
      pName      =   (DATA8*)PrimParPointer();

      switch(Hardware)
      {
        case HW_USB:
        {
        }
        break;
        case HW_BT:
        {
        }
        break;
        case HW_WIFI:
        {
          Index   =  cWiFiGetApListSize();
          cWiFiSetName((char*)pName, (int)Index);
          cWiFiIncApListSize();
          DspStat =  NOBREAK;
        }
        break;
      }
    }

  }
  SetDispatchStatus(DspStat);
}


void      cComRemove(void)
{
  DSPSTAT DspStat = FAILBREAK;
  DATA8   Hardware;
  DATA8   *pName;
  UBYTE   LocalIndex;

  Hardware  =  *(DATA8*)PrimParPointer();
  pName     =   (DATA8*)PrimParPointer();

  switch(Hardware)
  {
    case HW_USB:
    {
    }
    break;
    case HW_BT:
    {
      if (FAIL != cBtDeleteFavourItem((UBYTE *)pName))
      {
        DspStat  =  NOBREAK;
      }
    }
    break;
    case HW_WIFI:
    {
      cWiFiGetIndexFromName((char*)pName, (UBYTE*)&LocalIndex);

      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
        printf("Removing Index: %d\n\r", LocalIndex);
      #endif

      cWiFiDeleteAsKnown(LocalIndex);       // We removes the (favorit) "*"
      DspStat  =  NOBREAK;
    }
    break;
  }
}

UBYTE     cComGetBtStatus(void)
{
  return(cBtGetStatus());
}


UBYTE     cComGetWifiStatus(void)
{
  UBYTE   Flags;
  UBYTE   Status;

  Status = 0;

  if (OK == cWiFiGetOnStatus())
  {
    Status |=  0x03;                 // Wifi on + visible (always visible if on)
    Flags   =  cWiFiGetFlags((int)0);
    if (CONNECTED & Flags)
    {
      Status |= 0x04;                // Wifi connected to AP
    }
  }
  return(Status);
}


void      cComGetBrickName(DATA8 Length, DATA8 *pBrickName)
{
  snprintf((char*)pBrickName,Length,"%s",(char*)&(ComInstance.BrickName[0]));
}


DATA8     cComGetEvent(void)
{
  return(cBtGetEvent());
}

//*****************************************************************************
