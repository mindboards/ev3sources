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


/*! \page Mode2 decode interface Description
 *
 *- \subpage  Mode2Description
 *- \subpage  Mode2Decode
 *
 */

/*! \page Mode2Description Mode2 Description
 *
 *  <hr size="1"/>
 *
 *  Description of Mode2
 *
 *  All bytes received from a remote mode2 bluetooth device
 *  should be relayed unchanged to mode2 decoding.
 *
 *  All mode2 data bytes from mode2 decoding should
 *  be transmitted unchanged to the remote Mode2
 *  bluetooth device.
 *
 *  All brick application data for the remote mode2
 *  bluetooth device should be transferred for mode2
 *  decoding.
 */

/*! \page Mode2Decode Mode2 Decode Description
 *
 *  <hr size="1"/>
 *
 *  Interfaces to Mode2
 */

#include  "lms2012.h"
#include  "c_com.h"
#include  "c_bt.h"
#include  "c_i2c.h"

#include  <stdio.h>
#include  <fcntl.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <string.h>
#include  <signal.h>
#include  <sys/mman.h>
#include  <time.h>
#include  <errno.h>

#include  <sys/ioctl.h>
#include  <linux/i2c.h>
#include  <linux/i2c-dev.h>
#include  <pthread.h>


/* I2C Adresses */
enum
{
  WRITE_DATA   =  0x2A,    // Also referred to as 0x54
  READ_STATUS  =  0x2A,    // Also referred to as 0x55
  WRITE_RAW    =  0x2B,    // Also referred to as 0x56
  READ_DATA    =  0x2B     // Also referred to as 0x57
};

/* Bluetooth pins needs to be synchronized with pins in d_bt.c */
enum
{
  CTS_PIC,
  PIC_RST,
  PIC_EN,
  BLUETOOTH_PINS
};

enum
{
  SET   = 0,
  CLEAR = 1,
  HIIMP = 2
};

enum
{
  OVERRUN_ERROR       =  0x01,
  CRC_ERROR           =  0x02,
  INCORRECT_ACTION    =  0x03,
  UNEXPECTED_ERROR    =  0x04,
  RAW_OVERRUN_ERROR   =  0x05,
};


#define   I2CBUF_SIZE                   150
#define   APPDATABUF_SIZE               150
#define   MODE2BUF_SIZE                 1024   // Must be power of 2
#define   MIN_MSG_LEN                   6
#define   SLEEPuS                       ((ULONG)(1000))
#define   SEC_1                         (((ULONG)(1000000))/SLEEPuS)

typedef   struct
{
  UBYTE   Buf[MODE2BUF_SIZE];
  UWORD   InPtr;
  UWORD   OutPtr;
} MODE2BUF;


static    int       I2cFile;
static    int       BtFile;
static    int       ThreadRunState;
static    MODE2BUF  Mode2InBuf;
char      *RtnMsg = "I2c Func thread end";
static    struct    i2c_rdwr_ioctl_data    msg_rdwr;
static    struct    i2c_msg                i2cmsg;

static    UBYTE     Status;

static    READBUF   *pReadBuf;
static    WRITEBUF  *pMode2WriteBuf;

static    char      *pBundleIdString;
static    char      *pBundleSeedIdString;

static    char      TmpBuf[I2CBUF_SIZE + 1];


UBYTE     I2cReadStatus(UBYTE *pBuf);
UBYTE     I2cReadCts(void);
int       I2cRead(int fd, unsigned int addr, char *buf, unsigned char len);
int       I2cWrite(int fd, unsigned int addr, char *buf, unsigned char len);
void*     I2cCtrl(void *ptr);

void      I2cSetPIC_RST(void);
void      I2cClearPIC_RST(void);
void      I2cSetPIC_EN(void);
void      I2cClearPIC_EN(void);
void      I2cHiImpPIC_EN(void);


#define   BUFBytesFree                  (((Mode2InBuf.OutPtr - Mode2InBuf.InPtr) - 1)      &  ((UWORD)(MODE2BUF_SIZE - 1)))
#define   BUFBytesAvail                 ((Mode2InBuf.InPtr   - Mode2InBuf.OutPtr)          &  ((UWORD)(MODE2BUF_SIZE - 1)))
#define   BUFAddInPtr(Val)              (Mode2InBuf.InPtr    = ((Mode2InBuf.InPtr + Val)   &  ((UWORD)(MODE2BUF_SIZE - 1))))
#define   BUFAddOutPtr(Val)             (Mode2InBuf.OutPtr   = ((Mode2InBuf.OutPtr + Val)  &  ((UWORD)(MODE2BUF_SIZE - 1))))

