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


#ifndef PCASM
#include  <asm/types.h>
#endif

#define   HW_ID_SUPPORT

#include  "../../lms2012/source/lms2012.h"
#include  "../../lms2012/source/am1808.h"

int       Hw  =  0;

#define   MODULE_NAME                   "bluetooth_module"
#define   DEVICE1_NAME                  BT_DEVICE
#define   DEVICE2_NAME                  UPDATE_DEVICE

static    int  ModuleInit(void);
static    void ModuleExit(void);

#define   __USE_POSIX

#include  <linux/kernel.h>
#include  <linux/fs.h>

#include  <linux/sched.h>


#ifndef   PCASM
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


enum
{
  SET   = 0,
  CLEAR = 1,
  HIIMP = 2
};

enum      BluetoothPins
{
  CTS_PIC,
  PIC_RST,
  PIC_EN,
  BLUETOOTH_PINS
};


INPIN     EP2_BluetoothPin[BLUETOOTH_PINS] =
{
  { GP5_7  , NULL, 0 }, // CTS_PIC
  { GP4_14 , NULL, 0 }, // PIC_RST
  { GP3_3  , NULL, 0 }, // PIC_EN
};


INPIN     FINALB_BluetoothPin[BLUETOOTH_PINS] =
{
  { GP5_7  , NULL, 0 }, // CTS_PIC
  { GP4_14 , NULL, 0 }, // PIC_RST
  { GP3_3  , NULL, 0 }, // PIC_EN
};


INPIN     FINAL_BluetoothPin[BLUETOOTH_PINS] =
{
  { GP5_7  , NULL, 0 }, // CTS_PIC
  { GP4_14 , NULL, 0 }, // PIC_RST
  { GP3_3  , NULL, 0 }, // PIC_EN
};


INPIN     *pBluetoothPin[] =
{
  [FINAL]     =   (INPIN*)&FINAL_BluetoothPin[0],    //  FINAL   platform
  [FINALB]    =   (INPIN*)&FINALB_BluetoothPin[0],   //  FINALB  platform
  [EP2]       =   (INPIN*)&EP2_BluetoothPin[0],      //  EP2     platform
};


static    void  __iomem *GpioBase;
static    UBYTE         *pLocalRam;

void      SetGpio(int Pin)
{
  int     Tmp = 0;
  void    __iomem *Reg;

  while ((MuxRegMap[Tmp].Pin != -1) && (MuxRegMap[Tmp].Pin != Pin))
  {
    Tmp++;
  }
  if (MuxRegMap[Tmp].Pin == Pin)
  {
    Reg   =  da8xx_syscfg0_base + 0x120 + (MuxRegMap[Tmp].MuxReg << 2);

    *(u32*)Reg &=  MuxRegMap[Tmp].Mask;
    *(u32*)Reg |=  MuxRegMap[Tmp].Mode;

    if (Pin < NO_OF_GPIOS)
    {
      #ifdef DEBUG
        printk("    GP%d_%-2d   0x%08X and 0x%08X or 0x%08X\n",(Pin >> 4),(Pin & 0x0F),(u32)Reg, MuxRegMap[Tmp].Mask, MuxRegMap[Tmp].Mode);
      #endif
    }
    else
    {
      #ifdef DEBUG
        printk("   OUTPUT FUNCTION 0x%08X and 0x%08X or 0x%08X\n",(u32)Reg, MuxRegMap[Tmp].Mask, MuxRegMap[Tmp].Mode);
      #endif
    }
  }
  else
  {
    printk("    GP%d_%-2d Not found (Const no. %d, Tmp = %d)\n",(Pin >> 4),(Pin & 0x0F), Pin, Tmp);
  }
}


void      InitGpio(void)
{
  int     Pin;

  REGUnlock;

  for (Pin = 0; Pin < BLUETOOTH_PINS; Pin++)
  {
    pBluetoothPin[Hw][Pin].pGpio  =  (struct gpio_controller *__iomem)(GpioBase + ((pBluetoothPin[Hw][Pin].Pin >> 5) * 0x28) + 0x10);
    pBluetoothPin[Hw][Pin].Mask   =  (1 << (pBluetoothPin[Hw][Pin].Pin & 0x1F));

    SetGpio(pBluetoothPin[Hw][Pin].Pin);
  }

  REGLock;
}


#define   BtFloat(pin)              {\
                                      (*pBluetoothPin[Hw][pin].pGpio).dir   |=  pBluetoothPin[Hw][pin].Mask;\
                                    }

#define   BtRead(pin)               ((*pBluetoothPin[Hw][pin].pGpio).in_data &  pBluetoothPin[Hw][pin].Mask)

