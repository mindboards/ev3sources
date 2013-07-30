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


#ifndef C_INPUT_H_
#define C_INPUT_H_

#include  "lms2012.h"

#define   INPUT_PORTS                   INPUTS
#define   INPUT_DEVICES                 (INPUT_PORTS * CHAIN_DEPT)

#define   OUTPUT_PORTS                  OUTPUTS
#define   OUTPUT_DEVICES                (OUTPUT_PORTS * CHAIN_DEPT)

#define   DEVICES                       (INPUT_DEVICES + OUTPUT_DEVICES)

#define   INPUT_VALUES                  (INPUT_PORTS * 3)
#define   INPUT_VALUE_SIZE              5
#define   INPUT_BUFFER_SIZE             (INPUT_VALUES * INPUT_VALUE_SIZE)
#define   INPUT_SIZE                    (INPUT_VALUES * 2)

RESULT    cInputInit(void);

RESULT    cInputOpen(void);

RESULT    cInputClose(void);

RESULT    cInputExit(void);

void      cInputChar(DATA8 Char);

void      cInputUpdate(UWORD Time);

RESULT    cInputCompressDevice(DATA8 *pDevice,UBYTE Layer,UBYTE Port);

RESULT    cInputGetDeviceData(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData);

RESULT    cInputSetChainedDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode);

void      cInputDeviceList(void);

RESULT	  cInputStartTypeDataUpload(void);

#define   INPUT_DEVICE_LIST   OC(opINPUT_DEVICE_LIST,&cInputDeviceList,7,0 )


void      cInputDevice(void);

void      cInputRead(void);

void      cInputReadSi(void);

void      cInputReadExt(void);

void      cInputTest(void);

void      cInputReady(void);

void      cInputWrite(void);

void      cInputSample(void);


typedef   struct
{
  UWORD   InvalidTime;                        //!< mS from type change to valid data
  UWORD   TimeoutTimer;                       //!< mS allowed to be busy timer
  UWORD   TypeIndex;                          //!< Index to information in "TypeData" table
  DATA8   Connection;                         //!< Connection type (from DCM)
  OBJID   Owner;
  RESULT  DevStatus;
  DATA8   Busy;
  DATAF   Raw[MAX_DEVICE_DATASETS];           //!< Raw value (only updated when "cInputReadDeviceRaw" function is called)
#ifndef DISABLE_BUMBED
  DATAF   OldRaw;
  DATA32  Changes;
  DATA32  Bumps;
#endif
#ifdef Linux_X86
  UWORD   Timer;
  UBYTE   Dir;
#endif
}
DEVICE;


typedef   struct
{
  DATA8   InUse;
  DATAF   Min;
  DATAF   Max;
}
CALIB;


typedef struct
{
  //*****************************************************************************
  // Input Global variables
  //*****************************************************************************

#ifndef DISABLE_DAISYCHAIN
  UBYTE     Data[MAX_DEVICE_DATALENGTH];
#endif
  ANALOG    Analog;
  ANALOG    *pAnalog;

  UART      Uart;
  UART      *pUart;

  IIC       Iic;
  IIC       *pIic;

  int       UartFile;
  int       AdcFile;
  int       DcmFile;
  int       IicFile;

  DEVCON    DevCon;
  UARTCTL   UartCtl;
  IICCTL    IicCtl;
  IICDAT    IicDat;

  DATA32    InputNull;

  DATA8     TmpMode[INPUT_PORTS];

  DATA8     ConfigurationChanged[MAX_PROGRAMS];

  DATA8     DeviceType[DEVICES];              //!< Type of all devices - for easy upload
  DATA8     DeviceMode[DEVICES];              //!< Mode of all devices
  DEVICE    DeviceData[DEVICES];              //!< Data for all devices

  UWORD     NoneIndex;
  UWORD     UnknownIndex;
  DATA8     DCMUpdate;

  DATA8     TypeModes[MAX_DEVICE_TYPE + 1];   //!< No of modes for specific type

  UWORD     MaxDeviceTypes;                   //!< Number of device type/mode entries in tabel
  TYPES     *TypeData;                        //!< Type specific data
  UWORD     IicDeviceTypes;                   //!< Number of IIC device type/mode entries in tabel
  IICSTR    *IicString;
  IICSTR    IicStr;

  DATA16    TypeDataTimer;
  DATA16    TypeDataIndex;


  CALIB     Calib[MAX_DEVICE_TYPE][MAX_DEVICE_MODES];
}
INPUT_GLOBALS;

#if       (HARDWARE == SIMULATION)
  extern INPUT_GLOBALS * gInputInstance;
  #define InputInstance (*gInputInstance)

  void setInputInstance(INPUT_GLOBALS * _Instance);
  INPUT_GLOBALS * getInputInstance();
#else
  extern INPUT_GLOBALS InputInstance;
#endif

#endif /* C_INPUT_H_ */
