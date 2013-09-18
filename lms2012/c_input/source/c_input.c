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


/*! \page InputLibrary Input Library
 *
 *  <hr size="1"/>
 *
 *- \subpage  InputLibraryDescription
 *- \subpage  InputLibraryDeviceSetup
 *- \subpage  InputLibraryDeviceTypes
 */


/*! \page InputLibraryDescription Description
 *
 *  <hr size="1"/>
 *
 *  This library takes care of input port related things:
 *
 *  Setup devices, convert values, control ready/busy conditions and of course support input related byte codes.
 *
 *  The scheduling in this library is done by the VM requesting data when it reads device values, tests for ready/busy and requests device list.
 *
 */


/*! \page InputLibraryDeviceSetup Device Setup
 *
 *  <hr size="1"/>
 *
 *  The supported device types are stored in an internal "TypeData" table with a couple of fixed types (Error, Open, UnKnown) and the rest
 *  loaded from the "typedata" text file located in the "sys/settings" folder.
 *
 *  When a device is attached or removed on a port the DCM driver \ref DcmDriver "Device Detection Manager Driver"
 *  will change the ANALOG struct (polled by the VM requesting device list) and this way get the knowledge to this Input Library.\n
 *
 *  This change starts a search in the "TypeData" table if the port connection type has changed. If the device connection type is a "DUMB" (new sensor)
 *  the search is done among entries that have an "IdValue" - if match with the device IdValue is found - index to the "TypeData" table is saved as
 *  device "TypeIndex".
 *
 *  Independent of the connection type a search is started among all entries and if a match with the device type is found this index to "TypeData" table
 *  is saved as device "TypeIndex" (if connection type is "DUMB" - the new index will override the previous index).
 *
 *  An equal search is done when the VM reads a device value with a different type than the detected one.
 *
 *  If the VM reads a device with a different type than the detected one - the device "TypeIndex" will be changed accordingly.
 *
 */


/*! \page InputLibraryDeviceTypes Device Types
 *
 *  <hr size="1"/>
 *
 \verbatim

    The "Device Type" is a DATA8 (signed byte) used for quick reference to a devices type in the VM byte codes.
    The range is from 0 to 127 and it is possible to have 100 different device types. The value is allocated below:


    Type No     Description
    --------    ----------------------------------------------------

    0           "Don't change type" type

    1..50       Reserved for LEGO existing and future devices

    51..100     Free to 3th. party devices

    101..127    Reserved for internal use


    An additional MODE is used to change function on sensors that supports more than one function.
 \endverbatim
  \ref types
 *
 *
 *
 *
 *
 *
 *
 */


#include  <string.h>
#include  <math.h>
#include  "lms2012.h"
#include  "c_input.h"
#include  "c_output.h"
#include  "c_ui.h"
#include  "c_com.h"
#ifndef    DISABLE_DAISYCHAIN_COM_CALL
#include  "c_daisy.h"
#endif
#include  "../../c_memory/source/c_memory.h"

#ifdef    DEBUG_C_INPUT
#define   DEBUG
#define   DEBUG_TRACE_MODE_CHANGE
#define   DEBUG_C_INPUT_DAISYCHAIN
#endif

#if (HARDWARE != SIMULATION)
#include  <stdio.h>
#include  <stdlib.h>
#include  <fcntl.h>
#include  <unistd.h>
#include  <sys/mman.h>
#include  <sys/ioctl.h>
#include  <sys/stat.h>

#ifndef   DISABLE_DAISYCHAIN_COM_CALL
  static  DATA8  DaisyBuf[64];
  static  DATA8  ActLayer = 0;
  static  DATA8  MoreLayers = 0;

#endif


INPUT_GLOBALS InputInstance;	
#else
#define snprintf _snprintf
#include  <stdio.h>
#include  <stdlib.h>

INPUT_GLOBALS * gInputInstance;

void setInputInstance(INPUT_GLOBALS * _Instance)
{
  gInputInstance = _Instance;
}

INPUT_GLOBALS* getInputInstance()
{
  return gInputInstance;
}
#endif


#ifndef DISABLE_DAISYCHAIN
RESULT    cInputComSetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode);
#endif

TYPES     TypeDefault[] =
{
//   Name                    Type                   Connection     Mode  DataSets  Format  Figures  Decimals  Views   RawMin  RawMax  PctMin  PctMax  SiMin   SiMax   Time   IdValue  Pins Symbol
  { "PORT ERROR"           , TYPE_ERROR           , CONN_ERROR   , 0   , 0       , 0     , 4    ,   0     ,   1   ,   0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0,     0,      'f', ""     },
  { "NONE"                 , TYPE_NONE            , CONN_NONE    , 0   , 0       , 0     , 4    ,   0     ,   1   ,   0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0,     0,      'f', ""     },
  { "UNKNOWN"              , TYPE_UNKNOWN         , CONN_UNKNOWN , 0   , 1       , 1     , 4    ,   0     ,   1   ,   0.0, 1023.0,    0.0,  100.0,    0.0, 1023.0,    0,     0,      'f', ""     },
  { "\0" }
};

#ifdef DEBUG
#define   LINESIZE    255

void      cInputPrintLine(char *pBuffer)
{
  printf("%s",pBuffer);
}

void      cInputShowTypeData(void)
{
  char    Buffer[LINESIZE];
  UBYTE   Index;
  DATA8   Type;
  DATA8   Mode;
  DATA8   Connection;
  DATA8   LastType;
  int     Pos;


  snprintf(Buffer,LINESIZE,"//! \\page types Known Device Types\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//!\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//! <hr size=\"1\"/>\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//! Following devices are supported (except devices starting with //)\\n\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//! Devices marked with * means that the data is supplied by the device itself\\n\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//! Devices marked with # is not supported in View and Datalog apps\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//!\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//! \\verbatim\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//  Type  Mode  Name      DataSets  Format  Figures  Decimals  Views  Conn. Pins  RawMin   RawMax   PctMin  PctMax  SiMin    SiMax    Time  IdValue  Symbol\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//  ----  ----  --------  --------  ------  -------  --------  -----  ----- ----  -------  -------  ------  ------  -------  -------  ----  -------  --------\r\n");
  cInputPrintLine(Buffer);

  LastType  =  TYPE_ERROR;
  Index = 0;


  while ((Index < InputInstance.MaxDeviceTypes) && (InputInstance.TypeData[Index].Name[0] != 0))
  {
    Type  = InputInstance.TypeData[Index].Type;

    if (Type <= MAX_VALID_TYPE)
    {
      Mode  = InputInstance.TypeData[Index].Mode;
      Connection  =  InputInstance.TypeData[Index].Connection;

      if (Type != LastType)
      {
        snprintf(Buffer,LINESIZE,"\r\n");
        cInputPrintLine(Buffer);

        LastType  =  Type;
      }

      Pos   =  0;

      if (Connection == CONN_INPUT_UART)
      {
        if (Mode < InputInstance.TypeData[Index].Views)
        {
          Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"*   ");
        }
        else
        {
          Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"* # ");
        }
      }
      else
      {
        Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"    ");
      }

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%-4u",Type);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"  ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%-4u",Mode);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"  ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%-*.*s",12,12,InputInstance.TypeData[Index].Name);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"  ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%u",InputInstance.TypeData[Index].DataSets);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"       ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%u",InputInstance.TypeData[Index].Format);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"     ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%u",InputInstance.TypeData[Index].Figures);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"        ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%u",InputInstance.TypeData[Index].Decimals);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"         ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%u",InputInstance.TypeData[Index].Views);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"      ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%3u",Connection);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"   ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"0x%02X",InputInstance.TypeData[Index].Pins);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos," ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%8.1f",InputInstance.TypeData[Index].RawMin);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos," ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%8.1f",InputInstance.TypeData[Index].RawMax);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"    ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%4.0f",InputInstance.TypeData[Index].PctMin);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"    ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%4.0f",InputInstance.TypeData[Index].PctMax);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos," ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%8.1f",InputInstance.TypeData[Index].SiMin);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos," ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%8.1f",InputInstance.TypeData[Index].SiMax);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"  ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%4u",InputInstance.TypeData[Index].InvalidTime);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"    ");
      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%5u",InputInstance.TypeData[Index].IdValue);

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"  ");

      if (InputInstance.TypeData[Index].Symbol[0])
      {
        Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"%s",InputInstance.TypeData[Index].Symbol);
      }
      else
      {
        Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"_");
      }

      Pos  +=  snprintf(&Buffer[Pos],LINESIZE - Pos,"\r\n");
      cInputPrintLine(Buffer);
    }
    Index++;
  }
  snprintf(Buffer,LINESIZE,"\r\n");
  cInputPrintLine(Buffer);

  snprintf(Buffer,LINESIZE,"//!  \\endverbatim\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//!  See connection types \\ref connectiontypes \"Conn.\"\r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//!  \r\n");
  cInputPrintLine(Buffer);
  snprintf(Buffer,LINESIZE,"//!  \\n\r\n");
  cInputPrintLine(Buffer);

}
#endif


void      Memcpy(void *pDest,const void *pSrc,size_t No)
{
#ifndef DISABLE_AD_WORD_PROTECT
  UWORD   *pWDest;
  UWORD   *pWSrc;
  ULONG   *pLDest;
  ULONG   *pLSrc;

  if (((No & 3) == 0) && (((int)pDest & 3) == 0) && (((int)pSrc & 3) == 0))
  {
    pLDest  =  (ULONG*)pDest;
    pLSrc   =  (ULONG*)pSrc;
    while (No)
    {
      *pLDest  =  *pLSrc;
      pLDest++;
      pLSrc++;
      No -=  4;
    }
  }
  else
  {
    if (((No & 1) == 0) && (((int)pDest & 1) == 0) && (((int)pSrc & 1) == 0))
    {
      pWDest  =  (UWORD*)pDest;
      pWSrc   =  (UWORD*)pSrc;
      while (No)
      {
        *pWDest  =  *pWSrc;
        pWDest++;
        pWSrc++;
        No -=  2;
      }
    }
    else
    {
      memcpy(pDest,pSrc,No);
    }
  }
#else
  memcpy(pDest,pSrc,No);
#endif
}


IMGDATA CLR_LAYER_CLR_CHANGES[]       = { opINPUT_DEVICE,CLR_CHANGES,0,0,opINPUT_DEVICE,CLR_CHANGES,0,1,opINPUT_DEVICE,CLR_CHANGES,0,2,opINPUT_DEVICE,CLR_CHANGES,0,3,opOBJECT_END };
IMGDATA CLR_LAYER_CLR_BUMBED[]        = { opUI_BUTTON,FLUSH,opOBJECT_END };
IMGDATA CLR_LAYER_OUTPUT_RESET[]      = { opOUTPUT_RESET,0,15,opOBJECT_END };
IMGDATA CLR_LAYER_OUTPUT_CLR_COUNT[]  = { opOUTPUT_CLR_COUNT,0,15,opOBJECT_END };
IMGDATA CLR_LAYER_INPUT_WRITE[]       = { opINPUT_WRITE,0,0,1,DEVCMD_RESET,opINPUT_WRITE,0,1,1,DEVCMD_RESET,opINPUT_WRITE,0,2,1,DEVCMD_RESET,opINPUT_WRITE,0,3,1,DEVCMD_RESET,opOBJECT_END };


void      ClrLayer(void)
{
  ExecuteByteCode(CLR_LAYER_CLR_CHANGES,NULL,NULL);
  ExecuteByteCode(CLR_LAYER_CLR_BUMBED,NULL,NULL);
  ExecuteByteCode(CLR_LAYER_OUTPUT_RESET,NULL,NULL);
  ExecuteByteCode(CLR_LAYER_OUTPUT_CLR_COUNT,NULL,NULL);
  ExecuteByteCode(CLR_LAYER_INPUT_WRITE,NULL,NULL);
}


IMGDATA STOP_LAYER[]   =  { opOUTPUT_PRG_STOP, opOBJECT_END };

void      StopLayer(void)
{
  ExecuteByteCode(STOP_LAYER,NULL,NULL);
}


//                DEVICE MAPPING
//
// Device         0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
//
// Layer          0   0   0   0   1   1   1   1   2   2   2   2   3   3   3   3   0   0   0   0   1   1   1   1   2   2   2   2   3   3   3   3
// Port (INPUT)   0   1   2   3   0   1   2   3   0   1   2   3   0   1   2   3   16  17  18  19  16  17  18  19  16  17  18  19  16  17  18  19
// Port (OUTPUT)  -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   0   1   2   3   0   1   2   3   0   1   2   3   0   1   2   3
// Output         0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1

RESULT    cInputExpandDevice(DATA8 Device,DATA8 *pLayer,DATA8 *pPort,DATA8 *pOutput)
{ // pPort: pOutput=0 -> 0..3 , pOutput=1 -> 0..3

  RESULT  Result = FAIL;

  if ((Device >= 0) && (Device < DEVICES))
  {
    if (Device >= INPUT_DEVICES)
    { // OUTPUT

      *pOutput  =  1;
      Device   -=  INPUT_DEVICES;
      *pPort    =  (Device % OUTPUT_PORTS);
      *pPort   +=  INPUT_DEVICES;
    }
    else
    { // INPUT

      *pOutput  =  0;
      *pPort    =  Device % INPUT_PORTS;

    }
    *pLayer     =  Device / CHAIN_DEPT;

    Result      =  OK;
  }

  return (Result);
}


RESULT    cInputCompressDevice(DATA8 *pDevice,UBYTE Layer,UBYTE Port)
{ // Port: Input 0..3 , Output 16..19

  RESULT  Result = FAIL;

  if (Port >= INPUT_DEVICES)
  { // OUTPUT

    *pDevice  =  (OUTPUT_PORTS * Layer);
    *pDevice +=  Port;

  }
  else
  { // INPUT

    *pDevice  =  (INPUT_PORTS * Layer);
    *pDevice +=  Port;
  }

  if ((*pDevice >= 0) && (*pDevice < DEVICES))
  {
    Result  =  OK;
  }

  return (Result);
}


RESULT    cInputInsertNewIicString(DATA8 Type,DATA8 Mode,DATA8 *pManufacturer,DATA8 *pSensorType,DATA8 SetupLng,ULONG SetupString,DATA8 PollLng,ULONG PollString,DATA8 ReadLng)
{
  RESULT  Result    = FAIL;  // FAIL=full, OK=new, BUSY=found
  IICSTR  *pTmp;
  UWORD   Index     = 0;

  if ((Type >= 0) && (Type < (MAX_DEVICE_TYPE + 1)) && (Mode >= 0) && (Mode < MAX_DEVICE_MODES))
  { // Type and mode valid


    while ((Index < InputInstance.IicDeviceTypes) && (Result != BUSY))
    { // trying to find device type

      if ((InputInstance.IicString[Index].Type == Type) && (InputInstance.IicString[Index].Mode == Mode))
      { // match on type and mode

        Result    =  BUSY;
      }
      Index++;
    }
    if (Result != BUSY)
    { // device type not found

      if (InputInstance.IicDeviceTypes < MAX_DEVICE_TYPES)
      { // Allocate room for a new type/mode

        if (cMemoryRealloc((void*)InputInstance.IicString,(void**)&pTmp,(DATA32)(sizeof(IICSTR) * (InputInstance.IicDeviceTypes + 1))) == OK)
        { // Success

          InputInstance.IicString  =  pTmp;

          InputInstance.IicString[Index].Type          =  Type;
          InputInstance.IicString[Index].Mode          =  Mode;
          snprintf((char*)InputInstance.IicString[Index].Manufacturer,IIC_NAME_LENGTH + 1,"%s",(char*)pManufacturer);
          snprintf((char*)InputInstance.IicString[Index].SensorType,IIC_NAME_LENGTH + 1,"%s",(char*)pSensorType);
          InputInstance.IicString[Index].SetupLng      =  SetupLng;
          InputInstance.IicString[Index].SetupString   =  SetupString;
          InputInstance.IicString[Index].PollLng       =  PollLng;
          InputInstance.IicString[Index].PollString    =  PollString;
          InputInstance.IicString[Index].ReadLng       =  ReadLng;
//          printf("cInputInsertNewIicString  %-3u %01u IIC %u 0x%08X %u 0x%08X %s %s\r\n",Type,Mode,SetupLng,SetupString,PollLng,PollString,pManufacturer,pSensorType);

          InputInstance.IicDeviceTypes++;
          Result    =  OK;
        }
      }
    }
    if (Result == FAIL)
    { // No room for type/mode

      LogErrorNumber(TYPEDATA_TABEL_FULL);
    }
  }
  else
  { // Type or mode invalid

    printf("Iic  error %d: m=%d IIC\r\n",Type,Mode);
  }

  return (Result);
}


RESULT    cInputGetIicString(DATA8 Type,DATA8 Mode,IICSTR *IicStr)
{
  RESULT  Result    = FAIL;  // FAIL=full, OK=new, BUSY=found
  UWORD   Index     = 0;


  (*IicStr).SetupLng      =  0;
  (*IicStr).SetupString   =  0;
  (*IicStr).PollLng       =  0;
  (*IicStr).PollString    =  0;
  (*IicStr).ReadLng       =  0;

  if ((Type >= 0) && (Type < (MAX_DEVICE_TYPE + 1)) && (Mode >= 0) && (Mode < MAX_DEVICE_MODES))
  { // Type and mode valid


    while ((Index < InputInstance.IicDeviceTypes) && (Result != OK))
    { // trying to find device type

      if ((InputInstance.IicString[Index].Type == Type) && (InputInstance.IicString[Index].Mode == Mode))
      { // match on type and mode

        (*IicStr).Type          =  Type;
        (*IicStr).Mode          =  Mode;
        snprintf((char*)IicStr->Manufacturer, sizeof(IicStr->Manufacturer),"%s", (char*)InputInstance.IicString[Index].Manufacturer);
        snprintf((char*)IicStr->SensorType, sizeof(IicStr->SensorType), "%s",(char*)InputInstance.IicString[Index].SensorType);
        (*IicStr).SetupLng      =  InputInstance.IicString[Index].SetupLng;
        (*IicStr).SetupString   =  InputInstance.IicString[Index].SetupString;
        (*IicStr).PollLng       =  InputInstance.IicString[Index].PollLng;
        (*IicStr).PollString    =  InputInstance.IicString[Index].PollString;
        (*IicStr).ReadLng       =  InputInstance.IicString[Index].ReadLng;

        Result          =  OK;
      }
      Index++;
    }
  }

  return (Result);
}


RESULT    cInputGetNewTypeDataPointer(SBYTE *pName,DATA8 Type,DATA8 Mode,DATA8 Connection,TYPES **ppPlace)
{
  RESULT  Result    = FAIL;  // FAIL=full, OK=new, BUSY=found
  UWORD   Index     = 0;

  *ppPlace  =  NULL;

  if ((Type >= 0) && (Type < (MAX_DEVICE_TYPE + 1)) && (Mode >= 0) && (Mode < MAX_DEVICE_MODES))
  { // Type and mode valid

    while ((Index < InputInstance.MaxDeviceTypes) && (Result != BUSY))
    { // trying to find device type

      if ((InputInstance.TypeData[Index].Type == Type) && (InputInstance.TypeData[Index].Mode == Mode) && (InputInstance.TypeData[Index].Connection == Connection))
      { // match on type, mode and connection

        *ppPlace  =  &InputInstance.TypeData[Index];
        Result    =  BUSY;
      }
      Index++;
    }
    if (Result != BUSY)
    { // device type not found

      if (InputInstance.MaxDeviceTypes < MAX_DEVICE_TYPES)
      { // Allocate room for a new type/mode

        if (cMemoryRealloc((void*)InputInstance.TypeData,(void**)ppPlace,(DATA32)(sizeof(TYPES) * (InputInstance.MaxDeviceTypes + 1))) == OK)
        { // Success

          InputInstance.TypeData  =  *ppPlace;

          *ppPlace  =  &InputInstance.TypeData[InputInstance.MaxDeviceTypes];
          InputInstance.TypeModes[Type]++;
          InputInstance.MaxDeviceTypes++;
          Result    =  OK;
        }
      }
    }
    if (Result == FAIL)
    { // No room for type/mode

      LogErrorNumber(TYPEDATA_TABEL_FULL);
    }
  }
  else
  { // Type or mode invalid

    printf("Type error %d: m=%d c=%d n=%s\r\n",Type,Mode,Connection,pName);
  }

  return (Result);
}