#define   BtHigh(pin)               {\
                                      (*pBluetoothPin[Hw][pin].pGpio).set_data  =  pBluetoothPin[Hw][pin].Mask;\
                                      (*pBluetoothPin[Hw][pin].pGpio).dir      &= ~pBluetoothPin[Hw][pin].Mask;\
                                    }

#define   BtLow(pin)                {\
                                      (*pBluetoothPin[Hw][pin].pGpio).clr_data  =  pBluetoothPin[Hw][pin].Mask;\
                                      (*pBluetoothPin[Hw][pin].pGpio).dir      &= ~pBluetoothPin[Hw][pin].Mask;\
                                    }



// DEVICE1 ********************************************************************


static ssize_t Device1Write(struct file *File,const char *Buffer,size_t Count,loff_t *Data)
{
  SBYTE   Buf[20];
  int     Lng;

  Lng  =  Count;
  copy_from_user(Buf,Buffer,Count);

  if (SET == (UBYTE)(Buf[0]))
  {
    if (BLUETOOTH_PINS > (UBYTE)(Buf[1]))
    {
      BtHigh((UBYTE)(Buf[1]));
    }
  }
  else
  {
    if (CLEAR == (UBYTE)(Buf[0]))
    {
      if (BLUETOOTH_PINS > (UBYTE)(Buf[1]))
      {
        BtLow((UBYTE)(Buf[1]));
      }
    }
    else
    {
      if (BLUETOOTH_PINS > (UBYTE)(Buf[1]))
      {
        BtFloat((UBYTE)(Buf[1]));
      }
    }
  }

  return (Lng);
}


static ssize_t Device1Read(struct file *File,char *Buffer,size_t Count,loff_t *Offset)
{

  if (BtRead(CTS_PIC))
  {
    Buffer[0] = 1;
  }
  else
  {
    Buffer[0] = 0;
  }

  return(1);
}


static ssize_t Device2Write(struct file *File,const char *Buffer,size_t Count,loff_t *Data)
{
  *((ULONG*)&(pLocalRam[2])) = 0x5555AAAA;
  return(1);
}


static ssize_t Device2Read(struct file *File,char *Buffer,size_t Count,loff_t *Offset)
{
  Buffer[0] = pLocalRam[0];
  Buffer[1] = pLocalRam[1];
  return(1);
}


static    const struct file_operations Device1Entries =
{
  .owner   =  THIS_MODULE,
  .read    =  Device1Read,
  .write   =  Device1Write
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

  Result  =  misc_register(&Device1);
  if (Result)
  {
    printk("  %s device register failed\n",DEVICE1_NAME);
  }
  else
  {
    #ifdef DEBUG
      printk("  %s device register succes\n",DEVICE1_NAME);
    #endif
  }

  return (Result);
}


static void Device1Exit(void)
{
  misc_deregister(&Device1);
#ifdef DEBUG
  printk("  %s device unregistered\n",DEVICE1_NAME);
#endif
}


/* Device 2 is to handle pirmware update status writing into */
/* internal RAM                                              */
static    const struct file_operations Device2Entries =
{
  .owner   =  THIS_MODULE,
  .read    =  Device2Read,
  .write   =  Device2Write
};


static    struct miscdevice Device2 =
{
  MISC_DYNAMIC_MINOR,
  DEVICE2_NAME,
  &Device2Entries
};


static int Device2Init(void)
{
  int     Result = -1;

  Result  =  misc_register(&Device2);
  if (Result)
  {
    printk("  %s device register failed\n",DEVICE2_NAME);
  }
  else
  {
    #ifdef DEBUG
      printk("  %s device register succes\n",DEVICE2_NAME);
    #endif
  }

  return (Result);
}


static void Device2Exit(void)
{
  misc_deregister(&Device2);
#ifdef DEBUG
  printk("  %s device unregistered\n",DEVICE2_NAME);
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

  /* Local memory for saving the firmware update status */
  if (request_mem_region(0xFFFF1FFC,0x00000004,MODULE_NAME) >= 0)
  {

    pLocalRam  =  (UBYTE*)ioremap(0xFFFF1FFA,0x00000006);

    if (pLocalRam != NULL)
    {
      #ifdef DEBUG
        printk("%s Local address mapped\n",MODULE_NAME);
      #endif
    }
  }

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
      Device2Init();

      BtHigh(PIC_RST);
      BtLow(PIC_EN);
      BtFloat(CTS_PIC);
    }
  }
  return (0);
}


static void ModuleExit(void)
{
  #ifdef DEBUG
    printk("%s exit started\n",MODULE_NAME);
  #endif

  BtHigh(PIC_RST);
  BtLow(PIC_EN);

  Device1Exit();
  Device2Exit();

}