#define   DISCONNDueToErr               {\
                                          cBtDiscChNo(0);\
                                          ThreadRunState = 0;\
                                        }


RESULT    I2cInit(READBUF *pBuf, WRITEBUF *pWriteBuf, char *pBundleId, char *pBundleSeedId)
{
  RESULT  Result = FAIL;

  BtFile  =  open(BT_DEVICE_NAME, O_RDWR);

  if (BtFile >= 0)
  {
    Result = OK;
  }

  pReadBuf            =  pBuf;
  pMode2WriteBuf      =  pWriteBuf;
  pBundleIdString     =  pBundleId;
  pBundleSeedIdString =  pBundleSeedId;

  return(Result);
}


void      I2cExit(void)
{
  I2cStop();
  if (BtFile >= MIN_HANDLE)
  {
    close(BtFile);
  }
}


void      I2cStart(void)
{
  pthread_t I2cFunc;
  int       I2cRet;
  char *message     = "I2c Func thread";

  Mode2InBuf.InPtr  = 0;
  Mode2InBuf.OutPtr = 0;
  Status            = MODE2_BOOTING;

  ThreadRunState = 1;
  I2cRet = pthread_create(&I2cFunc, NULL, I2cCtrl, (void*) message);
}


void      I2cStop(void)
{
  ThreadRunState = 0;
}


