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


/*! \page OutputLibrary Output Library
 *
\verbatim
Byte codes:

opOUTPUT_GET_TYPE     LAYER   NO       *TYPE                                   // Get output device type
opOUTPUT_SET_TYPE     LAYER   NO       TYPE                                    // Set output device type
opOUTPUT_RESET        LAYER   NOS                                              // Reset position  (POS=0)
opOUTPUT_STOP         LAYER   NOS      BRAKE                                   // Stop outputs    (brake or coast)
opOUTPUT_POWER        LAYER   NOS      POWER                                   // Set power       (suspend regulation and positioning)
opOUTPUT_SPEED        LAYER   NOS      SPEED                                   // Set speed (relative to polarity - enables regulation if tacho)
opOUTPUT_START        LAYER   NOS                                              // Starts outputs from present values
opOUTPUT_POLARITY     LAYER   NOS      POL                                     // Set polarity    (0=toggle)
opOUTPUT_READ         LAYER   NOS      *SPEED *STEPS                           // Read actual speed and steps from last reset
opOUTPUT_TEST         LAYER   NOS      *Busy                                   //
opOUTPUT_READY        LAYER   NOS                                              // Wait for new action completed or overruled
opOUTPUT_POSITION     LAYER   NOS      POS                                     // Set position    (absolute from last reset)

opOUTPUT_STEP_POWER   LAYER   NOS      POWER   STEP1   STEP2   STEP3   BRAKE   // Set all parameters, start if not started and power != 0
opOUTPUT_TIME_POWER   LAYER   NOS      POWER   TIME1   TIME2   TIME3   BRAKE   // Set all parameters, start if not started and power != 0
opOUTPUT_STEP_SPEED   LAYER   NOS      SPEED   STEP1   STEP2   STEP3   BRAKE   // Set all parameters, start if not started and power != 0
opOUTPUT_TIME_SPEED   LAYER   NOS      SPEED   TIME1   TIME2   TIME3   BRAKE   // Set all parameters, start if not started and power != 0
opOUTPUT_STEP_SYNC    LAYER   NOS      SPEED   TURN    STEP    BRAKE           // Set all parameters, start if not started and power != 0
opOUTPUT_TIME_SYNC    LAYER   NOS      SPEED   TURN    TIME    BRAKE           // Set all parameters, start if not started and power != 0
opOUTPUT_CLR_COUNT    LAYER   NOS                                              // Clears the tacho count used when in sensor mode
opOUTPUT_GET_COUNT    LAYER   NO       *STEPS                                  // Gets the tacho count used in sensor mode


Parameters:
                      LAYER   DATA8    [0..3]              // USB chain layer                      (0=Master, 1=Slave1)
                      NO      DATA8    [0..3]              // Output port number
                      TYPE    DATA8    [0..255]            // Output device type                   (0=none)
                      NOS     DATA8    [0x00..0x0F]        // Bit field representing output 1 to 4 (0x01, 0x02, 0x04, 0x08)
                      BRAKE   DATA8    [0..1]              // Output state after stop              (0=Coast,  1=Brake)
                      POWER   DATA8    [+-0..100%]         // Power relative to polarity
                      SPEED   DATA8    [+-0..100%]         // Speed relative to polarity           (0->BRAKE=1)
                      STEPS   DATA32   [+-0..MAX]          // Steps in degrees                     (0=infinite)
                      POL     DATA8    [+-0..1]            // Polarity +-1, 0=toggle               (multiplied to SPEED and POWER)
                      POS     DATA32   [+-0..MAX]          // Steps in degrees                     (0=infinite)
                      STEP1   DATA32   [0..MAX]            // Steps used to ramp up
                      STEP2   DATA32   [0..MAX]            // Steps used for constant speed
                      STEP3   DATA32   [0..MAX]            // Steps used to ramp down
                      TIME1   DATA32   [0..MAX]            // Time [mS] to ramp up
                      TIME2   DATA32   [0..MAX]            // Time [mS] for constant speed
                      TIME3   DATA32   [0..MAX]            // Time [mS] to ramp down
                      TURN    DATA16   [-200..200]         // Turn ratio between two syncronized motors
*/

