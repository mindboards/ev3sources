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

/*! \page Sound Driver Module
 *
 *
 *-  \subpage SoundDriverModuleResources
 */


#define   HW_ID_SUPPORT

#include  "../../lms2012/source/lms2012.h"
#include  "../../lms2012/source/am1808.h"

#ifdef    DEBUG_D_SOUND
#define   DEBUG
#endif

#define   MODULE_NAME                   "sound_module"
#define   DEVICE1_NAME                  SOUND_DEVICE

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
  TIMING_SAMPLES,
  ONE_SHOT,
  MANUAL,
  READY_FOR_SAMPLES,
  IDLE
} TIMER_MODES;

/*
 * Variables
 */
static    ULONG   *SYSCFG0;
static    ULONG   *PSC1;
static    UWORD   *eHRPWM0;
static    void    __iomem *GpioBase;
static    UWORD   Duration;
static    UWORD   Period;
static    UBYTE   Level;
static 	  UBYTE	  TimerMode = READY_FOR_SAMPLES;
static	  SOUND		SoundDefault;
static	  SOUND   *pSound = &SoundDefault;
static    struct hrtimer  Device1Timer;
static    ktime_t         Device1Time;


/*
 * Defines related to hardware PWM output
 */

#define CFGCHIP0          0x60            // 32 bit wide index into SYSCFG0
#define PLL_MASTER_LOCK   0x00001000
#define DEFAULT_FREQUENCY 16499           // 16499 should give 8 KHz
#define DEFAULT_LEVEL     1000            // 25% D.C.

static UWORD	SoundBuffers[SOUND_BUFFER_COUNT][SOUND_FILE_BUFFER_SIZE];
static UBYTE  SoundChunkSize[SOUND_BUFFER_COUNT];
static UWORD  BufferReadPointer;
static UBYTE	BufferReadIndex;
static UBYTE	BufferWriteIndex;

// TBCTL (Time-Base Control)
// = = = = = = = = = = = = = = = = = = = = = = = = = =
#define   TB_COUNT_UP                   0x0     // TBCNT MODE bits
#define   TB_DISABLE                    0x0     // PHSEN bit
#define   TB_ENABLE                     0x4
#define   TB_SHADOW                     0x0     // PRDLD bit
#define   TB_IMMEDIATE                  0x8
#define   TB_SYNC_DISABLE               0x30    // SYNCOSEL bits
#define   TB_HDIV1                      0x0     // HSPCLKDIV bits
#define   TB_DIV1                       0x0     // CLKDIV bits
#define   TB_UP                         0x2000  // PHSDIR bit

// CMPCTL (Compare Control)
// = = = = = = = = = = = = = = = = = = = = = = = = = =
#define   CC_CTR_A_ZERO                 0x0     // LOADAMODE bits
#define   CC_CTR_B_ZERO                 0x0
#define   CC_A_SHADOW                   0x00    // SHDWAMODE and SHDWBMODE bits
#define   CC_B_SHADOW                   0x00
#define   CC_B_NO_SHADOW                0x40

#define   TBCTL                         0x0
#define   TBPHS                         0x3
#define   TBCNT                         0x4
#define   TBPRD                         0x5
#define   CMPCTL                        0x7
#define   CMPA                          0x9
#define   CMPB                          0xA
#define   AQCTLA                        0xB
#define   AQCTLB                        0xC

// Action-Qualifier Output B Control Register AQCTLB:

//                              1111 11
//                        bit   5432 1098 7654 3210

//      Bit 15 Reserved         R000 0000 0000 0000
//      Bit 14 -                0R00 0000 0000 0000
//      Bit 13 -                00R0 0000 0000 0000
//      Bit 12 -                000R 0000 0000 0000
#define SOUND_RESERVED          0x0000

//      Bit 11 CBD              0000 0000 0000 0000
//      Bit 10 -                0000 0100 0000 0000 CLEAR Forces EPWM0B LOW
#define SOUND_CBD               0x0400

//      Bit  9 CBU              0000 0000 0000 0000
//      Bit  8 -                0000 0001 0000 0000 CLEAR Forces EPWM0B LOW
#define SOUND_CBU               0x0100

//      Bit  7 CAD              0000 0000 N000 0000 DO NOTHING
//      Bit  6 -                0000 0000 0N00 0000 -
#define SOUND_CAD               0x0000

//      Bit  5 CAU              0000 0000 00N0 0000 DO NOTHING
//      Bit  4 -                0000 0000 000N 0000 -
#define SOUND_CAU               0x0000