void*     I2cCtrl(void *ptr)
{
  UWORD   Size;
  UBYTE   Buf[200];
  UWORD   Check;

  I2cFile  =  -1;

  if (I2cFile >= MIN_HANDLE)
  {
    close (I2cFile);
    I2cFile = -1;
  }

  I2cClearPIC_EN();
  I2cSetPIC_RST();
  usleep(1000);
  I2cClearPIC_RST();
  usleep(1000);
  I2cSetPIC_EN();

  Mode2InBuf.InPtr   =  0;
  Mode2InBuf.OutPtr  =  0;

  I2cFile = open("/dev/i2c-1", O_RDWR);

  if (I2cFile < 0)
  {
    #ifdef DEBUG
      printf("Can't open device file /dev/i2c-1\n");
    #endif
  }
  else
  {
    ioctl(I2cFile,I2C_SLAVE, 0);
  }

  // Check that CTS is going high within 500ms else error
  Check = 1;
  while (0 != Check)
  {
    if (I2cReadCts())
    {
      Check = 0;
    }
    else
    {
      if (Check < 500)
      {
        usleep(1000);
        Check++;
      }
      else
      {
        // If 500ms passed without CTS going high
        // set error
        Check  = 0;
        Status = MODE2_FALIURE;
      }
    }
  }

  Check = 0;
  while(ThreadRunState)
  {
    if (Check++ > SEC_1)
    {
      // if CTS is low for 1 sec or more then it is an error
      DISCONNDueToErr;
    }
    else
    {
      if (I2cReadCts())
      {
        Check = 0;          // CTS = High -> reset timer

        // Read status when CTS is high
        // Status returns byte 0 = Status
        //                byte 1 = Depending on status
        //                byte 2 = Depending on status
        I2cReadStatus(Buf);

        switch(Buf[0])
        {
          case 'E':
          {
            switch(Buf[1])
            {
              case OVERRUN_ERROR:
              {
                // Occurs when having transferred to many bytes
                // to mode2 decoding
                Status = MODE2_FALIURE;
                #ifdef DEBUG
                  printf("\r\nERROR - OVERRUN\r\n");
                #endif
              }
              break;
              case CRC_ERROR:
              {
                Status = MODE2_FALIURE;
                #ifdef DEBUG
                  printf("\r\nERROR - CRC\r\n");
                #endif
              }
              break;
              case INCORRECT_ACTION:
              {
                Status = MODE2_FALIURE;
                #ifdef DEBUG
                  printf("\r\nERROR - INCORRECT_ACTION\r\n");
                #endif
              }
              break;
              case UNEXPECTED_ERROR:
              {
                Status = MODE2_FALIURE;
                #ifdef DEBUG
                  printf("\r\nERROR - UNEXPECTED\r\n");
                #endif
              }
              break;
              case RAW_OVERRUN_ERROR:
              {
                Status = MODE2_FALIURE;
                #ifdef DEBUG
                  printf("\r\nERROR - RAW_OVERRUN\r\n");
                #endif
              }
              break;
              default:
              {
                // Unknown Error
                Status = MODE2_FALIURE;
                #ifdef DEBUG
                  printf("\r\nERROR - UNKNOWN\r\n");
                #endif
              }
              break;
            }
            DISCONNDueToErr;
          }
          break;

          case 'A':
          {
            #ifdef DEBUG
              UWORD Test;
            #endif

            // Check if TX Bt buffers are ready to accept data
            if (cBtI2cBufReady())
            {
              Size = Buf[1];

              if (0 < Size)
              {
                // Bytes for mode2 decoding are ready - read them
                if (0 > I2cRead(I2cFile, READ_DATA, (char *)Buf, Size))
                {
                  // Error
                  #ifdef DEBUG
                    printf("\r\n'A' Read error \r\n");
                  #endif
                }

                #ifdef DEBUG
                  printf("\r\nA - Reading mode2 data from decoding to Tx on Bluetooth\r\n");
                  for(Test = 0; Test < Size; Test++)
                  {
                    printf("Buf[%d] = %02X\r\n",Test,Buf[Test]);
                  }
                #endif

                // Send the bytes for mode2 decoding
                cBtI2cToBtBuf(Buf, Size);
              }
            }
          }
          break;

          case 'W':
          {
            UWORD BytesToTx;
            UWORD TmpCnt;

            // More data for mode2 decoding is needed at address 0x54
            #ifdef DEBUG
              printf("\r\nW - Waiting for more mode2 data\r\n");
            #endif

            BytesToTx = BUFBytesAvail;
            if (BytesToTx)
            {
              if (BytesToTx > (UWORD)(Buf[1]))
              {
                // Do not send more bytes than requested
                BytesToTx = (UWORD)(Buf[1]);
              }

              #ifdef DEBUG
                printf("\r\n Remote data:");
              #endif
              for (TmpCnt = 0; TmpCnt < BytesToTx; TmpCnt++)
              {
                #ifdef DEBUG
                  printf("%02X, ",Mode2InBuf.Buf[Mode2InBuf.OutPtr]);
                #endif
                TmpBuf[TmpCnt] = Mode2InBuf.Buf[Mode2InBuf.OutPtr];
                BUFAddOutPtr(1);
              }

              if (0 > I2cWrite(I2cFile, WRITE_DATA, (char*)TmpBuf, BytesToTx))
              {
                // Error
                #ifdef DEBUG
                  printf("\r\n'W' write error \r\n");
                #endif
              }
            }
          }
          break;

          case 'I':
          {

            UWORD  TmpCnt;
            UWORD  BytesAvail;

            BytesAvail = BUFBytesAvail;

            if (MIN_MSG_LEN <= BytesAvail)
            {
              // Minimum msg length coming from remote device has been received
              #ifdef DEBUG
                printf("\r\nData to decoding from Bluetooth = %d, Inptr = %d, OutPtr = %d\r\n",BytesAvail,Mode2InBuf.InPtr,Mode2InBuf.OutPtr);
              #endif

              // Send minimum msg length + 0xFF header
              TmpBuf[0] = 0xFF;
              for (TmpCnt = 0; TmpCnt < MIN_MSG_LEN; TmpCnt++)
              {
                TmpBuf[TmpCnt + 1] = Mode2InBuf.Buf[Mode2InBuf.OutPtr];
                BUFAddOutPtr(1);
              }

              if(0 > I2cWrite(I2cFile, WRITE_DATA, (char*)TmpBuf, (MIN_MSG_LEN + 1)))
              {
                DISCONNDueToErr;
              }
              else
              {
                #ifdef DEBUG
                  printf("\r\n.... Remote data to the Pic, Bytes transferred %d\r\n",ByteCnt);
                #endif
              }
            }
            else
            {
              UWORD ByteCnt;

              // Check for LEGO App data to the mode2 decoding
              // ByteCnt can be bigger than i2c buffer size, if that is the case
              // then ByteCnt has to be split up in I2CBUF_SIZE
              ByteCnt  =  pMode2WriteBuf->InPtr - pMode2WriteBuf->OutPtr;

              if(ByteCnt)
              {
                if (ByteCnt > (I2CBUF_SIZE - 1))
                {
                  // Only send max buffer size
                  ByteCnt = (I2CBUF_SIZE - 1);
                }

                TmpBuf[0] = ByteCnt;
                memcpy((char *)&(TmpBuf[1]), (char *)&(pMode2WriteBuf->Buf[pMode2WriteBuf->OutPtr]), ByteCnt);
                pMode2WriteBuf->OutPtr += ByteCnt;

                if (pMode2WriteBuf->OutPtr == pMode2WriteBuf->InPtr)
                {
                  // All bytes has been written - clear the buffer
                  pMode2WriteBuf->InPtr  = 0;
                  pMode2WriteBuf->OutPtr = 0;
                }

                if (0 > I2cWrite(I2cFile, WRITE_RAW, (char *)&(TmpBuf[0]), ByteCnt + 1))
                {
                  DISCONNDueToErr;
                }
                else
                {
                  #ifdef DEBUG
                    printf("\r\n.... Application data to the Pic, Bytes to send %d\r\n",ByteCnt);
                  #endif
                }

              }
            }
            #ifdef DEBUG
              printf("\r\nI - Idle state\r\n");
            #endif
          }
          break;

          case 'R':
          {
            // Raw data ready for our application - send it to mode1 decoding
            #ifdef DEBUG
              UWORD Test;
            #endif

            if (READ_BUF_EMPTY  ==  pReadBuf->Status)
            {
              // Buf[1] contains the number of bytes ready to be read
              Size = Buf[1];
              if (0 <= I2cRead(I2cFile, READ_DATA, (char*)&(pReadBuf->Buf[0]), Size))
              {
                pReadBuf->OutPtr = 0;
                pReadBuf->InPtr  = Size;
                pReadBuf->Status = READ_BUF_FULL;

                #ifdef DEBUG
                  printf("\r\nR - %d bytes of App data read \r\n",Size);
                  for(Test = 0; Test < Size; Test++)
                  {
                    printf("AppData[%d] = %02X\r\n",Test,pReadBuf->Buf[Test]);
                  }
                #endif

                DecodeMode1(0);
              }
            }
            else
            {
              DecodeMode1(0);
            }
          }
          break;

          case 'B':
          {
            char  IdString[MAX_BUNDLE_ID_SIZE + MAX_BUNDLE_SEED_ID_SIZE];

            strcpy(&IdString[1],(char*)pBundleSeedIdString);
            strcat(&IdString[1],(char*)pBundleIdString);

            IdString[0] = (UBYTE)(strlen(&(IdString[1])));

            if (0 <= I2cWrite(I2cFile, WRITE_DATA, (char*)IdString, (IdString[0] + 1)))
            {
              if (MODE2_BOOTING == Status)
              {
                // When bundle strings transferred then everything is assumed to be ok
                Status = MODE2_SUCCESS;
              }
            }
          }
          break;

          case 'S':
          {
            #ifdef DEBUG
              printf("\r\nS - Reset source = %02X, Status = %02X\r\n",Buf[1], Buf[2]);
            #endif

            if (0 == Buf[2])
            {
              Status = MODE2_FALIURE;
              DISCONNDueToErr;
              #ifdef DEBUG
                printf("\r\nERROR - failure on reset\r\n");
              #endif
            }
          }
          break;

          default:
          {
            Status = MODE2_FALIURE;
            #ifdef DEBUG
              printf("\r\nStatus return not recognized....\r\n");
            #endif
          }
          break;
        }
      }
    }
    usleep(SLEEPuS);
  }

  if (I2cFile >= MIN_HANDLE)
  {
    close (I2cFile);
    I2cFile = -1;
  }
  I2cClearPIC_EN();
  pthread_exit((void *)RtnMsg);
}