/*
 * SYNCRONIZATION:
 *
 *  Speed -100 to +100 is move forward or move backwards
 *  Turn ratio is how tight you turn and to what direction you turn
 *   - 0 value is moving straight forward
 *   - Negative values turns to the left
 *   - Positive values turns to the right
 *   - Value -100 stops the left motor
 *   - Value +100 stops the right motor
 *   - Values less than -100 makes the left motor run the opposite
 *     direction of the right motor (Spin)
 *   - Values greater than +100 makes the right motor run the opposite
 *     direction of the left motor (Spin)
 *
 *  Example: opOUTPUT_TIME_SYNC(0, 10, 100, 50, 10000,1)
 *
 *  0     = Layer
 *  10    = Motor bit field - Motor B and D
 *  100   = Motor speed     - Motor B will run at speed 100 (because ratio is positive)
 *  10    = Turn ratio      - Motor D will run at speed 50
 *  10000 = time in mS      - Motors will run for 10 sec.
 *  1     = Brake bit       - When 10 sec. has elapsed then brake both motors
 *
 *
 *  Example: opOUTPUT_TIME_SYNC(0, 10, 100, 150, 10000,1)
 *
 *  0     = Layer
 *  10    = Motor bit field - Motor B and D
 *  100   = Motor speed     - Motor B will run at speed 100 (because ratio is positive)
 *  10    = Turn ratio      - Motor D will run at speed -50
 *  10000 = time in mS      - Motors will run for 10 sec.
 *  1     = Brake bit       - When 10 sec. has elapsed then brake both motors
 *
 *
 *  Example: opOUTPUT_TIME_SYNC(0, 10, 100, -50, 10000,1)
 *
 *  0     = Layer
 *  10    = Motor bit field - Motor B and D
 *  100   = Motor speed     - Motor B will run at speed  50 (because ratio is positive)
 *  10    = Turn ratio      - Motor D will run at speed 100
 *  10000 = time in mS      - Motors will run for 10 sec.
 *  1     = Brake bit       - When 10 sec. has elapsed then brake both motors
 *
 *
 *  Example: opOUTPUT_TIME_SYNC(0, 10, 200, -150, 10000,1)
 *
 *  0     = Layer
 *  10    = Motor bit field - Motor B and D
 *  100   = Motor speed     - Motor B will run at speed -50 (because ratio is positive)
 *  10    = Turn ratio      - Motor D will run at speed 100
 *  10000 = time in mS      - Motors will run for 10 sec.
 *  1     = Brake bit       - When 10 sec. has elapsed then brake both motors
 *
 *\

\endverbatim
 *
 */


#include  "lms2012.h"
#include  "c_output.h"
#ifndef    DISABLE_DAISYCHAIN_COM_CALL
  #include  "c_daisy.h"
#endif

#if (HARDWARE != SIMULATION)

  #include  <stdio.h>
  #include  <fcntl.h>
  #include  <stdlib.h>
  #include  <unistd.h>
  #include  <string.h>
  #include  <signal.h>
  #include  <sys/mman.h>

  OUTPUT_GLOBALS OutputInstance;
  #ifndef    DISABLE_DAISYCHAIN_COM_CALL
    static  DATA8  DaisyBuf[64];
  #endif


#else

  OUTPUT_GLOBALS * gOutputInstance;

  void setOutputInstance(OUTPUT_GLOBALS * _Instance)
  {
    gOutputInstance= _Instance;
  }

  OUTPUT_GLOBALS* getOutputInstance()
  {
    return gOutputInstance;
  }

#endif

#ifdef Linux_X86
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

uint DELAY_COUNTER = 0;
UBYTE	BusyOnes = 0;

void      OutputReset(void)
{
  UBYTE   Tmp;
  DATA8   StopArr[3];

  for(Tmp = 0; Tmp < OUTPUTS; Tmp++)
  {
    OutputInstance.Owner[Tmp] = 0;
  }

  StopArr[0] = (DATA8)opOUTPUT_STOP;
  StopArr[1] = 0x0F;
  StopArr[2] = 0x00;
  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,StopArr,3);
  }
}


RESULT    cOutputInit(void)
{
  RESULT      Result = FAIL;
  MOTORDATA   *pTmp;

  // To ensure that pMotor is never uninitialised
  OutputInstance.pMotor  =  OutputInstance.MotorData;

  // Open the handle for writing commands
  OutputInstance.PwmFile  =  open(PWM_DEVICE_NAME,O_RDWR);

  if (OutputInstance.PwmFile >= 0)
  {

    // Open the handle for reading motor values - shared memory
    OutputInstance.MotorFile  =  open(MOTOR_DEVICE_NAME,O_RDWR | O_SYNC);
    if (OutputInstance.MotorFile >= 0)
    {
      pTmp  =  (MOTORDATA*)mmap(0, sizeof(OutputInstance.MotorData), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, OutputInstance.MotorFile, 0);

      if (pTmp == MAP_FAILED)
      {
        LogErrorNumber(OUTPUT_SHARED_MEMORY);
        close(OutputInstance.MotorFile);
        close(OutputInstance.PwmFile);
      }
      else
      {
        OutputInstance.pMotor  =  (MOTORDATA*)pTmp;
        Result  =  cOutputOpen();
      }
    }
  }

  return (Result);
}


RESULT    cOutputOpen(void)
{
  RESULT  Result = FAIL;

  UBYTE   PrgStart =  opPROGRAM_START;

  OutputReset();

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,&PrgStart,1);
  }

  Result  =  OK;

  return (Result);
}


RESULT    cOutputClose(void)
{
  return (OK);
}


RESULT    cOutputExit(void)
{
  RESULT  Result = FAIL;

  OutputReset();

  if (OutputInstance.MotorFile >= 0)
  {
    munmap(OutputInstance.pMotor,sizeof(OutputInstance.MotorData));
    close(OutputInstance.MotorFile);
  }

  if (OutputInstance.PwmFile >= 0)
  {
    close(OutputInstance.PwmFile);
  }

  Result  =  OK;

  return (Result);
}


void    cOutputSetTypes(char *pTypes)
{
  DATA8   TypeArr[5];

  TypeArr[0] = opOUTPUT_SET_TYPE;
  memcpy(&TypeArr[1], pTypes, 4);

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,TypeArr,sizeof(TypeArr));
  }

}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b> UBYTE    cOutputPackParam (DATA32 Val, DATA8 *pStr)  </b>
 *
 *- Helper function to pack parameters - always to const parameters -
 *- This is to pack parameters back into a string ready for Daisy
 *- chain transmission
 *
 *  \param  (DATA8)   Val     - 32 bit value you would like to pack
 *  \param  (DATA8*)  pStr    - String pointer where to pack the 32
 *                              bit Val
 */