//      Bit  3 PRD              0000 00N0 0000 1000 Forces EPWM0B HIGH
//      Bit  2 -                0000 000N 0000 0000 -
#define SOUND_PRD               0x0008

//      Bit  1 ZRO              0000 00N0 0000 0010 Forces EPWM0B HIGH
//      Bit  0 -                0000 000N 0000 0000 -
#define SOUND_ZRO               0x0002




#define SOUND_SYMM_TONE (SOUND_RESERVED +\
                         SOUND_CBD +\
                         SOUND_CBU +\
                         SOUND_CAD +\
                         SOUND_CAU +\
                         SOUND_PRD +\
                         SOUND_ZRO)

/*
 * Macro defines related to hardware PWM output
 */

// Put the "Magic key" into KICK0R and KICK1R to open for manipulation
// Stop Time Base Clock (TBCLK) - clear bit TBCLKSYNC in CFGCHIP1
// Remove the "Magic key" from KICK0R and KICK1R to lock

#define   EHRPWMClkDisable              {\
                                          eHRPWM0[TBCTL]  = 0xC033;\
/*                                        REGUnlock;\
                                          iowrite32((ioread32(&SYSCFG0[CFGCHIP0]) & ~PLL_MASTER_LOCK),&SYSCFG0[0x60]);\
                                          REGLock;*/\
                                        }

// Put the "Magic key" into KICK0R and KICK1R to open for manipulation
// Start Time Base Clock (TBCLK) - set bit TBCLKSYNC in CFGCHIP1
// Remove the "Magic key" from KICK0R and KICK1R to lock

#define   EHRPWMClkEnable               {\
                                          eHRPWM0[TBCTL]  = 0xC030; \
                                          REGUnlock;\
                                          iowrite32((ioread32(&SYSCFG0[CFGCHIP0]) | PLL_MASTER_LOCK),&SYSCFG0[0x60]);\
                                          REGLock;\
                                        }

#define   EHRPWMClkEnableTone           {\
                                          eHRPWM0[TBCTL]  = 0xDC30;\
                                          REGUnlock;\
                                          iowrite32((ioread32(&SYSCFG0[CFGCHIP0]) | PLL_MASTER_LOCK),&SYSCFG0[0x60]);\
                                          REGLock;\
                                        }

#define   SETPwmPeriod(Prd)               {\
                                          eHRPWM0[TBPRD] = Prd; /* A factor of sample-rate  */\
                                                                /* For lowering the quanti- */\
                                                                /* zation noise (simpler    */\
                                                                /* filtering)               */\
                                        }

#define   SETSoundLevel(Level)          {\
                                          eHRPWM0[CMPB] = Level;  /* The amplitude for this */\
                                                                  /* very and/or following  */\
                                                                  /* samples/periods        */\
                                        }

#define   STOPPwm                       {\
                                          iowrite16(0x00, &eHRPWM0[TBCTL]);\
                                          iowrite16(0x00, &eHRPWM0[CMPCTL]);\
                                          EHRPWMClkDisable;\
                                        }

#define   SOUNDPwmModuleSetupPcm        { \
                                          \
                                          /* eHRPWM Module */\
                                          /*  In eHRPWM1[TBCTL] TB_DISABLE, TB_SHADOW, TB_HDIV1, TB_DIV1, TB_COUNT_UP all cleared */\
                                          EHRPWMClkDisable;\
                                          eHRPWM0[TBPHS]  = 0; /* Phase register cleared */\
                                          eHRPWM0[TBCNT]  = 0; /* Clear TB counter */\
/*                                        eHRPWM0[TBCTL]  = 0xC030;*/\
                                          eHRPWM0[CMPCTL] = (CC_B_SHADOW | CC_CTR_B_ZERO);\
                                          eHRPWM0[AQCTLB] = 0x0102;\
                                          EHRPWMClkEnable;\
                                        }

#define   SOUNDPwmModuleSetupTone        { \
                                          \
                                          /* eHRPWM Module */\
                                          /*  In eHRPWM1[TBCTL] TB_DISABLE, TB_SHADOW, TB_HDIV1, TB_DIV1, TB_COUNT_UP all cleared */\
                                          EHRPWMClkDisable;\
                                          eHRPWM0[TBPHS]  = 0; /* Phase register cleared */\
                                          eHRPWM0[TBCNT]  = 0; /* Clear TB counter */\