RESULT    cInputInsertTypeData(char *pFilename)
{
  RESULT  Result = FAIL;
  LFILE   *pFile;
  char    Buf[256];
  char    Name[256];
  char    Symbol[256];
  char    Manufacturer[256];
  char    SensorType[256];
  unsigned int Type;
  unsigned int Connection;
  unsigned int Mode;
  unsigned int DataSets;
  unsigned int Format;
  unsigned int Figures;
  unsigned int Decimals;
  unsigned int Views;
  unsigned int Pins;
  unsigned int Time;
  unsigned int IdValue;
  unsigned int SetupLng;
  unsigned int SetupString;
  unsigned int PollLng;
  unsigned int PollString;
  int ReadLng;
  char    *Str;
  TYPES   Tmp;
  TYPES   *pTypes;
  int     Count;

  pFile = fopen (pFilename,"r");
  if (pFile != NULL)
  {
    do
    {
      Str       =  fgets(Buf,255,pFile);
      Buf[255]  =  0;
      if (Str != NULL)
      {
        if ((Buf[0] != '/') && (Buf[0] != '*'))
        {
          Count  =  sscanf(Buf,"%u %u %s %u %u %u %u %u %u %x %f %f %f %f %f %f %u %u %s",&Type,&Mode,Name,&DataSets,&Format,&Figures,&Decimals,&Views,&Connection,&Pins,&Tmp.RawMin,&Tmp.RawMax,&Tmp.PctMin,&Tmp.PctMax,&Tmp.SiMin,&Tmp.SiMax,&Time,&IdValue,Symbol);
          if (Count == TYPE_PARAMETERS)
          {
            Tmp.Type         =  (DATA8)Type;
            Tmp.Mode         =  (DATA8)Mode;
            Tmp.DataSets     =  (DATA8)DataSets;
            Tmp.Format       =  (DATA8)Format;
            Tmp.Figures      =  (DATA8)Figures;
            Tmp.Decimals     =  (DATA8)Decimals;
            Tmp.Connection   =  (DATA8)Connection;
            Tmp.Views        =  (DATA8)Views;
            Tmp.Pins         =  (DATA8)Pins;
            Tmp.InvalidTime  =  (UWORD)Time;
            Tmp.IdValue      =  (UWORD)IdValue;

            Result  =  cInputGetNewTypeDataPointer((SBYTE*)Name,(DATA8)Type,(DATA8)Mode,(DATA8)Connection,&pTypes);
//            printf("cInputTypeDataInit\r\n");
            if (Result == OK)
            {
              (*pTypes)  =  Tmp;

              Count  =  0;
              while ((Name[Count]) && (Count < TYPE_NAME_LENGTH))
              {
                if (Name[Count] == '_')
                {
                  (*pTypes).Name[Count]  =  ' ';
                }
                else
                {
                  (*pTypes).Name[Count]  =  Name[Count];
                }
                Count++;
              }
              (*pTypes).Name[Count]    =  0;

              if (Symbol[0] == '_')
              {
                (*pTypes).Symbol[0]  =  0;
              }
              else
              {
                Count  =  0;
                while ((Symbol[Count]) && (Count < SYMBOL_LENGTH))
                {
                  if (Symbol[Count] == '_')
                  {
                    (*pTypes).Symbol[Count]  =  ' ';
                  }
                  else
                  {
                    (*pTypes).Symbol[Count]  =  Symbol[Count];
                  }
                  Count++;
                }
                (*pTypes).Symbol[Count]    =  0;
              }
              if (Tmp.Connection == CONN_NXT_IIC)
              { // NXT IIC sensor

                // setup string + poll string
                // 3 0x01420000 2 0x01000000

                Count  =  sscanf(Buf,"%u %u %s %u %u %u %u %u %u %x %f %f %f %f %f %f %u %u %s %s %s %u %X %u %X %d",&Type,&Mode,Name,&DataSets,&Format,&Figures,&Decimals,&Views,&Connection,&Pins,&Tmp.RawMin,&Tmp.RawMax,&Tmp.PctMin,&Tmp.PctMax,&Tmp.SiMin,&Tmp.SiMax,&Time,&IdValue,Symbol,Manufacturer,SensorType,&SetupLng,&SetupString,&PollLng,&PollString,&ReadLng);
                if (Count == (TYPE_PARAMETERS + 7))
                {
                  cInputInsertNewIicString(Type,Mode,(DATA8*)Manufacturer,(DATA8*)SensorType,(DATA8)SetupLng,(ULONG)SetupString,(DATA8)PollLng,(ULONG)PollString,(DATA8)ReadLng);
//                  printf("%02u %01u IIC %u 0x%08X %u 0x%08X %u\r\n",Type,Mode,SetupLng,SetupString,PollLng,PollString,ReadLng);
                }
              }
            }
          }
        }
      }
    }
    while (Str != NULL);
    fclose (pFile);

    Result  =  OK;
  }

  return (Result);
}


void      cInputTypeDataInit(void)
{
  char    PrgNameBuf[255];
  UWORD   Index   = 0;
  UBYTE   TypeDataFound = 0;

  // Set TypeMode to mode = 0
  Index  =  0;
  while (Index < (MAX_DEVICE_TYPE + 1))
  {
    InputInstance.TypeModes[Index]  =  0;
    Index++;
  }

  // Insert default types into TypeData
  Index  =  0;
  while ((Index < (InputInstance.MaxDeviceTypes + 1)) && (TypeDefault[Index].Name[0] != 0))
  {
    InputInstance.TypeData[Index]  =  TypeDefault[Index];

    snprintf((char*)InputInstance.TypeData[Index].Name,TYPE_NAME_LENGTH + 1,"%s",(char*)TypeDefault[Index].Name);

    if (InputInstance.TypeData[Index].Type == TYPE_NONE)
    {
      InputInstance.NoneIndex  =  Index;
    }
    if (InputInstance.TypeData[Index].Type == TYPE_UNKNOWN)
    {
      InputInstance.UnknownIndex  =  Index;
    }
    InputInstance.TypeModes[InputInstance.TypeData[Index].Type]++;
    Index++;
  }

//  printf("Search start\r\n");
  snprintf(PrgNameBuf,vmFILENAMESIZE,"%s/%s%s",vmSETTINGS_DIR,TYPEDATE_FILE_NAME,EXT_CONFIG);

  if (cInputInsertTypeData(PrgNameBuf) == OK)
  {
    TypeDataFound  =  1;
  }

  for (Index = TYPE_THIRD_PARTY_START;Index <= TYPE_THIRD_PARTY_END;Index++)
  {
    snprintf(PrgNameBuf,vmFILENAMESIZE,"%s/%s%02d%s",vmSETTINGS_DIR,TYPEDATE_FILE_NAME,Index,EXT_CONFIG);
    if (cInputInsertTypeData(PrgNameBuf) == OK)
    {
      TypeDataFound  =  1;
    }
  }
//  printf("Done\r\n");

  if (!TypeDataFound)
  {
    LogErrorNumber(TYPEDATA_FILE_NOT_FOUND);
  }
}


RESULT    cInputSetupDevice(DATA8 Device,DATA8 Repeat,DATA16 Time,DATA8 WrLng,DATA8 *pWrData,DATA8 RdLng,DATA8 *pRdData)
{

  InputInstance.IicDat.Result  =  FAIL;

  if (Device < INPUTS)
  {
    if (InputInstance.DeviceData[Device].Connection == CONN_NXT_IIC)
    { // Device is an IIC device

      if (InputInstance.IicFile >= MIN_HANDLE)
      { // Driver installed

        if (WrLng > MAX_DEVICE_DATALENGTH)
        {
          WrLng  =  MAX_DEVICE_DATALENGTH;
        }
        if (RdLng > MAX_DEVICE_DATALENGTH)
        {
          RdLng  =  MAX_DEVICE_DATALENGTH;
        }

        if (Time != 0)
        {
          if (Time < MIN_IIC_REPEAT_TIME)
          {
            Time  =  MIN_IIC_REPEAT_TIME;
          }
          if (Time > MAX_IIC_REPEAT_TIME)
          {
            Time  =  MAX_IIC_REPEAT_TIME;
          }
        }

        InputInstance.IicDat.Result   =  BUSY;
        InputInstance.IicDat.Port     =  Device;
        InputInstance.IicDat.Repeat   =  Repeat;
        InputInstance.IicDat.Time     =  Time;
        InputInstance.IicDat.WrLng    =  WrLng;
        InputInstance.IicDat.RdLng    =  RdLng;

        Memcpy(&InputInstance.IicDat.WrData[0],pWrData,InputInstance.IicDat.WrLng);

        ioctl(InputInstance.IicFile,IIC_SETUP,&InputInstance.IicDat);

        if (InputInstance.IicDat.Result == OK)
        {
          Memcpy(pRdData,&InputInstance.IicDat.RdData[0],InputInstance.IicDat.RdLng);
        }
      }
    }
  }

  return (InputInstance.IicDat.Result);
}


#ifndef DISABLE_DAISYCHAIN
/* Daisy chain interface

c_com
RESULT    cComSetDeviceInfo(DATA8 Length,UBYTE *pInfo);                                                     // Write type data to chain
RESULT    cComGetDeviceInfo(DATA8 Length,UBYTE *pInfo);                                                     // Read type data from chain
RESULT    cComSetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode);                                  // Write mode to chain
RESULT    cComGetDeviceData(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData);    // Read device data from chain

c_input
RESULT    cInputGetDeviceData(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData);  // Chain read device data
*/
#endif

RESULT    cInputGetData(DATA8 Layer,DATA8 Port,DATA16 Time,DATA16 *pInit,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData);


RESULT    cInputFindDumbInputDevice(DATA8 Device,DATA8 Type,DATA8 Mode,UWORD *pTypeIndex)
{
  RESULT  Result = FAIL;
  UWORD   IdValue = 0;
  UWORD   Index = 0;
  UWORD   Tmp;

  // get actual id value
  IdValue  =  CtoV((*InputInstance.pAnalog).InPin1[Device]);

  while ((Index < InputInstance.MaxDeviceTypes) && (Result != OK))
  {
    Tmp  =  InputInstance.TypeData[Index].IdValue;

    if (Tmp >= IN1_ID_HYSTERESIS)
    {
      if ((IdValue >= (Tmp - IN1_ID_HYSTERESIS)) && (IdValue < (Tmp + IN1_ID_HYSTERESIS)))
      { // id value match

        if (Type == TYPE_UNKNOWN)
        { // first search

          // "type data" entry found
          *pTypeIndex  =  Index;

        }
        else
        { // next search

          if (Type == InputInstance.TypeData[Index].Type)
          { //

            *pTypeIndex  =  Index;
          }
        }
        if (Mode == InputInstance.TypeData[Index].Mode)
        { // mode match

          // "type data" entry found
          *pTypeIndex  =  Index;

          // skip looping
          Result  =  OK;
        }
      }
    }
    Index++;
  }

  return (Result);
}


RESULT    cInputFindDumbOutputDevice(DATA8 Device,DATA8 Type,DATA8 Mode,UWORD *pTypeIndex)
{
  RESULT  Result = FAIL;
  UWORD   IdValue = 0;
  UWORD   Index = 0;
  UWORD   Tmp;

  // get actual id value
  IdValue  =  (*InputInstance.pAnalog).OutPin5Low[Device - INPUT_DEVICES];

  while ((Index < InputInstance.MaxDeviceTypes) && (Result != OK))
  {
    Tmp  =  InputInstance.TypeData[Index].IdValue;

    if (Tmp >= OUT5_ID_HYSTERESIS)
    {
      if ((IdValue >= (Tmp - OUT5_ID_HYSTERESIS)) && (IdValue < (Tmp + OUT5_ID_HYSTERESIS)))
      { // id value match

        // "type data" entry found
        *pTypeIndex  =  Index;

        if (Mode == InputInstance.TypeData[Index].Mode)
        { // mode match

          // "type data" entry found
          *pTypeIndex  =  Index;

          // skip looping
          Result  =  OK;
        }
      }
    }
    Index++;
  }

  return (Result);
}


RESULT    cInputFindDevice(DATA8 Type,DATA8 Mode,UWORD *pTypeIndex)
{
  RESULT  Result = FAIL;
  UWORD   Index = 0;

  while ((Index < InputInstance.MaxDeviceTypes) && (Result != OK))
  {
    if (Type == InputInstance.TypeData[Index].Type)
    { // type match

      *pTypeIndex  =  Index;

      if (Mode == InputInstance.TypeData[Index].Mode)
      { // mode match

        // "type data" entry found
        *pTypeIndex  =  Index;

        // skip looping
        Result  =  OK;
      }
    }
    Index++;
  }

  return (Result);
}


void      cInputResetDevice(DATA8 Device,UWORD TypeIndex)
{
  PRGID   TmpPrgId;

  InputInstance.DeviceType[Device]              =  InputInstance.TypeData[TypeIndex].Type;
  InputInstance.DeviceMode[Device]              =  InputInstance.TypeData[TypeIndex].Mode;

  InputInstance.DeviceData[Device].InvalidTime  =  InputInstance.TypeData[TypeIndex].InvalidTime;
  InputInstance.DeviceData[Device].DevStatus    =  BUSY;

  // save new type
  InputInstance.DeviceData[Device].TypeIndex    =  TypeIndex;

  if (InputInstance.DeviceType[Device] != TYPE_UNKNOWN)
  {
    // configuration changed
    for (TmpPrgId = 0;TmpPrgId < MAX_PROGRAMS;TmpPrgId++)
    {
      InputInstance.ConfigurationChanged[TmpPrgId]++;
    }
  }
}


void      cInputSetDeviceType(DATA8 Device,DATA8 Type, DATA8 Mode,int Line)
{
  UWORD   Index;
  char    Buf[INPUTS * 2 + 1];
  UWORD   TypeIndex;
  DATA8   Layer;
  DATA8   Port;
  DATA8   Output;

  if (cInputExpandDevice(Device,&Layer,&Port,&Output) == OK)
  { // Device within range

    if (InputInstance.DeviceData[Device].Connection == CONN_NONE)
    {
      Type       =  TYPE_NONE;
    }
    // default type is unknown!
    TypeIndex  =  InputInstance.UnknownIndex;

    if (Layer == 0)
    { // Local device

      if (Output == 0)
      { // Input device

        // TRY TO FIND DUMB INPUT DEVICE
        if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_DUMB)
        { // search "type data" for matching "dumb" input device

          cInputFindDumbInputDevice(Device,Type,Mode,&TypeIndex);
        }

        // IF NOT FOUND YET - TRY TO FIND TYPE ANYWAY
        if (TypeIndex == InputInstance.UnknownIndex)
        { // device not found or not "dumb" input/output device

          cInputFindDevice(Type,Mode,&TypeIndex);
        }

        if (InputInstance.DeviceData[Device].TypeIndex != TypeIndex)
        { // type or mode has changed

          cInputResetDevice(Device,TypeIndex);

          (*InputInstance.pUart).Status[Device]      &= ~UART_DATA_READY;
          (*InputInstance.pIic).Status[Device]       &= ~IIC_DATA_READY;
          (*InputInstance.pAnalog).Updated[Device]    =  0;

          if (InputInstance.DeviceData[Device].Connection == CONN_NXT_IIC)
          { // Device is an IIC device

            cInputGetIicString(InputInstance.DeviceType[Device],InputInstance.DeviceMode[Device],&InputInstance.IicStr);
            InputInstance.IicStr.Port  =  Device;
            InputInstance.IicStr.Time  =  InputInstance.DeviceData[Device].InvalidTime;

            if ((InputInstance.IicStr.SetupLng) || (InputInstance.IicStr.PollLng))
            {
//              printf("%u %-4u %-3u %01u IIC %u 0x%08X %u 0x%08X %d\r\n",InputInstance.IicStr.Port,InputInstance.IicStr.Time,InputInstance.IicStr.Type,InputInstance.IicStr.Mode,InputInstance.IicStr.SetupLng,InputInstance.IicStr.SetupString,InputInstance.IicStr.PollLng,InputInstance.IicStr.PollString,InputInstance.IicStr.ReadLng);

              if (InputInstance.IicFile >= MIN_HANDLE)
              {
                ioctl(InputInstance.IicFile,IIC_SET,&InputInstance.IicStr);
              }
            }
          }

          // SETUP DRIVERS
          for (Index = 0;Index < INPUTS;Index++)
          { // Initialise pin setup string to do nothing

            Buf[Index]    =  '-';
          }
          Buf[Index]      =  0;

          // insert "pins" in setup string
          Buf[Device]     =  InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Pins;

          // write setup string to "Device Connection Manager" driver
          if (InputInstance.DcmFile >= MIN_HANDLE)
          {
            write(InputInstance.DcmFile,Buf,INPUTS);
          }

          for (Index = 0;Index < INPUTS;Index++)
          { // build setup string for UART and IIC driver

            InputInstance.DevCon.Connection[Index]  =  InputInstance.DeviceData[Index].Connection;
            InputInstance.DevCon.Type[Index]        =  InputInstance.TypeData[InputInstance.DeviceData[Index].TypeIndex].Type;
            InputInstance.DevCon.Mode[Index]        =  InputInstance.TypeData[InputInstance.DeviceData[Index].TypeIndex].Mode;
          }

          // write setup string to "UART Device Controller" driver
          if (InputInstance.UartFile >= MIN_HANDLE)
          {
            ioctl(InputInstance.UartFile,UART_SET_CONN,&InputInstance.DevCon);
          }
          if (InputInstance.IicFile >= MIN_HANDLE)
          {
            ioctl(InputInstance.IicFile,IIC_SET_CONN,&InputInstance.DevCon);
          }

#ifdef DEBUG_TRACE_MODE_CHANGE
          printf("c_input   cInputSetDeviceType: I   D=%-3d C=%-3d Ti=%-3d N=%s\r\n",Device,InputInstance.DeviceData[Device].Connection,InputInstance.DeviceData[Device].TypeIndex,InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Name);
#endif
        }
      }
      else
      { // Output device

        // TRY TO FIND DUMB OUTPUT DEVICE
        if (InputInstance.DeviceData[Device].Connection == CONN_OUTPUT_DUMB)
        { // search "type data" for matching "dumb" output device

          cInputFindDumbInputDevice(Device,Type,Mode,&TypeIndex);
        }

        // IF NOT FOUND YET - TRY TO FIND TYPE ANYWAY
        if (TypeIndex == InputInstance.UnknownIndex)
        { // device not found or not "dumb" input/output device

          cInputFindDevice(Type,Mode,&TypeIndex);
        }

        if (InputInstance.DeviceData[Device].TypeIndex != TypeIndex)
        { // type or mode has changed

          cInputResetDevice(Device,TypeIndex);

          for (Index = 0;Index < OUTPUT_PORTS;Index++)
          { // build setup string "type" for output

            Buf[Index]    =  InputInstance.DeviceType[Index + INPUT_DEVICES];
          }
          Buf[Index]      =  0;
          cOutputSetTypes(Buf);

#ifdef DEBUG_TRACE_MODE_CHANGE
          printf("c_input   cInputSetDeviceType: O   D=%-3d C=%-3d Ti=%-3d N=%s\r\n",Device,InputInstance.DeviceData[Device].Connection,InputInstance.DeviceData[Device].TypeIndex,InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Name);
#endif
        }
      }


    }
    else
    { // Not local device

      // IF NOT FOUND YET - TRY TO FIND TYPE ANYWAY
      if (TypeIndex == InputInstance.UnknownIndex)
      { // device not found or not "dumb" input/output device

        cInputFindDevice(Type,Mode,&TypeIndex);
      }

      if (InputInstance.DeviceData[Device].TypeIndex != TypeIndex)
      { // type or mode has changed

#ifndef DISABLE_DAISYCHAIN

        if ((InputInstance.DeviceData[Device].Connection != CONN_NONE) && (InputInstance.DeviceData[Device].Connection != CONN_ERROR) && (InputInstance.DeviceType[Device] != TYPE_UNKNOWN))
        {
          if (cDaisyReady() != BUSY)
          {

            cInputResetDevice(Device,TypeIndex);
            InputInstance.DeviceData[Device].InvalidTime  =  DAISYCHAIN_MODE_TIME;
            cInputComSetDeviceType(Layer,Port,InputInstance.DeviceType[Device],InputInstance.DeviceMode[Device]);

#ifdef DEBUG_TRACE_MODE_CHANGE
            printf("c_input   cInputSetDeviceType: D   D=%-3d C=%-3d Ti=%-3d N=%s\r\n",Device,InputInstance.DeviceData[Device].Connection,InputInstance.DeviceData[Device].TypeIndex,InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Name);
#endif

#ifdef ENABLE_STATUS_TEST
            if (Device == TESTDEVICE)
            {
              VMInstance.Status &= ~0x08;
            }
#endif
          }
          else
          {
#ifdef ENABLE_STATUS_TEST
            if (Device == TESTDEVICE)
            {
              VMInstance.Status |=  0x08;
            }
#endif
            InputInstance.DeviceData[Device].DevStatus    =  BUSY;
          }
        }
        else
        {
          cInputResetDevice(Device,TypeIndex);
        }

#else

        cInputResetDevice(Device,TypeIndex);

#ifdef DEBUG_TRACE_MODE_CHANGE
        printf("c_input   cInputSetDeviceType: D   D=%-3d C=%-3d Ti=%-3d N=%s\r\n",Device,InputInstance.DeviceData[Device].Connection,InputInstance.DeviceData[Device].TypeIndex,InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Name);
#endif
#endif

      }
    }
#ifdef BUFPRINTSIZE
    BufPrint('p',"%-4d - D=%d SetDevice T=%-3d M=%d C=%-3d\r\n",Line,Device,InputInstance.DeviceType[Device],InputInstance.DeviceMode[Device],InputInstance.DeviceData[Device].Connection);
#endif
  }
}