UBYTE      cOutputPackParam(DATA32 Val, DATA8 *pStr)
{
  DATA8   Len;

  Len = 0;
  if ((Val < 32) && (Val > -32))
  {
    pStr[Len] = (DATA8)(Val & 0x0000003F);
    Len++;
  }
  else
  {
    if ((Val < DATA8_MAX) && (Val > DATA8_MIN))
    {
      pStr[Len] = 0x81;
      Len++;
      pStr[Len] = (DATA8)Val;
      Len++;
    }
    else
    {
      if ((Val < DATA16_MAX) && (Val > DATA16_MIN))
      {
        pStr[Len] = 0x82;
        Len++;
        ((UBYTE*)pStr)[Len] = (UBYTE)(Val & 0x00FF);
        Len++;
        ((UBYTE*)pStr)[Len] = (UBYTE)((Val >> 8) & 0x00FF);
        Len++;
      }
      else
      {
        pStr[Len] = 0x83;
        Len++;
        ((UBYTE*)pStr)[Len] = (UBYTE)(Val & 0x000000FF);
        Len++;
        ((UBYTE*)pStr)[Len] = (UBYTE)((Val >>  8) & 0x000000FF);
        Len++;
        ((UBYTE*)pStr)[Len] = (UBYTE)((Val >> 16) & 0x000000FF);
        Len++;
        ((UBYTE*)pStr)[Len] = (UBYTE)((Val >> 24) & 0x000000FF);
        Len++;
      }
    }
  }
  return(Len);
}
/*

UBYTE     cMotorGetBusyFlags(void)
{
  int     test, test2;
  char    BusyReturn[10]; // Busy mask

  if (OutputInstance.PwmFile >= 0)
  {
    read(OutputInstance.PwmFile,BusyReturn,4);
    sscanf(BusyReturn,"%u %u",&test,&test2);
  }
  else
  {
    test = 0;
  }
  printf("cMotorGetBusyFlags test = %d\n\r", test);
  return(test);
}*/

void ResetDelayCounter(UBYTE Pattern)
{
	BusyOnes = Pattern;
	DELAY_COUNTER = 0;
}

UBYTE     cMotorGetBusyFlags(void)
{
  int     test, test2;
  char    BusyReturn[10]; // Busy mask

  if (OutputInstance.PwmFile >= 0)
  {
    read(OutputInstance.PwmFile,BusyReturn,4);
    sscanf(BusyReturn,"%u %u",&test,&test2);
  }
  else
  {
    test = 0;
  }
  if(DELAY_COUNTER < 25)
  {
	  test = BusyOnes;
	  DELAY_COUNTER++;
  }

  return(test);
}


void      cMotorSetBusyFlags(UBYTE Flags)
{
  if (OutputInstance.MotorFile >= 0)
  {
    write(OutputInstance.MotorFile, &Flags, sizeof(Flags));
  }
}



//******* BYTE CODE SNIPPETS **************************************************


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_PRG_STOP (LAYER, NO, TYPE)  </b>
 *
 *- Program stop\n
 *- Dispatch status unchanged
 *
 */
/*! \brief  opOUTPUT_PRG_STOP byte code
 *
 */
void      cOutputPrgStop(void)
{
  DSPSTAT DspStat = NOBREAK;
  DATA8   PrgStop;

  PrgStop = (DATA8)opPROGRAM_STOP;
  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,&PrgStop,1);
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_SET_TYPE (LAYER, NO, TYPE)  </b>
 *
 *- Set output type\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NO      - Output no [0..3]
 *  \param  (DATA8)   TYPE    - Output device type
 */
/*! \brief  opOUTPUT_SET_TYPE byte code
 *
 */