/*                                        eHRPWM0[TBCTL]  = 0xDC30;*/\
                                          eHRPWM0[CMPCTL] = (CC_B_SHADOW | CC_CTR_B_ZERO);\
                                          eHRPWM0[AQCTLB] = 0x0102;\
/*                                        EHRPWMClkEnable;*/\
                                          EHRPWMClkEnableTone;\
                                        }

#define   SOUNDEnable           {\
                                  (*SoundPin[SOUNDEN].pGpio).set_data    =  SoundPin[SOUNDEN].Mask;\
                                  (*SoundPin[SOUNDEN].pGpio).dir        &= ~SoundPin[SOUNDEN].Mask;\
                                }

#define   SOUNDDisable          {\
                                  (*SoundPin[SOUNDEN].pGpio).clr_data    =  SoundPin[SOUNDEN].Mask;\
                                  (*SoundPin[SOUNDEN].pGpio).dir        &= ~SoundPin[SOUNDEN].Mask;\
                                }

#define   SOUNDPwmPoweron       {\
                                  iowrite32(0x00000003, &PSC1[0x291]); /* Set ePWM module power on      */\
                                  iowrite32(0x00000003, &PSC1[0x48]);  /* Evaluate all the NEXT fields  */\
                                }

enum      SoundPins
{
  SOUNDEN,        // Sound Enable to Amp
  SOUND_ARMA,     // PWM audio signal from ARM
  SOUND_PINS
};


//*****************************************************************************
#if       (HARDWARE == FINAL)
//***************************************SETSoundLevel**************************************

/*! \page SoundModuleResources Gpios and Resources used for Module
 *
 *  Describes use of gpio and resources\n
 *
 *  \verbatim
 */

INPIN     SoundPin[SOUND_PINS] =
{
  { GP6_15 , NULL, 0 }, // SOUNDEN
  { EPWM0B , NULL, 0 }, // SOUND_ARMA
};

/*  \endverbatim
 *  \n
 */

#endif

//*****************************************************************************
#if       (HARDWARE == ONE2ONE)
//*****************************************************************************

/*! \page SoundModuleResources Gpios and Resources used for Module
 *
 *  Describes use of gpio and resources\n
 *
 *  \verbatim
 */

INPIN     SoundPin[SOUND_PINS] =
{
  { GP2_8  , NULL, 0 }, // SOUNDEN
  { EPWM0B , NULL, 0 }, // SOUND_ARMA
};

/*  \endverbatim
 *  \n
 */

#endif

//*****************************************************************************
#if       (HARDWARE == A4)
//*****************************************************************************

INPIN     SoundPin[SOUND_PINS] =
{
  { GP5_6  , NULL, 0 }, // SOUNDEN
  { EPWM0B , NULL, 0 }, // SOUND_ARMA
};

#endif

//*****************************************************************************
#if       (HARDWARE == EVALBOARD)
//*****************************************************************************

INPIN     SoundPin[SOUND_PINS] =
{
  { GP7_4  , NULL, 0 }, // SOUNDEN
  { EPWM0B , NULL, 0 }, // SOUND_ARMA
};

#endif

//*****************************************************************************

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
      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
        printk("    GP%d_%-2d   0x%08X and 0x%08X or 0x%08X\n",(Pin >> 4),(Pin & 0x0F),(u32)Reg, MuxRegMap[Tmp].Mask, MuxRegMap[Tmp].Mode);
      #endif
    }
    else
    {
      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
        printk("    FUNCTION 0x%08X and 0x%08X or 0x%08X\n",(u32)Reg, MuxRegMap[Tmp].Mask, MuxRegMap[Tmp].Mode);
      #endif
    }
  }
  else
  {
    //#define DEBUG
    #undef DEBUG
    #ifdef DEBUG
      printk("*   GP%d_%-2d  ********* ERROR not found *********\n",(Pin >> 4),(Pin & 0x0F));
    #endif
  }

}


void      InitGpio(void)
{
  int     Pin;

  // unlock
  REGUnlock;

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("  Sound\n");
  #endif

  for (Pin = 0;Pin < SOUND_PINS;Pin++)
  {
    SoundPin[Pin].pGpio  =  (struct gpio_controller *__iomem)(GpioBase + ((SoundPin[Pin].Pin >> 5) * 0x28) + 0x10);
    SoundPin[Pin].Mask   =  (1 << (SoundPin[Pin].Pin & 0x1F));

    SetGpio(SoundPin[Pin].Pin);
  }

  // lock
  REGLock;
}