void      cInputCalDataInit(void)
{
  DATA8   Type;
  DATA8   Mode;
  int     File;
  char    PrgNameBuf[vmFILENAMESIZE];

  snprintf(PrgNameBuf,vmFILENAMESIZE,"%s/%s%s",vmSETTINGS_DIR,vmCALDATA_FILE_NAME,vmEXT_CONFIG);
  File  =  open(PrgNameBuf,O_RDONLY);
  if (File >= MIN_HANDLE)
  {
    if (read(File,(UBYTE*)&InputInstance.Calib,sizeof(InputInstance.Calib)) != sizeof(InputInstance.Calib))
    {
      close (File);
      File  =  -1;
    }
    else
    {
      close (File);
    }
  }
  if (File < 0)
  {
    for (Type = 0;Type < (MAX_DEVICE_TYPE);Type++)
    {
      for (Mode = 0;Mode < MAX_DEVICE_MODES;Mode++)
      {
        InputInstance.Calib[Type][Mode].InUse  =  0;
      }
    }
  }
}


void      cInputCalDataExit(void)
{
  int     File;
  char    PrgNameBuf[vmFILENAMESIZE];

  snprintf(PrgNameBuf,vmFILENAMESIZE,"%s/%s%s",vmSETTINGS_DIR,vmCALDATA_FILE_NAME,vmEXT_CONFIG);
  File  =  open(PrgNameBuf,O_CREAT | O_WRONLY | O_TRUNC,SYSPERMISSIONS);
  if (File >= MIN_HANDLE)
  {
    write(File,(UBYTE*)&InputInstance.Calib,sizeof(InputInstance.Calib));
    close(File);
  }
}


#ifndef DISABLE_OLD_COLOR

#define   FALSE                         0
#define   TRUE                          1

#define   SENSOR_RESOLUTION             1023L

/* Remember this is ARM AD converter  - 3,3 VDC as max voltage      */
/* When in color mode background value is substracted => min = 0!!! */
#define   AD_MAX                        2703L
#define   AD_FS                         3300L

#define   COLORSENSORBGMIN              (214/(AD_FS/AD_MAX))
#define   COLORSENSORMIN                (1L/(AD_FS/AD_MAX)) /* 1 inserted else div 0 (1L/(120/AD_MAX)) */
#define   COLORSENSORMAX                ((AD_MAX * AD_FS)/3300)
#define   COLORSENSORPCTDYN             (UBYTE)(((COLORSENSORMAX - COLORSENSORMIN) * 100L)/AD_MAX)
#define   COLORSENSORBGPCTDYN           (UBYTE)(((COLORSENSORMAX - COLORSENSORBGMIN) * 100L)/AD_MAX)

void      cInputCalcFullScale(UWORD *pRawVal, UWORD ZeroPointOffset, UBYTE PctFullScale, UBYTE InvStatus)
{
  if (*pRawVal >= ZeroPointOffset)
  {
    *pRawVal -= ZeroPointOffset;
  }
  else
  {
    *pRawVal = 0;
  }

  *pRawVal = (*pRawVal * 100)/PctFullScale;
  if (*pRawVal > SENSOR_RESOLUTION)
  {
    *pRawVal = SENSOR_RESOLUTION;
  }
  if (TRUE == InvStatus)
  {
    *pRawVal = SENSOR_RESOLUTION - *pRawVal;
  }
}


void      cInputCalibrateColor(COLORSTRUCT *pC, UWORD *pNewVals)
{
  UBYTE CalRange;

  if ((pC->ADRaw[BLANK]) < pC->CalLimits[1])
  {
    CalRange = 2;
  }
  else
  {
    if ((pC->ADRaw[BLANK]) < pC->CalLimits[0])
    {
      CalRange = 1;
    }
    else
    {
      CalRange = 0;
    }
  }

  pNewVals[RED] = 0;
  if ((pC->ADRaw[RED]) > (pC->ADRaw[BLANK]))
  {
    pNewVals[RED] = (UWORD)(((ULONG)((pC->ADRaw[RED]) - (pC->ADRaw[BLANK])) * (pC->Calibration[CalRange][RED])) >> 16);
  }

  pNewVals[GREEN] = 0;
  if ((pC->ADRaw[GREEN]) > (pC->ADRaw[BLANK]))
  {
     pNewVals[GREEN] = (UWORD)(((ULONG)((pC->ADRaw[GREEN]) - (pC->ADRaw[BLANK])) * (pC->Calibration[CalRange][GREEN])) >> 16);
  }

  pNewVals[BLUE] = 0;
  if ((pC->ADRaw[BLUE]) > (pC->ADRaw[BLANK]))
  {
    pNewVals[BLUE] = (UWORD)(((ULONG)((pC->ADRaw[BLUE]) -(pC->ADRaw[BLANK])) * (pC->Calibration[CalRange][BLUE])) >> 16);
  }

  pNewVals[BLANK] = (pC->ADRaw[BLANK]);
  cInputCalcFullScale(&(pNewVals[BLANK]), COLORSENSORBGMIN, COLORSENSORBGPCTDYN, FALSE);
  (pNewVals[BLANK]) = (UWORD)(((ULONG)(pNewVals[BLANK]) * (pC->Calibration[CalRange][BLANK])) >> 16);
}


DATAF     cInputCalculateColor(COLORSTRUCT *pC)
{
  DATAF   Result ;


  Result  =  DATAF_NAN;

  // Color Sensor values has been calculated -
  // now calculate the color and put it in Sensor value
  if (((pC->SensorRaw[RED]) > (pC->SensorRaw[BLUE] )) &&
      ((pC->SensorRaw[RED]) > (pC->SensorRaw[GREEN])))
  {

    // If all 3 colors are less than 65 OR (Less that 110 and bg less than 40)
    if (((pC->SensorRaw[RED])   < 65) ||
        (((pC->SensorRaw[BLANK]) < 40) && ((pC->SensorRaw[RED])  < 110)))
    {
      Result  =  (DATAF)BLACKCOLOR;
    }
    else
    {
      if (((((pC->SensorRaw[BLUE]) >> 2)  + ((pC->SensorRaw[BLUE]) >> 3) + (pC->SensorRaw[BLUE])) < (pC->SensorRaw[GREEN])) &&
          ((((pC->SensorRaw[GREEN]) << 1)) > (pC->SensorRaw[RED])))
      {
        Result  =  (DATAF)YELLOWCOLOR;
      }
      else
      {

        if ((((pC->SensorRaw[GREEN]) << 1) - ((pC->SensorRaw[GREEN]) >> 2)) < (pC->SensorRaw[RED]))
        {

          Result  =  (DATAF)REDCOLOR;
        }
        else
        {

          if ((((pC->SensorRaw[BLUE]) < 70) ||
              ((pC->SensorRaw[GREEN]) < 70)) ||
             (((pC->SensorRaw[BLANK]) < 140) && ((pC->SensorRaw[RED]) < 140)))
          {
            Result  =  (DATAF)BLACKCOLOR;
          }
          else
          {
            Result  =  (DATAF)WHITECOLOR;
          }
        }
      }
    }
  }
  else
  {

    // Red is not the dominant color
    if ((pC->SensorRaw[GREEN]) > (pC->SensorRaw[BLUE]))
    {

      // Green is the dominant color
      // If all 3 colors are less than 40 OR (Less that 70 and bg less than 20)
      if (((pC->SensorRaw[GREEN])  < 40) ||
          (((pC->SensorRaw[BLANK]) < 30) && ((pC->SensorRaw[GREEN])  < 70)))
      {
        Result  =  (DATAF)BLACKCOLOR;
      }
      else
      {
        if ((((pC->SensorRaw[BLUE]) << 1)) < (pC->SensorRaw[RED]))
        {
          Result  =  (DATAF)YELLOWCOLOR;
        }
        else
        {
          if ((((pC->SensorRaw[RED]) + ((pC->SensorRaw[RED])>>2)) < (pC->SensorRaw[GREEN])) ||
              (((pC->SensorRaw[BLUE]) + ((pC->SensorRaw[BLUE])>>2)) < (pC->SensorRaw[GREEN])))
          {
            Result  =  (DATAF)GREENCOLOR;
          }
          else
          {
            if ((((pC->SensorRaw[RED]) < 70) ||
                ((pC->SensorRaw[BLUE]) < 70)) ||
                (((pC->SensorRaw[BLANK]) < 140) && ((pC->SensorRaw[GREEN]) < 140)))
            {
              Result  =  (DATAF)BLACKCOLOR;
            }
            else
            {
              Result  =  (DATAF)WHITECOLOR;
            }
          }
        }
      }
    }
    else
    {

      // Blue is the most dominant color
      // Colors can be blue, white or black
      // If all 3 colors are less than 48 OR (Less that 85 and bg less than 25)
      if (((pC->SensorRaw[BLUE])   < 48) ||
          (((pC->SensorRaw[BLANK]) < 25) && ((pC->SensorRaw[BLUE])  < 85)))
      {
        Result  =  (DATAF)BLACKCOLOR;
      }
      else
      {
        if ((((((pC->SensorRaw[RED]) * 48) >> 5) < (pC->SensorRaw[BLUE])) &&
            ((((pC->SensorRaw[GREEN]) * 48) >> 5) < (pC->SensorRaw[BLUE])))
            ||
            (((((pC->SensorRaw[RED])   * 58) >> 5) < (pC->SensorRaw[BLUE])) ||
             ((((pC->SensorRaw[GREEN]) * 58) >> 5) < (pC->SensorRaw[BLUE]))))
        {
          Result  =  (DATAF)BLUECOLOR;
        }
        else
        {

          // Color is white or Black
          if ((((pC->SensorRaw[RED])  < 60) ||
              ((pC->SensorRaw[GREEN]) < 60)) ||
             (((pC->SensorRaw[BLANK]) < 110) && ((pC->SensorRaw[BLUE]) < 120)))
          {
            Result  =  (DATAF)BLACKCOLOR;
          }
          else
          {
            if ((((pC->SensorRaw[RED])  + ((pC->SensorRaw[RED])   >> 3)) < (pC->SensorRaw[BLUE])) ||
                (((pC->SensorRaw[GREEN]) + ((pC->SensorRaw[GREEN]) >> 3)) < (pC->SensorRaw[BLUE])))
            {
              Result  =  (DATAF)BLUECOLOR;
            }
            else
            {
              Result  =  (DATAF)WHITECOLOR;
            }
          }
        }
      }
    }
  }


  return (Result);
}


#ifndef DISABLE_DAISYCHAIN

RESULT    cInputGetColor(DATA8 Device,DATA8 *pData)
{
  RESULT  Result = FAIL;

  cInputCalibrateColor(&(*InputInstance.pAnalog).NxtCol[Device],(*InputInstance.pAnalog).NxtCol[Device].SensorRaw);

  switch (InputInstance.DeviceMode[Device])
  {
    case 2 :
    { // NXT-COL-COL

      pData[0]  =  cInputCalculateColor(&(*InputInstance.pAnalog).NxtCol[Device]);
      Result  =  OK;
    }
    break;

    case 1 :
    { // NXT-COL-AMB

      Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).NxtCol[Device].ADRaw[BLANK],(size_t)2);
      Result  =  OK;
    }
    break;

    case 0 :
    { // NXT-COL-RED

      Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).NxtCol[Device].ADRaw[RED],(size_t)2);
      Result  =  OK;
    }
    break;

    case 3 :
    { // NXT-COL-GRN

      Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).NxtCol[Device].ADRaw[GREEN],(size_t)2);
      Result  =  OK;
    }
    break;

    case 4 :
    { // NXT-COL-BLU

      Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).NxtCol[Device].ADRaw[BLUE],(size_t)2);
      Result  =  OK;
    }
    break;

    case 5 :
    { // NXT-COL-RAW

      Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).NxtCol[Device].SensorRaw[0],(size_t)(COLORS * 2));
      Result  =  OK;
    }
    break;
  }

  return (Result);
}

#else

DATAF     cInputGetColor(DATA8 Device,DATA8 Index)
{
  DATAF   Result ;

  Result  =  DATAF_NAN;

  cInputCalibrateColor(&(*InputInstance.pAnalog).NxtCol[Device],(*InputInstance.pAnalog).NxtCol[Device].SensorRaw);

  switch (InputInstance.DeviceMode[Device])
  {
    case 2 :
    { // NXT-COL-COL

      Result  =  cInputCalculateColor(&(*InputInstance.pAnalog).NxtCol[Device]);
    }
    break;

    case 1 :
    { // NXT-COL-AMB

      Result  =  (*InputInstance.pAnalog).NxtCol[Device].ADRaw[BLANK];
    }
    break;

    case 0 :
    { // NXT-COL-RED

      Result  =  (*InputInstance.pAnalog).NxtCol[Device].ADRaw[RED];
    }
    break;

    case 3 :
    { // NXT-COL-GRN

      Result  =  (*InputInstance.pAnalog).NxtCol[Device].ADRaw[GREEN];
    }
    break;

    case 4 :
    { // NXT-COL-BLU

      Result  =  (*InputInstance.pAnalog).NxtCol[Device].ADRaw[BLUE];
    }
    break;

    case 5 :
    { // NXT-COL-RAW

      if (Index < COLORS)
      {
        Result  =  (DATAF)(*InputInstance.pAnalog).NxtCol[Device].SensorRaw[Index];
      }
    }
    break;
  }

  return (Result);
}

#endif

#endif


#ifndef DISABLE_DAISYCHAIN
/*

DEVICE INFO FROM FIRST SLAVE
============================

cInputUpdate(UWORD Time)
|
cInputDcmUpdate(UWORD Time)
|
cInputComGetDeviceInfo(DATA8 Length,UBYTE *pInfo)
|
cComGetDeviceInfo(DATA8 Length,UBYTE *pInfo)
|
cDaisyGetDeviceInfo(DATA8 Length,UBYTE *pInfo)
|

-------------------- USB DAISYCHAIN --------------------

^
(cDaisySetDeviceInfo(DATA8 Length,UBYTE *pInfo))
^
cComSetDeviceInfo(DATA8 Length,UBYTE *pInfo)
^
cInputComSetDeviceInfo(DATA8 Length,UBYTE *pInfo)
^
cInputCheckUartInfo(UBYTE Port)
^
cInputDcmUpdate(UWORD Time)




CHANGE MODE ON FIRST SLAVE
==============================

opINPUT_DEVICE(READY_SI,1,..)
|
cInputSetDeviceType(DATA8 Device,DATA8 Type, DATA8 Mode)
|
cInputComSetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode)
|
cComSetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode)
|
cDaisySetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode)
|

-------------------- USB DAISYCHAIN --------------------

|
cInputSetChainedDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode)
|
cInputSetDeviceType(DATA8 Device,DATA8 Type, DATA8 Mode)





READ SENSOR VALUE FROM FIRST SLAVE
==================================

opINPUT_DEVICE(READY_SI,1,..)
|
cInputReadDeviceSi(DATA8 Device,DATA8 Index,DATA16 Time,DATA16 *pInit)
|
cInputReadDeviceRaw(DATA8 Device,DATA8 Index,DATA16 Time,DATA16 *pInit)
|
cInputGetData(DATA8 Layer,DATA8 Port,DATA16 Time,DATA16 *pInit,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
|
cInputComGetDeviceData(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
|
cComGetDeviceData(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
|
cDaisyGetDownstreamData(DATA8 Layer,DATA8 Sensor,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
|

-------------------- USB DAISYCHAIN --------------------

|
cDaisyPushUpStream(void)
|
cInputGetDeviceData(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
|
cInputGetData(DATA8 Layer,DATA8 Port,DATA16 Time,DATA16 *pInit,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)

*/



RESULT    cInputSetChainedDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode)
{
  RESULT  Result = FAIL;
  DATA8   Device;

  Result  =  cInputCompressDevice(&Device,Layer,Port);
  if (Result == OK)
  { // Device valid

    if ((InputInstance.DeviceType[Device] > 0) && (InputInstance.DeviceType[Device] <= MAX_VALID_TYPE))
    { // Device type valid

      if (Type != TYPE_UNKNOWN)
      {
        cInputSetDeviceType(Device,Type,Mode,__LINE__);
      }
#ifdef DEBUG_TRACE_DAISYCHAIN
      printf("c_input   cInputSetDeviceType:     L=%-2d P=%-2d T=%-3d M=%d\r\n",Layer,Port,Type,Mode);
#endif
    }
  }
  else
  {
#ifdef DEBUG_TRACE_DAISYCHAIN
    printf("c_input   cInputSetDeviceType: FAIL  L=%-2d P=%-2d T=%-3d M=%d\r\n",Layer,Port,Type,Mode);
#endif
  }

  return (Result);
}


RESULT    cInputComSetDeviceInfo(DATA8 Length,UBYTE *pInfo)
{
  RESULT  Result = FAIL;
#ifdef DEBUG_TRACE_DAISYCHAIN
  TYPES   *pType;
#endif

#ifndef DISABLE_DAISYCHAIN_COM_CALL

  if (cDaisyReady() != BUSY)
  {
    Result  =  cComSetDeviceInfo(Length,pInfo);
  }
  else
  {
    Result  =  BUSY;
  }

#else
  Result  =  OK;
#endif

#ifdef DEBUG_TRACE_DAISYCHAIN
  pType  =  (TYPES*)pInfo;

  if (Result == OK)
  {
    printf("c_com     cComSetDeviceInfo:       l=%-2d N=%s\r\n",Length,(*pType).Name);
  }
  else
  {
    if (Result == BUSY)
    {
      printf("c_com     cComSetDeviceInfo: BUSY  l=%-2d N=%s\r\n",Length,(*pType).Name);
    }
    else
    {
      printf("c_com     cComSetDeviceInfo: FAIL  l=%-2d N=%s\r\n",Length,(*pType).Name);
    }
  }
#endif

  return (Result);
}