void      cOutputSetType(void)
{
  DATA8   Layer;
  DATA8   No;
  DATA8   Type;
  UBYTE   Len;
  DSPSTAT DspStat = NOBREAK;
  IP      TmpIp;

  TmpIp  =  GetObjectIp();
  Len    =  0;
  Layer  =  *(DATA8*)PrimParPointer();
  No     =  *(DATA8*)PrimParPointer();
  Type   =  *(DATA8*)PrimParPointer();

  if (Layer == 0)
  {
    if ((No >= 0) && (No < OUTPUTS))
    {
      if (OutputInstance.OutputType[No] != Type)
      {
        OutputInstance.OutputType[No]  =  Type;

        if ((Type == TYPE_NONE) || (Type == TYPE_ERROR))
        {
#ifdef DEBUG
          printf("                Output %c Disable\r\n",'A' + No);
#endif
        }
        else
        {
#ifdef DEBUG
          printf("                Output %c Enable\r\n",'A' + No);
#endif
        }
      }
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_RESET;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)No, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Type, &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_RESET (LAYER, NOS)  </b>
 *
 *- Resets the Tacho counts \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 */
/*! \brief  opOUTPUT_RESET byte code
 *
 */
void      cOutputReset(void)
{
  DATA8   Layer;
  UBYTE   Nos;
  UBYTE   Len;
  DSPSTAT DspStat = NOBREAK;
  IP      TmpIp;

  DATA8   ResetArr[2];

  TmpIp  =  GetObjectIp();
  Len    =  0;
  Layer  =  *(DATA8*)PrimParPointer();
  Nos    =  *(DATA8*)PrimParPointer();

  if (Layer == 0)
  {

    ResetArr[0] = opOUTPUT_RESET;
    ResetArr[1] = Nos;

    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,ResetArr,sizeof(ResetArr));
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_RESET;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Nos, &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_STOP (LAYER, NOS)  </b>
 *
 *- Stops the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 *  \param  (DATA8)   BRAKE   - Brake [0,1]
 */
/*! \brief  opOUTPUT_STOP byte code
 *
 */
void      cOutputStop(void)
{
  DATA8   Layer;
  UBYTE   Nos;
  UBYTE   Brake;
  UBYTE   Len;
  DSPSTAT DspStat = NOBREAK;
  IP      TmpIp;

  DATA8   StopArr[3];

  TmpIp  =  GetObjectIp();
  Len    =  0;
  Layer  =  *(DATA8*)PrimParPointer();
  Nos    =  *(DATA8*)PrimParPointer();
  Brake  =  *(DATA8*)PrimParPointer();

  if (Layer == 0)
  {
    StopArr[0] = (DATA8)opOUTPUT_STOP;
    StopArr[1] = Nos;
    StopArr[2] = Brake;

    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,StopArr,sizeof(StopArr));
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_STOP;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Nos, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Brake, &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_SPEED (LAYER, NOS, SPEED)  </b>
 *
 *- Set speed of the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 *  \param  (DATA8)   SPEED   - Speed [-100..100%]
 *
 *
 */
/*! \brief  opOUTPUT_SPEED byte code
 *
 */
void      cOutputSpeed(void)
{
  DATA8   Layer;
  UBYTE   Nos;
  DATA8   Speed;
  UBYTE   Len;
  DSPSTAT DspStat = NOBREAK;
  IP      TmpIp;

  DATA8   SetSpeed[3];

  TmpIp  =  GetObjectIp();
  Len    =  0;
  Layer  =  *(DATA8*)PrimParPointer();
  Nos    =  *(DATA8*)PrimParPointer();
  Speed  =  *(DATA8*)PrimParPointer();

  if (Layer == 0)
  {


    SetSpeed[0] = (DATA8)opOUTPUT_SPEED;
    SetSpeed[1] = Nos;
    SetSpeed[2] = Speed;

    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,SetSpeed,sizeof(SetSpeed));
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_SPEED;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Nos, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Speed, &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_POWER (LAYER, NOS, SPEED)  </b>
 *
 *- Set power of the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 *  \param  (DATA8)   POWER   - Power [-100..100%]
 */
/*! \brief  opOUTPUT_POWER byte code
 *
 */
void      cOutputPower(void)
{
  DATA8   Layer;
  UBYTE   Nos;
  DATA8   Power;
  DATA8   SetPower[3];
  DATA8   Len;
  DSPSTAT DspStat = NOBREAK;
  IP      TmpIp;

  TmpIp  =  GetObjectIp();
  Len    =  0;
  Layer  =  *(DATA8*)PrimParPointer();
  Nos    =  *(DATA8*)PrimParPointer();
  Power  =  *(DATA8*)PrimParPointer();

  if (Layer == 0)
  {
    SetPower[0] = (DATA8)opOUTPUT_POWER;
    SetPower[1] = Nos;
    SetPower[2] = Power;
    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,SetPower,sizeof(SetPower));
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_POWER;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Nos, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Power, &(DaisyBuf[Len]));

        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        else
        {
          // printf("cOutPut @ opOUTPUT_POWER after cDaisyDownStreamCmd - OK and WriteState = %d\n\r", cDaisyGetLastWriteState());
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_START (LAYER, NOS)  </b>
 *
 *- Starts the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 */
/*! \brief  opOUTPUT_START byte code
 *
 */
void      cOutputStart(void)
{
  DATA8   Tmp;
  DATA8   Layer;
  UBYTE   Nos;
  DATA8   Len;
  DSPSTAT DspStat = NOBREAK;
  IP      TmpIp;

  DATA8   StartMotor[2];

  TmpIp  =  GetObjectIp();
  Len    =  0;
  Layer  =  *(DATA8*)PrimParPointer();
  Nos    =  *(DATA8*)PrimParPointer();

  if (Layer == 0)
  {
    StartMotor[0] = (DATA8)opOUTPUT_START;
    StartMotor[1] = Nos;
    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,StartMotor,sizeof(StartMotor));

      for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
      {
        if (Nos & (0x01 << Tmp))
        {
          OutputInstance.Owner[Tmp] = CallingObjectId();
        }
      }
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_START;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Nos, &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        else
        {
          //printf("cOutPut @ opOUTPUT_START after cDaisyDownStreamCmd - OK and WriteState = %d\n\r", cDaisyGetLastWriteState());
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);

      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_POLARITY (LAYER, NOS, POL)  </b>
 *
 *- Set polarity of the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 *  \param  (DATA8)   POL     - Polarity [-1,0,1]
 *
 *-  Polarity:
 *   - -1 makes the motor run backward
 *   -  1 makes the motor run forward
 *   -  0 makes the motor run the opposite direction
 */
/*! \brief  opOUTPUT_POLARITY byte code
 *
 */
void      cOutputPolarity(void)
{
  DATA8   Layer;
  DATA8   Polarity[3];
  UBYTE   Len;
  DSPSTAT DspStat = NOBREAK;
  IP      TmpIp;

  TmpIp       =  GetObjectIp();
  Len         =  0;
  Layer       =  *(DATA8*)PrimParPointer();
  Polarity[0] =   (DATA8)opOUTPUT_POLARITY;
  Polarity[1] =  *(DATA8*)PrimParPointer();
  Polarity[2] =  *(DATA8*)PrimParPointer();

  if (Layer == 0)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,Polarity,sizeof(Polarity));
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_POLARITY;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Polarity[1], &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Polarity[2], &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_STEP_POWER (LAYER, NOS, POWER, STEP1, STEP2, STEP3, BRAKE)  </b>
 *
 *- Set Ramp up, constant and rampdown steps and power of the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 *  \param  (DATA8)   POWER   - Power [-100..100]
 *  \param  (DATA32)  STEP1   - Tacho pulses [0..MAX]
 *  \param  (DATA32)  STEP2   - Tacho pulses [0..MAX]
 *  \param  (DATA32)  STEP3   - Tacho pulses [0..MAX]
 *  \param  (DATA8)   BRAKE   - 0 = Coast, 1 = BRAKE
 */
/*! \brief  opOUTPUT_STEP_POWER byte code
 *
 */
void      cOutputStepPower(void)
{
  DATA8       Layer;
  DATA8       Tmp;
  STEPPOWER   StepPower;
  UBYTE       Len;
  DSPSTAT     DspStat = NOBREAK;
  IP          TmpIp;

  TmpIp            =  GetObjectIp();
  Len              =  0;
  Layer            =  *(DATA8*)PrimParPointer();
  StepPower.Cmd    =   opOUTPUT_STEP_POWER;
  StepPower.Nos    =  *(DATA8*)PrimParPointer();
  StepPower.Power  =  *(DATA8*)PrimParPointer();
  StepPower.Step1  =  *(DATA32*)PrimParPointer();
  StepPower.Step2  =  *(DATA32*)PrimParPointer();
  StepPower.Step3  =  *(DATA32*)PrimParPointer();
  StepPower.Brake  =  *(DATA8*)PrimParPointer();

  if (0 == Layer)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,(DATA8*)&(StepPower.Cmd),sizeof(StepPower));

      for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
      {
        // Set calling id for all involved inputs
        if (StepPower.Nos & (0x01 << Tmp))
        {
          OutputInstance.Owner[Tmp] = CallingObjectId();
        }
      }
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_STEP_POWER;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Power, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Step1, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Step2, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Step3, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Brake, &(DaisyBuf[Len]));

        //if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepPower.Nos))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }

        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepPower.Nos);

      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_TIME_POWER (LAYER, NOS, POWER, TIME1, TIME2, TIME3, BRAKE)  </b>
 *
 *- Set Ramp up, constant and rampdown steps and power of the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 *  \param  (DATA8)   POWER   - Power [-100..100]
 *  \param  (DATA32)  TIME1   - Time in Ms [0..MAX]
 *  \param  (DATA32)  TIME2   - Time in Ms [0..MAX]
 *  \param  (DATA32)  TIME3   - Time in Ms [0..MAX]
 *  \param  (DATA8)   BRAKE   - 0 = Coast, 1 = BRAKE
 */
/*! \brief  opOUTPUT_TIME_POWER byte code
 *
 */
void      cOutputTimePower(void)
{
  DATA8       Layer;
  DATA8       Tmp;
  TIMEPOWER   TimePower;
  UBYTE       Len;
  DSPSTAT     DspStat = NOBREAK;
  IP          TmpIp;

  TmpIp            =  GetObjectIp();
  Len              =  0;
  Layer            =  *(DATA8*) PrimParPointer();
  TimePower.Cmd    =  opOUTPUT_TIME_POWER;
  TimePower.Nos    =  *(DATA8*) PrimParPointer();
  TimePower.Power  =  *(DATA8*) PrimParPointer();
  TimePower.Time1  =  *(DATA32*)PrimParPointer();
  TimePower.Time2  =  *(DATA32*)PrimParPointer();
  TimePower.Time3  =  *(DATA32*)PrimParPointer();
  TimePower.Brake  =  *(DATA8*) PrimParPointer();

  if (0 == Layer)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,(DATA8*)&(TimePower.Cmd),sizeof(TimePower));

      for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
      {
        // Set calling id for all involved inputs
        if (TimePower.Nos & (0x01 << Tmp))
        {
          OutputInstance.Owner[Tmp] = CallingObjectId();
        }
      }
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_TIME_POWER;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Power, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Time1, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Time2, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Time3, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Brake, &(DaisyBuf[Len]));
        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimePower.Nos))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimePower.Nos);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}