/*! \page PwmModule
 *
 *  <hr size="1"/>
 *  <b>     write </b>
 *
 */
/*! \brief    GetPeriphealBasePtr
 *
 *  Helper function for getting the peripheal HW base address
 *
 */

void    GetPeriphealBasePtr(ULONG Address, ULONG Size, ULONG **Ptr)
{
  /* eCAP0 pointer */
  if (request_mem_region(Address,Size,MODULE_NAME) >= 0)
  {

    *Ptr  =  (ULONG*)ioremap(Address,Size);

    if (*Ptr != NULL)
    {
      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
        printk("%s memory Remapped from 0x%08lX\n",DEVICE1_NAME,(unsigned long)*Ptr);
      #endif
    }
    else
    {
      //#define DEBUG
      #undef DEBUG
      #ifdef DEBUG
        printk("Memory remap ERROR");
      #endif
    }
  }
  else
  {
    //#define DEBUG
    #undef DEBUG
    #ifdef DEBUG
      printk("Region request error");
    #endif
  }
}

// DEVICE1 ********************************************************************

static void Device1TimerSetTiming(ULONG Secs, ULONG NanoSecs )
{
  Device1Time  =  ktime_set(Secs, NanoSecs);
}

static void Device1TimerStart(void)     // (Re) start the high-resolution timer
{
  hrtimer_start(&Device1Timer,Device1Time,HRTIMER_MODE_REL);
}

static void Device1TimerInitDuration(void)
{
  UWORD Sec;
  ULONG nSec;

  Sec = (UWORD)(Duration / 1000);                       // Whole secs
  nSec = (ULONG)((Duration - Sec * 1000) * 1000000);    // Raised to mSec
  TimerMode = ONE_SHOT;
  Device1TimerSetTiming(Sec, nSec);
  Device1TimerStart();                                  // Start / reStart
}

static void Device1TimerCancel(void)
{
  TimerMode = IDLE;
  hrtimer_cancel(&Device1Timer);
}

static void Device1TimerExit(void)
{
  Device1TimerCancel();
}

static enum hrtimer_restart Device1TimerInterrupt1(struct hrtimer *pTimer)
{
  UWORD TempLevel;

  enum hrtimer_restart ret = HRTIMER_RESTART;

  if (0 < hrtimer_forward_now(pTimer,Device1Time))
  {
    // Take care sample missed!!!!!
  }

  switch(TimerMode)
  {
    case READY_FOR_SAMPLES: if(SoundChunkSize[BufferReadIndex] > 0) // Any samples D/L yet?
                            {

                              TimerMode = TIMING_SAMPLES;           // We're ready, next interrupt will
                                                                    // bring the sound alive
                            }
                            break;

    case TIMING_SAMPLES:    // Get new sample - if any

                            if(SoundChunkSize[BufferReadIndex] > 0) // Anything to use in Buffers?
                            {
                              // Get raw sample
                              TempLevel = SoundBuffers[BufferReadIndex][BufferReadPointer++];
                              if(TempLevel == 0) TempLevel++;
                              // Add volume i.e. scale the PWM level

                              TempLevel = (UWORD)(Level * TempLevel);
                              SETSoundLevel(TempLevel);

                              SoundChunkSize[BufferReadIndex]--;

                              if(SoundChunkSize[BufferReadIndex] < 1)
                              {
                                BufferReadPointer = 0;
                                BufferReadIndex++;
                                if(BufferReadIndex >= SOUND_BUFFER_COUNT)
                                  BufferReadIndex = 0;
                              }
                            }
                            else
                            {
                              ret = HRTIMER_NORESTART;

                              SOUNDDisable;
                              TimerMode = IDLE;
                              (*pSound).Status  =  OK;
                              // ret(urn value) already set
                            }
                            break;

    case ONE_SHOT:          // Stop tone - duration elapsed!
                            ret = HRTIMER_NORESTART;
                            EHRPWMClkDisable;
                            SOUNDDisable;
                            TimerMode = IDLE;
                            (*pSound).Status  =  OK;
                            // ret(urn value) already set

    case IDLE:              break;
    case MANUAL:            break;
    default:                ret = HRTIMER_NORESTART;
                            break;
    }

    return (ret);   // Keep on or stop doing the job
}

static void Device1TimerInit8KHz(void)
{
  /* Setup 125 uS timer interrupt*/

    TimerMode = READY_FOR_SAMPLES;    // Allow for D/L to SoundBuffer(s)
    Device1TimerSetTiming(0, 125000); // 8 KHz. sample-rate
    Device1TimerStart();              // Start / reStart
}