UBYTE     I2cReadCts(void)
{
  UBYTE    Buf[10];
  UBYTE    RtnVal;

  RtnVal = 0;
  if (BtFile >= 0)
  {
    read(BtFile, Buf, 1);

    if (Buf[0])
    {
      RtnVal = 1;
    }
  }
  return(RtnVal);
}


void      I2cSetPIC_RST(void)
{
  UBYTE   Set[2];

  Set[0] = SET;
  Set[1] = PIC_RST;

  if (BtFile >= 0)
  {
    write(BtFile, Set, sizeof(Set));
  }
}


void      I2cClearPIC_RST(void)
{
  UBYTE   Clr[2];

  Clr[0] = CLEAR;
  Clr[1] = PIC_RST;

  if (BtFile >= 0)
  {
    write(BtFile, Clr, sizeof(Clr));
  }
}


void      I2cSetPIC_EN(void)
{
  UBYTE   Set[2];

  Set[0] = SET;
  Set[1] = PIC_EN;

  if (BtFile >= 0)
  {
    write(BtFile, Set, sizeof(Set));
  }
}


void      I2cClearPIC_EN(void)
{
  UBYTE   Clr[2];

  Clr[0] = CLEAR;
  Clr[1] = PIC_EN;

  if (BtFile >= 0)
  {
    write(BtFile, Clr, sizeof(Clr));
  }
}