/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_STEP_SPEED (LAYER, NOS, SPEED, STEP1, STEP2, STEP3, BRAKE)  </b>
 *
 *- Set Ramp up, constant and rampdown steps and power of the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 *  \param  (DATA8)   SPEED   - Power [-100..100]
 *  \param  (DATA32)  STEP1   - Tacho pulses [0..MAX]
 *  \param  (DATA32)  STEP2   - Tacho pulses [0..MAX]
 *  \param  (DATA32)  STEP3   - Tacho pulses [0..MAX]
 *  \param  (DATA8)   BRAKE   - 0 = Coast, 1 = BRAKE
 */
/*! \brief  opOUTPUT_STEP_SPEED byte code
 *
 */
void      cOutputStepSpeed(void)
{
  DATA8       Layer;
  DATA8       Tmp;
  STEPSPEED   StepSpeed;
  UBYTE       Len;
  DSPSTAT     DspStat = NOBREAK;
  IP          TmpIp;

  //DEBUG
//  int i;

  TmpIp            =  GetObjectIp();
  Len              =  0;
  Layer            =  *(DATA8*)PrimParPointer();
  StepSpeed.Cmd    =   opOUTPUT_STEP_SPEED;
  StepSpeed.Nos    =  *(DATA8*)PrimParPointer();
  StepSpeed.Speed  =  *(DATA8*)PrimParPointer();
  StepSpeed.Step1  =  *(DATA32*)PrimParPointer();
  StepSpeed.Step2  =  *(DATA32*)PrimParPointer();
  StepSpeed.Step3  =  *(DATA32*)PrimParPointer();
  StepSpeed.Brake  =  *(DATA8*)PrimParPointer();
/*
  printf("StepSpeed.Cmd = %d\n\r", StepSpeed.Cmd);
  printf("StepSpeed.Nos = %d\n\r", StepSpeed.Nos);
  printf("StepSpeed.Speed = %d\n\r", StepSpeed.Speed);
  printf("StepSpeed.Step1 = %d\n\r", StepSpeed.Step1);
  printf("StepSpeed.Step2 = %d\n\r", StepSpeed.Step2);
  printf("StepSpeed.Step3 = %d\n\r", StepSpeed.Step3);
  printf("StepSpeed.Brake = %d\n\r", StepSpeed.Brake);

*/
  if (0 == Layer)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,(DATA8*)&(StepSpeed.Cmd),sizeof(StepSpeed));

      for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
      {// Set calling id for all involved inputs

        if (StepSpeed.Nos & (0x01 << Tmp))
        {
          OutputInstance.Owner[Tmp] = CallingObjectId();
        }
      }
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_STEP_SPEED;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Speed, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Step1, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Step2, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Step3, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Brake, &(DaisyBuf[Len]));

       /* printf("Len = %d\n\r", Len);
        for(i = 0; i < Len; i++)
                            printf("DaisyBuf[%d]= %x\n\r", i, DaisyBuf[i]);
                          printf("\n\r");
*/

        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepSpeed.Nos))
        {
          printf("NOT ok txed cOutputStepSpeed\n\r");
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        else
        {
  /*        for(i = 0; i < Len; i++)
                    printf("DaisyBuf[%d]= %d, ", i, DaisyBuf[i]);
                  printf("\n\r");
*/
        }
        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepSpeed.Nos);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_TIME_SPEED (LAYER, NOS, SPEED, STEP1, STEP2, STEP3, BRAKE)  </b>
 *
 *- Set Ramp up, constant and rampdown steps and power of the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 *  \param  (DATA8)   SPEED   - Power [-100..100]
 *  \param  (DATA32)  STEP1   - Time in mS [0..MAX]
 *  \param  (DATA32)  STEP2   - Time in mS [0..MAX]
 *  \param  (DATA32)  STEP3   - Time in mS [0..MAX]
 *  \param  (DATA8)   BRAKE   - 0 = Coast, 1 = BRAKE
 */