RESULT    cInputComGetDeviceInfo(DATA8 Length,UBYTE *pInfo)
{
  RESULT  Result = FAIL;
#ifdef DEBUG_TRACE_DAISYCHAIN
  TYPES   *pType;
#endif

#ifndef DISABLE_DAISYCHAIN_COM_CALL
  Result  =  cComGetDeviceInfo(Length,pInfo);
#else
  Result  =  FAIL;
#endif

#ifdef DEBUG_TRACE_DAISYCHAIN
  if (Result == OK)
  {
    pType  =  (TYPES*)pInfo;
    printf("c_com     cComGetDeviceInfo:       C=%-3d N=%s\r\n",(*pType).Connection,(*pType).Name);
  }
#endif

  return (Result);
}


RESULT    cInputComSetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode)
{
  RESULT  Result = FAIL;

#ifndef DISABLE_DAISYCHAIN_COM_CALL
  Result  =  cComSetDeviceType(Layer,Port,Type,Mode);
#else
  Result  =  OK;
#endif

#ifdef DEBUG_TRACE_DAISYCHAIN
  if (Result == OK)
  {
    printf("c_com     cComSetDeviceType:       L=%-2d P=%-2d T=%-3d M=%d\r\n",Layer,Port,Type,Mode);
  }
  else
  {
    if (Result == BUSY)
    {
      printf("c_com     cComSetDeviceType: BUSY  L=%-2d P=%-2d T=%-3d M=%d\r\n",Layer,Port,Type,Mode);
    }
    else
    {
      printf("c_com     cComSetDeviceType: FAIL  L=%-2d P=%-2d T=%-3d M=%d\r\n",Layer,Port,Type,Mode);
    }
  }
#endif

  return (Result);
}


RESULT    cInputComGetDeviceData(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
{
  RESULT  Result = FAIL;

#ifndef DISABLE_DAISYCHAIN_COM_CALL
  Result  =  cComGetDeviceData(Layer,Port,Length,pType,pMode,pData);

#ifdef DEBUG_TRACE_DAISYCHAIN
  if (Result == OK)
  {
    printf("c_com     cComGetDeviceData:       L=%-2d P=%-2d T=%-3d M=%d 0x%02X\r\n",Layer,Port,*pType,*pMode,*pData);
  }
  else
  {
    if (Result == BUSY)
    {
      printf("c_com     cComGetDeviceData: BUSY  L=%-2d P=%-2d T=%-3d M=%d\r\n",Layer,Port,*pType,*pMode);
    }
    else
    {
      printf("c_com     cComGetDeviceData: FAIL  L=%-2d P=%-2d T=%-3d M=%d\r\n",Layer,Port,*pType,*pMode);
    }
  }
#endif

#else
  Result  =  FAIL;
#endif

  return (Result);
}


RESULT    cInputComGetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
{
  RESULT  Result = FAIL;

#ifndef DISABLE_DAISYCHAIN_COM_CALL
  Result  =  cComGetDeviceData(Layer,Port,Length,pType,pMode,pData);

#else
  Result  =  FAIL;
#endif

  return (Result);
}


RESULT    cInputGetDeviceData(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
{
  RESULT  Result;

  Result  =  cInputGetData(Layer,Port,0,NULL,Length,pType,pMode,pData);

#ifdef DEBUG_C_INPUT_DAISYCHAIN
  if (Result == OK)
  {
    printf("c_com     cInputGetDeviceData:    L=%-2d P=%-2d T=%-3d M=%d\r\n",Layer,Port,*pType,*pMode);
  }
  else
  {
    if (Result == BUSY)
    {
      printf("c_com     cInputGetDeviceData: BUSY L=%-2d P=%-2d T=%-3d M=%d\r\n",Layer,Port,*pType,*pMode);
    }
    else
    {
      printf("c_com     cInputGetDeviceData: FAIL L=%-2d P=%-2d T=%-3d M=%d\r\n",Layer,Port,*pType,*pMode);
    }
  }
#endif

  return (Result);
}


RESULT    cInputGetData(DATA8 Layer,DATA8 Port,DATA16 Time,DATA16 *pInit,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData)
{
#ifdef DEBUG_C_INPUT_FAST_DATALOG
#ifndef DISABLE_FAST_DATALOG_BUFFER
  static  UWORD Cnt;
  static  UWORD Old;
  UWORD   New;
  DATA16  TmpTime;
#endif
#endif
  RESULT  Result = FAIL;
  DATA8   Device;
#ifndef DISABLE_FAST_DATALOG_BUFFER
  DATA16  InPointer;
  DATA16  Pointer;
#endif

  if (cInputCompressDevice(&Device,Layer,Port) == OK)
  { // Device valid

#ifdef DEBUG_C_INPUT_DAISYCHAIN
    printf("c_input   cInputGetDeviceData      D=%-3d L=%d P=%-2d l=%-2d\r\n",Device,Layer,Port,Length);
#endif

    if (Length >= MAX_DEVICE_DATALENGTH)
    { // Length OK

      memset(pData,0,(size_t)Length);

      *pType  =  InputInstance.DeviceType[Device];
      *pMode  =  InputInstance.DeviceMode[Device];

      if ((InputInstance.DeviceData[Device].Connection != CONN_NONE) && (InputInstance.DeviceData[Device].Connection != CONN_ERROR))
      { // Device connected

        if (Layer == 0)
        { // Device local

          if (Port < INPUT_DEVICES)
          { // Device is input device

            if ((Time < 0) || (Time > DEVICE_LOGBUF_SIZE))
            {
              Time  =  0;
            }

            if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_UART)
            { // Device is an UART device

#ifndef DISABLE_FAST_DATALOG_BUFFER

              Pointer     =  (*InputInstance.pUart).Actual[Port];
              InPointer   =  (*InputInstance.pUart).LogIn[Port];
              if (pInit != NULL)
              {
#ifdef DEBUG_C_INPUT_DATALOG
                printf("c_input   cInputGetDeviceData      Act=%-4d In=%-4d Out=%-4d Time=%-4d ",(*InputInstance.pUart).Actual[Port],(*InputInstance.pUart).LogIn[Port],*pInit,Time);
#endif

                if ((*pInit < 0) || (*pInit > DEVICE_LOGBUF_SIZE))
                {
                  *pInit  =  Pointer;
                  (*InputInstance.pUart).Repeat[Port][*pInit]  =  0;
#ifdef DEBUG_C_INPUT_FAST_DATALOG
                  if (Port == 0)
                  {
                    Cnt     =  0;
                  }
#endif
                }

                if (Time != 0)
                {
#ifdef DEBUG_C_INPUT_FAST_DATALOG
                  TmpTime  =  Time;

                  if ((Port == 0) && ((*InputInstance.pUart).Repeat[Port][*pInit] > 1))
                  {
                    printf("C=%-5d A=%-4d I=%-4d P=%-4d O=%-3d N=%-3d R=%-3d \r\n",Cnt,(*InputInstance.pUart).Actual[Port],(*InputInstance.pUart).LogIn[Port],Pointer,Old,New,(*InputInstance.pUart).Repeat[Port][*pInit]);
                  }
#endif

                  Pointer  =  *pInit;
                  while ((Time) && ((*InputInstance.pUart).Repeat[Port][Pointer] > 1))
                  {
                    ((*InputInstance.pUart).Repeat[Port][Pointer])--;
                    Time--;
                  }
                  while ((Time) && (Pointer != InPointer))
                  {

                    if (++Pointer >= DEVICE_LOGBUF_SIZE)
                    {
                      Pointer  =  0;
                    }

                    if (Pointer != InPointer)
                    {
                      (*pInit)  =  Pointer;
                    }
                    Time--;
                  }
                  Pointer  =  (*pInit);
#ifdef DEBUG_C_INPUT_FAST_DATALOG
                  if ((Port == 0) && (TmpTime >= 1))
                  {
                    New  =  (*InputInstance.pUart).Raw[Device][Pointer][0];
                    if ((New < (Old - TmpTime)) || (New > (Old + TmpTime)))
                    {
                      if ((New != 0) && ((*InputInstance.pUart).Repeat[Port][*pInit]))
                      {
                        printf("C=%-5d A=%-4d I=%-4d P=%-4d O=%-3d N=%-3d R=%-3d ",Cnt,(*InputInstance.pUart).Actual[Port],InPointer,Pointer,Old,New,(*InputInstance.pUart).Repeat[Port][*pInit]);
                      }
                    }
                    Old  =  New;
                  }
                  if (Port == 0)
                  {
                    Cnt++;
                  }
#endif
                }
#ifdef DEBUG_C_INPUT_DATALOG
                printf("Pointer=%-4d\r\n",Pointer);
#endif

              }
              Memcpy((void*)pData,(const void*)&(*InputInstance.pUart).Raw[Device][Pointer],(size_t)UART_DATA_LENGTH);

#else
              Memcpy((void*)pData,(const void*)&(*InputInstance.pUart).Raw[Device],UART_DATA_LENGTH);
#endif
              Result  =  OK;
            }
            else
            { // Device is not an UART device

              if (InputInstance.DeviceData[Device].Connection == CONN_NXT_IIC)
              { // Device is an IIC device

#ifndef DISABLE_FAST_DATALOG_BUFFER

              Pointer     =  (*InputInstance.pIic).Actual[Port];
              InPointer   =  (*InputInstance.pIic).LogIn[Port];
              if (pInit != NULL)
              {
#ifdef DEBUG_C_INPUT_DATALOG
                printf("c_input   cInputGetDeviceData      Act=%-4d In=%-4d Out=%-4d Time=%-4d ",(*InputInstance.pIic).Actual[Port],(*InputInstance.pIic).LogIn[Port],*pInit,Time);
#endif

                if ((*pInit < 0) || (*pInit > DEVICE_LOGBUF_SIZE))
                {
                  *pInit  =  Pointer;
                  (*InputInstance.pIic).Repeat[Port][*pInit]  =  0;
#ifdef DEBUG_C_INPUT_FAST_DATALOG
                  if (Port == 0)
                  {
                    Cnt     =  0;
                  }
#endif
                }
                if (Time != 0)
                {
                  Pointer  =  *pInit;
                  while ((Time) && ((*InputInstance.pIic).Repeat[Port][Pointer] > 1))
                  {
                    ((*InputInstance.pIic).Repeat[Port][Pointer])--;
                    Time--;
                  }
                  while ((Time) && (Pointer != InPointer))
                  {

                    if (++Pointer >= DEVICE_LOGBUF_SIZE)
                    {
                      Pointer  =  0;
                    }

                    if (Pointer != InPointer)
                    {
                      (*pInit)  =  Pointer;
                    }
                    Time--;
                  }
                  Pointer  =  (*pInit);
#ifdef DEBUG_C_INPUT_FAST_DATALOG
                  if (Port == 0)
                  {
                    Cnt++;
                    New  =  (UWORD)(*InputInstance.pIic).Raw[Device][Pointer][0];
                    New +=  (UWORD)(*InputInstance.pIic).Raw[Device][Pointer][1] << 8;
                    printf("C=%-5d   A=%-4d I=%-4d P=%-4d O=%-5d N=%-5d R=%-3d\r\n ",Cnt,(*InputInstance.pIic).Actual[Port],InPointer,Pointer,Old,New,(*InputInstance.pIic).Repeat[Port][*pInit]);
                    Old  =  New;;
                  }
#endif
                }
              }
              Memcpy((void*)pData,(const void*)&(*InputInstance.pIic).Raw[Device][Pointer],(size_t)IIC_DATA_LENGTH);

#else
              Memcpy((void*)pData,(const void*)&(*InputInstance.pIic).Raw[Device],IIC_DATA_LENGTH);
#endif
              Result  =  OK;
              }
              else
              { // Device is not an IIC device

                if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_DUMB)
                { // Device is a new dumb input device

#ifndef DISABLE_FAST_DATALOG_BUFFER

                  if (pInit != NULL)
                  {
                    Pointer     =  (*InputInstance.pAnalog).Actual[Port];
                    InPointer   =  (*InputInstance.pAnalog).LogIn[Port];
#ifdef DEBUG_C_INPUT_DATALOG
                    printf("c_input   cInputGetDeviceData      Act=%-4d In=%-4d Out=%-4d Time=%-4d ",(*InputInstance.pAnalog).Actual[Port],(*InputInstance.pAnalog).LogIn[Port],*pInit,Time);
#endif

                    if ((*pInit < 0) || (*pInit > DEVICE_LOGBUF_SIZE))
                    {
                      *pInit  =  Pointer;
                    }
                    if (Time != 0)
                    {
                      Pointer  =  *pInit;
                      while ((Time) && (Pointer != InPointer))
                      {

                        if (++Pointer >= DEVICE_LOGBUF_SIZE)
                        {
                          Pointer  =  0;
                        }

                        if (Pointer != InPointer)
                        {
                          (*pInit)  =  Pointer;
                        }
                        Time--;
                      }
                      Pointer  =  (*pInit);
                    }
#ifdef DEBUG_C_INPUT_DATALOG
                    printf("Pointer=%-4d\r\n",Pointer);
#endif
                    if ((InputInstance.DeviceType[Device] == 16) && (InputInstance.DeviceMode[Device] == 1))
                    {
                      Memcpy((void*)pData,(const void*)&InputInstance.DeviceData[Device].Changes,(size_t)4);
                    }
                    else
                    {
                      Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).Pin6[Device][Pointer],(size_t)2);
                    }
                  }
                  else
                  {
                    if ((InputInstance.DeviceType[Device] == 16) && (InputInstance.DeviceMode[Device] == 1))
                    {
                      Memcpy((void*)pData,(const void*)&InputInstance.DeviceData[Device].Changes,(size_t)4);
                    }
                    else
                    {
                      Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).InPin6[Device],(size_t)2);
                    }
                  }
#else
                  if ((InputInstance.DeviceType[Device] == 16) && (InputInstance.DeviceMode[Device] == 1))
                  {
                    Memcpy((void*)pData,(const void*)&InputInstance.DeviceData[Device].Changes,(size_t)4);
                  }
                  else
                  {
                    Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).InPin6[Device],(size_t)2);
                  }
#endif
                  Result  =  OK;
                }
                else
                {
#ifndef DISABLE_OLD_COLOR

                  if (InputInstance.DeviceData[Device].Connection == CONN_NXT_COLOR)
                  { // Device is a nxt color sensor

                    Result  =  cInputGetColor(Device,pData);
                  }
                  else
                  { // Device is an old dumb input device

#ifndef DISABLE_FAST_DATALOG_BUFFER

                    if (pInit != NULL)
                    {
                      Pointer     =  (*InputInstance.pAnalog).Actual[Port];
                      InPointer   =  (*InputInstance.pAnalog).LogIn[Port];
#ifdef DEBUG_C_INPUT_DATALOG
                      printf("c_input   cInputGetDeviceData      Act=%-4d In=%-4d Out=%-4d Time=%-4d ",(*InputInstance.pAnalog).Actual[Port],(*InputInstance.pAnalog).LogIn[Port],*pInit,Time);
#endif

                      if ((*pInit < 0) || (*pInit > DEVICE_LOGBUF_SIZE))
                      {
                        *pInit  =  Pointer;
                      }
                      if (Time != 0)
                      {
                        Pointer  =  *pInit;
                        while ((Time) && (Pointer != InPointer))
                        {

                          if (++Pointer >= DEVICE_LOGBUF_SIZE)
                          {
                            Pointer  =  0;
                          }

                          if (Pointer != InPointer)
                          {
                            (*pInit)  =  Pointer;
                          }
                          Time--;
                        }
                        Pointer  =  (*pInit);
                      }
#ifdef DEBUG_C_INPUT_DATALOG
                      printf("Pointer=%-4d\r\n",Pointer);
#endif

                      if ((InputInstance.DeviceType[Device] == 1) && (InputInstance.DeviceMode[Device] == 1))
                      {
                        Memcpy((void*)pData,(const void*)&InputInstance.DeviceData[Device].Bumps,(size_t)4);
                      }
                      else
                      {
                        Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).Pin1[Device][Pointer],(size_t)2);
                      }
                    }
                    else
                    {
                      if ((InputInstance.DeviceType[Device] == 1) && (InputInstance.DeviceMode[Device] == 1))
                      {
                        Memcpy((void*)pData,(const void*)&InputInstance.DeviceData[Device].Bumps,(size_t)4);
                      }
                      else
                      {
                        Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).InPin1[Device],(size_t)2);
                      }
                    }
#else
                    if ((InputInstance.DeviceType[Device] == 1) && (InputInstance.DeviceMode[Device] == 1))
                    {
                      Memcpy((void*)pData,(const void*)&InputInstance.DeviceData[Device].Bumps,(size_t)4);
                    }
                    else
                    {
                      Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).InPin1[Device],(size_t)2);
                    }
#endif
                    Result  =  OK;
                  }
#else
                  // Device is an old dumb input device

#ifndef DISABLE_FAST_DATALOG_BUFFER
                  if (pInit != NULL)
                  {
                    Pointer     =  (*InputInstance.pAnalog).Actual[Port];
                    InPointer   =  (*InputInstance.pAnalog).LogIn[Port];
#ifdef DEBUG_C_INPUT_DATALOG
                    printf("c_input   cInputGetDeviceData      Act=%-4d In=%-4d Out=%-4d Time=%-4d ",(*InputInstance.pAnalog).Actual[Port],*pInit,Time);
#endif

                    if ((*pInit < 0) || (*pInit > DEVICE_LOGBUF_SIZE))
                    {
                      *pInit  =  Pointer;
                    }
                    if (Time != 0)
                    {
                      Pointer  =  *pInit;
                      while ((Time) && (Pointer != InPointer))
                      {
                        (*pInit)  =  Pointer;

                        if (++Pointer >= DEVICE_LOGBUF_SIZE)
                        {
                          Pointer  =  0;
                        }
                        Time--;
                      }
                      Pointer  =  (*pInit);
                    }
#ifdef DEBUG_C_INPUT_DATALOG
                    printf("Pointer=%-4d\r\n",Pointer);
#endif
                    Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).Pin1[Device][Pointer],(size_t)2);
                  }
                  else
                  {
                    Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).InPin1[Device],(size_t)2);
                  }
#else
                  Memcpy((void*)pData,(const void*)&(*InputInstance.pAnalog).InPin1[Device],(size_t)2);
#endif

                  Result  =  OK;
#endif
                }
              }
            }
          }
          else
          { // Device is output device

            if (InputInstance.DeviceMode[Device] == 2)
            {
              Memcpy((void*)pData,(const void*)&OutputInstance.pMotor[Device - INPUT_DEVICES].Speed,(size_t)1);
              Result  =  OK;
            }
            else
            {
              Memcpy((void*)pData,(const void*)&OutputInstance.pMotor[Device - INPUT_DEVICES].TachoSensor,(size_t)4);
              Result  =  OK;
            }

            InputInstance.DeviceData[Device].DevStatus      =  OK;
          }
          if (Result == OK)
          {
            Result  =  InputInstance.DeviceData[Device].DevStatus;
          }

        }
        else
        { // Device is daisy chained

          Result  =  cInputComGetDeviceData(Layer,Port,MAX_DEVICE_DATALENGTH,pType,pMode,pData);

        }
      }
    }
  }

  return (Result);
}