static ssize_t Device1Write(struct file *File, const char *Buffer, size_t Count, loff_t *Data)
{

    char    CommandBuffer[SOUND_FILE_BUFFER_SIZE + 1];	// Space for command @ byte 0
    UWORD   Frequency;
    UWORD   PwmLevel;
    int		BytesWritten = 0;
    int 	i = 0;

    copy_from_user(CommandBuffer, Buffer, Count);

    switch(CommandBuffer[0])
    {
      case SERVICE:
                      if(Count > 1)
      	  	  	  	  	{
    	  	  	  	    	  if(SoundChunkSize[BufferWriteIndex] == 0) // Is the requested RingBuffer[Index] ready for write?
    	  	  	  	    	  {
    	  	  	  	    	    for(i = 1; i < Count; i++)
    	  	  	  	    	      SoundBuffers[BufferWriteIndex][i-1] = CommandBuffer[i];
    	  	  	  	    	    SoundChunkSize[BufferWriteIndex] = Count - 1;
    	  	  	  	    	    BufferWriteIndex++;
    	  	  	  	    	    if(BufferWriteIndex >= SOUND_BUFFER_COUNT)
    	  	  	  	    	      BufferWriteIndex = 0;
    	  	  	  	    	    BytesWritten = Count - 1;
    	  	  	  	    	  }
      	  	  	  	  	}
          	  	  	  	break;

      case TONE:
    	  	  	  	    SOUNDDisable;
                        SOUNDPwmModuleSetupTone;
                        Level = CommandBuffer[1];
                        Frequency = (UWORD)(CommandBuffer[2] + (CommandBuffer[3] << 8));
                        Duration = (UWORD) (CommandBuffer[4] + (CommandBuffer[5] << 8));

                        if (Frequency < SOUND_MIN_FRQ)
                        {
                          Frequency = SOUND_MIN_FRQ;
                        }
                        else if (Frequency > SOUND_MAX_FRQ)
                        {
                          Frequency = SOUND_MAX_FRQ;
                        }

                        Period = (UWORD)((SOUND_TONE_MASTER_CLOCK / Frequency) - 1);
                        SETPwmPeriod(Period);

                        if (Level > 100) Level = 100;

                        PwmLevel = (UWORD)(((ulong)(Period) * (ulong)(Level)) /(ulong)(200)); // Level from % to ticks (Duty Cycle)

                        SETSoundLevel(PwmLevel);

                        if(Duration > 0)  // Infinite or specific?
                        {
                          Device1TimerInitDuration();
                          TimerMode = ONE_SHOT;
                        }
                        else
                          TimerMode = MANUAL;

                        SOUNDEnable;

                        break;

      case REPEAT:	  // Handled in Green Layer
    	  	  	  	    break;

      case PLAY:
    	  	  	  	  SOUNDDisable;
                      SOUNDPwmModuleSetupPcm;
                      Level = CommandBuffer[1];
                      Period = (UWORD)(SOUND_MASTER_CLOCK / 64000); // 64 KHz => 8 shots of each sample
      	  	  	  	  SETPwmPeriod(Period);                         // helps filtering

    	  	  	  	    BufferWriteIndex = 0;		// Reset to first ring Buffer
      	  	  	  	  BufferReadIndex = 0;		// -
      	  	  	  	  BufferReadPointer = 0;	// Ready for very first sample
      	  	  	  	  for (i = 0; i < SOUND_BUFFER_COUNT; i++)
      	  	  	  		  SoundChunkSize[i] = 0;	// Reset all Buffer sizes

      	  	  	  	  Device1TimerInit8KHz();   // TimerMode = READY_FOR_SAMPLES;
      	  	  	  	  if(Level != 0)
      	  	  	  	    SOUNDEnable;
      	  	  	  	  // Else we remove any "selfmade" noise
      	  	  	  	  // Better n/s+n ratio ;-)
      	  	          BytesWritten = 2; // CMD and Level
    	  	  	  	    break;

      case BREAK:     Device1TimerCancel();
                      SOUNDDisable;
                      EHRPWMClkDisable;
                      TimerMode = IDLE;
                      (*pSound).Status  =  OK;
                      break;

      default:
                      break;
    }

  return (BytesWritten);
}


static ssize_t Device1Read(struct file *File,char *Buffer,size_t Count,loff_t *Offset)
{
  int     Lng     = 0;

  Lng  =  snprintf(Buffer,Count,"%s\r",DEVICE1_NAME);

  return (Lng);
}

