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


/*! \page IicModule IIC Module
 *
 *  <hr size="1"/>
 *
 *  Manages communication with intelligent IIC devices on an input port.\n
 *
 *-  \subpage IicDriver
 *-  \subpage IicModuleMemory
 *-  \subpage IicModuleResources
 *
 */


/*! \page IicDriver IIC Device Controller
 *
 *  <hr size="1"/>
 *
 *  Manages the sequence necessary when adding a IIC devices to an input port.\n
 *
 * \n
 *
 *  The IIC Device Controller gets information from the DCM driver \ref DcmDriver "Device Detection Manager Driver"
 *  about the port state and when the DCM driver detects a IIC device on the port the sequence below the following defines
 *  is started inside the IIC Device Controller.
 *
 * \verbatim
*/

/*!
\endverbatim
 *
 *  \n
 */



#ifndef PCASM
#include  <asm/types.h>
#endif

#define   HW_ID_SUPPORT

#include  "../../lms2012/source/lms2012.h"
#include  "../../lms2012/source/am1808.h"

#ifndef    DISABLE_FIQ_IIC
#include "d_iic.h"
#endif

#ifdef    DEBUG
#define   DEBUG_D_IIC
#endif
#ifdef    DEBUG_D_IIC
#define   DEBUG_IIC_WRITE
#define   HIGHDEBUG
#define   DEBUG_TRACE_MODE_CHANGE
#endif



#define   MODULE_NAME                   "iic_module"
#define   DEVICE1_NAME                  IIC_DEVICE

static    int  ModuleInit(void);
static    void ModuleExit(void);

#define   __USE_POSIX

#include  <linux/kernel.h>
#include  <linux/fs.h>

#include  <linux/sched.h>


#ifndef   PCASM
#include  <linux/circ_buf.h>
#include  <linux/hrtimer.h>

#include  <linux/mm.h>
#include  <linux/hrtimer.h>

#include  <linux/init.h>
#include  <linux/uaccess.h>
#include  <linux/debugfs.h>

#include  <linux/ioport.h>
#include  <asm/gpio.h>
#include  <asm/io.h>
#include  <linux/module.h>
#include  <linux/miscdevice.h>
#include  <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("The LEGO Group");
MODULE_DESCRIPTION(MODULE_NAME);
MODULE_SUPPORTED_DEVICE(DEVICE1_NAME);

module_init(ModuleInit);
module_exit(ModuleExit);

#else
// Keep Eclipse happy
#endif


#define   NO_OF_IIC_PORTS             INPUTS


#ifndef    DISABLE_FIQ_IIC

extern    INPIN IicPortPin[NO_OF_IIC_PORTS][IIC_PORT_PINS];

#else

enum      IicPortPins
{
  IIC_PORT_BUFFER_CTRL,
  IIC_PORT_CLOCK,
  IIC_PORT_DATA,
  IIC_PORT_PINS,
};

INPIN     IicPortPin[NO_OF_IIC_PORTS][IIC_PORT_PINS];

#endif


int       Hw = 0;

/*! \page IicModuleResources Gpios and Resources used for Module
 *
 *  Describes use of gpio and resources\n
 *
 *  \verbatim
 */