DATAF     cInputReadDeviceRaw(DATA8 Device,DATA8 Index,DATA16 Time,DATA16 *pInit)
{
  DATAF   Result = (DATAF)0;
  DATA8   Layer;
  DATA8   Port;
  DATA8   Output;
  DATA8   Type;
  DATA8   Mode;
  DATA8   DataSets;
  UBYTE   *pData;


  Result  =  DATAF_NAN;

#ifdef DEBUG_C_INPUT_DAISYCHAIN
  printf("c_input   cInputReadDeviceRaw:     D=%-3d I=%d B=%d\r\n",Device,Index,InputInstance.DeviceData[Device].DevStatus);
#endif

  if (cInputExpandDevice(Device,&Layer,&Port,&Output) == OK)
  { // Device valid

    pData  =  InputInstance.Data;
    InputInstance.DeviceData[Device].Raw[Index]       =  DATAF_NAN;

    if (cInputGetData(Layer,Port,Time,pInit,MAX_DEVICE_DATALENGTH,&Type,&Mode,(DATA8*)pData) == OK)
    {

      DataSets  =  InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].DataSets;
      if (Index < DataSets)
      {
        switch (InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Format & 0x0F)
        {
          case DATA_8 :
          {
            if (((DATA8*)pData)[Index] != DATA8_NAN)
            {
              InputInstance.DeviceData[Device].Raw[Index]   =  (DATAF)((DATA8*)pData)[Index];
            }
          }
          break;

          case DATA_16 :
          {
            if (((DATA16*)pData)[Index] != DATA16_NAN)
            {
              InputInstance.DeviceData[Device].Raw[Index]   =  (DATAF)((DATA16*)pData)[Index];

//!<  \todo EMETER hack
              if ((Type == 99) && (Mode == 7) && (Index == 4))
              {
                InputInstance.DeviceData[Device].Raw[Index] *=  (DATAF)1000.0;
              }
            }
          }
          break;

          case DATA_32 :
          {
            if (((DATA32*)pData)[Index] != DATA32_NAN)
            {
              InputInstance.DeviceData[Device].Raw[Index]   =  (DATAF)((DATA32*)pData)[Index];
            }
          }
          break;

          case DATA_F :
          {
            InputInstance.DeviceData[Device].Raw[Index]   =  (DATAF)((DATAF*)pData)[Index];
          }
          break;

        }
      }

      Result  =  InputInstance.DeviceData[Device].Raw[Index];
    }
  }

  return (Result);
}


#else


DATAF     cInputReadDeviceRaw(DATA8 Device,DATA8 Index,DATA16 Time,DATA16 *pInit)
{
  DATAF   Result;
  DATA8   DataSets;
  void    *pResult;

  Result  =  DATAF_NAN;

#ifdef DEBUG_C_INPUT_DAISYCHAIN
  printf("c_input   cInputReadDeviceRaw:     D=%-3d B=%d\r\n",Device,InputInstance.DeviceData[Device].DevStatus);
#endif
  if ((Device >= 0) && (Device < DEVICES) && (Index >= 0) && (Index < MAX_DEVICE_DATASETS))
  {
    // Parameters are valid

    if (InputInstance.DeviceData[Device].DevStatus == OK)
    {

      if (Device < INPUTS)
      {
        // Device is a local sensor

        if ((InputInstance.DeviceData[Device].Connection != CONN_NONE) && (InputInstance.DeviceData[Device].Connection != CONN_ERROR))
        {
          // Device is connected right

          if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_UART)
          {
            // Device is a UART sensor

            pResult   =  (void*)&(*InputInstance.pUart).Raw[Device];
            DataSets  =  InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].DataSets;

            if (Index < DataSets)
            {
              switch (InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Format & 0x0F)
              {
                case DATA_8 :
                {
                  InputInstance.DeviceData[Device].Raw[Index]   =  (DATAF)*(((DATA8*)pResult) + Index);
                }
                break;

                case DATA_16 :
                {
                  InputInstance.DeviceData[Device].Raw[Index]   =  (DATAF)*(((DATA16*)pResult) + Index);
                }
                break;

                case DATA_32 :
                {
                  InputInstance.DeviceData[Device].Raw[Index]   =  (DATAF)*(((DATA32*)pResult) + Index);
                }
                break;

                case DATA_F :
                {
                  InputInstance.DeviceData[Device].Raw[Index]   =  (DATAF)*(((DATAF*)pResult) + Index);
                }
                break;

                default :
                {
                  InputInstance.DeviceData[Device].Raw[Index]   =  DATAF_NAN;
                }
                break;

              }
            }
            else
            {
              InputInstance.DeviceData[Device].Raw[Index]       =  DATAF_NAN;
            }
          }
          else
          {
            // Device is not a UART sensor

            if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_DUMB)
            {
              // Device is new dumb

              InputInstance.DeviceData[Device].Raw[Index]       =  (DATAF)(*InputInstance.pAnalog).InPin6[Device];
            }
            else
            {
#ifndef DISABLE_OLD_COLOR

              if (InputInstance.DeviceData[Device].Connection == CONN_NXT_COLOR)
              {
                // Device is nxt color

                InputInstance.DeviceData[Device].Raw[Index]       =  (DATAF)cInputGetColor(Device,Index);
              }
              else
              {
                // Device is old dumb

                InputInstance.DeviceData[Device].Raw[Index]       =  (DATAF)(*InputInstance.pAnalog).InPin1[Device];
              }
#else
              // Device is old dumb

              InputInstance.DeviceData[Device].Raw[Index]       =  (DATAF)(*InputInstance.pAnalog).InPin1[Device];

#endif
            }
          }
        }
        else
        {
          InputInstance.DeviceData[Device].Raw[Index]           =  DATAF_NAN;
        }
      }
      if ((Device >= INPUT_DEVICES) && (Device < (INPUT_DEVICES + OUTPUTS)))
      {
        // Device is connected on output port

        if ((InputInstance.DeviceData[Device].Connection == CONN_NONE) || (InputInstance.DeviceData[Device].Connection == CONN_ERROR))
        {
          InputInstance.DeviceData[Device].Raw[Index]           =  DATAF_NAN;
        }
        else
        {
          if (InputInstance.DeviceMode[Device] == 2)
          {
            InputInstance.DeviceData[Device].Raw[Index]         =  (DATAF)OutputInstance.pMotor[Device - INPUT_DEVICES].Speed;
          }
          else
          {
            InputInstance.DeviceData[Device].Raw[Index]         =  (DATAF)OutputInstance.pMotor[Device - INPUT_DEVICES].TachoSensor;
          }
        }

      }
      Result  =  InputInstance.DeviceData[Device].Raw[Index];
    }
  }

  return (Result);
}
#endif


void      cInputWriteDeviceRaw(DATA8 Device,DATA8 Connection,DATA8 Type,DATAF DataF)
{
  UBYTE   Byte;
  UWORD   Word;

  if (Device < INPUTS)
  {
    if (Type == TYPE_KEEP)
    {
      Type    =  InputInstance.DeviceType[Device];
    }
    if (InputInstance.DeviceType[Device] != Type)
    {
      InputInstance.DeviceData[Device].Connection     =  Connection;
      cInputSetDeviceType(Device,Type,0,__LINE__);
    }
    if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_UART)
    {
    }
    else
    {
      if (InputInstance.DeviceData[Device].Connection == CONN_NXT_IIC)
      {
        Byte  =  (UBYTE)DataF;
        Word  =  (UWORD)DataF;

#ifndef DISABLE_FAST_DATALOG_BUFFER
        (*InputInstance.pAnalog).Actual[Device] =  0;
        if ((InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Format & 0x0F) == 0)
        {
          (*InputInstance.pIic).Raw[Device][(*InputInstance.pAnalog).Actual[Device]][0]        =  Byte;
          (*InputInstance.pIic).Raw[Device][(*InputInstance.pAnalog).Actual[Device]][1]        =  0;
        }
        else
        {
          (*InputInstance.pIic).Raw[Device][(*InputInstance.pAnalog).Actual[Device]][0]        =  (UBYTE)Word;
          (*InputInstance.pIic).Raw[Device][(*InputInstance.pAnalog).Actual[Device]][1]        =  (UBYTE)(Word >> 8);
        }
        InputInstance.DeviceData[Device].DevStatus  =  OK;
#else
        if ((InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Format & 0x0F) == 0)
        {
          (*InputInstance.pIic).Raw[Device][0]        =  Byte;
          (*InputInstance.pIic).Raw[Device][1]        =  0;
        }
        else
        {
          (*InputInstance.pIic).Raw[Device][0]        =  (UBYTE)Word;
          (*InputInstance.pIic).Raw[Device][1]        =  (UBYTE)(Word >> 8);
        }
        InputInstance.DeviceData[Device].DevStatus  =  OK;
#endif
      }
      else
      {
#ifndef DISABLE_FAST_DATALOG_BUFFER
        (*InputInstance.pAnalog).Actual[Device]   =  0;
        (*InputInstance.pAnalog).LogIn[Device]    =  0;

        if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_DUMB)
        {
          (*InputInstance.pAnalog).Pin6[Device][(*InputInstance.pAnalog).Actual[Device]]  =  (DATA16)DataF;

        }
        else
        {
          (*InputInstance.pAnalog).Pin1[Device][(*InputInstance.pAnalog).Actual[Device]]  =  (DATA16)DataF;
        }
        InputInstance.DeviceData[Device].DevStatus  =  OK;
#endif
        if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_DUMB)
        {
          (*InputInstance.pAnalog).InPin6[Device]  =  (DATA16)DataF;
        }
        else
        {
          (*InputInstance.pAnalog).InPin1[Device]  =  (DATA16)DataF;
        }
        InputInstance.DeviceData[Device].DevStatus  =  OK;
      }
    }
  }
  if ((Device >= INPUT_DEVICES) && (Device < (INPUT_DEVICES + OUTPUTS)))
  {
    if (Type == TYPE_KEEP)
    {
      Type    =  InputInstance.DeviceType[Device];
    }
    if (InputInstance.DeviceType[Device] != Type)
    {
      InputInstance.DeviceData[Device].Connection     =  CONN_UNKNOWN;
      cInputSetDeviceType(Device,Type,0,__LINE__);
    }
    if (InputInstance.DeviceMode[Device] == 2)
    {
      OutputInstance.pMotor[Device - INPUT_DEVICES].Speed  =  (DATA32)DataF;
    }
    else
    {
      OutputInstance.pMotor[Device - INPUT_DEVICES].TachoSensor  =  (DATA32)DataF;
    }
    InputInstance.DeviceData[Device].DevStatus  =  OK;
  }
}


DATA8     cInputReadDevicePct(DATA8 Device,DATA8 Index,DATA16 Time,DATA16 *pInit)
{
  DATA8   Result  =  DATA8_NAN;
  UWORD   TypeIndex;
  DATAF   Raw;
  DATA8   Type;
  DATA8   Mode;
  DATAF   Min;
  DATAF   Max;
  DATAF   Pct;

  Raw         =  cInputReadDeviceRaw(Device,Index,Time,pInit);

  if (!(isnan(Raw)))
  {
    TypeIndex   =  InputInstance.DeviceData[Device].TypeIndex;

    Type        =  InputInstance.TypeData[TypeIndex].Type;
    Mode        =  InputInstance.TypeData[TypeIndex].Mode;
    Min         =  InputInstance.TypeData[TypeIndex].RawMin;
    Max         =  InputInstance.TypeData[TypeIndex].RawMax;

    if ((Type > 0) && (Type < (MAX_DEVICE_TYPE + 1)) && (Mode >= 0) && (Mode < MAX_DEVICE_MODES))
    {
      if (InputInstance.Calib[Type][Mode].InUse)
      {
        Min     =  InputInstance.Calib[Type][Mode].Min;
        Max     =  InputInstance.Calib[Type][Mode].Max;
      }
    }

    Pct    =  (((Raw - Min) * (InputInstance.TypeData[TypeIndex].PctMax - InputInstance.TypeData[TypeIndex].PctMin)) / (Max - Min) + InputInstance.TypeData[TypeIndex].PctMin);

    if (Pct > InputInstance.TypeData[TypeIndex].PctMax)
    {
      Pct  =  InputInstance.TypeData[TypeIndex].PctMax;
    }
    if (Pct < InputInstance.TypeData[TypeIndex].PctMin)
    {
      Pct  =  InputInstance.TypeData[TypeIndex].PctMin;
    }
    Result  =  (DATA8)Pct;
  }

  return (Result);
}


DATAF     cInputReadDeviceSi(DATA8 Device,DATA8 Index,DATA16 Time,DATA16 *pInit)
{
  UWORD   TypeIndex;
  DATAF   Raw;
  DATA8   Type;
  DATA8   Mode;
  DATA8   Connection;
  DATAF   Min;
  DATAF   Max;

  Raw           =  cInputReadDeviceRaw(Device,Index,Time,pInit);

  if (!(isnan(Raw)))
  {
    TypeIndex   =  InputInstance.DeviceData[Device].TypeIndex;

    Type        =  InputInstance.TypeData[TypeIndex].Type;
    Mode        =  InputInstance.TypeData[TypeIndex].Mode;
    Min         =  InputInstance.TypeData[TypeIndex].RawMin;
    Max         =  InputInstance.TypeData[TypeIndex].RawMax;

    if ((Type > 0) && (Type < (MAX_DEVICE_TYPE + 1)) && (Mode >= 0) && (Mode < MAX_DEVICE_MODES))
    {
      if (InputInstance.Calib[Type][Mode].InUse)
      {
        Min     =  InputInstance.Calib[Type][Mode].Min;
        Max     =  InputInstance.Calib[Type][Mode].Max;
      }
    }

    Raw         =  (((Raw - Min) * (InputInstance.TypeData[TypeIndex].SiMax - InputInstance.TypeData[TypeIndex].SiMin)) / (Max - Min) + InputInstance.TypeData[TypeIndex].SiMin);

    // Limit values on dumb connections if "pct" or "_"
    Connection  =  InputInstance.TypeData[TypeIndex].Connection;
    if ((Connection == CONN_NXT_DUMB) || (Connection == CONN_INPUT_DUMB) || (Connection == CONN_OUTPUT_DUMB) || (Connection == CONN_OUTPUT_TACHO))
    {
      if ((InputInstance.TypeData[TypeIndex].Symbol[0] == 'p') || (InputInstance.TypeData[TypeIndex].Symbol[0] == ' ') || (InputInstance.TypeData[TypeIndex].Symbol[0] == 0))
      {
        if (Raw > InputInstance.TypeData[TypeIndex].SiMax)
        {
          Raw     =  InputInstance.TypeData[TypeIndex].SiMax;
        }
        if (Raw < InputInstance.TypeData[TypeIndex].SiMin)
        {
          Raw     =  InputInstance.TypeData[TypeIndex].SiMin;
        }
      }
    }

  }

  return (Raw);
}


RESULT    cInputCheckUartInfo(UBYTE Port)
{
  RESULT  Result = BUSY;
  TYPES   *pTmp;


  if (InputInstance.UartFile >= MIN_HANDLE)
  { // Driver installed

    if (((*InputInstance.pUart).Status[Port] & UART_PORT_CHANGED))
    { // something has changed

      if (InputInstance.TmpMode[Port] > 0)
      { // check each mode

        InputInstance.TmpMode[Port]--;

        // Get info
        InputInstance.UartCtl.Port  =  Port;
        InputInstance.UartCtl.Mode  =  InputInstance.TmpMode[Port];
        ioctl(InputInstance.UartFile,UART_READ_MODE_INFO,&InputInstance.UartCtl);

        if (InputInstance.UartCtl.TypeData.Name[0])
        { // Info available

          Result  =  cInputGetNewTypeDataPointer(InputInstance.UartCtl.TypeData.Name,InputInstance.UartCtl.TypeData.Type,InputInstance.UartCtl.TypeData.Mode,CONN_INPUT_UART,&pTmp);
          if (pTmp != NULL)
          { // Tabel index found

            if (InputInstance.DeviceType[Port] == TYPE_UNKNOWN)
            { // Use first mode info to set type

              InputInstance.DeviceType[Port]    =  InputInstance.UartCtl.TypeData.Type;
            }

            if (Result == OK)
            { // New mode

              // Insert in tabel
              Memcpy(pTmp,&InputInstance.UartCtl.TypeData,sizeof(TYPES));

            }
#ifndef DISABLE_DAISYCHAIN
            if (cInputComSetDeviceInfo(MAX_DEVICE_INFOLENGTH,(UBYTE*)&InputInstance.UartCtl.TypeData) == BUSY)
            { // Chain not ready - roll back

              ioctl(InputInstance.UartFile,UART_NACK_MODE_INFO,&InputInstance.UartCtl);
              InputInstance.TmpMode[Port]++;
            }
#endif

#ifdef BUFPRINTSIZE
            BufPrint('p',"P=%d T=%-3d M=%d N=%s\r\n",Port,InputInstance.UartCtl.TypeData.Type,InputInstance.UartCtl.TypeData.Mode,InputInstance.UartCtl.TypeData.Name);
#endif
          }
        }
      }
      else
      { // All modes received set device mode 0

        InputInstance.UartCtl.Port  =  Port;
        ioctl(InputInstance.UartFile,UART_CLEAR_CHANGED,&InputInstance.UartCtl);
        (*InputInstance.pUart).Status[Port] &= ~UART_PORT_CHANGED;
        cInputSetDeviceType(Port,InputInstance.DeviceType[Port],0,__LINE__);
      }
    }

    if (((*InputInstance.pUart).Status[Port] & UART_DATA_READY))
    {
      if (!((*InputInstance.pUart).Status[Port] & UART_PORT_CHANGED))
      {
        Result  =  OK;
      }
    }
  }

  return (Result);
}


RESULT    cInputCheckIicInfo(UBYTE Port)
{
  RESULT  Result = BUSY;
  DATA8   Type;
  DATA8   Mode;
  UWORD   Index;

  if (InputInstance.IicFile >= MIN_HANDLE)
  { // Driver installed

    if ((*InputInstance.pAnalog).InDcm[Port] == TYPE_NXT_IIC)
    {

      if ((*InputInstance.pIic).Changed[Port])
      { // something has changed

        InputInstance.IicStr.Port  =  Port;

        ioctl(InputInstance.IicFile,IIC_READ_TYPE_INFO,&InputInstance.IicStr);

        Index  =  IIC_NAME_LENGTH;
        while ((Index) && ((InputInstance.IicStr.Manufacturer[Index] == ' ') || (InputInstance.IicStr.Manufacturer[Index] == 0)))
        {
          InputInstance.IicStr.Manufacturer[Index]  =  0;
          Index--;
        }
        Index  =  IIC_NAME_LENGTH;
        while ((Index) && ((InputInstance.IicStr.SensorType[Index] == ' ') || (InputInstance.IicStr.SensorType[Index] == 0)))
        {
          InputInstance.IicStr.SensorType[Index]  =  0;
          Index--;
        }

        // Find 3th party type
        Type   =  TYPE_IIC_UNKNOWN;
        Mode   =  0;
        Index  =  0;
        while ((Index < InputInstance.IicDeviceTypes) && (Type == TYPE_IIC_UNKNOWN))
        { // Check list

          if (strcmp((char*)InputInstance.IicStr.Manufacturer,(char*)InputInstance.IicString[Index].Manufacturer) == 0)
          { // Manufacturer found

            if (strcmp((char*)InputInstance.IicStr.SensorType,(char*)InputInstance.IicString[Index].SensorType) == 0)
            { // Type found

              Type   =  InputInstance.IicString[Index].Type;
              Mode   =  InputInstance.IicString[Index].Mode;
            }
          }
          Index++;
        }
        cInputSetDeviceType(Port,Type,Mode,__LINE__);
      }
      (*InputInstance.pIic).Changed[Port]  =  0;
    }
    if (((*InputInstance.pIic).Status[Port] & IIC_DATA_READY))
    {
      Result  =  OK;
    }
    else
    {
      Result  =  BUSY;
    }
  }

  return (Result);
}


/*! \brief    Update Device Types
 *
 *            Called when the VM read the device list
 *
 */