#define     SHM_LENGTH    (sizeof(SoundDefault))
#define     NPAGES        ((SHM_LENGTH + PAGE_SIZE - 1) / PAGE_SIZE)
static void *kmalloc_ptr;

static int Device1Mmap(struct file *filp, struct vm_area_struct *vma)
{
   int ret;

   ret = remap_pfn_range(vma,vma->vm_start,virt_to_phys((void*)((unsigned long)pSound)) >> PAGE_SHIFT,vma->vm_end-vma->vm_start,PAGE_SHARED);

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
  int i = 0;
  UWORD   *pTmp;

  // Get pointers to memory-mapped registers
  GetPeriphealBasePtr(0x01C14000, 0x190, (ULONG**)&SYSCFG0); /* SYSCFG0 Pointer */
  GetPeriphealBasePtr(0x01F00000, 0x1042,(ULONG**)&eHRPWM0); /* eHRPWM0 Pointer */
  GetPeriphealBasePtr(0x01E27000, 0xA80, (ULONG**)&PSC1);    /* PSC1 pointer    */

  Result  =  misc_register(&Device1);
  if (Result)
  {
    //#define DEBUG
    #undef DEBUG
    #ifdef DEBUG
      printk("  %s device register failed\n",DEVICE1_NAME);
    #endif
  }
  else
  {
    // allocate kernel shared memory for SoundFlags used by Test etc.
    // showing the state of the sound-module used by async and waiting
    // use from VM

        if ((kmalloc_ptr = kmalloc((NPAGES + 2) * PAGE_SIZE, GFP_KERNEL)) != NULL)
        {
          pTmp = (UWORD*)((((unsigned long)kmalloc_ptr) + PAGE_SIZE - 1) & PAGE_MASK);
          for (i = 0; i < NPAGES * PAGE_SIZE; i += PAGE_SIZE)
          {
            SetPageReserved(virt_to_page(((unsigned long)pTmp) + i));
          }
          pSound =  (SOUND*)pTmp;

          SOUNDPwmPoweron;

          /* Setup the Sound PWM peripherals */

          SOUNDPwmModuleSetupPcm;

          /* Setup 125 uS timer interrupt*/
          Device1TimerSetTiming(0, 125000); // Default to 8 KHz. sample-rate
          hrtimer_init(&Device1Timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
          // Timer Callback function - do the sequential job
          Device1Timer.function  =  Device1TimerInterrupt1;
          Device1TimerCancel();

          SOUNDDisable; // Disable the Sound Power Amp

          //#define DEBUG
          #undef DEBUG
		      #ifdef DEBUG
        	  printk("  %s device register succes\n",DEVICE1_NAME);
		      #endif

        	(*pSound).Status  =  OK;  // We're ready for making "noise"
        }
  }

  return (Result);
}

static void Device1Exit(void)
{
  UWORD   *pTmp;
  int i = 0;

  Device1TimerExit();
  misc_deregister(&Device1);

  #define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("%s exit started\n",MODULE_NAME);
  #endif

  iounmap(SYSCFG0);
  iounmap(eHRPWM0);
  iounmap(PSC1);

  // free shared memory
  pTmp    =  (UWORD*)pSound;
  pSound  =  &SoundDefault;

  for (i = 0; i < NPAGES * PAGE_SIZE; i+= PAGE_SIZE)
  {
    ClearPageReserved(virt_to_page(((unsigned long)pTmp) + i));

    #define DEBUG
    #undef DEBUG
    #ifdef DEBUG
      printk("  %s memory page %d unmapped\n",DEVICE1_NAME,i);
    #endif
  }

  kfree(kmalloc_ptr);


}


// MODULE *********************************************************************


#ifndef PCASM
module_param (HwId, charp, 0);
#endif

static int ModuleInit(void)
{
  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("HwId = %d\n",HWID);
  #endif

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("%s init started\n",MODULE_NAME);
  #endif

  if (request_mem_region(DA8XX_GPIO_BASE,0xD8,MODULE_NAME) >= 0)
  {
    GpioBase  =  (void*)ioremap(DA8XX_GPIO_BASE,0xD8);
    if (GpioBase != NULL)
    {
      //#define DEBUG
      #undef DEBUG
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
  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("%s exit started\n",MODULE_NAME);
  #endif

  SOUNDDisable; // Disable the Sound Power Amp
  Device1Exit();

  iounmap(GpioBase);
}