INPIN     EP2_IicPortPin[][IIC_PORT_PINS] =
{
  { // Input port 1
    { GP8_11 , NULL, 0 }, // Buffer disable
    { GP1_0  , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP0_15 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
  { // Input port 2
    { GP8_14 , NULL, 0 }, // Buffer disable
    { GP8_3  , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP0_13 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
  { // Input port 3
    { GP7_9  , NULL, 0 }, // Buffer disable
    { GP1_12 , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP1_14 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
  { // Input port 4
    { GP7_10 , NULL, 0 }, // Buffer disable
    { GP1_11 , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP1_15 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
};


INPIN     FINALB_IicPortPin[][IIC_PORT_PINS] =
{
  { // Input port 1
    { GP8_11 , NULL, 0 }, // Buffer disable
    { GP1_0  , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP0_15 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
  { // Input port 2
    { GP8_14 , NULL, 0 }, // Buffer disable
    { GP8_3  , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP0_13 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
  { // Input port 3
    { GP7_9  , NULL, 0 }, // Buffer disable
    { GP1_12 , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP1_14 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
  { // Input port 4
    { GP7_10 , NULL, 0 }, // Buffer disable
    { GP1_11 , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP1_15 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
};


INPIN     FINAL_IicPortPin[][IIC_PORT_PINS] =
{
  { // Input port 1
    { GP8_11 , NULL, 0 }, // Buffer disable
    { GP0_2  , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP0_15 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
  { // Input port 2
    { GP8_14 , NULL, 0 }, // Buffer disable
    { GP0_14 , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP0_13 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
  { // Input port 3
    { GP7_9  , NULL, 0 }, // Buffer disable
    { GP0_12 , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP1_14 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
  { // Input port 4
    { GP7_10 , NULL, 0 }, // Buffer disable
    { GP1_13 , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
    { GP1_15 , NULL, 0 }, // Pin 6  - Digital output  // DATA
  },
};



/*  \endverbatim
 *  \n
 */


#define   PUDisable                     iowrite32(ioread32(da8xx_syscfg1_base + 0x0C) & ~0xFFFFFFFF,da8xx_syscfg1_base + 0x0C)

INPIN     *pIicPortPin[] =
{
  [FINAL]     =   (INPIN*)&FINAL_IicPortPin[0],    //  FINAL   platform
  [FINALB]    =   (INPIN*)&FINALB_IicPortPin[0],   //  FINALB  platform
  [EP2]       =   (INPIN*)&EP2_IicPortPin[0],      //  EP2     platform
};


//*****************************************************************************

static    void      __iomem *GpioBase;

void      SetGpio(int Pin)
{
  int     Tmp = 0;
  void    __iomem *Reg;

  if (Pin >= 0)
  {
    // unlock
    REGUnlock;

    while ((MuxRegMap[Tmp].Pin != -1) && (MuxRegMap[Tmp].Pin != Pin))
    {
      Tmp++;
    }
    if (MuxRegMap[Tmp].Pin == Pin)
    {
      Reg   =  da8xx_syscfg0_base + 0x120 + (MuxRegMap[Tmp].MuxReg << 2);

      *(u32*)Reg &=  MuxRegMap[Tmp].Mask;
      *(u32*)Reg |=  MuxRegMap[Tmp].Mode;
  #ifdef DEBUG
      printk("    GP%d_%-2d  0x%08X and 0x%08X or 0x%08X\n",(Pin >> 4),(Pin & 0x0F),(u32)Reg, MuxRegMap[Tmp].Mask, MuxRegMap[Tmp].Mode);
  #endif
    }
    else
    {
      printk("*   GP%d_%-2d  ********* ERROR not found *********\n",(Pin >> 4),(Pin & 0x0F));
    }
    // lock
    REGLock;
  }
}


void      InitGpio(void)
{
  int     Port;
  int     Pin;


  memcpy(IicPortPin,pIicPortPin[Hw],sizeof(EP2_IicPortPin));

#ifdef DEBUG
  printk("\n\n");
  printk("FINALB_IicPortPin            = %p\n",FINALB_IicPortPin);
  printk("pIicPortPin[Hw]              = %p\n",pIicPortPin[Hw]);
  printk("sizeof(INPIN)                = %d\n",sizeof(INPIN));
  printk("sizeof(EP2_IicPortPin)       = %d\n",sizeof(EP2_IicPortPin));

#endif

  if (memcmp((const void*)IicPortPin,(const void*)pIicPortPin[Hw],sizeof(EP2_IicPortPin)) != 0)
  {
    printk("%s IicPortPin tabel broken!\n",MODULE_NAME);
  }

  for (Port = 0;Port < NO_OF_IIC_PORTS;Port++)
  {
#ifdef DEBUG
    printk("  Iic   port %d\n",Port + 1);
#endif
    for (Pin = 0;Pin < IIC_PORT_PINS;Pin++)
    {
      if ((Port != DEBUG_UART) || (Pin != IIC_PORT_CLOCK))
      {
        if (IicPortPin[Port][Pin].Pin >= 0)
        {
          IicPortPin[Port][Pin].pGpio  =  (struct gpio_controller *__iomem)(GpioBase + ((IicPortPin[Port][Pin].Pin >> 5) * 0x28) + 0x10);
          IicPortPin[Port][Pin].Mask   =  (1 << (IicPortPin[Port][Pin].Pin & 0x1F));

          SetGpio(IicPortPin[Port][Pin].Pin);
        }
      }
    }
  }

  // unlock
  REGUnlock;

  // Disable pull up/down
  PUDisable;

  // lock
  REGLock;
}


#ifdef    DISABLE_FIQ_IIC
#define   PINFloat(port,pin)            {\
                                          (*IicPortPin[port][pin].pGpio).dir |=  IicPortPin[port][pin].Mask;\
                                        }


#define   PINRead(port,pin)             ((*IicPortPin[port][pin].pGpio).in_data & IicPortPin[port][pin].Mask)


#define   PINHigh(port,pin)             {\
                                          (*IicPortPin[port][pin].pGpio).set_data  =  IicPortPin[port][pin].Mask;\
                                          (*IicPortPin[port][pin].pGpio).dir      &= ~IicPortPin[port][pin].Mask;\
                                        }

#define   PINLow(port,pin)              {\
                                          (*IicPortPin[port][pin].pGpio).clr_data  =  IicPortPin[port][pin].Mask;\
                                          (*IicPortPin[port][pin].pGpio).dir      &= ~IicPortPin[port][pin].Mask;\
                                        }
#endif


typedef struct
{
  UWORD   Timer;
  UWORD   Time;
  UBYTE   Initialised;
  UBYTE   ChangeMode;
  UBYTE   State;
  UBYTE   OldState;
  UBYTE   SubState;
  UBYTE   Repeat;
  UBYTE   Cmd;
  UBYTE   Mode;
  UBYTE   Addr;
  UBYTE   Retries;
  SBYTE   Name[TYPE_NAME_LENGTH + 1];
  UBYTE   Reverse;
  UBYTE   InLength;
  UBYTE   InPointer;
  UBYTE   OutLength;
  UBYTE   OutPointer;
  UBYTE   InBuffer[IIC_DATA_LENGTH];
  UBYTE   OutBuffer[IIC_DATA_LENGTH];
}
IICPORT;


IICPORT  IicPortDefault =
{
  0,                        // Timer
  0,                        // Time
  0,                        // Initialised
  0,                        // ChangeMode
  0,                        // State
  -1,                       // OldState
  0,                        // SubState
  0,                        // Repeat
  0,                        // Cmd
  0,                        // Mode
  1,                        // Addr
  3,                        // Retries
  "",                       // Name
  0,                        // Reverse
  0,                        // InLength
  0,                        // InPointer
  0,                        // OutLength
  0,                        // OutPointer
};


#ifdef DEBUG_IIC_WRITE

#define   IICBUFFERSIZE    250
static    char IicBuffer[IICBUFFERSIZE];

#define   LOGPOOLSIZE       100000
ULONG     LogPointer = 0;
ULONG     LogOutPointer  =  0;
char      LogPool[LOGPOOLSIZE];

void      IicWrite(char *pString)
{
  ULONG   Tmp;

  while (*pString)
  {
    LogPool[LogPointer]  =  *pString;

    Tmp  =  LogPointer;
    Tmp++;
    if (Tmp >= LOGPOOLSIZE)
    {
      Tmp  =  0;
    }
    if (Tmp != LogOutPointer)
    {
      LogPointer  =  Tmp;
      pString++;

    }
    else
    {
      pString++;
    }
  }
}
#endif


UBYTE     IicConfigured[INPUTS];
IICPORT   IicPort[INPUTS];

static    IIC  IicDefault;
static    IIC *pIic   =  &IicDefault;

// FIQ Interface **************************************************************

#ifndef   DISABLE_FIQ_IIC

#ifndef   PCASM
#include  <linux/circ_buf.h>
#include  <linux/delay.h>
#endif

extern struct IIC_control_t IicCtrl;
extern void iic_fiq_start_transfer(unsigned int time, bool fiq_nirq);

#endif


static void IicPortDisable(UBYTE Port)
{
#ifndef   DISABLE_FIQ_IIC

  IicCtrl.port_enabled &= ~(1 << Port);

  if (Port != DEBUG_UART)
  {
    PINHigh(Port,IIC_PORT_BUFFER_CTRL);
  }
  PINInput(Port,IIC_PORT_DATA);

#endif
}


static void IicPortEnable(UBYTE Port)
{
#ifndef   DISABLE_FIQ_IIC

  SetGpio(IicPortPin[Port][IIC_PORT_CLOCK].Pin);

  PINLow(Port,IIC_PORT_BUFFER_CTRL);
  PINHigh(Port,IIC_PORT_CLOCK);
  PINHigh(Port,IIC_PORT_DATA);
  PINOutput(Port,IIC_PORT_BUFFER_CTRL);
  PINOutput(Port,IIC_PORT_CLOCK);
  PINOutput(Port,IIC_PORT_DATA);

  IicCtrl.data_package[Port].clock_state = 1;

  IicCtrl.port_enabled |=  (1 << Port);

#endif
}


static UBYTE IicPortBusy(UBYTE Port)
{
  UBYTE   Result = 0;

#ifndef   DISABLE_FIQ_IIC

  if (IicCtrl.transfers_active)
  {
    Result  =  1;
  }

#endif

  return (Result);
}


static RESULT IicPortSend(UBYTE Port)
{
  RESULT  Result = OK;

#ifndef   DISABLE_FIQ_IIC
  DATA8   Tmp;

  IicCtrl.data_package[Port].addr             =  IicPort[Port].OutBuffer[0];

#ifdef HIGHDEBUG
  snprintf(IicBuffer,IICBUFFERSIZE,"    %d Send %02X   ",Port,IicPort[Port].OutBuffer[0]);
  IicWrite(IicBuffer);
#endif
  for (Tmp = 1;Tmp < IicPort[Port].OutLength;Tmp++)
  {
#ifdef HIGHDEBUG
    snprintf(IicBuffer,IICBUFFERSIZE,"%02X ",IicPort[Port].OutBuffer[Tmp]);
    IicWrite(IicBuffer);
#endif
    IicCtrl.data_package[Port].data[Tmp - 1]  =  IicPort[Port].OutBuffer[Tmp];
  }
#ifdef HIGHDEBUG
  snprintf(IicBuffer,IICBUFFERSIZE,"\r\n");
  IicWrite(IicBuffer);
#endif
  IicCtrl.data_package[Port].write_length     =  IicPort[Port].OutLength - 1;
  IicCtrl.data_package[Port].read_length      =  IicPort[Port].InLength;
  IicCtrl.data_package[Port].port             =  Port;

  IicCtrl.data_package[Port].nacked           =  0;
  IicCtrl.data_package[Port].clock_state      =  1;
  IicCtrl.data_package[Port].transfer_state   =  TRANSFER_START;

  iic_fiq_start_transfer(50,1);

#endif

  return (Result);
}


static RESULT IicPortReceive(UBYTE Port,UBYTE *pTmpBuffer)
{
  RESULT  Result = BUSY;

#ifndef   DISABLE_FIQ_IIC
  DATA8   Tmp;

  if (IicCtrl.data_package[Port].transfer_state == TRANSFER_IDLE)
  {
    memset((void*)pTmpBuffer,0,IIC_DATA_LENGTH);
#ifdef HIGHDEBUG
    snprintf(IicBuffer,IICBUFFERSIZE,"    %d Receiving   ",Port);
    IicWrite(IicBuffer);
#endif
    for (Tmp = 0;Tmp < IicPort[Port].InLength;Tmp++)
    {
#ifdef HIGHDEBUG
      snprintf(IicBuffer,IICBUFFERSIZE,"%02X ",IicCtrl.data_package[Port].data[Tmp]);
      IicWrite(IicBuffer);
#endif
      pTmpBuffer[Tmp]        =  IicCtrl.data_package[Port].data[Tmp];
    }
#ifdef HIGHDEBUG
      snprintf(IicBuffer,IICBUFFERSIZE,"\r\n");
      IicWrite(IicBuffer);
#endif

    if (!(IicCtrl.data_package[Port].nacked))
    {
      Result  =  OK;
    }
    else
    {
      Result  =  FAIL;
    }
  }

#endif

  return (Result);
}


// DEVICE1 ********************************************************************


enum      IIC_STATE
{
  IIC_IDLE,
  IIC_INIT,
  IIC_RESTART,
  IIC_ENABLE,
  IIC_NXT_TEMP_START,
  IIC_NXT_TEMP_WRITE,
  IIC_NXT_TEMP_READ,
  IIC_MANUFACTURER_START,
  IIC_MANUFACTURER_WRITE,
  IIC_MANUFACTURER_READ,
  IIC_TYPE_START,
  IIC_TYPE_WRITE,
  IIC_TYPE_READ,
  IIC_SETUP_WAIT,
  IIC_SETUP_START,
  IIC_SETUP_WRITE,
  IIC_SETUP_READ,
  IIC_WAITING,
  IIC_WRITING,
  IIC_READING,
  IIC_REPEAT,
  IIC_ERROR,
  IIC_EXIT,
  IIC_STATES
};


char      IicStateText[IIC_STATES][50] =
{
  "IDLE\n",
  "INIT",
  "RESTART",
  "ENABLE",
  "IIC_NXT_TEMP_START",
  "IIC_NXT_TEMP_WRITE",
  "IIC_NXT_TEMP_READ",
  "MANUFACTURER_START",
  "MANUFACTURER_WRITE",
  "MANUFACTURER_READ",
  "TYPE_START",
  "TYPE_WRITE",
  "TYPE_READ",
  "SETUP_WAIT",
  "SETUP_START",
  "SETUP_WRITE",
  "SETUP_READ",
  "WAITING",
  "WRITING",
  "READING",
  "REPEAT",
  "ERROR",
  "EXIT"
};


DATA8     IicPortType[INPUTS];
IICSTR    IicStrings[INPUTS];


#define   IIC_TIMER_RESOLUTION            20 // [100u]
static    struct hrtimer Device1Timer;
static    ktime_t        Device1Time;


static enum hrtimer_restart Device1TimerInterrupt1(struct hrtimer *pTimer)
{
  UBYTE   Port;
  UBYTE   Tmp;
  UBYTE   TmpBuffer[IIC_DATA_LENGTH];

  hrtimer_forward_now(pTimer,Device1Time);

  for (Port = 0;Port < NO_OF_IIC_PORTS;Port++)
  { // look at one port at a time

    switch (IicPort[Port].State)
    { // Main state machine

      case IIC_IDLE :
      { // Port inactive

        (*pIic).Status[Port]        =  0;
      }
      break;

      case IIC_INIT :
      { // Initialise port hardware

        memset((void*)IicStrings[Port].Manufacturer,0,IIC_NAME_LENGTH + 1);
        memset((void*)IicStrings[Port].SensorType,0,IIC_NAME_LENGTH + 1);
        IicPort[Port].State         =  IIC_ENABLE;
      }
      break;

      case IIC_RESTART :
      {
        IicPortDisable(Port);
        IicPort[Port].State         =  IIC_ENABLE;
      }
      break;

      case IIC_ENABLE :
      { // Initialise port variables

        IicPortEnable(Port);

        IicPort[Port]               =  IicPortDefault;

        IicPort[Port].Timer         =  0;

        IicPort[Port].State         =  IIC_NXT_TEMP_START;
      }
      break;





      case IIC_NXT_TEMP_START :
      {
        if (++(IicPort[Port].Timer) >= (10 / IIC_TIMER_RESOLUTION))
        {
          IicPort[Port].OutBuffer[0]  =  0x4C;
          IicPort[Port].OutBuffer[1]  =  0x01;
          IicPort[Port].OutBuffer[2]  =  0x60;
          IicPort[Port].OutBuffer[3]  =  0x00;
          IicPort[Port].OutLength     =  3;
          IicPort[Port].InLength      =  1;
          IicPort[Port].Repeat        =  1;
          IicPort[Port].Time          =  0;
          IicPort[Port].Timer         =  0;
          IicPort[Port].State         =  IIC_NXT_TEMP_WRITE;
        }
      }
      break;

      case IIC_NXT_TEMP_WRITE :
      {
        IicPortSend(Port);
        IicPort[Port].Timer       =  0;
        IicPort[Port].State       =  IIC_NXT_TEMP_READ;
      }
      break;

      case IIC_NXT_TEMP_READ :
      {
        if (IicPortReceive(Port,TmpBuffer) != BUSY)
        {
          if (TmpBuffer[0] == 0x60)
          {
            memcpy((void*)IicStrings[Port].Manufacturer,(void*)"LEGO",IIC_NAME_LENGTH);
            IicStrings[Port].Manufacturer[IIC_NAME_LENGTH]  =  0;
            memcpy((void*)IicStrings[Port].SensorType,(void*)"Temp.",IIC_NAME_LENGTH);
            IicStrings[Port].SensorType[IIC_NAME_LENGTH]  =  0;

            (*pIic).Changed[Port]       =  1;

            IicPort[Port].Initialised   =  1;
            IicPort[Port].Timer         =  0;
            IicPort[Port].State         =  IIC_SETUP_WAIT;
          }
          else
          {
            IicPort[Port].Timer       =  0;
            IicPort[Port].State         =  IIC_MANUFACTURER_START;
          }
        }
        if (++(IicPort[Port].Timer) >= (500 / IIC_TIMER_RESOLUTION))
        {
          IicPort[Port].Timer       =  0;
          IicPort[Port].State         =  IIC_MANUFACTURER_START;
        }
      }
      break;

      case IIC_MANUFACTURER_START :
      {
        if (++(IicPort[Port].Timer) >= (10 / IIC_TIMER_RESOLUTION))
        {
          IicPort[Port].OutBuffer[0]  =  IicPort[Port].Addr;
          IicPort[Port].OutBuffer[1]  =  0x08;
          IicPort[Port].OutBuffer[2]  =  0x00;
          IicPort[Port].OutBuffer[3]  =  0x00;
          IicPort[Port].OutLength     =  2;
          IicPort[Port].InLength      =  8;
          IicPort[Port].Repeat        =  1;
          IicPort[Port].Time          =  0;
          IicPort[Port].Timer         =  0;
          IicPort[Port].State         =  IIC_MANUFACTURER_WRITE;
        }
      }
      break;

      case IIC_MANUFACTURER_WRITE :
      {
        IicPortSend(Port);
        IicPort[Port].Timer       =  0;
        IicPort[Port].State       =  IIC_MANUFACTURER_READ;
      }
      break;

      case IIC_MANUFACTURER_READ :
      {
        if (IicPortReceive(Port,TmpBuffer) != BUSY)
        {
          memcpy((void*)IicStrings[Port].Manufacturer,(void*)TmpBuffer,IIC_NAME_LENGTH);
          IicStrings[Port].Manufacturer[IIC_NAME_LENGTH]  =  0;

          if (TmpBuffer[0] == 0x08)
          {
            if (IicPort[Port].Addr < 0x50)
            {
              IicPort[Port].Addr++;
              IicPort[Port].Timer         =  0;
              IicPort[Port].State         =  IIC_MANUFACTURER_START;
            }
            else
            {
              if (--IicPort[Port].Retries > 0)
              {
                IicPort[Port].Addr          =  0x01;
                IicPort[Port].Timer         =  0;
                IicPort[Port].State         =  IIC_MANUFACTURER_START;
              }
              else
              {
                IicPort[Port].Initialised   =  1;
                IicPort[Port].State         =  IIC_SETUP_START;
              }
            }
          }
          else
          {
            if (TmpBuffer[0] == 0)
            {
              memcpy((void*)IicStrings[Port].Manufacturer,(void*)"LEGO",IIC_NAME_LENGTH);
              IicStrings[Port].Manufacturer[IIC_NAME_LENGTH]  =  0;
            }
            IicPort[Port].Initialised     =  1;
            IicPort[Port].State           =  IIC_TYPE_START;
          }

        }
        if (++(IicPort[Port].Timer) >= (500 / IIC_TIMER_RESOLUTION))
        {
          printk("IIC timeout\n");
          IicPort[Port].Addr        =  0x01;
          (*pIic).Status[Port]     &= ~IIC_WRITE_REQUEST;
          (*pIic).Status[Port]     &= ~IIC_DATA_READY;
          IicPort[Port].State       =  IIC_WAITING;
        }
      }
      break;

      case IIC_TYPE_START :
      {
        IicPort[Port].OutBuffer[0]  =  IicPort[Port].Addr;
        IicPort[Port].OutBuffer[1]  =  0x10;
        IicPort[Port].OutBuffer[2]  =  0x00;
        IicPort[Port].OutBuffer[3]  =  0x00;
        IicPort[Port].OutLength     =  2;
        IicPort[Port].InLength      =  8;
        IicPort[Port].Repeat        =  1;
        IicPort[Port].Time          =  0;
        IicPort[Port].Timer         =  0;
        IicPort[Port].State         =  IIC_TYPE_WRITE;
      }
      break;

      case IIC_TYPE_WRITE :
      {
        IicPortSend(Port);
        IicPort[Port].Timer       =  0;
        IicPort[Port].State       =  IIC_TYPE_READ;
      }
      break;

      case IIC_TYPE_READ :
      {
        if (IicPortReceive(Port,TmpBuffer) != BUSY)
        {
          memcpy((void*)IicStrings[Port].SensorType,(void*)TmpBuffer,IIC_NAME_LENGTH);
          IicStrings[Port].SensorType[IIC_NAME_LENGTH]  =  0;

          if (TmpBuffer[0] == 0)
          {
            memcpy((void*)IicStrings[Port].SensorType,(void*)"Store",IIC_NAME_LENGTH);
            IicStrings[Port].Manufacturer[IIC_NAME_LENGTH]  =  0;
          }

          (*pIic).Changed[Port]       =  1;

          IicPort[Port].Initialised   =  1;
          IicPort[Port].Timer         =  0;
          IicPort[Port].State         =  IIC_SETUP_WAIT;
        }
        if (++(IicPort[Port].Timer) >= (500 / IIC_TIMER_RESOLUTION))
        {
          (*pIic).Status[Port]     &= ~IIC_WRITE_REQUEST;
          (*pIic).Status[Port]     &= ~IIC_DATA_READY;
          IicPort[Port].State       =  IIC_WAITING;
        }
      }
      break;

      case IIC_SETUP_WAIT :
      {
        if (++(IicPort[Port].Timer) >= (10000 / IIC_TIMER_RESOLUTION))
        {
          IicPort[Port]               =  IicPortDefault;
          IicPort[Port].Timer         =  0;
          IicPort[Port].State         =  IIC_NXT_TEMP_START;
        }
      }
      break;

      case IIC_SETUP_START :
      {
#ifndef DISABLE_FAST_DATALOG_BUFFER
        (*pIic).Actual[Port]        =  0;
        (*pIic).LogIn[Port]         =  0;
#endif

        if (IicStrings[Port].SetupLng)
        {
          IicPort[Port].OutBuffer[0]  =  (UBYTE)((IicStrings[Port].SetupString >> 24) & 0xFF);
          IicPort[Port].OutBuffer[1]  =  (UBYTE)((IicStrings[Port].SetupString >> 16) & 0xFF);
          IicPort[Port].OutBuffer[2]  =  (UBYTE)((IicStrings[Port].SetupString >>  8) & 0xFF);
          IicPort[Port].OutBuffer[3]  =  (UBYTE)((IicStrings[Port].SetupString) & 0xFF);
          IicPort[Port].OutLength     =  IicStrings[Port].SetupLng;
          IicPort[Port].InLength      =  0;
          IicPort[Port].Repeat        =  1;
          IicPort[Port].Time          =  0;
          IicPort[Port].State         =  IIC_SETUP_WRITE;
        }
        else
        {
          IicPort[Port].State         =  IIC_SETUP_READ;
        }
        IicPort[Port].Timer           =  0;
      }
      break;

      case IIC_SETUP_WRITE :
      {
        IicPortSend(Port);
        IicPort[Port].Timer       =  0;
        IicPort[Port].State       =  IIC_SETUP_READ;
      }
      break;

      case IIC_SETUP_READ :
      {
        if (IicPortReceive(Port,TmpBuffer) != BUSY)
        {
          IicPort[Port].State           =  IIC_WAITING;
          if (IicStrings[Port].PollLng)
          {
            IicPort[Port].OutBuffer[0]  =  (UBYTE)((IicStrings[Port].PollString >> 24) & 0xFF);
            IicPort[Port].OutBuffer[1]  =  (UBYTE)((IicStrings[Port].PollString >> 16) & 0xFF);
            IicPort[Port].OutBuffer[2]  =  (UBYTE)((IicStrings[Port].PollString >>  8) & 0xFF);
            IicPort[Port].OutBuffer[3]  =  (UBYTE)((IicStrings[Port].PollString) & 0xFF);
            IicPort[Port].OutLength     =  IicStrings[Port].PollLng;
            if (IicStrings[Port].ReadLng < 0)
            {
              IicPort[Port].InLength    =  0 - IicStrings[Port].ReadLng;
              IicPort[Port].Reverse     =  1;
            }
            else
            {
              IicPort[Port].InLength    =  IicStrings[Port].ReadLng;
              IicPort[Port].Reverse     =  0;
            }
            IicPort[Port].Repeat        =  0;
            IicPort[Port].Time          =  IicStrings[Port].Time;
            IicPort[Port].Timer         =  0;
            IicPort[Port].State         =  IIC_WRITING;
            (*pIic).Status[Port]        =  0;
          }
          IicPort[Port].Initialised     =  1;
        }
        if (++(IicPort[Port].Timer) >= (500 / IIC_TIMER_RESOLUTION))
        {
          (*pIic).Status[Port]     &= ~IIC_WRITE_REQUEST;
          (*pIic).Status[Port]     &= ~IIC_DATA_READY;
          IicPort[Port].State       =  IIC_WAITING;
        }
      }
      break;

      case IIC_WAITING :
      {
        if ((*pIic).Status[Port] & IIC_WRITE_REQUEST)
        {
          if (IicPortBusy(Port) == 0)
          {
            IicPort[Port].Timer       =  0;
            IicPort[Port].State       =  IIC_WRITING;
          }
        }
      }
      break;

      case IIC_WRITING :
      {
        IicPortSend(Port);
        IicPort[Port].State       =  IIC_READING;
      }
      break;

      case IIC_READING :
      {
        if (IicPortReceive(Port,TmpBuffer) != BUSY)
        {

#ifndef DISABLE_FAST_DATALOG_BUFFER
          if (IicPort[Port].InLength > 1)
          {
            if (IicPort[Port].Reverse)
            {
              for (Tmp = 0;Tmp < IicPort[Port].InLength;Tmp++)
              {
                IicPort[Port].InBuffer[Tmp]                  =  TmpBuffer[Tmp];
                (*pIic).Raw[Port][(*pIic).LogIn[Port]][Tmp]  =  TmpBuffer[Tmp];
              }
            }
            else
            {
              for (Tmp = 0;Tmp < IicPort[Port].InLength;Tmp++)
              {
                IicPort[Port].InBuffer[Tmp]       =  TmpBuffer[(IicPort[Port].InLength - 1) - Tmp];
                (*pIic).Raw[Port][(*pIic).LogIn[Port]][Tmp]  =  TmpBuffer[(IicPort[Port].InLength - 1) - Tmp];
              }
            }
          }
          else
          {
            IicPort[Port].InBuffer[0]                  =  TmpBuffer[0];
            IicPort[Port].InBuffer[1]                  =  0;
            (*pIic).Raw[Port][(*pIic).LogIn[Port]][0]  =  TmpBuffer[0];
            (*pIic).Raw[Port][(*pIic).LogIn[Port]][1]  =  0;
          }

          (*pIic).Actual[Port]  =  (*pIic).LogIn[Port];
          (*pIic).Repeat[Port][(*pIic).Actual[Port]]  =  0;

          if (++((*pIic).LogIn[Port]) >= DEVICE_LOGBUF_SIZE)
          {
            (*pIic).LogIn[Port]      =  0;
          }
#else
          if (IicPort[Port].InLength > 1)
          {
            if (IicPort[Port].Reverse)
            {
              for (Tmp = 0;Tmp < IicPort[Port].InLength;Tmp++)
              {
                IicPort[Port].InBuffer[Tmp]       =  TmpBuffer[Tmp];
                (*pIic).Raw[Port][Tmp]            =  TmpBuffer[Tmp];
              }
            }
            else
            {
              for (Tmp = 0;Tmp < IicPort[Port].InLength;Tmp++)
              {
                IicPort[Port].InBuffer[Tmp]       =  TmpBuffer[(IicPort[Port].InLength - 1) - Tmp];
                (*pIic).Raw[Port][Tmp]            =  TmpBuffer[(IicPort[Port].InLength - 1) - Tmp];
              }
            }
          }
          else
          {
            IicPort[Port].InBuffer[0]                  =  TmpBuffer[0];
            IicPort[Port].InBuffer[1]                  =  0;
            (*pIic).Raw[Port][0]  =  TmpBuffer[0];
            (*pIic).Raw[Port][1]  =  0;
          }
#endif

          (*pIic).Status[Port]     |=  IIC_DATA_READY;
          IicPort[Port].State       =  IIC_REPEAT;

          if (IicPort[Port].Repeat != 0)
          {
            IicPort[Port].Repeat--;
            if (IicPort[Port].Repeat == 0)
            {
              IicPort[Port].State       =  IIC_WAITING;
            }
          }
        }
        if (++(IicPort[Port].Timer) >= (5000 / IIC_TIMER_RESOLUTION))
        {
          (*pIic).Status[Port]     &= ~IIC_WRITE_REQUEST;
          (*pIic).Status[Port]     &= ~IIC_DATA_READY;
          IicPort[Port].State       =  IIC_WAITING;
        }
      }
      break;

      case IIC_REPEAT :
      {
        if ((*pIic).Status[Port] & IIC_WRITE_REQUEST)
        {
          (*pIic).Status[Port]      =  IIC_WRITE_REQUEST;
          IicPort[Port].State       =  IIC_WAITING;
        }
        else
        {
          if (++(IicPort[Port].Timer) >= (UWORD)(IicPort[Port].Time / (IIC_TIMER_RESOLUTION / 10)))
          {
            IicPort[Port].Timer       =  0;
            IicPort[Port].State       =  IIC_WRITING;
          }
        }
      }
      break;

      case IIC_EXIT :
      {
        IicPortDisable(Port);
        IicPort[Port]                           =  IicPortDefault;

        (*pIic).Status[Port]                    =  0;

        IicPort[Port].State                     =  IIC_IDLE;
      }
      break;

    }

#ifndef DISABLE_FAST_DATALOG_BUFFER
    ((*pIic).Repeat[Port][(*pIic).Actual[Port]]) += (IIC_TIMER_RESOLUTION / 10);
#endif

#ifdef HIGHDEBUG
    if (IicPort[Port].OldState != IicPort[Port].State)
    {
      IicPort[Port].OldState    =  IicPort[Port].State;
      if (IicPort[Port].State  !=  IIC_ENABLE)
      {
        snprintf(IicBuffer,IICBUFFERSIZE,"    %d %s\r\n",Port,IicStateText[IicPort[Port].State]);
      }
      else
      {
        snprintf(IicBuffer,IICBUFFERSIZE,"*** %d %s ***\r\n",Port,IicStateText[IicPort[Port].State]);
      }
      IicWrite(IicBuffer);
    }
#endif

  }


  return (HRTIMER_RESTART);
}


static int Device1Ioctl(struct inode *pNode, struct file *File, unsigned int Request, unsigned long Pointer)
{
  DEVCON  *pDevCon;
  IICSTR  *pIicStr;
  IICDAT  *pIicDat;
  DATA8   Port = 0;

  switch (Request)
  {

    case IIC_SET_CONN :
    {
      pDevCon   =  (DEVCON*)Pointer;

      for (Port = 0;Port < INPUTS;Port++)
      {
        if ((*pDevCon).Connection[Port] == CONN_NXT_IIC)
        {
          if (IicConfigured[Port] == 0)
          {
#ifdef DEBUG_TRACE_MODE_CHANGE
//            printk("d_iic  %d   Device1Ioctl: Changing to IIC\n",Port);
#endif
            IicConfigured[Port]        =  1;
            IicPortType[Port]          =  (*pDevCon).Type[Port];
            IicPort[Port].State        =  IIC_INIT;
          }
          else
          {
            if (IicPort[Port].Initialised)
            {
              if (IicPort[Port].Mode != (*pDevCon).Mode[Port])
              {
#ifdef DEBUG_TRACE_MODE_CHANGE
//                printk("d_iic  %d   Device1Ioctl: Changing to    %c\n",Port,(*pDevCon).Mode[Port] + '0');
#endif
                IicPort[Port].Mode         =  (*pDevCon).Mode[Port];
                IicPort[Port].ChangeMode   =  1;
                (*pIic).Status[Port]      &= ~IIC_DATA_READY;
                IicPort[Port].State        =  IIC_SETUP_START;
              }
            }
          }
        }
        else
        {
#ifdef DEBUG_TRACE_MODE_CHANGE
//          printk("d_iic  %d   Device1Ioctl: Changing to non IIC\n",Port);
#endif
          (*pIic).Status[Port] &= ~IIC_DATA_READY;
          if (IicConfigured[Port])
          {
            IicConfigured[Port]        =  0;
            IicPort[Port].State        =  IIC_EXIT;
          }
        }
      }

    }
    break;

    case IIC_SET :
    {
      pIicStr                     = (IICSTR*)Pointer;

      Port                        =  (*pIicStr).Port;

      memcpy((void*)&IicStrings[Port],(void*)pIicStr,sizeof(IICSTR));
      IicPort[Port].State         =  IIC_SETUP_START;

    }
    break;

    case IIC_SETUP :
    {
      pIicDat                     = (IICDAT*)Pointer;

      Port                        =  (*pIicDat).Port;
      (*pIicDat).Result           =  BUSY;

      printk("1\n");
      if (!((*pIic).Status[Port] & IIC_WRITE_REQUEST))
      {
        printk("2\n");
        IicPort[Port].Repeat      =  (*pIicDat).Repeat;
        IicPort[Port].Time        =  (*pIicDat).Time;
        IicPort[Port].OutLength   =  (*pIicDat).WrLng;
        if ((*pIicDat).RdLng < 0)
        {
          IicPort[Port].InLength    =  0 - (*pIicDat).RdLng;
          IicPort[Port].Reverse     =  1;
        }
        else
        {
          IicPort[Port].InLength    =  (*pIicDat).RdLng;
          IicPort[Port].Reverse     =  0;
        }

        if (IicPort[Port].OutLength > IIC_DATA_LENGTH)
        {
          IicPort[Port].OutLength =   IIC_DATA_LENGTH;
        }
        if (IicPort[Port].InLength > IIC_DATA_LENGTH)
        {
          IicPort[Port].InLength  =   IIC_DATA_LENGTH;
        }

        memcpy((void*)&IicPort[Port].OutBuffer[0],(void*)&(*pIicDat).WrData[0],IicPort[Port].OutLength);
        memset((void*)&IicPort[Port].InBuffer[0],0,IIC_DATA_LENGTH);

        (*pIic).Status[Port]  =  IIC_WRITE_REQUEST;
      }

      if (((*pIic).Status[Port] & IIC_DATA_READY))
      {
        printk("3\n");
        memcpy((void*)&(*pIicDat).RdData[0],(void*)&IicPort[Port].InBuffer[0],IicPort[Port].InLength);

        (*pIicDat).Result         =  OK;
        (*pIic).Status[Port]      =  0;
      }
    }
    break;

    case IIC_READ_TYPE_INFO :
    {
      pIicStr                     = (IICSTR*)Pointer;

      Port                        =  (*pIicStr).Port;
      memcpy((void*)((*pIicStr).Manufacturer),(void*)IicStrings[Port].Manufacturer,IIC_NAME_LENGTH + 1);
      memcpy((void*)((*pIicStr).SensorType),(void*)IicStrings[Port].SensorType,IIC_NAME_LENGTH + 1);

      (*pIic).Changed[Port]       =  0;

    }
    break;

  }

  return (0);
}


static ssize_t Device1Write(struct file *File,const char *Buffer,size_t Count,loff_t *Data)
{
  int     Lng     = 0;

  Lng           =  Count;

  return (Lng);
}


static ssize_t Device1Read(struct file *File,char *Buffer,size_t Count,loff_t *Offset)
{
  int     Lng     = 0;

#ifdef DEBUG_IIC_WRITE
  if (LogOutPointer != LogPointer)
  {
    while ((Count--) && (LogOutPointer != LogPointer))
    {
      Buffer[Lng++]  =  LogPool[LogOutPointer];
      Buffer[Lng]    =  0;

      LogOutPointer++;
      if (LogOutPointer >= LOGPOOLSIZE)
      {
        LogOutPointer  =  0;
      }
    }
  }
  if (Lng == 0)
  {
    IicWrite(IicBuffer);
    snprintf(IicBuffer,IICBUFFERSIZE,"-----------------------------------------------------------------\r\n");
    IicWrite(IicBuffer);
    snprintf(IicBuffer,IICBUFFERSIZE,"    IIC DUMP\r\n");
    IicWrite(IicBuffer);
    snprintf(IicBuffer,IICBUFFERSIZE,"-----------------------------------------------------------------\r\n");
    IicWrite(IicBuffer);
  }
#else
  int     Tmp;
  int     Port;

  Port   =  0;
  Tmp    =  5;
  while ((Count > Tmp) && (Port < INPUTS))
  {
    if (Port != (INPUTS - 1))
    {
      Tmp    =  snprintf(&Buffer[Lng],4,"%2u ",(UWORD)IicPort[Port].State);
    }
    else
    {
      Tmp    =  snprintf(&Buffer[Lng],5,"%2u\r",(UWORD)IicPort[Port].State);
    }
    Lng   +=  Tmp;
    Count -=  Tmp;
    Port++;
  }
#endif

  return (Lng);
}


#define     SHM_LENGTH    (sizeof(IicDefault))
#define     NPAGES        ((SHM_LENGTH + PAGE_SIZE - 1) / PAGE_SIZE)
static void *kmalloc_ptr;

static int Device1Mmap(struct file *filp, struct vm_area_struct *vma)
{
   int ret;

   ret = remap_pfn_range(vma,vma->vm_start,virt_to_phys((void*)((unsigned long)pIic)) >> PAGE_SHIFT,vma->vm_end-vma->vm_start,PAGE_SHARED);

   if (ret != 0)
   {
     ret  =  -EAGAIN;
   }

   return (ret);
}


static    const struct file_operations Device1Entries =
{
  .owner        = THIS_MODULE,
  .read         = Device1Read,
  .write        = Device1Write,
  .mmap         = Device1Mmap,
  .ioctl        = Device1Ioctl,
};


static    struct miscdevice Device1 =
{
  MISC_DYNAMIC_MINOR,
  DEVICE1_NAME,
  &Device1Entries
};


static int Device1Init(void)
{
  int     Result = -1;
  UWORD   *pTmp;
  int     i;
#ifndef   DISABLE_FIQ_IIC
  UBYTE   Port;
#endif

  Result  =  misc_register(&Device1);
  if (Result)
  {
    printk("  %s device register failed\n",DEVICE1_NAME);
  }
  else
  {
    // allocate kernel shared memory for analog values (pAnalog)
    if ((kmalloc_ptr = kmalloc((NPAGES + 2) * PAGE_SIZE, GFP_KERNEL)) != NULL)
    {
      pTmp = (UWORD*)((((unsigned long)kmalloc_ptr) + PAGE_SIZE - 1) & PAGE_MASK);
      for (i = 0; i < NPAGES * PAGE_SIZE; i += PAGE_SIZE)
      {
        SetPageReserved(virt_to_page(((unsigned long)pTmp) + i));
      }
      pIic         =  (IIC*)pTmp;
      memset(pIic,0,sizeof(IIC));

#ifndef   DISABLE_FIQ_IIC
      IicCtrl.port_enabled  = 0;

      for (Port = 0;Port < INPUTS;Port++)
      {
        IicPort[Port]                               =  IicPortDefault;
        IicConfigured[Port]                         =  0;
        IicCtrl.data_package[Port].transfer_state   =  TRANSFER_IDLE;
        IicStrings[Port].Manufacturer[0]            =  0;
        IicStrings[Port].SensorType[0]              =  0;
        if (Port == DEBUG_UART)
        {
          PINLow(Port,IIC_PORT_BUFFER_CTRL);
          PINOutput(Port,IIC_PORT_BUFFER_CTRL);
        }
      }
#endif

      Device1Time  =  ktime_set(0,IIC_TIMER_RESOLUTION * 100000);
      hrtimer_init(&Device1Timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
      Device1Timer.function  =  Device1TimerInterrupt1;
      hrtimer_start(&Device1Timer,Device1Time,HRTIMER_MODE_REL);

#ifdef DEBUG
      printk("  %s device register succes\n",DEVICE1_NAME);
#endif
    }
    else
    {
      printk("  %s kmalloc failed !!\n",DEVICE1_NAME);
    }
  }

  return (Result);
}


static void Device1Exit(void)
{
  IIC     *pTmp;
  int     i;

  hrtimer_cancel(&Device1Timer);

  pTmp      =  pIic;
  pIic      =  &IicDefault;

  // free shared memory
  for (i = 0; i < NPAGES * PAGE_SIZE; i+= PAGE_SIZE)
  {
    ClearPageReserved(virt_to_page(((unsigned long)pTmp) + i));
#ifdef DEBUG
    printk("  %s memory page %d unmapped\n",DEVICE1_NAME,i);
#endif
  }
  kfree(kmalloc_ptr);

  misc_deregister(&Device1);
#ifdef DEBUG
  printk("  %s device unregistered\n",DEVICE1_NAME);
#endif
}




// MODULE *********************************************************************


#ifndef PCASM
module_param (HwId, charp, 0);
#endif

static int ModuleInit(void)
{
  Hw  =  HWID;

  if (Hw < PLATFORM_START)
  {
    Hw  =  PLATFORM_START;
  }
  if (Hw > PLATFORM_END)
  {
    Hw  =  PLATFORM_END;
  }

#ifdef DEBUG
  printk("%s init started\n",MODULE_NAME);
#endif
  if (request_mem_region(DA8XX_GPIO_BASE,0xD8,MODULE_NAME) >= 0)
  {
    GpioBase  =  (void*)ioremap(DA8XX_GPIO_BASE,0xD8);
    if (GpioBase != NULL)
    {
#ifdef DEBUG
      printk("%s gpio address mapped\n",MODULE_NAME);
#endif

      InitGpio();

      Device1Init();
    }
  }

  return (0);
}


static void ModuleExit(void)
{
#ifdef DEBUG
  printk("%s exit started\n",MODULE_NAME);
#endif

  Device1Exit();

  iounmap(GpioBase);
}