void      cInputDcmUpdate(UWORD Time)
{
  RESULT  Result = BUSY;
  DATA8   Device;
  DATA8   Port;
#ifndef DISABLE_DAISYCHAIN
  TYPES   Tmp;
  TYPES   *pTmp;
  DATA16  Index;
  DATA8   Layer;
  DATA8   Output;
  DATA8   Type;
  DATA8   Mode;
#endif

  if (InputInstance.DCMUpdate)
  {
    for (Device = 0;Device < DEVICES;Device++)
    {

      if ((Device >= 0) && (Device < INPUTS))
      { // Device is local input port

        Port  =  Device;

        if (InputInstance.DeviceData[Device].Connection !=  (*InputInstance.pAnalog).InConn[Port])
        { // Connection type has changed

          InputInstance.DeviceData[Device].Connection   =  (*InputInstance.pAnalog).InConn[Port];
          cInputSetDeviceType(Device,(*InputInstance.pAnalog).InDcm[Port],0,__LINE__);
          InputInstance.DeviceMode[Device]              =  0;
          InputInstance.TmpMode[Device]                 =  MAX_DEVICE_MODES;
          InputInstance.DeviceData[Device].DevStatus    =  BUSY;
        }

        if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_UART)
        { // UART device

          Result  =  cInputCheckUartInfo(Port);

        }
        else
        {
          if (InputInstance.DeviceData[Device].Connection == CONN_NXT_IIC)
          { // IIC device

            Result  =  cInputCheckIicInfo(Port);
          }
          else
          { // Analogue device

            if ((*InputInstance.pAnalog).Updated[Device])
            {
              Result  =  OK;
            }
          }
        }
      }
      else
      {
        if ((Device >= INPUT_DEVICES) && (Device < (INPUT_DEVICES + OUTPUTS)))
        { // Device is local output port

          Port  =  Device - INPUT_DEVICES;

          if (InputInstance.DeviceData[Device].Connection !=  (*InputInstance.pAnalog).OutConn[Port])
          { // Connection type has changed

            InputInstance.DeviceData[Device].Connection  =  (*InputInstance.pAnalog).OutConn[Port];
            cInputSetDeviceType(Device,(*InputInstance.pAnalog).OutDcm[Port],0,__LINE__);
          }

          Result  =  OK;

        }
        else
        { // Device is from daisy chain

#ifndef DISABLE_DAISYCHAIN

          cInputExpandDevice(Device,&Layer,&Port,&Output);

          Result  =  cInputComGetDeviceType(Layer,Port,MAX_DEVICE_DATALENGTH,&Type,&Mode,(DATA8*)&Tmp);

          if ((Type > 0) && (Type <= MAX_VALID_TYPE) && (Result != FAIL))
          {
            InputInstance.DeviceData[Device].Connection  =  CONN_DAISYCHAIN;
          }
          else
          {
            Type  =  TYPE_NONE;
            InputInstance.DeviceData[Device].Connection  =  CONN_NONE;
          }

          if (InputInstance.DeviceType[Device] != Type)
          {
            cInputSetDeviceType(Device,Type,0,__LINE__);
          }
#else

          InputInstance.DeviceData[Device].Connection  =  CONN_NONE;

#endif

        }
      }

      if ((InputInstance.DeviceData[Device].Connection == CONN_NONE) || (InputInstance.DeviceData[Device].Connection == CONN_ERROR))
      {
        InputInstance.DeviceData[Device].DevStatus  =  BUSY;
      }
      else
      {
        if (InputInstance.DeviceData[Device].InvalidTime >= Time)
        {
          InputInstance.DeviceData[Device].InvalidTime -=  Time;
          InputInstance.DeviceData[Device].DevStatus  =  BUSY;
        }
        else
        {
          InputInstance.DeviceData[Device].InvalidTime  =  0;
          if (Result == OK)
          {
            InputInstance.DeviceData[Device].DevStatus    =  OK;

#ifdef BUFPRINTSIZE
            BufPrint('p',"D=%-2d M=%d OK\r\n",(int)Device,InputInstance.DeviceMode[Device]);
#endif
          }
          else
          {
            InputInstance.DeviceData[Device].DevStatus  =  BUSY;
          }
        }
      }
      if (InputInstance.DeviceData[Device].TimeoutTimer >= Time)
      {
        InputInstance.DeviceData[Device].TimeoutTimer -=  Time;
      }
      else
      {
        InputInstance.DeviceData[Device].TimeoutTimer  =  0;
      }


    }
  }

#ifndef DISABLE_DAISYCHAIN

  if (cInputComGetDeviceInfo(MAX_DEVICE_INFOLENGTH,(UBYTE*)&Tmp) == OK)
  {
    Result  =  cInputGetNewTypeDataPointer(Tmp.Name,Tmp.Type,Tmp.Mode,Tmp.Connection,&pTmp);
    if (pTmp != NULL)
    {
      if (Result == OK)
      {
        (*pTmp)  =  Tmp;
#ifdef DEBUG_TRACE_DAISYCHAIN
        printf("c_input   cInputDcmUpdate: NEW     T=%-3d M=%d C=%-3d N=%s\r\n",Tmp.Type,Tmp.Mode,Tmp.Connection,Tmp.Name);
#endif
      }
#ifdef DEBUG_TRACE_DAISYCHAIN
      else
      {
        printf("c_input   cInputDcmUpdate: KNOWN   T=%-3d M=%d C=%-3d N=%s\r\n",Tmp.Type,Tmp.Mode,Tmp.Connection,Tmp.Name);
      }
#endif
    }
  }

  if (InputInstance.TypeDataIndex < InputInstance.MaxDeviceTypes)
  { // Upload TypeData info through daisy chain

    if (InputInstance.TypeDataTimer == 0)
    {
      Index  =  InputInstance.TypeDataIndex;
      InputInstance.TypeDataIndex++;

      if (InputInstance.TypeData[Index].Name[0] != 0)
      {
        if (InputInstance.TypeData[Index].Type <= MAX_VALID_TYPE)
        { // Entry valid

          if (cInputComSetDeviceInfo(MAX_DEVICE_INFOLENGTH,(UBYTE*)&InputInstance.TypeData[Index]) == BUSY)
          { // Roll back

            InputInstance.TypeDataIndex--;

  #ifdef ENABLE_STATUS_TEST
            VMInstance.Status |=  0x10;
  #endif
          }
  #ifdef ENABLE_STATUS_TEST
          else
          {
            VMInstance.Status &= ~0x10;
          }
  #endif

        }
      }
    }
    else
    {
      if (InputInstance.TypeDataTimer >= Time)
      {
        InputInstance.TypeDataTimer -=  Time;
      }
      else
      {
        InputInstance.TypeDataTimer  =  0;
      }
    }
  }
  else
  {
    InputInstance.TypeDataIndex  =  DATA16_MAX;
  }

#endif
#ifdef ENABLE_STATUS_TEST
  VMInstance.Status &= ~0x07;

  if (InputInstance.DeviceData[TESTDEVICE].DevStatus == OK)
  {
    VMInstance.Status |=  0x01;
  }
  else
  {
    if (InputInstance.DeviceData[TESTDEVICE].DevStatus == BUSY)
    {
      VMInstance.Status |=  0x02;
    }
    else
    {
      VMInstance.Status |=  0x04;
    }
  }
#endif
}


RESULT  cInputStartTypeDataUpload(void)
{
  InputInstance.TypeDataIndex  =  0;
  InputInstance.TypeDataTimer  =  DELAY_TO_TYPEDATA;

  return (OK);
}


#ifdef Linux_X86
void      cInputSimulate(UWORD Time,DATA8 Device,DATA8 Connection,DATA8 Type)
{
  DATAF   Raw;
  DATAF   Min;
  DATAF   Max;
  DATAF   Inc;
  UWORD   Timeout;

  InputInstance.DeviceData[Device].Timer +=  Time;

  Timeout   =  20;
  Raw       =  cInputReadDeviceRaw(Device,0,0,NULL);
  Min       =  InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].RawMin;
  Max       =  InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].RawMax;
  Inc       =  (Max - Min) / (DATAF)100;
  if ((Inc < (DATAF)0) && (Inc > (DATAF)-1))
  {
    Inc     = -1;
    Timeout =  200;
  }
  if ((Inc > (DATAF)0) && (Inc < (DATAF)1))
  {
    Inc     =  1;
    Timeout =  200;
  }

  if (InputInstance.DeviceData[Device].Timer >= Timeout)
  {
    InputInstance.DeviceData[Device].Timer  =  0;

    if (isnan(Raw))
    {
      Raw  =  (DATAF)0;
    }

    if (InputInstance.DeviceData[Device].Dir)
    { // Down

      Raw -=  Inc;

      if (Inc >= (DATAF)0)
      {

        if (Raw <= Min)
        {
          Raw  =  Min;
          InputInstance.DeviceData[Device].Dir  =  0;
        }
        if (Raw >= Max)
        {
          Raw  =  Max;
        }
      }
      else
      {
        if (Raw >= Min)
        {
          Raw  =  Min;
          InputInstance.DeviceData[Device].Dir  =  0;
        }
        if (Raw <= Max)
        {
          Raw  =  Max;
        }
      }
    }
    else
    { // Up

      Raw +=  Inc;

      if (Inc >= (DATAF)0)
      {
        if (Raw <= Min)
        {
          Raw  =  Min;
        }
        if (Raw >= Max)
        {
          Raw  =  Max;
          InputInstance.DeviceData[Device].Dir  =  1;
        }
      }
      else
      {
        if (Raw >= Min)
        {
          Raw  =  Min;
        }
        if (Raw <= Max)
        {
          Raw  =  Max;
          InputInstance.DeviceData[Device].Dir  =  1;
        }
      }
    }

    cInputWriteDeviceRaw(Device,Connection,Type,Raw);
  }
}
#endif


void      cInputUpdate(UWORD Time)
{
#ifndef DISABLE_BUMBED
  DATA8   Device;
  DATAF   Value;
  DATAF   Diff;
#endif

  cInputDcmUpdate(Time);

#ifndef DISABLE_BUMBED
  for (Device = 0;Device < INPUT_PORTS;Device++)
  { // check each port for changes

    if ((InputInstance.DeviceType[Device] == 1) || (InputInstance.DeviceType[Device] == 16))
    {
      if (InputInstance.DeviceType[Device] == 1)
      {
        Value  =  (DATAF)(*InputInstance.pAnalog).InPin1[Device];
      }
      if (InputInstance.DeviceType[Device] == 16)
      {
        Value  =  (DATAF)(*InputInstance.pAnalog).InPin6[Device];
      }

      Diff    =  Value - InputInstance.DeviceData[Device].OldRaw;

      if (Diff >= (DATAF)500)
      {
        InputInstance.DeviceData[Device].Bumps   +=  (DATA32)1;
      }
      if (Diff <= (DATAF)-500)
      {
        InputInstance.DeviceData[Device].Changes +=  (DATA32)1;
      }

      InputInstance.DeviceData[Device].OldRaw     =  Value;
    }
    else
    {
      InputInstance.DeviceData[Device].Changes    =  (DATA32)0;
      InputInstance.DeviceData[Device].Bumps      =  (DATA32)0;
      InputInstance.DeviceData[Device].OldRaw     =  (DATAF)0;
    }
  }
#endif
#ifdef Linux_X86
  cInputSimulate(Time,0,CONN_UNKNOWN,2);
  cInputSimulate(Time,1,CONN_UNKNOWN,3);
  cInputSimulate(Time,2,CONN_UNKNOWN,1);
  cInputSimulate(Time,3,CONN_UNKNOWN,4);
  cInputSimulate(Time,16,CONN_UNKNOWN,7);
  cInputSimulate(Time,17,CONN_UNKNOWN,7);
  cInputSimulate(Time,18,CONN_UNKNOWN,8);
  cInputSimulate(Time,19,CONN_UNKNOWN,8);
#endif
}


RESULT    cInputInit(void)
{
  RESULT  Result = OK;
  ANALOG  *pAdcTmp;
  UART    *pUartTmp;
  IIC     *pIicTmp;
  PRGID   TmpPrgId;
  UWORD   Tmp;
  UWORD   Set;

  InputInstance.TypeDataIndex   =  DATA16_MAX;

  InputInstance.MaxDeviceTypes  =  3;

  cMemoryMalloc((void*)&InputInstance.TypeData,(DATA32)(sizeof(TYPES) * InputInstance.MaxDeviceTypes));

  InputInstance.IicDeviceTypes  =  1;

  cMemoryMalloc((void*)&InputInstance.IicString,(DATA32)(sizeof(IICSTR) * InputInstance.IicDeviceTypes));

  InputInstance.pAnalog     =  &InputInstance.Analog;

  InputInstance.pUart       =  &InputInstance.Uart;

  InputInstance.pIic        =  &InputInstance.Iic;

  InputInstance.AdcFile     =  open(ANALOG_DEVICE_NAME,O_RDWR | O_SYNC);
  InputInstance.UartFile    =  open(UART_DEVICE_NAME,O_RDWR | O_SYNC);
  InputInstance.DcmFile     =  open(DCM_DEVICE_NAME,O_RDWR | O_SYNC);
  InputInstance.IicFile     =  open(IIC_DEVICE_NAME,O_RDWR | O_SYNC);
  InputInstance.DCMUpdate   =  1;

  if (InputInstance.AdcFile >= MIN_HANDLE)
  {
    pAdcTmp  =  (ANALOG*)mmap(0, sizeof(ANALOG), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, InputInstance.AdcFile, 0);

    if (pAdcTmp == MAP_FAILED)
    {
#ifndef Linux_X86
      LogErrorNumber(ANALOG_SHARED_MEMORY);
      Result  =  FAIL;
#endif
      InputInstance.DCMUpdate   =  0;
    }
    else
    {
      InputInstance.pAnalog  =  pAdcTmp;
    }
  }
  else
  {
#ifndef Linux_X86
    LogErrorNumber(ANALOG_DEVICE_FILE_NOT_FOUND);
    Result  =  FAIL;
#endif
    InputInstance.DCMUpdate   =  0;
  }


  if (InputInstance.UartFile >= MIN_HANDLE)
  {
    pUartTmp  =  (UART*)mmap(0, sizeof(UART), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, InputInstance.UartFile, 0);

    if (pUartTmp == MAP_FAILED)
    {
#ifndef Linux_X86
      LogErrorNumber(UART_SHARED_MEMORY);
      Result  =  FAIL;
#endif
      InputInstance.DCMUpdate   =  0;
    }
    else
    {
      InputInstance.pUart  =  pUartTmp;
    }
  }
  else
  {
#ifndef Linux_X86
    LogErrorNumber(UART_DEVICE_FILE_NOT_FOUND);
    Result  =  FAIL;
#endif
//    InputInstance.DCMUpdate   =  0;
  }


  if (InputInstance.IicFile >= MIN_HANDLE)
  {
    pIicTmp  =  (IIC*)mmap(0, sizeof(UART), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, InputInstance.IicFile, 0);

    if (pIicTmp == MAP_FAILED)
    {
#ifndef Linux_X86
      LogErrorNumber(IIC_SHARED_MEMORY);
      Result  =  FAIL;
#endif
      InputInstance.DCMUpdate   =  0;
    }
    else
    {
      InputInstance.pIic  =  pIicTmp;
    }
  }
  else
  {
#ifndef Linux_X86
    LogErrorNumber(IIC_DEVICE_FILE_NOT_FOUND);
    Result  =  FAIL;
#endif
//    InputInstance.DCMUpdate   =  0;
  }


  cInputTypeDataInit();
  cInputCalDataInit();

  for (Tmp = 0;Tmp < DEVICES;Tmp++)
  {
    for (Set = 0;Set < MAX_DEVICE_DATASETS;Set++)
    {
      InputInstance.DeviceData[Tmp].Raw[Set]  =  DATAF_NAN;
    }
    InputInstance.DeviceData[Tmp].Owner       =  0;
    InputInstance.DeviceData[Tmp].Busy        =  0;
    InputInstance.DeviceData[Tmp].Connection  =  CONN_NONE;
    InputInstance.DeviceData[Tmp].DevStatus   =  BUSY;
    InputInstance.DeviceData[Tmp].TypeIndex   =  InputInstance.NoneIndex;
    InputInstance.DeviceType[Tmp]             =  TYPE_NONE;
    InputInstance.DeviceMode[Tmp]             =  0;
#ifndef DISABLE_BUMBED
    InputInstance.DeviceData[Tmp].Changes     =  (DATA32)0;
    InputInstance.DeviceData[Tmp].Bumps       =  (DATA32)0;
#endif
  }

  for (Tmp = 0;Tmp < INPUT_PORTS;Tmp++)
  {
    InputInstance.TmpMode[Tmp]  =  MAX_DEVICE_MODES;
  }

  for (TmpPrgId = 0;TmpPrgId < MAX_PROGRAMS;TmpPrgId++)
  {
    InputInstance.ConfigurationChanged[TmpPrgId]  =  0;
  }

  return (Result);
}


RESULT    cInputOpen(void)
{
  RESULT  Result = FAIL;

  Result  =  OK;

  return (Result);
}


RESULT    cInputClose(void)
{
  RESULT  Result = FAIL;

  Result  =  OK;

  return (Result);
}


RESULT    cInputExit(void)
{
  RESULT  Result = FAIL;

  cInputCalDataExit();

  if (InputInstance.AdcFile >= MIN_HANDLE)
  {
    munmap(InputInstance.pAnalog,sizeof(ANALOG));
    close(InputInstance.AdcFile);
  }

  if (InputInstance.UartFile >= MIN_HANDLE)
  {
    munmap(InputInstance.pUart,sizeof(UART));
    close(InputInstance.UartFile);
  }

  if (InputInstance.IicFile >= MIN_HANDLE)
  {
    munmap(InputInstance.pIic,sizeof(IIC));
    close(InputInstance.IicFile);
  }

  if (InputInstance.DcmFile >= MIN_HANDLE)
  {
    munmap(InputInstance.pUart,sizeof(UART));
    close(InputInstance.DcmFile);
  }

  if (InputInstance.IicString != NULL)
  {
    free((void*)InputInstance.IicString);
  }
  if (InputInstance.TypeData != NULL)
  {
    free((void*)InputInstance.TypeData);
  }

  Result  =  OK;

  return (Result);
}


DATA8     cInputGetDevice(void)
{
  DATA8   Layer;
  DATA8   No;

  Layer  =  *(DATA8*)PrimParPointer();
  No     =  *(DATA8*)PrimParPointer();

  return (No + (Layer * INPUT_PORTS));
}


void      cInputSetType(DATA8 Device,DATA8 Type,DATA8 Mode,int Line)
{
  if (InputInstance.DeviceData[Device].DevStatus == OK)
  {
    if (Type == TYPE_KEEP)
    {
      Type  =  InputInstance.DeviceType[Device];
    }
    if (Mode == MODE_KEEP)
    { // Get actual mode

      Mode  =  InputInstance.DeviceMode[Device];
    }
    if ((InputInstance.DeviceType[Device] != Type) || (InputInstance.DeviceMode[Device] != Mode))
    {
      cInputSetDeviceType(Device,Type,Mode,Line);
    }
  }
}


//******* BYTE CODE SNIPPETS **************************************************


/*! \page cInput Input
 *  <hr size="1"/>
 *  <b>     opINPUT_DEVICE_LIST (LENGTH, ARRAY, CHANGED)  </b>
 *
 *- Read all available devices on input and output(chain)\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LENGTH   - Maximum number of device types (normally 32)
 *  \return (DATA8)   ARRAY    - First element of DATA8 array of types (normally 32)
 *  \return (DATA8)   CHANGED  - Changed status
 */
/*! \brief  opINPUT_DEVICE_LIST byte code
 *
 */
void      cInputDeviceList(void)
{
  PRGID   TmpPrgId;
  DATA8   Length;
  DATA8   *pDevices;
  DATA8   *pChanged;
  DATA8   Count;

  TmpPrgId            =  CurrentProgramId();
  Length              =  *(DATA8*)PrimParPointer();
  pDevices            =  (DATA8*)PrimParPointer();
  pChanged            =  (DATA8*)PrimParPointer();

  *pChanged                                     =  InputInstance.ConfigurationChanged[TmpPrgId];
  InputInstance.ConfigurationChanged[TmpPrgId]  =  0;
  Count                                         =  0;
  while ((Count < Length) && (Count < DEVICES))
  {
    pDevices[Count]   =  InputInstance.DeviceType[Count];

    Count++;
  }
  if (Count < Length)
  {
    pDevices[Count]  =  0;
  }
}