/*! \brief  opOUTPUT_TIME_SPEED byte code
 *
 */
void      cOutputTimeSpeed(void)
{
  DATA8       Layer;
  DATA8       Tmp;
  TIMESPEED   TimeSpeed;
  UBYTE       Len;
  DSPSTAT     DspStat = NOBREAK;
  IP          TmpIp;

  TmpIp            =  GetObjectIp();
  Len              =  0;
  Layer            =  *(DATA8*)PrimParPointer();
  TimeSpeed.Cmd    =   (DATA8)opOUTPUT_TIME_SPEED;
  TimeSpeed.Nos    =  *(DATA8*)PrimParPointer();
  TimeSpeed.Speed  =  *(DATA8*)PrimParPointer();
  TimeSpeed.Time1  =  *(DATA32*)PrimParPointer();
  TimeSpeed.Time2  =  *(DATA32*)PrimParPointer();
  TimeSpeed.Time3  =  *(DATA32*)PrimParPointer();
  TimeSpeed.Brake  =  *(DATA8*)PrimParPointer();

  if (0 == Layer)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,(DATA8*)&(TimeSpeed.Cmd),sizeof(TimeSpeed));

      for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
      {
        // Set calling id for all involved inputs
        if (TimeSpeed.Nos & (0x01 << Tmp))
        {
          OutputInstance.Owner[Tmp] = CallingObjectId();
        }
      }
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_TIME_SPEED;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Speed, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Time1, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Time2, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Time3, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Brake, &(DaisyBuf[Len]));
        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimeSpeed.Nos))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimeSpeed.Nos);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_STEP_SYNC (LAYER, NOS, SPEED, TURN, STEP, BRAKE)  </b>
 *
 *- \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field   [0x00..0x0F]
 *  \param  (DATA8)   SPEED   - Power              [-100..100]
 *  \param  (DATA16)  TURN    - Turn Ratio         [-200..200]
 *  \param  (DATA32)  STEP    - Tacho Pulses       [0..MAX]
 *  \param  (DATA8)   BRAKE   - 0 = Coast, 1 = BRAKE
 */
