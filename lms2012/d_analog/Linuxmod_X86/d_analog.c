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
#include  <linux/time.h>
#endif

#include  "../../lms2012/source/lms2012.h"

#define   MODULE_NAME                   "analog_module"
#define   DEVICE1_NAME                  ANALOG_DEVICE

static    int  ModuleInit(void);
static    void ModuleExit(void);

#define   __USE_POSIX

#include  <linux/kernel.h>
#include  <linux/fs.h>
#include  <linux/sched.h>

#ifndef   PCASM
#include  <linux/mm.h>
#include  <linux/hrtimer.h>
#include  <linux/slab.h>

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

// DEVICE1 ********************************************************************

static    struct hrtimer Device1Timer;
static    ktime_t        Device1Time;

static    ANALOG  AnalogDefault;

static    ANALOG *pAnalog   =  &AnalogDefault;
static    UWORD  *pInputs   = (UWORD*)&AnalogDefault;


static enum hrtimer_restart Device1TimerInterrupt1(struct hrtimer *pTimer)
{
  // restart timer
  hrtimer_forward_now(pTimer,Device1Time);

#ifndef DISABLE_PREEMPTED_VM
  ((*pAnalog).PreemptMilliSeconds)++;
#endif

  return (HRTIMER_RESTART);
}


#define     SHM_LENGTH    (sizeof(AnalogDefault))
#define     NPAGES        ((SHM_LENGTH + PAGE_SIZE - 1) / PAGE_SIZE)
static void *kmalloc_ptr;

static int Device1Mmap(struct file *filp, struct vm_area_struct *vma)
{
   int ret;

   ret = remap_pfn_range(vma,vma->vm_start,virt_to_phys((void*)((unsigned long)pAnalog)) >> PAGE_SHIFT,vma->vm_end-vma->vm_start,PAGE_SHARED);

   if (ret != 0)
   {
     ret  =  -EAGAIN;
   }

   return (ret);
}


static    const struct file_operations Device1Entries =
{
  .owner        = THIS_MODULE,
  .mmap         = Device1Mmap,
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
      pAnalog      =  (ANALOG*)pTmp;
      pInputs      =  pTmp;

      Device1Time  =  ktime_set(0,DEVICE_UPDATE_TIME);
      hrtimer_init(&Device1Timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
      Device1Timer.function  =  Device1TimerInterrupt1;
      hrtimer_start(&Device1Timer,Device1Time,HRTIMER_MODE_REL);

#ifndef DISABLE_PREEMPTED_VM
      (*pAnalog).PreemptMilliSeconds = 1;
#endif

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
  UWORD   *pTmp;
  int     i;

  hrtimer_cancel(&Device1Timer);

  pTmp     =  pInputs;
  pInputs  =  (UWORD*)&AnalogDefault;
  pAnalog  =  &AnalogDefault;

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


static int ModuleInit(void)
{
  printk("%s init started\n",MODULE_NAME);

  Device1Init();

  return (0);
}


static void ModuleExit(void)
{
  printk("%s exit started\n",MODULE_NAME);

  Device1Exit();
}