/*! \page cInput
 *  <hr size="1"/>
 *  <b>     opINPUT_DEVICE (CMD, .....)  </b>
 *
 *- Read information about device\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   CMD               - \ref inputdevicesubcode
 *
 *\n
 *  - CMD = GET_TYPEMODE
 *\n  Get device type and mode\n
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \return (DATA8) \ref types "TYPE" - Device type
 *    -  \return (DATA8)   MODE         - Device mode [0..7]
 *
 *\n
 *  - CMD = GET_NAME
 *\n  Get device name\n
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = GET_SYMBOL
 *\n  Get device symbol\n
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = GET_FORMAT
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \return (DATA8)   DATASETS     - Number of data sets\n
 *    -  \return (DATA8)   FORMAT       - Format [0..3]\n
 *    -  \return (DATA8)   MODES        - Number of modes [1..8]\n
 *    -  \return (DATA8)   VIEWS        - Number of views [1..8]\n
 *
 *\n
 *  - CMD = GET_RAW
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \return (DATA32)  VALUE        - 32 bit raw value\n
 *
 *\n
 *  - CMD = GET_MODENAME
 *\n  Get device mode name\n
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \param  (DATA8)   MODE         - Mode\n
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = GET_FIGURES
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \return (DATA8)   FIGURES      - Total number of figures (inclusive decimal point and decimals\n
 *    -  \return (DATA8)   DECIMALS     - Number of decimals\n
 *
 *\n
 *  - CMD = GET_MINMAX
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \return (DATAF)   MIN          - Min SI value\n
 *    -  \return (DATAF)   MAX          - Max SI value\n
 *
 *\n
 *  - CMD = READY_PCT
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \param  (DATA8) \ref types "TYPE" - Device type (0 = don't change type)
 *    -  \param  (DATA8)   MODE         - Device mode [0..7] (-1 = don't change mode)
 *    -  \param  (DATA8)   VALUES       - Number of return values
 *
 *       if (VALUES == 1)
 *       \return (DATA8)   VALUE1       - First value from input
 *
 *\n
 *  - CMD = READY_RAW
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \param  (DATA8) \ref types "TYPE" - Device type (0 = don't change type)
 *    -  \param  (DATA8)   MODE         - Device mode [0..7] (-1 = don't change mode)
 *    -  \param  (DATA8)   VALUES       - Number of return values
 *
 *       if (VALUES == 1)
 *       \return (DATA32)  VALUE1       - First value from input
 *
 *\n
 *  - CMD = READY_SI
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \param  (DATA8) \ref types "TYPE" - Device type (0 = don't change type)
 *    -  \param  (DATA8)   MODE         - Device mode [0..7] (-1 = don't change mode)
 *    -  \param  (DATA8)   VALUES       - Number of return values
 *
 *       if (VALUES == 1)
 *       \return (DATAF)   VALUE1       - First value from input
 *
 *\n
 *  - CMD = GET_CHANGES
 *\n  Get positive changes since last clear\n
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \return (DATAF)   VALUE        - Positive changes since last clear\n
 *
 *\n
 *  - CMD = GET_BUMPS
 *\n  Get negative changes since last clear\n
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \return (DATAF)   VALUE        - Negative changes since last clear\n
 *
 *\n
 *  - CMD = CLR_CHANGES
 *\n  Clear changes and bumps\n
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *
 *\n
 *  - CMD = CAL_MINMAX
 *\n  Apply new minimum and maximum raw value for device type to be used in scaling PCT and SI\n
 *    -  \param  (DATA8) \ref types "TYPE" - Device type [1..101]
 *    -  \param  (DATA8)   MODE         - Device mode [0..7]
 *    -  \param  (DATA32)  CAL_MIN      - 32 bit raw minimum value (Zero)\n
 *    -  \param  (DATA32)  CAL_MAX      - 32 bit raw maximum value (Full scale)\n
 *
 *\n
 *  - CMD = CAL_MIN
 *\n  Apply new minimum raw value for device type to be used in scaling PCT and SI\n
 *    -  \param  (DATA8) \ref types "TYPE" - Device type [1..101]
 *    -  \param  (DATA8)   MODE         - Device mode [0..7]
 *    -  \param  (DATA32)  CAL_MIN      - 32 bit SI minimum value (Zero)\n
 *
 *\n
 *  - CMD = CAL_MAX
 *\n  Apply new maximum raw value for device type to be used in scaling PCT and SI\n
 *    -  \param  (DATA8) \ref types "TYPE" - Device type [1..101]
 *    -  \param  (DATA8)   MODE         - Device mode [0..7]
 *    -  \param  (DATA32)  CAL_MAX      - 32 bit SI maximum value (Full scale)\n
 *
 *\n
 *  - CMD = CAL_DEFAULT
 *\n  Apply the default minimum and maximum raw value for device type to be used in scaling PCT and SI\n
 *    -  \param  (DATA8) \ref types "TYPE" - Device type [1..101]
 *    -  \param  (DATA8)   MODE         - Device mode [0..7]
 *
 *\n
 *  - CMD = SETUP
 *\n  Generic setup/read IIC sensors \ref cinputdevicesetup "Example"\n
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3]
 *    -  \param  (DATA8)   NO           - Port number
 *    -  \param  (DATA8)   REPEAT       - Repeat setup/read "REPEAT" times  (0 = infinite)
 *    -  \param  (DATA16)  TIME         - Time between repeats [10..1000mS] (0 = 10)
 *    -  \param  (DATA8)   WRLNG        - No of bytes to write
 *    -  \param  (DATA8)   WRDATA       - DATA8 array  (handle) of data to write\n
 *    -  \param  (DATA8)   RDLNG        - No of bytes to read
 *    -  \return (DATA8)   RDDATA       - DATA8 array  (handle) to read into\n
 *
 *\n
 *  - CMD = CLR_ALL
 *\n  Clear all devices (e.c. counters, angle, ...)\n
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3] (-1 = all)
 *
 *\n
 *  - CMD = STOP_ALL
 *\n  Stop all devices (e.c. motors, ...)\n
 *    -  \param  (DATA8)   LAYER        - Chain layer number [0..3] (-1 = all)
 *
 *\n
 *
 */
/*! \brief  opINPUT_DEVICE byte code
 *
 */
void      cInputDevice(void)
{
  IP      TmpIp;
  DATA8   Cmd;
  DATA8   Device = 0;
  DATA8   Layer;
  DATA8   Length;
  DATA8   *pDestination;
  DATA8   Count = 0;
  DATA8   Modes = 0;
  DATA8   *TmpName;
  DATA8   Data8 = 0;
  DATA32  Data32 = 0;
  DATA8   Type;
  DATA8   Mode;
  DATA8   Connection;
  DATA8   Values;
  DATA8   Value;
  DATA8   Views = 0;
  UWORD   Index = 0;
  OBJID   Owner;
  DATA8   Busy;
  DATAF   DataF = (DATAF)0;
  DATAF   Min = (DATAF)0;
  DATAF   Max = (DATAF)0;
  DATA8   Tmp;
  DATA8   Repeat;
  DATA16  Time;
  DATA8   WrLng;
  DATA8   RdLng;
  DATA8   *pWrData;
  DATA8   *pRdData;
  UWORD   TypeIndex;
#ifndef    DISABLE_DAISYCHAIN_COM_CALL
  RESULT  Result;
#endif


  TmpIp   =  GetObjectIp();
  Cmd     =  *(DATA8*)PrimParPointer();
  if ((Cmd != CAL_MINMAX) && (Cmd != CAL_MIN) && (Cmd != CAL_MAX) && (Cmd != CAL_DEFAULT) && (Cmd != CLR_ALL) && (Cmd != STOP_ALL))
  {
    Device  =  cInputGetDevice();
  }

  switch (Cmd)
  { // Function

    case CAL_MINMAX :
    {
      Type    =  *(DATA8*)PrimParPointer();
      Mode    =  *(DATA8*)PrimParPointer();
      Min     =  (DATAF)*(DATA32*)PrimParPointer();
      Max     =  (DATAF)*(DATA32*)PrimParPointer();

      if ((Type > 0) && (Type < (MAX_DEVICE_TYPE + 1)) && (Mode >= 0) && (Mode < MAX_DEVICE_MODES))
      {
        InputInstance.Calib[Type][Mode].Min       =  Min;
        InputInstance.Calib[Type][Mode].Max       =  Max;
      }
    }
    break;

    case CAL_MIN :
    {
      Type    =  *(DATA8*)PrimParPointer();
      Mode    =  *(DATA8*)PrimParPointer();
      Min     =  (DATAF)*(DATA32*)PrimParPointer();

      if ((Type > 0) && (Type < (MAX_DEVICE_TYPE + 1)) && (Mode >= 0) && (Mode < MAX_DEVICE_MODES))
      {
        if (cInputFindDevice(Type,Mode,&TypeIndex) == OK)
        {
          if (InputInstance.Calib[Type][Mode].InUse == 0)
          {
            InputInstance.Calib[Type][Mode].InUse   =  1;
            InputInstance.Calib[Type][Mode].Max     =  InputInstance.TypeData[TypeIndex].RawMax;
          }
          else
          {
            Min     =  InputInstance.Calib[Type][Mode].Min + (((Min - InputInstance.TypeData[TypeIndex].SiMin) * (InputInstance.Calib[Type][Mode].Max - InputInstance.Calib[Type][Mode].Min)) / (InputInstance.TypeData[TypeIndex].SiMax - InputInstance.TypeData[TypeIndex].SiMin));
          }
          InputInstance.Calib[Type][Mode].Min       =  Min;
        }
      }
    }
    break;

    case CAL_MAX :
    {
      Type    =  *(DATA8*)PrimParPointer();
      Mode    =  *(DATA8*)PrimParPointer();
      Max     =  (DATAF)*(DATA32*)PrimParPointer();

      if ((Type > 0) && (Type < (MAX_DEVICE_TYPE + 1)) && (Mode >= 0) && (Mode < MAX_DEVICE_MODES))
      {
        if (cInputFindDevice(Type,Mode,&TypeIndex) == OK)
        {
          if (InputInstance.Calib[Type][Mode].InUse == 0)
          {
            InputInstance.Calib[Type][Mode].InUse   =  1;
            InputInstance.Calib[Type][Mode].Min     =  InputInstance.TypeData[TypeIndex].RawMin;
          }
          else
          {
            Max     =  InputInstance.Calib[Type][Mode].Min + (((Max - InputInstance.TypeData[TypeIndex].SiMin) * (InputInstance.Calib[Type][Mode].Max - InputInstance.Calib[Type][Mode].Min)) / (InputInstance.TypeData[TypeIndex].SiMax - InputInstance.TypeData[TypeIndex].SiMin));
          }
          InputInstance.Calib[Type][Mode].Max       =  Max;
        }
      }
    }
    break;

    case CAL_DEFAULT :
    {
      Type    =  *(DATA8*)PrimParPointer();
      Mode    =  *(DATA8*)PrimParPointer();

      if ((Type > 0) && (Type < (MAX_DEVICE_TYPE + 1)) && (Mode >= 0) && (Mode < MAX_DEVICE_MODES))
      {
        InputInstance.Calib[Type][Mode].InUse   =  0;
      }
    }
    break;

    case GET_TYPEMODE :
    {
      Type    =  TYPE_NONE;
      Mode    =  0;

      if (Device < DEVICES)
      {
        Type  =  InputInstance.DeviceType[Device];
        Mode  =  InputInstance.DeviceMode[Device];
      }
      *(DATA8*)PrimParPointer()  =  Type;
      *(DATA8*)PrimParPointer()  =  Mode;
    }
    break;

    case GET_CONNECTION :
    {
      Connection  =  TYPE_NONE;

      if (Device < DEVICES)
      {
        Connection  =  InputInstance.DeviceData[Device].Connection;
      }
      *(DATA8*)PrimParPointer()  =  Connection;
    }
    break;

    case GET_NAME :
    {
      Length        =  *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();
      Count         =  0;

      if (Device < DEVICES)
      {

        TmpName  =  InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Name;

        if (VMInstance.Handle >= 0)
        {
          Tmp  =  (DATA8)strlen((char*)TmpName) + 1;

          if (Length == -1)
          {
            Length  =  Tmp;
          }
          pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
        }
        if (pDestination != NULL)
        {
          while ((Count < (Length - 1)) && (TmpName[Count]))
          {
            pDestination[Count]  =  TmpName[Count];

            Count++;
          }
          while (Count < (Length - 1))
          {
            pDestination[Count]  =  ' ';

            Count++;
          }
        }
      }
      if (pDestination != NULL)
      {
        pDestination[Count]  =  0;
      }
    }
    break;

    case GET_SYMBOL :
    {
      Length        =  *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();
      Count         =  0;

      if (Device < DEVICES)
      {

        TmpName  =  InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Symbol;

        if (VMInstance.Handle >= 0)
        {
          Tmp  =  (DATA8)strlen((char*)TmpName) + 1;

          if (Length == -1)
          {
            Length  =  Tmp;
          }
          pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
        }
        if (pDestination != NULL)
        {
          while ((Count < (Length - 1)) && (TmpName[Count]))
          {
            pDestination[Count]  =  TmpName[Count];

            Count++;
          }
          while (Count < (Length - 1))
          {
            pDestination[Count]  =  ' ';

            Count++;
          }
        }
      }
      if (pDestination != NULL)
      {
        pDestination[Count]  =  0;
      }
    }
    break;

    case GET_FORMAT :
    {
      if (Device < DEVICES)
      {
        Count  =  (DATA8)InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].DataSets;
        Data8  =  (DATA8)InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Format;
        Modes  =  (DATA8)InputInstance.TypeModes[InputInstance.DeviceType[Device]];
        Views  =  (DATA8)InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Views;
      }
      *(DATA8*)PrimParPointer()  =  Count;
      *(DATA8*)PrimParPointer()  =  Data8;
      *(DATA8*)PrimParPointer()  =  Modes;
      *(DATA8*)PrimParPointer()  =  Views;
    }
    break;

    case GET_RAW :
    {
      Data32   =  DATA32_NAN;
      if (Device < DEVICES)
      {
        DataF  =  cInputReadDeviceRaw(Device,0,0,NULL);
        if (!isnan(DataF))
        {
          Data32  =  (DATA32)DataF;
        }
      }
      *(DATA32*)PrimParPointer()  =  Data32;
    }
    break;

    case GET_FIGURES :
    {
      if (Device < DEVICES)
      {
        Count  =  (DATA8)InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Figures;
        Data8  =  (DATA8)InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].Decimals;
      }
      *(DATA8*)PrimParPointer()  =  Count;
      *(DATA8*)PrimParPointer()  =  Data8;
    }
    break;

    case GET_MINMAX :
    {
      if (Device < DEVICES)
      {
        Min  =  (DATAF)InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].SiMin;
        Max  =  (DATAF)InputInstance.TypeData[InputInstance.DeviceData[Device].TypeIndex].SiMax;
      }
      *(DATAF*)PrimParPointer()  =  Min;
      *(DATAF*)PrimParPointer()  =  Max;
    }
    break;

    case GET_MODENAME :
    {
      Mode          =  *(DATA8*)PrimParPointer();
      Length        =  *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();
      Count         =  0;

      if (Device < DEVICES)
      {
        Type    =  InputInstance.DeviceType[Device];
        if ((Type >= 0) && (Type < (MAX_DEVICE_TYPE + 1)))
        {
          // try to find device type
          Index =  0;
          while (Index < InputInstance.MaxDeviceTypes)
          {
            if (InputInstance.TypeData[Index].Type == Type)
            { // match on type

              if (InputInstance.TypeData[Index].Mode == Mode)
              { // match on mode

                TmpName  =  InputInstance.TypeData[Index].Name;

                if (VMInstance.Handle >= 0)
                {
                  Tmp  =  (DATA8)strlen((char*)TmpName) + 1;

                  if (Length == -1)
                  {
                    Length  =  Tmp;
                  }
                  pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
                }
                if (pDestination != NULL)
                {
                  while ((Count < (Length - 1)) && (TmpName[Count]))
                  {
                    pDestination[Count]  =  TmpName[Count];

                    Count++;
                  }
                  while (Count < (Length - 1))
                  {
                    pDestination[Count]  =  ' ';

                    Count++;
                  }
                }
                Index        =  InputInstance.MaxDeviceTypes;
              }
            }
            Index++;
          }
        }
      }
      if (pDestination != NULL)
      {
        pDestination[Count]  =  0;
      }
    }
    break;

    case SET_RAW :
    {
      Type    =  *(DATA8*)PrimParPointer();
      Data32  =  *(DATA32*)PrimParPointer();

      cInputWriteDeviceRaw(Device,InputInstance.DeviceData[Device].Connection,Type,Data32);
    }
    break;

#ifndef DISABLE_BUMBED
    case GET_CHANGES :
    {
      DataF  =  DATAF_NAN;
      DataF  =  (DATAF)0;
      if (Device < DEVICES)
      {
        if ((InputInstance.DeviceData[Device].Connection != CONN_NONE) && (InputInstance.DeviceData[Device].Connection != CONN_ERROR))
        {
          DataF   =  (DATAF)InputInstance.DeviceData[Device].Changes;
        }
      }
      *(DATAF*)PrimParPointer()  =  DataF;
    }
    break;

    case GET_BUMPS :
    {
      DataF  =  DATAF_NAN;
      DataF  =  (DATAF)0;
      if (Device < DEVICES)
      {
        if ((InputInstance.DeviceData[Device].Connection != CONN_NONE) && (InputInstance.DeviceData[Device].Connection != CONN_ERROR))
        {
          DataF   =  (DATAF)InputInstance.DeviceData[Device].Bumps;
        }
      }
      *(DATAF*)PrimParPointer()  =  DataF;
    }
    break;

    case CLR_CHANGES :
    {
      if (Device < DEVICES)
      {
        InputInstance.DeviceData[Device].Changes  =  (DATA32)0;
        InputInstance.DeviceData[Device].Bumps    =  (DATA32)0;
      }
    }
    break;
