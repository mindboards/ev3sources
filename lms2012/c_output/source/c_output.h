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


#ifndef C_OUTPUT_H_
#define C_OUTPUT_H_

RESULT    cOutputInit(void);

RESULT    cOutputOpen(void);

RESULT    cOutputClose(void);

RESULT    cOutputExit(void);

void      cOutputSetTypes(char *pTypes);
void      cOutputSetType(void);
UBYTE     cMotorGetBusyFlags(void);
void      cMotorSetBusyFlags(UBYTE Flags);
void	    ResetDelayCounter(UBYTE Pattern);

void      cOutputReset(void);
void      cOutputStop(void);
void      cOutputPrgStop(void);
void      cOutputPower(void);
void      cOutputSpeed(void);
void      cOutputStart(void);
void      cOutputPolarity(void);

void      cOutputRead(void);
void      cOutputTest(void);
void      cOutputReady(void);
void      cOutputStepPower(void);
void      cOutputTimePower(void);
void      cOutputStepSpeed(void);
void      cOutputTimeSpeed(void);
void      cOutputStepSync(void);
void      cOutputTimeSync(void);
void      cOutputClrCount(void);
void      cOutputGetCount(void);

typedef struct
{
  //*****************************************************************************
  // Output Global variables
  //*****************************************************************************

  DATA8       OutputType[OUTPUTS];
  OBJID       Owner[OUTPUTS];

  int         PwmFile;
  int         MotorFile;

  MOTORDATA   MotorData[OUTPUTS];
  MOTORDATA   *pMotor;
}
OUTPUT_GLOBALS;

#if       (HARDWARE == SIMULATION)
  extern OUTPUT_GLOBALS * gOutputInstance;
  #define OutputInstance (*gOutputInstance)

  void setOutputInstance(OUTPUT_GLOBALS * _Instance);
  OUTPUT_GLOBALS * getOutputInstance();
#else
  extern OUTPUT_GLOBALS OutputInstance;
#endif

#endif /* C_OUTPUT_H_ */