/*! \brief  opOUTPUT_STEP_SYNC byte code
 *
 */
void      cOutputStepSync(void)
{
  DATA8       Layer;
  DATA8       Tmp;
  STEPSYNC    StepSync;
  UBYTE       Len;
  DSPSTAT     DspStat = NOBREAK;
  IP          TmpIp;

  TmpIp          =  GetObjectIp();
  Len            =  0;
  Layer          =  *(DATA8*)PrimParPointer();
  StepSync.Cmd   =   opOUTPUT_STEP_SYNC;
  StepSync.Nos   =  *(DATA8*)PrimParPointer();
  StepSync.Speed =  *(DATA8*)PrimParPointer();
  StepSync.Turn  =  *(DATA16*)PrimParPointer();
  StepSync.Step  =  *(DATA32*)PrimParPointer();
  StepSync.Brake =  *(DATA8*)PrimParPointer();

  if (0 == Layer)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,(DATA8*)&(StepSync.Cmd),sizeof(StepSync));

      for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
      {
        // Set calling id for all involved outputs
        if (StepSync.Nos & (0x01 << Tmp))
        {
          OutputInstance.Owner[Tmp] = CallingObjectId();
        }
      }
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_STEP_SYNC;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSync.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSync.Speed, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSync.Turn,  &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSync.Step,  &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSync.Brake, &(DaisyBuf[Len]));
        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepSync.Nos))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepSync.Nos);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_TIME_SYNC (LAYER, NOS, SPEED, TURN, STEP, BRAKE)  </b>
 *
 *- \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field   [0x00..0x0F]
 *  \param  (DATA8)   SPEED   - Power              [-100..100]
 *  \param  (DATA16)  TURN    - Turn Ratio         [-200..200]
 *  \param  (DATA32)  TIME    - Time in ms         [0..MAX]
 *  \param  (DATA8)   BRAKE   - 0 = Coast, 1 = BRAKE
 *
 */
/*! \brief  opOUTPUT_STEP_SYNC byte code
 *
 */
void      cOutputTimeSync(void)
{
  DATA8     Layer;
  DATA8     Tmp;
  TIMESYNC  TimeSync;
  UBYTE     Len;
  DSPSTAT   DspStat = NOBREAK;
  IP        TmpIp;

  TmpIp          =  GetObjectIp();
  Len            =  0;
  Layer          =  *(DATA8*)PrimParPointer();
  TimeSync.Cmd   =  opOUTPUT_TIME_SYNC;
  TimeSync.Nos   =  *(DATA8*)PrimParPointer();
  TimeSync.Speed =  *(DATA8*)PrimParPointer();
  TimeSync.Turn  =  *(DATA16*)PrimParPointer();
  TimeSync.Time  =  *(DATA32*)PrimParPointer();
  TimeSync.Brake =  *(DATA8*)PrimParPointer();

  if (0 == Layer)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile,(DATA8*)&(TimeSync.Cmd),sizeof(TimeSync));

      for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
      {
        // Set calling id for all involved outputs
        if (TimeSync.Nos & (0x01 << Tmp))
        {
          OutputInstance.Owner[Tmp] = CallingObjectId();
        }
      }
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_TIME_SYNC;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSync.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSync.Speed, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSync.Turn,  &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSync.Time,  &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSync.Brake, &(DaisyBuf[Len]));
        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimeSync.Nos))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimeSync.Nos);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page   cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_READ (LAYER, NO, *SPEED, *TACHO) </b>
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NO      - Output no [0..3]
 *  \param  (DATA8)   *SPEED  - Speed [-100..100]
 *  \param  (DATA32)  *TACHO  - Tacho pulses [-MAX .. +MAX]
 *
 */
/*! \brief  opOUTPUT_READ byte code
 *
 */
void      cOutputRead(void)
{

  DATA8   Layer;
  DATA8   No;
  DATA8   Speed = 0;
  DATA32  Tacho = 0;

  Layer  = *(DATA8*)PrimParPointer();
  No     = *(DATA8*)PrimParPointer();

  if (0 == Layer)
  {
    if (No < OUTPUTS)
    {
      Speed  =  OutputInstance.pMotor[No].Speed;
      Tacho  =  OutputInstance.pMotor[No].TachoCounts;
    }
  }
  *(DATA8*)PrimParPointer()   =  Speed;
  *(DATA32*)PrimParPointer()  =  Tacho;
}



/*! \page   cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_READY (LAYER, NOS) </b>
 *
 *- Wait for output ready (wait for completion)\n
 *- Dispatch status can change to BUSYBREAK
 *- cOUTPUT_START command has no effect on this command
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 *
 *
 */
/*! \brief  opOUTPUT_READY byte code
 *
 */