#endif

    case READY_PCT : // Fall through
    case READY_RAW : // Fall through
    case READY_SI :
    {
      Type        =  *(DATA8*)PrimParPointer();
      Mode        =  *(DATA8*)PrimParPointer();
      Values      =  *(DATA8*)PrimParPointer();

      Value       =  0;
      Busy        =  0;
      Owner       =  CallingObjectId();

#ifdef DEBUG_C_INPUT_DAISYCHAIN
      if (Device == TESTDEVICE)
      {
        printf("c_input   opINPUT_DEVICE READY_XX: D=%-3d T=%-3d M=%-3d B=%d C=%-3d\r\n",Device,Type,Mode,InputInstance.DeviceData[Device].DevStatus,InputInstance.DeviceData[Device].Connection);
      }
#endif
      if (Device < DEVICES)
      { // Device valid

        if ((InputInstance.DeviceData[Device].Connection != CONN_NONE) && (InputInstance.DeviceData[Device].Connection != CONN_ERROR))
        { // Device present

          if (Type == TYPE_KEEP)
          { // Get actual type

            Type  =  InputInstance.DeviceType[Device];
          }
          if (Mode == MODE_KEEP)
          { // Get actual mode

            Mode  =  InputInstance.DeviceMode[Device];
          }
          if (InputInstance.DeviceData[Device].Busy == 0)
          {
            if ((InputInstance.DeviceType[Device] != Type) || (InputInstance.DeviceMode[Device] != Mode))
            { // Must change type or mode so check if owner is OK

              if ((InputInstance.DeviceData[Device].Owner == 0) || (InputInstance.DeviceData[Device].Owner == Owner))
              { // Owner is OK

#ifdef DEBUG_TRACE_MODE_CHANGE
              printf("c_input   opINPUT_DEVICE READY_XX: D=%-3d Change to type %d mode %d\r\n",Device,Type,Mode);
#endif
                InputInstance.DeviceData[Device].Owner  =  Owner;
                cInputSetDeviceType(Device,Type,Mode,__LINE__);
                InputInstance.DeviceData[Device].TimeoutTimer  =  MAX_DEVICE_BUSY_TIME;
                InputInstance.DeviceData[Device].Busy   =  0;
#ifdef ENABLE_STATUS_TEST
                if (Device == TESTDEVICE)
                {
                  VMInstance.Status &= ~0x40;
                }
#endif
              }
              else
              { // Another owner

#ifdef DEBUG_TRACE_MODE_CHANGE
                printf("c_input   opINPUT_DEVICE READY_XX: D=%-3d Trying to change to type %d mode %d\r\n",Device,Type,Mode);
#endif
#ifdef ENABLE_STATUS_TEST
                if (Device == TESTDEVICE)
                {
                  VMInstance.Status |=  0x40;
                }
#endif
                Busy  =  1;
              }
            }
          }
          if (Busy == 0)
          {
            if (InputInstance.DeviceData[Device].DevStatus == BUSY)
            {
              Busy  =  1;

              if (InputInstance.DeviceData[Device].Busy == 0)
              {
                InputInstance.DeviceData[Device].TimeoutTimer  =  MAX_DEVICE_BUSY_TIME;
                InputInstance.DeviceData[Device].Busy  =  1;
              }
              else
              {
                if (InputInstance.DeviceData[Device].TimeoutTimer == 0)
                {
  #ifdef DEBUG_TRACE_MODE_CHANGE
                  printf("c_input   opINPUT_DEVICE READY_XX: D=%-3d Timeout when trying to change to type %d mode %d\r\n",Device,Type,Mode);
  #endif
                  InputInstance.DeviceData[Device].Owner  =  0;
                  InputInstance.DeviceData[Device].Busy   =  0;
                  Busy  =  0;
                }
              }
            }
            else
            {
              while ((Value < Values) && (Value < MAX_DEVICE_DATASETS))
              {
                switch (Cmd)
                {
                  case READY_PCT :
                  {
                    *(DATA8*)PrimParPointer()     =  (DATA8)cInputReadDevicePct(Device,Value,0,NULL);
                  }
                  break;

                  case READY_RAW :
                  {
                    DataF  =  cInputReadDeviceRaw(Device,Value,0,NULL);
                    if (isnan(DataF))
                    {
                      *(DATA32*)PrimParPointer()  =  DATA32_NAN;
                    }
                    else
                    {
                      *(DATA32*)PrimParPointer()  =  (DATA32)DataF;
                    }
                  }
                  break;

                  case READY_SI :
                  {
                    DataF                       =  (DATAF)cInputReadDeviceSi(Device,Value,0,NULL);
                    *(DATAF*)PrimParPointer()   =  DataF;

                  }
                  break;

                }
                Value++;
              }
            }

          }
        }
        else
        {
          SetInstructions(0);
        }
        if (InputInstance.DeviceData[Device].DevStatus != BUSY)
        {
          InputInstance.DeviceData[Device].Owner  =  0;
          InputInstance.DeviceData[Device].Busy   =  0;
  #ifdef ENABLE_STATUS_TEST
          if (Device == TESTDEVICE)
          {
            VMInstance.Status &= ~0x20;
          }
  #endif
        }
  #ifdef ENABLE_STATUS_TEST
        else
        {
          if (Device == TESTDEVICE)
          {
            VMInstance.Status |=  0x20;
          }
        }
  #endif
      }

      if  (Busy)
      { // Busy -> block VMThread

        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
      else
      { // Not busy -> be sure to pop all parameters

        while (Value < Values)
        {
          switch (Cmd)
          {
            case READY_PCT :
            {
              *(DATA8*)PrimParPointer()   =  DATA8_NAN;
            }
            break;

            case READY_RAW :
            {
              *(DATA32*)PrimParPointer()  =  DATA32_NAN;
            }
            break;

            case READY_SI :
            {
              *(DATAF*)PrimParPointer()   =  DATAF_NAN;
            }
            break;

          }
          Value++;
        }
      }
    }
    break;

    case SETUP :
    { // INPUT_DEVICE(SETUP,LAYER,NO,REPEAT,TIME,WRLNG,WRDATA,RDLNG,RDDATA)

      Repeat        =  *(DATA8*)PrimParPointer();
      Time          =  *(DATA16*)PrimParPointer();
      WrLng         =  *(DATA8*)PrimParPointer();
      pWrData       =  (DATA8*)PrimParPointer();
      RdLng         =  *(DATA8*)PrimParPointer();
      pRdData       =  (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        pRdData  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)RdLng);
      }
      if (pRdData != NULL)
      {

        if  (cInputSetupDevice(Device,Repeat,Time,WrLng,pWrData,RdLng,pRdData) == BUSY)
        { // Busy -> block VMThread

          SetObjectIp(TmpIp - 1);
          SetDispatchStatus(BUSYBREAK);
        }
      }
    }
    break;

    case CLR_ALL :
    {
      Layer   =  *(DATA8*)PrimParPointer();

      if (Layer == 0)
      {
        ClrLayer();
      }
#ifndef    DISABLE_DAISYCHAIN_COM_CALL
      else
      {
        Result  =  OK;

        if (Layer == -1)
        {
          MoreLayers    =  1;
          Layer         =  ActLayer;
        }

        if (Layer == 0)
        {
          ClrLayer();
        }
        else
        {
          Result          =  cDaisyReady();

          if (Result == OK)
          { // Ready for command

            DaisyBuf[0]   =  0;
            DaisyBuf[1]   =  0;
            DaisyBuf[2]   =  opINPUT_DEVICE;
            DaisyBuf[3]   =  CLR_ALL;
            DaisyBuf[4]   =  0;

            Result        =  cDaisyDownStreamCmd(DaisyBuf, 5, Layer);
          }
        }

        if (Result != BUSY)
        { // If job done

          if (MoreLayers)
          { // More layers ?

            ActLayer++;
            if (ActLayer >= CHAIN_DEPT)
            { // No more layers

              ActLayer    =  0;
              MoreLayers  =  0;
            }
            else
            { // Next layer

              Result    =  BUSY;
            }
          }
        }

        if (Result == BUSY)
        { // Job not done - hold execution

          SetObjectIp(TmpIp - 1);
          SetDispatchStatus(BUSYBREAK);
        }
      }
#endif
    }
    break;

    case STOP_ALL :
    {
      Layer   =  *(DATA8*)PrimParPointer();

      if (Layer == 0)
      {
        StopLayer();
      }
#ifndef    DISABLE_DAISYCHAIN_COM_CALL
      else
      {
        Result  =  OK;

        if (Layer == -1)
        {
          MoreLayers    =  1;
          Layer         =  ActLayer;
        }

        if (Layer == 0)
        {
          StopLayer();
        }
        else
        {
          Result          =  cDaisyReady();

          if (Result == OK)
          { // Ready for command

            DaisyBuf[0]   =  0;
            DaisyBuf[1]   =  0;
            DaisyBuf[2]   =  opINPUT_DEVICE;
            DaisyBuf[3]   =  STOP_ALL;
            DaisyBuf[4]   =  0;

            Result        =  cDaisyDownStreamCmd(DaisyBuf, 5, Layer);
          }
        }

        if (Result != BUSY)
        { // If job done

          if (MoreLayers)
          { // More layers ?

            ActLayer++;
            if (ActLayer >= CHAIN_DEPT)
            { // No more layers

              ActLayer    =  0;
              MoreLayers  =  0;
            }
            else
            { // Next layer

              Result    =  BUSY;
            }
          }
        }

        if (Result == BUSY)
        { // Job not done - hold execution

          SetObjectIp(TmpIp - 1);
          SetDispatchStatus(BUSYBREAK);
        }
      }
#endif
    }
    break;

  }
}


/*! \page cInput
 *  <hr size="1"/>
 *  <b>     opINPUT_READ (LAYER, NO, TYPE, MODE, PCT)  </b>
 *
 *- Read device value in Percent\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NO      - Port number
 *  \param  (DATA8) \ref types "TYPE" - Device type (0 = don't change type)
 *  \param  (DATA8)   MODE    - Device mode [0..7] (-1 = don't change mode)
 *  \return (DATA8)   PCT     - Percent value from device
 */
/*! \brief  opINPUT_READ byte code
 *
 */
void      cInputRead(void)
{
  DATA8   Type;
  DATA8   Mode;
  DATA8   Device;

  Device      =  cInputGetDevice();
  Type        =  *(DATA8*)PrimParPointer();
  Mode        =  *(DATA8*)PrimParPointer();

  if (Device < DEVICES)
  {
    cInputSetType(Device,Type,Mode,__LINE__);
  }
  *(DATA8*)PrimParPointer()  =  cInputReadDevicePct(Device,0,0,NULL);
}


/*! \page cInput
 *  <hr size="1"/>
 *  <b>     opINPUT_READSI (LAYER, NO, TYPE, MODE, SI)  </b>
 *
 *- Read device value in SI units\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NO      - Port number
 *  \param  (DATA8) \ref types "TYPE" - Device type (0 = don't change type)
 *  \param  (DATA8)   MODE    - Device mode [0..7] (-1 = don't change mode)
 *  \return (DATAF)   SI      - SI unit value from device
 */
/*! \brief  opINPUT_READSI byte code
 *
 */
void      cInputReadSi(void)
{
  DATA8   Type;
  DATA8   Mode;
  DATA8   Device;

  Device      =  cInputGetDevice();
  Type        =  *(DATA8*)PrimParPointer();
  Mode        =  *(DATA8*)PrimParPointer();

#ifdef DEBUG_C_INPUT_DAISYCHAIN
  printf("c_input   opINPUT_READSI:          D=%-3d B=%d\r\n",Device,InputInstance.DeviceData[Device].DevStatus);
#endif
  if (Device < DEVICES)
  {
    cInputSetType(Device,Type,Mode,__LINE__);
  }
  *(DATAF*)PrimParPointer()  =  cInputReadDeviceSi(Device,0,0,NULL);
}


/*! \page   cInput
 *  <hr size="1"/>
 *  <b>     opINPUT_TEST (LAYER, NO, BUSY) </b>
 *
 *- Test if device busy (changing type or mode)\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER    - Chain layer number [0..3]
 *  \param  (DATA8)   NO       - Port number
 *  \return (DATA8)   BUSY     - Device busy flag (0 = ready, 1 = busy)
 *
 */
/*! \brief  opINPUT_TEST byte code
 *
 */
void      cInputTest(void)
{
  DATA8   Busy = 1;
  DATA8   Device;

  Device  =  cInputGetDevice();

  if (Device < DEVICES)
  {
#ifdef DEBUG_C_INPUT_DAISYCHAIN
    printf("c_input   opINPUT_TEST:            D=%-3d B=%d\r\n",Device,InputInstance.DeviceData[Device].DevStatus);
#endif
    if (InputInstance.DeviceData[Device].DevStatus != BUSY)
    {
      Busy  =  0;
    }
  }
  *(DATA8*)PrimParPointer()  =  Busy;
}



/*! \page   cInput
 *  <hr size="1"/>
 *  <b>     opINPUT_READY (LAYER, NO) </b>
 *
 *- Wait for device ready (wait for valid data)\n
 *- Dispatch status can change to BUSYBREAK
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NO      - Port number
 */
/*! \brief  opINPUT_READY byte code
 *
 */
void      cInputReady(void)
{
  IP      TmpIp;
  DATA8   Device;

  TmpIp   =  GetObjectIp();
  Device  =  cInputGetDevice();

  if (Device < DEVICES)
  {
    if (InputInstance.DeviceData[Device].DevStatus == BUSY)
    {
      SetObjectIp(TmpIp - 1);
      SetDispatchStatus(BUSYBREAK);
    }
  }
}


/*! \page cInput
 *  <hr size="1"/>
 *  <b>     opINPUT_WRITE (LAYER, NO, BYTES, DATA)  </b>
 *
 *- Write data to device (only UART devices)\n
 *- Dispatch status can change to BUSYBREAK
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NO      - Port number
 *  \param  (DATA8)   BYTES   - No of bytes to write [1..32]
 *  \param  (DATA8)   DATA    - First byte in DATA8 array to write
 */
/*! \brief  opINPUT_WRITE byte code
 *
 */
void      cInputWrite(void)
{
  DATA8   Bytes;
  DATA8   *Data;
  DATA8   Device;
  DATA8   Tmp;
  IP      TmpIp;
  DATA8   Buffer[UART_DATA_LENGTH + 1];
  DSPSTAT DspStat = FAILBREAK;

  TmpIp   =  GetObjectIp();

  Device  =  cInputGetDevice();
  Bytes   = *(DATA8*)PrimParPointer();
  Data    =  (DATA8*)PrimParPointer();

  if (Device < INPUT_DEVICES)
  {
    if (InputInstance.DeviceType[Device] != TYPE_TERMINAL)
    {
      if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_UART)
      {
        if ((Bytes > 0) && (Bytes <= UART_DATA_LENGTH))
        {
          if (((*InputInstance.pUart).Status[Device] & UART_WRITE_REQUEST))
          {
            DspStat  =  BUSYBREAK;
          }
          else
          {
            InputInstance.DeviceData[Device].DevStatus  =  BUSY;

            (*InputInstance.pUart).Status[Device]      &= ~UART_DATA_READY;

            Buffer[0]  =  Device;
            for (Tmp = 0;Tmp < Bytes;Tmp++)
            {
              Buffer[Tmp + 1]  =  Data[Tmp];
            }

            // write setup string to "UART Device Controller" driver
            if (InputInstance.UartFile >= MIN_HANDLE)
            {
              write(InputInstance.UartFile,Buffer,Bytes + 1);
            }
            DspStat  =  NOBREAK;
          }
        }
      }
      else
      {

        if (InputInstance.DeviceData[Device].Connection == CONN_NXT_IIC)
        {
          if ((Bytes > 0) && (Bytes <= IIC_DATA_LENGTH))
          {
            if (((*InputInstance.pIic).Status[Device] & IIC_WRITE_REQUEST))
            {
              DspStat  =  BUSYBREAK;
            }
            else
            {
              InputInstance.DeviceData[Device].DevStatus  =  BUSY;

              (*InputInstance.pIic).Status[Device]      &= ~IIC_DATA_READY;

              Buffer[0]  =  Device;
              for (Tmp = 0;Tmp < Bytes;Tmp++)
              {
                Buffer[Tmp + 1]  =  Data[Tmp];
              }

              // write setup string to "IIC Device Controller" driver
              if (InputInstance.IicFile >= MIN_HANDLE)
              {
                write(InputInstance.IicFile,Buffer,Bytes + 1);
              }
              DspStat  =  NOBREAK;
            }
          }
        }
        else
        { // don't bother if not UART or IIC device

          DspStat  =  NOBREAK;
        }
      }
    }
    else
    { // don't bother if TERMINAL

      DspStat  =  NOBREAK;
    }
  }
  if (DspStat == BUSYBREAK)
  { // Rewind IP

    SetObjectIp(TmpIp - 1);
  }
  SetDispatchStatus(DspStat);
}


/*! \page cInput
 *  <hr size="1"/>
 *  <b>     opINPUT_READEXT (LAYER, NO, TYPE, MODE, FORMAT, VALUES, VALUE1)  </b>
 *
 *- Read device value\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NO      - Port number
 *  \param  (DATA8) \ref types "TYPE" - Device type (0 = don't change type)
 *  \param  (DATA8)   MODE    - Device mode [0..7] (-1 = don't change mode)
 *  \param  (DATA8) \ref formats "FORMAT"  - Format (PCT, RAW, SI ...)
 *  \param  (DATA8)   VALUES  - Number of return values
 *
 *  if (VALUES == 1)
 *  \return (FORMAT)  VALUE1  - First value from device
 */
/*! \brief  opINPUT_READEXT byte code
 *
 */
void      cInputReadExt(void)
{
  DATAF   Raw;
  DATA8   Type;
  DATA8   Mode;
  DATA8   Format;
  DATA8   Values;
  DATA8   Device;
  DATA8   Value;

  Device  =  cInputGetDevice();
  Type    =  *(DATA8*)PrimParPointer();
  Mode    =  *(DATA8*)PrimParPointer();
  Format  =  *(DATA8*)PrimParPointer();
  Values  =  *(DATA8*)PrimParPointer();
  Value   =  0;

  if (Device < DEVICES)
  {
    cInputSetType(Device,Type,Mode,__LINE__);

    while ((Value < Values) && (Value < MAX_DEVICE_MODES))
    {
      switch (Format)
      {
        case DATA_PCT :
        {
          *(DATA8*)PrimParPointer()     =  (DATA8)cInputReadDevicePct(Device,Value,0,NULL);
        }
        break;

        case DATA_RAW :
        {
          Raw  =  cInputReadDeviceRaw(Device,Value,0,NULL);
          if (isnan(Raw))
          {
            *(DATA32*)PrimParPointer()  =  DATA32_NAN;
          }
          else
          {
            *(DATA32*)PrimParPointer()  =  (DATA32)Raw;
          }
        }
        break;

        case DATA_SI :
        {
          *(DATAF*)PrimParPointer()     =  (DATAF)cInputReadDeviceSi(Device,Value,0,NULL);
        }
        break;

        default :
        {
          *(DATA8*)PrimParPointer()     =  DATA8_NAN;
        }
        break;

      }
      Value++;
    }
  }
  while (Value < Values)
  {
    switch (Format)
    {
      case DATA_PCT :
      {
        *(DATA8*)PrimParPointer()   =  DATA8_NAN;
      }
      break;

      case DATA_RAW :
      {
        *(DATA32*)PrimParPointer()  =  DATA32_NAN;
      }
      break;

      case DATA_SI :
      {
        *(DATAF*)PrimParPointer()   =  DATAF_NAN;
      }
      break;

      default :
      {
        *(DATA8*)PrimParPointer()   =  DATA8_NAN;
      }
      break;

    }
    Value++;
  }
}


/*! \page cInput
 *  <hr size="1"/>
 *  <b>     opINPUT_SAMPLE (TIME, SAMPLES, INIT, DEVICES, TYPES, MODES, DATASETS, VALUES)  </b>
 *
 *- Sample devices (see \ref cinputsample "Example")\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  TIME      - Sample time [mS]
 *  \param  (DATA16)  SAMPLES   - Number of samples
 *  \param  (DATA16)  INIT      - DATA16 array (handle) - to start/reset buffer -> fill array with -1 otherwise don't change
 *  \param  (DATA8)   DEVICES   - DATA8 array  (handle) with devices to sample
 *  \param  (DATA8)   TYPES     - DATA8 array  (handle) with types
 *  \param  (DATA8)   MODES     - DATA8 array  (handle) with modes
 *  \param  (DATA8)   DATASETS  - DATA8 array  (handle) with data sets
 *  \return (DATAF)   VALUES    - DATAF array  (handle) with values
 *
 */
/*! \brief  opINPUT_SAMPLE byte code
 *
 */
void      cInputSample(void)
{
  DATA32  SampleTime;
  DATA32  Data32;
  DATA16  NoOfPorts;
  DATA16  *pInits;
  DATA8   *pDevices;
  DATA8   *pTypes;
  DATA8   *pModes;
  DATA8   *pDataSets;
  DATAF   *pValues;
  DATA16  Index;
  DATA8   Device;
  DATA8   Type;
  DATA8   Mode;


  SampleTime    =  *(DATA32*)PrimParPointer();
  NoOfPorts     =  *(DATA16*)PrimParPointer();
  pInits        =  (DATA16*)PrimParPointer();
  pDevices      =  (DATA8*)PrimParPointer();
  pTypes        =  (DATA8*)PrimParPointer();
  pModes        =  (DATA8*)PrimParPointer();
  pDataSets     =  (DATA8*)PrimParPointer();
  pValues       =  (DATAF*)PrimParPointer();
  if (VMInstance.Handle >= 0)
  {
    Data32      =  (DATA32)NoOfPorts;
    if (Data32 > MIN_ARRAY_ELEMENTS)
    {
      pValues   =  (DATAF*)VmMemoryResize(VMInstance.Handle,Data32);
    }
  }
  if (pValues != NULL)
  {
    for (Index = 0;Index < NoOfPorts;Index++)
    { // Service all devices

      Device    =  pDevices[Index];

      if (Device >= INPUTS)
      {
        Device +=  12;
      }

      pValues[Index]  =  DATAF_NAN;

      if ((Device >= 0) && (Device < DEVICES))
      {
        Type    =  pTypes[Index];
        Mode    =  pModes[Index];

        cInputSetType(Device,Type,Mode,__LINE__);

        pValues[Index]  =  cInputReadDeviceSi(Device,pDataSets[Index],SampleTime,&pInits[Index]);
      }
    }
  }
}


//*****************************************************************************