void      I2cHiImpPIC_EN(void)
{
  UBYTE   HiImp[2];

  HiImp[0] = HIIMP;
  HiImp[1] = PIC_EN;

  if (BtFile >= 0)
  {
    write(BtFile, HiImp, sizeof(HiImp));
  }
}


UBYTE     I2cReadStatus(UBYTE *pBuf)
{
  return(I2cRead(I2cFile, READ_STATUS, (char*)pBuf, 3));
}


// read len bytes stored in the device at address addr in array buf
// return -1 on error, 0 on success
int       I2cRead(int fd, unsigned int addr, char *buf, unsigned char len)
{
  int i;

  if(len > I2CBUF_SIZE)
  {
    #ifdef DEBUG
      fprintf(stderr,"I can only write I2CBUF_SIZE bytes at a time!\r\n");
    #endif
    Status = MODE2_FALIURE;
    return -1;
  }

  msg_rdwr.msgs  =  &i2cmsg;
  msg_rdwr.nmsgs =  1;

  i2cmsg.addr    =  addr;
  i2cmsg.flags   =  I2C_M_RD;
  i2cmsg.len     =  len;
  i2cmsg.buf     =  buf;

  if((i=ioctl(fd, I2C_RDWR, &msg_rdwr)) < 0)
  {
    #ifdef DEBUG
      printf("\r\nIOCTL Read error, Len = %d, i2cmsg.len = %d \r\n",len, i2cmsg.len);
    #endif

    Status = MODE2_FALIURE;
    return -1;
  }
  return 0;
}


int       I2cWrite(int fd, unsigned int addr, char *buf, unsigned char len)
{
  int    i;

  msg_rdwr.msgs  =  &i2cmsg;
  msg_rdwr.nmsgs =  1;

  i2cmsg.addr    =  addr;
  i2cmsg.flags   =  0;
  i2cmsg.len     =  len;
  i2cmsg.buf     =  buf;

  if((i = ioctl(fd,I2C_RDWR,&msg_rdwr)) < 0)
  {
    #ifdef DEBUG
      printf("\r\nIOCTL write error, Len = %d, i2cmsg.len = %d \r\n",len, i2cmsg.len);
    #endif

    Status = MODE2_FALIURE;
    return -1;
  }
  return(len);
}


// Fill in as many bytes as possible - return value is the number of bytes transferred
UWORD     DataToMode2Decoding(UBYTE *pBuf, UWORD Length)
{
  UWORD   BytesAccepted;
  UWORD   BytesFreeInBuf;
  UWORD   BytesTransferred;
  UWORD   Cnt;

  BytesAccepted = 0;

  // If buffer is empty if can accept more bytes otherwise it will wait accepting bytes
  BytesFreeInBuf = BUFBytesFree;
  if (BytesFreeInBuf)
  {
    if (Length > BytesFreeInBuf)
    {
      BytesTransferred = BytesFreeInBuf;
    }
    else
    {
      BytesTransferred = Length;
    }

    for(Cnt = 0; Cnt < BytesTransferred; Cnt++)
    {
      Mode2InBuf.Buf[Mode2InBuf.InPtr] = pBuf[Cnt];
      BUFAddInPtr(1);
    }
    BytesAccepted   =  BytesTransferred;
  }
  return(BytesAccepted);
}


UBYTE     I2cGetBootStatus(void)
{
  return(Status);
}