void      cOutputReady(void)
{

  OBJID   Owner;
  DATA8   Layer, Tmp, Nos;
  IP      TmpIp;
  DSPSTAT DspStat = NOBREAK;
  UBYTE   Bits;

  int     test;
  int     test2;

  char    BusyReturn[10]; // Busy mask

  TmpIp  =  GetObjectIp();

  Layer  = *(DATA8*)PrimParPointer();
  Nos    = *(DATA8*)PrimParPointer();
  Owner  =  CallingObjectId();

  if (0 == Layer)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      read(OutputInstance.PwmFile,BusyReturn,4);

      sscanf(BusyReturn,"%u %u",&test,&test2);

      for (Tmp = 0;(Tmp < OUTPUTS) && (DspStat == NOBREAK);Tmp++)
      {
        // Check resources for NOTREADY
        if (Nos & (1 << Tmp))
        {
          // Only relevant ones
          if (test & (1 << Tmp))
          {
            // If BUSY check for OVERRULED
            if (OutputInstance.Owner[Tmp] == Owner)
            {
              DspStat  =  BUSYBREAK;
            }
          }
        }
      }
    }
  }
  else
  {
    Bits = cDaisyCheckBusyBit(Layer, Nos);
    Bits = 0;

    for (Tmp = 0;(Tmp < OUTPUTS) && (DspStat == NOBREAK);Tmp++)
    {
      // Check resources for NOTREADY
      if (Nos & (1 << Tmp))
      {
        // Only relevant ones
        if (Bits & (1 << Tmp))
        {
          // If BUSY check for OVERRULED
          if (OutputInstance.Owner[Tmp] == Owner)
          {
            DspStat  =  BUSYBREAK;
          }
        }
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


/*! \page   cOutput
 *  \anchor opOUTPUT_TEST \n
 *  <hr size="1"/>
 *  <b>     opOUTPUT_TEST (LAYER, NOS, BUSY) </b>
 *
 *- Testing if output is not used \n
 *
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 *  \return (DATA8)   BUSY    - Output busy flag (0 = ready, 1 = Busy)
 */
/*! \brief  opOUTPUT_TEST byte code
 *
 */
void      cOutputTest(void)
{

  DATA8   Layer, Nos, Busy = 0;

  int     test;
  int     test2;

  char    BusyReturn[20]; // Busy mask

  Layer  = *(DATA8*)PrimParPointer();
  Nos    = *(DATA8*)PrimParPointer();

  if (0 == Layer)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      read(OutputInstance.PwmFile,BusyReturn,10);
      sscanf(BusyReturn,"%u %u",&test,&test2);

      if (Nos & (DATA8)test2)
      {
        Busy = 1;
      }
    }
  }
  else
  {
    if (cDaisyCheckBusyBit(Layer, Nos))
    {
      Busy = 1;
    }
  }
  *(DATA8*)PrimParPointer() = Busy;
}


/*! \page   cOutput
 *  \anchor opOUTPUT_TEST \n
 *  <hr size="1"/>
 *  <b>     opOUTPUT_CLR_COUNT (LAYER, NOS) </b>
 *
 *- Clearing tacho count when used as sensor \n
 *
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output bit field [0x00..0x0F]
 */
/*! \brief  opOUTPUT_CLR_COUNT byte code
 *
 */
void      cOutputClrCount(void)
{
  DATA8   Layer;
  DATA8   ClrCnt[2];
  UBYTE   Len;
  DSPSTAT DspStat = NOBREAK;
  IP      TmpIp;
  UBYTE   Tmp;

  TmpIp       =  GetObjectIp();
  Len         =  0;
  Layer       =  *(DATA8*)PrimParPointer();
  ClrCnt[0]   =  opOUTPUT_CLR_COUNT;
  ClrCnt[1]   =  *(DATA8*)PrimParPointer();

  if (0 == Layer)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      write(OutputInstance.PwmFile, ClrCnt, sizeof(ClrCnt));
    }

    // Also the user layer entry to get immediate clear
    for(Tmp = 0; Tmp < OUTPUTS; Tmp++)
    {
      if (ClrCnt[1] & (1 << Tmp))
      {
        OutputInstance.pMotor[Tmp].TachoSensor = 0;
      }
    }
  }
  else
  {
    #ifndef    DISABLE_DAISYCHAIN_COM_CALL
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_CLR_COUNT;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)ClrCnt[1], &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
    #endif
  }
  SetDispatchStatus(DspStat);
}


/*! \page   cOutput
 *  \anchor opOUTPUT_TEST \n
 *  <hr size="1"/>
 *  <b>     opOUTPUT_GET_COUNT (LAYER, NOS, *TACHO) </b>
 *
 *- Getting tacho count when used as sensor - values are in shared memory \n
 *
 *
 *  \param  (DATA8)   LAYER   - Chain layer number [0..3]
 *  \param  (DATA8)   NOS     - Output number [0x00..0x0F]
 *  \param  (DATA32)  *TACHO  - Tacho pulses [-MAX .. +MAX]
 */
/*! \brief  opOUTPUT_GET_COUNT byte code
 *
 */
void      cOutputGetCount(void)
{
  DATA8   Layer;
  DATA8   No;
  DATA32  Tacho;

  Layer  = *(DATA8*)PrimParPointer();
  No     = *(DATA8*)PrimParPointer();

  if (0 == Layer)
  {
    if (No < OUTPUTS)
    {
      Tacho  =  OutputInstance.pMotor[No].TachoSensor;
    }
  }
  *(DATA32*)PrimParPointer()  =  Tacho;
}


//*****************************************************************************
