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


/*! \page PowerModule Power Control Module
 *
 *  Manages the power supervising and budget.\n
 *
 *-  \subpage PowerModuleResources
 */


#ifndef PCASM
#include  <asm/types.h>
#endif

#define   HW_ID_SUPPORT

#include  "../../lms2012/source/lms2012.h"
#include  "../../lms2012/source/am1808.h"


#define   MODULE_NAME                   "power_module"
#define   DEVICE1_NAME                  POWER_DEVICE

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


#ifdef    DEBUG_D_POWER
#define   DEBUG
#endif


int       Hw = 0;

enum      PowerPins
{
  P_EN,
  PENON,
  SW_RECHARGE,
  TESTPIN,
  POWER_PINS
};


INPIN     PowerPin[POWER_PINS];

/*! \page PowerModuleResources Gpios and Resources used for Module
 *
 *  Describes use of gpio and resources\n
 *
 *  \verbatim
 */

INPIN     EP2_PowerPin[POWER_PINS] =
{
  { GP6_5  , NULL, 0 }, // P_EN
  { GP6_11 , NULL, 0 }, // 5VPENON
  { GP8_8  , NULL, 0 }, // SW_RECHARGE
  { GP6_2  , NULL, 0 }  // ADCACK
};


INPIN     FINALB_PowerPin[POWER_PINS] =
{
  { GP6_5  , NULL, 0 }, // P_EN
  { GP6_11 , NULL, 0 }, // 5VPENON
  { GP8_8  , NULL, 0 }, // SW_RECHARGE
  { GP2_7  , NULL, 0 }  // TP4
};


INPIN     FINAL_PowerPin[POWER_PINS] =
{
  { GP6_5  , NULL, 0 }, // P_EN
  { GP6_11 , NULL, 0 }, // 5VPENON
  { GP8_8  , NULL, 0 }, // SW_RECHARGE
  { GP2_7  , NULL, 0 }  // TP4
};


/*  \endverbatim
 *  \n
 */


INPIN     *pPowerPin[] =
{
  [FINAL]     =   FINAL_PowerPin,       //  FINAL   platform
  [FINALB]    =   FINALB_PowerPin,      //  FINALB  platform
  [EP2]       =   EP2_PowerPin,         //  EP2     platform
};


//*****************************************************************************

static    void      __iomem *GpioBase;

void      SetGpio(int Pin)
{
  int     Tmp = 0;
  void    __iomem *Reg;

  if (Pin >= 0)
  {
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
  }
}


void      InitGpio(void)
{
  int     Pin;

  // unlock
  REGUnlock;

#ifdef DEBUG
  printk("  Power\n");
#endif
  memcpy(PowerPin,pPowerPin[Hw],sizeof(EP2_PowerPin));
  if (memcmp((const void*)PowerPin,(const void*)pPowerPin[Hw],sizeof(EP2_PowerPin)) != 0)
  {
    printk("%s PowerPin tabel broken!\n",MODULE_NAME);
  }

  for (Pin = 0;Pin < POWER_PINS;Pin++)
  {
    if (PowerPin[Pin].Pin >= 0)
    {
      PowerPin[Pin].pGpio  =  (struct gpio_controller *__iomem)(GpioBase + ((PowerPin[Pin].Pin >> 5) * 0x28) + 0x10);
      PowerPin[Pin].Mask   =  (1 << (PowerPin[Pin].Pin & 0x1F));

      SetGpio(PowerPin[Pin].Pin);
    }
  }

  // lock
  REGLock;
}


#define   POWEROn                       {\
                                          (*PowerPin[P_EN].pGpio).set_data    =  PowerPin[P_EN].Mask;\
                                          (*PowerPin[P_EN].pGpio).dir        &= ~PowerPin[P_EN].Mask;\
                                          (*PowerPin[PENON].pGpio).set_data   =  PowerPin[PENON].Mask;\
                                          (*PowerPin[PENON].pGpio).dir       &= ~PowerPin[PENON].Mask;\
                                        }

#define   POWEROff                      {\
                                          (*PowerPin[PENON].pGpio).clr_data   =  PowerPin[PENON].Mask;\
                                          (*PowerPin[PENON].pGpio).dir       &= ~PowerPin[PENON].Mask;\
                                        }


#define   ACCUFloat                     {\
                                          (*PowerPin[SW_RECHARGE].pGpio).dir |=  PowerPin[SW_RECHARGE].Mask;\
                                        }


#define   ACCURead                      ((*PowerPin[SW_RECHARGE].pGpio).in_data & PowerPin[SW_RECHARGE].Mask)


#define   TESTOn                        {\
                                          (*PowerPin[TESTPIN].pGpio).set_data   =  PowerPin[TESTPIN].Mask;\
                                          (*PowerPin[TESTPIN].pGpio).dir       &= ~PowerPin[TESTPIN].Mask;\
                                        }

#define   TESTOff                       {\
                                          (*PowerPin[TESTPIN].pGpio).clr_data   =  PowerPin[TESTPIN].Mask;\
                                          (*PowerPin[TESTPIN].pGpio).dir       &= ~PowerPin[TESTPIN].Mask;\
                                        }


// DEVICE1 ********************************************************************

static    UBYTE PowerOffFlag = 0;

static int Device1Ioctl(struct inode *pNode, struct file *File, unsigned int Request, unsigned long Pointer)
{
  PowerOffFlag  =  1;

  return (0);
}


static ssize_t Device1Write(struct file *File,const char *Buffer,size_t Count,loff_t *Data)
{
  char    Buf[1];
  int     Lng     = 0;

  if (Count == 1)
  {
    copy_from_user(Buf,Buffer,Count);
    if (Buf[0])
    {
      TESTOn;
    }
    else
    {
      TESTOff;
    }
  }

  Lng           =  Count;

  return (Lng);
}


static ssize_t Device1Read(struct file *File,char *Buffer,size_t Count,loff_t *Offset)
{
  int     Lng     = 0;
  char    Accu;

  if (ACCURead)
  {
    Accu  =  '0';
  }
  else
  {
    Accu  =  '1';
  }

  Lng  =  snprintf(Buffer,Count,"%c\r",Accu);

  return (Lng);
}


static    const struct file_operations Device1Entries =
{
  .owner        = THIS_MODULE,
  .read         = Device1Read,
  .write        = Device1Write,
  .ioctl        = Device1Ioctl
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
    PowerOffFlag  =  0;

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
      POWEROn;
      TESTOff;
      ACCUFloat;

      Device1Init();
    }
  }

  return (0);
}


static void ModuleExit(void)
{
  ULONG   Tmp1;
  ULONG   Tmp2;

#ifdef DEBUG
  printk("%s exit started\n",MODULE_NAME);
#endif

  TESTOff;
  Device1Exit();

  if (PowerOffFlag)
  {
    for (Tmp1 = 0;Tmp1 < 0xFFFFFFFF;Tmp1++)
    {
      for (Tmp2 = 0;Tmp2 < 0xFFFFFFFF;Tmp2++)
      {
        POWEROff;
      }
    }
  }

  iounmap(GpioBase);

}
