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


#include  "lms2012.h"
#include  "c_timer.h"

#if       (HARDWARE != SIMULATION)

#include  <time.h>
#include  <sys/time.h>


#endif

#if (HARDWARE != SIMULATION)


ULONG     cTimerGetmS(void)
{
  ULONG   Result;
  struct  timeval tv;

  gettimeofday(&tv,NULL);
  Result  =  (ULONG)tv.tv_sec * 1000;
  Result +=  (ULONG)tv.tv_usec / 1000;

  return(Result);
}


ULONG     cTimerGetuS(void)
{
  struct  timeval tv;

  gettimeofday(&tv,NULL);
  VMInstance.TimeuS  =  (ULONG)tv.tv_sec * 1000000;
  VMInstance.TimeuS +=  (ULONG)tv.tv_usec;

  return(VMInstance.TimeuS);
}

#endif

//******* BYTE CODE SNIPPETS **************************************************



/*! \page cTimer Timer
 *  <hr size="1"/>
 *  <b>     opTIMER_WAIT (TIME, TIMER)  </b>
 *
 *- Setup timer to wait TIME mS\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  TIME    - Time to wait [mS]
 *  \param  (DATA32)  TIMER   - Variable used for timing
 */
/*! \brief  opTIMER_WAIT byte code
 *
 */
void      cTimerWait(void)
{
  ULONG   Time;

  Time  =  *(ULONG*)PrimParPointer();

  *(ULONG*)PrimParPointer()  =  cTimerGetmS() + Time;
}


/*! \page cTimer
 *  <hr size="1"/>
 *  <b>     opTIMER_READY (TIMER) </b>
 *
 *- Wait for timer ready (wait for timeout)\n
 *- Dispatch status can change to BUSYBREAK
 *
 *  \param  (DATA32)  TIMER   - Variable used for timing
 */
/*! \brief  opTIMER_READY byte code
 *
 */
void      cTimerReady(void)
{
  IP      TmpIp;
  DSPSTAT DspStat = BUSYBREAK;

  TmpIp   =  GetObjectIp();

  if (*(ULONG*)PrimParPointer() <= cTimerGetmS())
  {
    DspStat  =  NOBREAK;
  }
  if (DspStat == BUSYBREAK)
  { // Rewind IP

    SetObjectIp(TmpIp - 1);
  }
  SetDispatchStatus(DspStat);

}


/*! \page cTimer
 *  <hr size="1"/>
 *  <b>     opTIMER_READ (TIME) </b>
 *
 *- Read free running timer [mS]\n
 *- Dispatch status unchanged
 *
 *  \return (DATA32)  TIME    - Value
 */
/*! \brief  opTIMER_READ byte code
 *
 */
void      cTimerRead(void)
{
  *(DATA32*)PrimParPointer()  =  (DATA32)(cTimerGetmS() - VMInstance.Program[CurrentProgramId()].StartTime);
}


/*! \page cTimer
 *  <hr size="1"/>
 *  <b>     opTIMER_READ_US (TIME) </b>
 *
 *- Read free running timer [uS]\n
 *- Dispatch status unchanged
 *
 *  \return (DATA32)  TIME    - Value
 */
/*! \brief  opTIMER_READ_US byte code
 *
 */
void      cTimerReaduS(void)
{
  *(DATA32*)PrimParPointer()  =  (DATA32)cTimerGetuS();
}


//*****************************************************************************


