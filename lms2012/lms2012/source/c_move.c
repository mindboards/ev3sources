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


#include  <math.h>
#include  "lms2012.h"
#include  "c_move.h"


//******* BYTE CODE SNIPPETS **************************************************

/*! \page cMove Move
 *  <hr size="1"/>
 *  <b>     opINIT_BYTES (DESTINATION, LENGTH, SOURCE)  </b>
 *
 *- Move LENGTH number of DATA8 from BYTE STREAM to memory DESTINATION START\n
 *- Dispatch status unchanged
 *
 *  \return (DATA8)   DESTINATION   - First element in DATA8 array to be initiated
 *  \param  (DATA32)  LENGTH        - Number of elements to initiate
 *  \param  (DATA8)   SOURCE        - First element to initiate DATA8 array with
 */
/*! \brief  opINIT_BYTES
 *
 */
void      cMoveInitBytes(void)
{
  DATA8   *pDestination;
  DATA32  Length;

  pDestination  =  (DATA8*)PrimParPointer();
  Length        = *(DATA32*)PrimParPointer();

  while (Length)
  {
    *pDestination  =  *(DATA8*)PrimParPointer();
    pDestination++;
    Length--;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE8_8 (SOURCE, DESTINATION)  </b>
 *
 *- Move 8 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opMOVE8_8
 *
 */
void      cMove8to8(void)
{
  DATA8   Tmp;

  Tmp  =  *(DATA8*)PrimParPointer();
  *(DATA8*)PrimParPointer()  =  Tmp;
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE8_16 (SOURCE, DESTINATION)  </b>
 *
 *- Move 8 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opMOVE8_16
 *
 */
void      cMove8to16(void)
{
  DATA8   Tmp;

  Tmp  =  *(DATA8*)PrimParPointer();

  if (Tmp != DATA8_NAN)
  {
    *(DATA16*)PrimParPointer()  =  (DATA16)Tmp;
  }
  else
  {
    *(DATA16*)PrimParPointer()  =  DATA16_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE8_32 (SOURCE, DESTINATION)  </b>
 *
 *- Move 8 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opMOVE8_32
 *
 */
void      cMove8to32(void)
{
  DATA8   Tmp;

  Tmp  =  *(DATA8*)PrimParPointer();
  if (Tmp != DATA8_NAN)
  {
    *(DATA32*)PrimParPointer()  =  (DATA32)Tmp;
  }
  else
  {
    *(DATA32*)PrimParPointer()  =  DATA32_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE8_F (SOURCE, DESTINATION)  </b>
 *
 *- Move 8 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE
 *  \return (DATAF)   DESTINATION
 */
/*! \brief  opMOVE8_F
 *
 */
void      cMove8toF(void)
{
  DATA8   Tmp;

  Tmp  =  *(DATA8*)PrimParPointer();
  if (Tmp != DATA8_NAN)
  {
    *(DATAF*)PrimParPointer()  =  (DATAF)Tmp;
  }
  else
  {
    *(DATAF*)PrimParPointer()  =  DATAF_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE16_8 (SOURCE, DESTINATION)  </b>
 *
 *- Move 16 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opMOVE16_8
 *
 */
void      cMove16to8(void)
{
  DATA16  Tmp;

  Tmp  =  *(DATA16*)PrimParPointer();
  if (Tmp != DATA16_NAN)
  {
    if (Tmp > (DATA16)DATA8_MAX)
    {
      Tmp  =  (DATA16)DATA8_MAX;
    }
    if (Tmp < (DATA16)DATA8_MIN)
    {
      Tmp  =  (DATA16)DATA8_MIN;
    }
    *(DATA8*)PrimParPointer()  =  (DATA8)Tmp;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  DATA8_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE16_16 (SOURCE, DESTINATION)  </b>
 *
 *- Move 16 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opMOVE16_16
 *
 */
void      cMove16to16(void)
{
  DATA16  Tmp;

  Tmp  =  *(DATA16*)PrimParPointer();
  *(DATA16*)PrimParPointer()  =  Tmp;
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE16_32 (SOURCE, DESTINATION)  </b>
 *
 *- Move 16 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opMOVE16_32
 *
 */
void      cMove16to32(void)
{
  DATA16  Tmp;

  Tmp  =  *(DATA16*)PrimParPointer();
  if (Tmp != DATA16_NAN)
  {
    *(DATA32*)PrimParPointer()  =  (DATA32)Tmp;
  }
  else
  {
    *(DATA32*)PrimParPointer()  =  DATA32_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE16_F (SOURCE, DESTINATION)  </b>
 *
 *- Move 16 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE
 *  \return (DATAF)   DESTINATION
 */
/*! \brief  opMOVE16_F
 *
 */
void      cMove16toF(void)
{
  DATA16  Tmp;

  Tmp  =  *(DATA16*)PrimParPointer();
  if (Tmp != DATA16_NAN)
  {
    *(DATAF*)PrimParPointer()  =  (DATAF)Tmp;
  }
  else
  {
    *(DATAF*)PrimParPointer()  =  DATAF_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE32_8 (SOURCE, DESTINATION)  </b>
 *
 *- Move 32 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opMOVE32_8
 *
 */
void      cMove32to8(void)
{
  DATA32  Tmp;

  Tmp  =  *(DATA32*)PrimParPointer();
  if (Tmp != DATA32_NAN)
  {
    if (Tmp > (DATA32)DATA8_MAX)
    {
      Tmp  =  (DATA32)DATA8_MAX;
    }
    if (Tmp < (DATA32)DATA8_MIN)
    {
      Tmp  =  (DATA32)DATA8_MIN;
    }
    *(DATA8*)PrimParPointer()  =  (DATA8)Tmp;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  DATA8_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE32_16 (SOURCE, DESTINATION)  </b>
 *
 *- Move 32 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opMOVE32_16
 *
 */
void      cMove32to16(void)
{
  DATA32  Tmp;

  Tmp  =  *(DATA32*)PrimParPointer();
  if (Tmp != DATA32_NAN)
  {
    if (Tmp > (DATA32)DATA16_MAX)
    {
      Tmp  =  (DATA32)DATA16_MAX;
    }
    if (Tmp < (DATA32)DATA16_MIN)
    {
      Tmp  =  (DATA32)DATA16_MIN;
    }
    *(DATA16*)PrimParPointer()  =  (DATA16)Tmp;
  }
  else
  {
    *(DATA16*)PrimParPointer()  =  DATA16_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE32_32 (SOURCE, DESTINATION)  </b>
 *
 *- Move 32 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opMOVE32_32
 *
 */
void      cMove32to32(void)
{
  DATA32  Tmp;

  Tmp  =  *(DATA32*)PrimParPointer();
  *(DATA32*)PrimParPointer()  =  Tmp;
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVE32_F (SOURCE, DESTINATION)  </b>
 *
 *- Move 32 bit value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE
 *  \return (DATAF)   DESTINATION
 */
/*! \brief  opMOVE32_F
 *
 */
void      cMove32toF(void)
{
  DATA32  Tmp;

  Tmp  =  *(DATA32*)PrimParPointer();
  if (Tmp != DATA32_NAN)
  {
    *(DATAF*)PrimParPointer()  =  (DATAF)Tmp;
  }
  else
  {
    *(DATAF*)PrimParPointer()  =  DATAF_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVEF_8 (SOURCE, DESTINATION)  </b>
 *
 *- Move floating point value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   SOURCE
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opMOVEF_8
 *
 */
void      cMoveFto8(void)
{
  DATAF   Tmp;

  Tmp  =  *(DATAF*)PrimParPointer();
  if (!(isnan(Tmp)))
  {
    if (Tmp > (DATAF)DATA8_MAX)
    {
      Tmp  =  (DATAF)DATA8_MAX;
    }
    if (Tmp < (DATAF)DATA8_MIN)
    {
      Tmp  =  (DATAF)DATA8_MIN;
    }
    *(DATA8*)PrimParPointer()  =  (DATA8)Tmp;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  DATA8_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVEF_16 (SOURCE, DESTINATION)  </b>
 *
 *- Move floating point value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   SOURCE
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opMOVEF_16
 *
 */
void      cMoveFto16(void)
{
  DATAF   Tmp;

  Tmp  =  *(DATAF*)PrimParPointer();
  if (!(isnan(Tmp)))
  {
    if (Tmp > (DATAF)DATA16_MAX)
    {
      Tmp  =  (DATAF)DATA16_MAX;
    }
    if (Tmp < (DATAF)DATA16_MIN)
    {
      Tmp  =  (DATAF)DATA16_MIN;
    }
    *(DATA16*)PrimParPointer()  =  (DATA16)Tmp;
  }
  else
  {
    *(DATA16*)PrimParPointer()  =  DATA16_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVEF_32 (SOURCE, DESTINATION)  </b>
 *
 *- Move floating point value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   SOURCE
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opMOVEF_32
 *
 */
void      cMoveFto32(void)
{
  DATAF   Tmp;

  Tmp  =  *(DATAF*)PrimParPointer();
  if (!(isnan(Tmp)))
  {
    if (Tmp > (DATAF)DATA32_MAX)
    {
      Tmp  =  (DATAF)DATA32_MAX;
    }
    if (Tmp < (DATAF)DATA32_MIN)
    {
      Tmp  =  (DATAF)DATA32_MIN;
    }
    *(DATA32*)PrimParPointer()  =  (DATA32)Tmp;
  }
  else
  {
    *(DATA32*)PrimParPointer()  =  DATA32_NAN;
  }
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opMOVEF_F (SOURCE, DESTINATION)  </b>
 *
 *- Move floating point value from SOURCE to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   SOURCE
 *  \return (DATAF)   DESTINATION
 */
/*! \brief  opMOVEF_F
 *
 */
void      cMoveFtoF(void)
{
  DATAF   Tmp;

  Tmp  =  *(DATAF*)PrimParPointer();
  *(DATAF*)PrimParPointer()  =  Tmp;
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opREAD8 (SOURCE, INDEX, DESTINATION)  </b>
 *
 *- Read 8 bit value from SOURCE[INDEX] to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE      - First value in array of values\n
 *  \param  (DATA8)   INDEX       - Index to array member to read\n
 *  \return (DATA8)   DESTINATION - Variable to receive read value\n
 */
/*! \brief  opREAD8
 *
 */
void      cMoveRead8(void)
{
  DATA8   *pTmp;
  DATA8   Index;

  pTmp    =  (DATA8*)PrimParPointer();
  Index   = *(DATA8*)PrimParPointer();
  *(DATA8*)PrimParPointer()  =  pTmp[Index];
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opREAD16 (SOURCE, INDEX, DESTINATION)  </b>
 *
 *- Read 16 bit value from SOURCE[INDEX] to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE      - First value in array of values\n
 *  \param  (DATA8)   INDEX       - Index to array member to read\n
 *  \return (DATA16)  DESTINATION - Variable to receive read value\n
 */
/*! \brief  opREAD16
 *
 */
void      cMoveRead16(void)
{
  DATA16  *pTmp;
  DATA8   Index;

  pTmp    =  (DATA16*)PrimParPointer();
  Index   = *(DATA8*)PrimParPointer();
  *(DATA16*)PrimParPointer()  =  pTmp[Index];
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opREAD32 (SOURCE, INDEX, DESTINATION)  </b>
 *
 *- Read 32 bit value from SOURCE[INDEX] to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE      - First value in array of values\n
 *  \param  (DATA8)   INDEX       - Index to array member to read\n
 *  \return (DATA32)  DESTINATION - Variable to receive read value\n
 */
/*! \brief  opREAD32
 *
 */
void      cMoveRead32(void)
{
  DATA32  *pTmp;
  DATA8   Index;

  pTmp    =  (DATA32*)PrimParPointer();
  Index   = *(DATA8*)PrimParPointer();
  *(DATA32*)PrimParPointer()  =  pTmp[Index];
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opREADF (SOURCE, INDEX, DESTINATION)  </b>
 *
 *- Read floating point value from SOURCE[INDEX] to DESTINATION\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   SOURCE      - First value in array of values\n
 *  \param  (DATA8)   INDEX       - Index to array member to read\n
 *  \return (DATAF)   DESTINATION - Variable to receive read value\n
 */
/*! \brief  opREADF
 *
 */
void      cMoveReadF(void)
{
  DATAF   *pTmp;
  DATA8   Index;

  pTmp    =  (DATAF*)PrimParPointer();
  Index   = *(DATA8*)PrimParPointer();
  *(DATAF*)PrimParPointer()  =  pTmp[Index];
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opWRITE8 (SOURCE, INDEX, DESTINATION)  </b>
 *
 *- Write 8 bit value from SOURCE to DESTINATION[INDEX]\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE      - Variable to write\n
 *  \param  (DATA8)   INDEX       - Index to array member to write\n
 *  \param  (DATA8)   DESTINATION - Array to receive write value\n
 */
/*! \brief  opWRITE8
 *
 */
void      cMoveWrite8(void)
{
  DATA8   Tmp;
  DATA8   *pTmp;
  DATA8   Index;

  Tmp     = *(DATA8*)PrimParPointer();
  Index   = *(DATA8*)PrimParPointer();
  pTmp    =  (DATA8*)PrimParPointer();
  pTmp[Index]  =  Tmp;
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opWRITE16 (SOURCE, INDEX, DESTINATION)  </b>
 *
 *- Write 16 bit value from SOURCE to DESTINATION[INDEX]\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE      - Variable to write\n
 *  \param  (DATA8)   INDEX       - Index to array member to write\n
 *  \param  (DATA16)  DESTINATION - Array to receive write value\n
 */
/*! \brief  opWRITE16
 *
 */
void      cMoveWrite16(void)
{
  DATA16  Tmp;
  DATA16  *pTmp;
  DATA8   Index;

  Tmp     = *(DATA16*)PrimParPointer();
  Index   = *(DATA8*)PrimParPointer();
  pTmp    =  (DATA16*)PrimParPointer();
  pTmp[Index]  =  Tmp;
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opWRITE32 (SOURCE, INDEX, DESTINATION)  </b>
 *
 *- Write 32 bit value from SOURCE to DESTINATION[INDEX]\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE      - Variable to write\n
 *  \param  (DATA8)   INDEX       - Index to array member to write\n
 *  \param  (DATA32)  DESTINATION - Array to receive write value\n
 */
/*! \brief  opWRITE32
 *
 */
void      cMoveWrite32(void)
{
  DATA32  Tmp;
  DATA32  *pTmp;
  DATA8   Index;

  Tmp     = *(DATA32*)PrimParPointer();
  Index   = *(DATA8*)PrimParPointer();
  pTmp    =  (DATA32*)PrimParPointer();
  pTmp[Index]  =  Tmp;
}


/*! \page cMove
 *  <hr size="1"/>
 *  <b>     opWRITEF (SOURCE, INDEX, DESTINATION)  </b>
 *
 *- Write floating point value from SOURCE to DESTINATION[INDEX]\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   SOURCE      - Variable to write\n
 *  \param  (DATA8)   INDEX       - Index to array member to write\n
 *  \param  (DATAF)   DESTINATION - Array to receive write value\n
 */
/*! \brief  opWRITEF
 *
 */
void      cMoveWriteF(void)
{
  DATAF   Tmp;
  DATAF   *pTmp;
  DATA8   Index;

  Tmp     = *(DATAF*)PrimParPointer();
  Index   = *(DATA8*)PrimParPointer();
  pTmp    =  (DATAF*)PrimParPointer();
  pTmp[Index]  =  Tmp;
}


//*****************************************************************************
