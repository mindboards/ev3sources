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
#include  "c_math.h"
#include  <math.h>
#include  <stdio.h>
#include  <string.h>


//******* BYTE CODE SNIPPETS **************************************************

/*! \page cMath Math
 *  <hr size="1"/>
 *  <b>     opADD8 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Add two 8 bit values DESTINATION = SOURCE1 + SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE1
 *  \param  (DATA8)   SOURCE2
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opADD8
 *
 *
 */
void      cMathAdd8(void)
{
  DATA8   Tmp;

  Tmp  =  *(DATA8*)PrimParPointer();
  Tmp +=  *(DATA8*)PrimParPointer();
  *(DATA8*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opADD16 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Add two 16 bit values DESTINATION = SOURCE1 + SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE1
 *  \param  (DATA16)  SOURCE2
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opADD16
 *
 *
 */
void      cMathAdd16(void)
{
  DATA16  Tmp;

  Tmp  =  *(DATA16*)PrimParPointer();
  Tmp +=  *(DATA16*)PrimParPointer();
  *(DATA16*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opADD32 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Add two 32 bit values DESTINATION = SOURCE1 + SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE1
 *  \param  (DATA32)  SOURCE2
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opADD32
 *
 *
 */
void      cMathAdd32(void)
{
  DATA32  Tmp;

  Tmp  =  *(DATA32*)PrimParPointer();
  Tmp +=  *(DATA32*)PrimParPointer();
  *(DATA32*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opADDF (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Add two floating point values DESTINATION = SOURCE1 + SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   SOURCE1
 *  \param  (DATAF)   SOURCE2
 *  \return (DATAF)   DESTINATION
 */
/*! \brief  opADDF
 *
 *
 */
void      cMathAddF(void)
{
  DATAF   Tmp;

  Tmp  =  *(DATAF*)PrimParPointer();
  Tmp +=  *(DATAF*)PrimParPointer();
  *(DATAF*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opSUB8 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Subtract two 8 bit values DESTINATION = SOURCE1 - SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE1
 *  \param  (DATA8)   SOURCE2
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opSUB8
 *
 *
 */
void      cMathSub8(void)
{
  DATA8   Tmp;

  Tmp  =  *(DATA8*)PrimParPointer();
  Tmp -=  *(DATA8*)PrimParPointer();
  *(DATA8*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opSUB16 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Subtract two 16 bit values DESTINATION = SOURCE1 - SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE1
 *  \param  (DATA16)  SOURCE2
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opSUB16
 *
 *
 */
void      cMathSub16(void)
{
  DATA16  Tmp;

  Tmp  =  *(DATA16*)PrimParPointer();
  Tmp -=  *(DATA16*)PrimParPointer();
  *(DATA16*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opSUB32 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Subtract two 32 bit values DESTINATION = SOURCE1 - SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE1
 *  \param  (DATA32)  SOURCE2
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opSUB32
 *
 *
 */
void      cMathSub32(void)
{
  DATA32  Tmp;

  Tmp  =  *(DATA32*)PrimParPointer();
  Tmp -=  *(DATA32*)PrimParPointer();
  *(DATA32*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opSUBF (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Subtract two floating point values DESTINATION = SOURCE1 - SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   SOURCE1
 *  \param  (DATAF)   SOURCE2
 *  \return (DATAF)   DESTINATION
 */
/*! \brief  opSUBF
 *
 *
 */
void      cMathSubF(void)
{
  DATAF   Tmp;

  Tmp  =  *(DATAF*)PrimParPointer();
  Tmp -=  *(DATAF*)PrimParPointer();
  *(DATAF*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opMUL8 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Multiply two 8 bit values DESTINATION = SOURCE1 * SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE1
 *  \param  (DATA8)   SOURCE2
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opMUL8
 *
 *
 */
void      cMathMul8(void)
{
  DATA8   Tmp;

  Tmp  =  *(DATA8*)PrimParPointer();
  Tmp *=  *(DATA8*)PrimParPointer();
  *(DATA8*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opMUL16 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Multiply two 16 bit values DESTINATION = SOURCE1 * SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE1
 *  \param  (DATA16)  SOURCE2
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opMUL16
 *
 *
 */
void      cMathMul16(void)
{
  DATA16  Tmp;

  Tmp  =  *(DATA16*)PrimParPointer();
  Tmp *=  *(DATA16*)PrimParPointer();
  *(DATA16*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opMUL32 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Multiply two 32 bit values DESTINATION = SOURCE1 * SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE1
 *  \param  (DATA32)  SOURCE2
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opMUL32
 *
 *
 */
void      cMathMul32(void)
{
  DATA32  Tmp;

  Tmp  =  *(DATA32*)PrimParPointer();
  Tmp *=  *(DATA32*)PrimParPointer();
  *(DATA32*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opMULF (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Multiply two floating point values DESTINATION = SOURCE1 * SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   SOURCE1
 *  \param  (DATAF)   SOURCE2
 *  \return (DATAF)   DESTINATION
 */
/*! \brief  opMULF
 *
 *
 */
void      cMathMulF(void)
{
  DATAF   Tmp;

  Tmp  =  *(DATAF*)PrimParPointer();
  Tmp *=  *(DATAF*)PrimParPointer();
  *(DATAF*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opDIV8 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Divide two 8 bit values DESTINATION = SOURCE1 / SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE1
 *  \param  (DATA8)   SOURCE2
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opDIV8
 *
 *
 */
void      cMathDiv8(void)
{
  DATA8   X,Y;

  X  =  *(DATA8*)PrimParPointer();
  Y  =  *(DATA8*)PrimParPointer();
  if (Y == 0)
  {
    if (X > 0)
    {
      X  =  (DATA8)DATA8_MAX;
    }
    if (X < 0)
    {
      X  =  (DATA8)DATA8_MIN;
    }
  }
  else
  {
    X /=  Y;
  }
  *(DATA8*)PrimParPointer()  =  X;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opDIV16 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Divide two 16 bit values DESTINATION = SOURCE1 / SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE1
 *  \param  (DATA16)  SOURCE2
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opDIV16
 *
 *
 */
void      cMathDiv16(void)
{
  DATA16  X,Y;

  X  =  *(DATA16*)PrimParPointer();
  Y  =  *(DATA16*)PrimParPointer();
  if (Y == 0)
  {
    if (X > 0)
    {
      X  =  (DATA16)DATA16_MAX;
    }
    if (X < 0)
    {
      X  =  (DATA16)DATA16_MIN;
    }
  }
  else
  {
    X /=  Y;
  }
  *(DATA16*)PrimParPointer()  =  X;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opDIV32 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Divide two 32 bit values DESTINATION = SOURCE1 / SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE1
 *  \param  (DATA32)  SOURCE2
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opDIV32
 *
 *
 */
void      cMathDiv32(void)
{
  DATA32  X,Y;

  X  =  *(DATA32*)PrimParPointer();
  Y  =  *(DATA32*)PrimParPointer();
  if (Y == 0)
  {
    if (X > 0)
    {
      X  =  (DATA32)DATA32_MAX;
    }
    if (X < 0)
    {
      X  =  (DATA32)DATA32_MIN;
    }
  }
  else
  {
    X /=  Y;
  }
  *(DATA32*)PrimParPointer()  =  X;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opDIVF (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Divide two floating point values DESTINATION = SOURCE1 / SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   SOURCE1
 *  \param  (DATAF)   SOURCE2
 *  \return (DATAF)   DESTINATION
 */
/*! \brief  opDIVF
 *
 *
 */
void      cMathDivF(void)
{
  DATAF   X,Y;

  X  =  *(DATAF*)PrimParPointer();
  Y  =  *(DATAF*)PrimParPointer();
  X /=  Y;
  *(DATAF*)PrimParPointer()  =  X;
}


/*! \page Logic Logic
 *  <hr size="1"/>
 *  <b>     opOR8 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Or two 8 bit values DESTINATION = SOURCE1 | SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE1
 *  \param  (DATA8)   SOURCE2
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opOR8
 *
 *
 */
void      cMathOr8(void)
{
  DATA8   Tmp;

  Tmp  =  *(DATA8*)PrimParPointer();
  Tmp |=  *(DATA8*)PrimParPointer();
  *(DATA8*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opOR16 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Or two 16 bit values DESTINATION = SOURCE1 | SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE1
 *  \param  (DATA16)  SOURCE2
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opOR16
 *
 *
 */
void      cMathOr16(void)
{
  DATA16  Tmp;

  Tmp  =  *(DATA16*)PrimParPointer();
  Tmp |=  *(DATA16*)PrimParPointer();
  *(DATA16*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opOR32 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Or two 32 bit values DESTINATION = SOURCE1 | SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE1
 *  \param  (DATA32)  SOURCE2
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opOR32
 *
 *
 */
void      cMathOr32(void)
{
  DATA32  Tmp;

  Tmp  =  *(DATA32*)PrimParPointer();
  Tmp |=  *(DATA32*)PrimParPointer();
  *(DATA32*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opAND8 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- And two 8 bit values DESTINATION = SOURCE1 & SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE1
 *  \param  (DATA8)   SOURCE2
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opAND8
 *
 *
 */
void      cMathAnd8(void)
{
  DATA8   Tmp;

  Tmp  =  *(DATA8*)PrimParPointer();
  Tmp &=  *(DATA8*)PrimParPointer();
  *(DATA8*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opAND16 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- And two 16 bit values DESTINATION = SOURCE1 & SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE1
 *  \param  (DATA16)  SOURCE2
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opAND16
 *
 *
 */
void      cMathAnd16(void)
{
  DATA16  Tmp;

  Tmp  =  *(DATA16*)PrimParPointer();
  Tmp &=  *(DATA16*)PrimParPointer();
  *(DATA16*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opAND32 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- And two 32 bit values DESTINATION = SOURCE1 & SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE1
 *  \param  (DATA32)  SOURCE2
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opAND32
 *
 *
 */
void      cMathAnd32(void)
{
  DATA32  Tmp;

  Tmp  =  *(DATA32*)PrimParPointer();
  Tmp &=  *(DATA32*)PrimParPointer();
  *(DATA32*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opXOR8 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Exclusive or two 8 bit values DESTINATION = SOURCE1 ^ SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE1
 *  \param  (DATA8)   SOURCE2
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opXOR8
 *
 *
 */
void      cMathXor8(void)
{
  DATA8   Tmp;

  Tmp  =  *(DATA8*)PrimParPointer();
  Tmp ^=  *(DATA8*)PrimParPointer();
  *(DATA8*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opXOR16 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Exclusive or two 16 bit values DESTINATION = SOURCE1 ^ SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE1
 *  \param  (DATA16)  SOURCE2
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opXOR16
 *
 *
 */
void      cMathXor16(void)
{
  DATA16  Tmp;

  Tmp  =  *(DATA16*)PrimParPointer();
  Tmp ^=  *(DATA16*)PrimParPointer();
  *(DATA16*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opXOR32 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Exclusive or two 32 bit values DESTINATION = SOURCE1 ^ SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE1
 *  \param  (DATA32)  SOURCE2
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opXOR32
 *
 *
 */
void      cMathXor32(void)
{
  DATA32  Tmp;

  Tmp  =  *(DATA32*)PrimParPointer();
  Tmp ^=  *(DATA32*)PrimParPointer();
  *(DATA32*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opRL8 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Rotate left 8 bit value DESTINATION = SOURCE1 << SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   SOURCE1
 *  \param  (DATA8)   SOURCE2
 *  \return (DATA8)   DESTINATION
 */
/*! \brief  opRL8
 *
 *
 */
void      cMathRl8(void)
{
  DATA8   Tmp;

  Tmp   =  *(DATA8*)PrimParPointer();
  Tmp <<=  *(DATA8*)PrimParPointer();
  *(DATA8*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opRL16 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Rotate left 16 bit value DESTINATION = SOURCE1 << SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  SOURCE1
 *  \param  (DATA16)  SOURCE2
 *  \return (DATA16)  DESTINATION
 */
/*! \brief  opRL16
 *
 *
 */
void      cMathRl16(void)
{
  DATA16  Tmp;

  Tmp   =  *(DATA16*)PrimParPointer();
  Tmp <<=  *(DATA16*)PrimParPointer();
  *(DATA16*)PrimParPointer()  =  Tmp;
}


/*! \page Logic
 *  <hr size="1"/>
 *  <b>     opRL32 (SOURCE1, SOURCE2, DESTINATION)  </b>
 *
 *- Rotate left 32 bit value DESTINATION = SOURCE1 << SOURCE2\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  SOURCE1
 *  \param  (DATA32)  SOURCE2
 *  \return (DATA32)  DESTINATION
 */
/*! \brief  opRL32
 *
 *
 */
void      cMathRl32(void)
{
  DATA32  Tmp;

  Tmp   =  *(DATA32*)PrimParPointer();
  Tmp <<=  *(DATA32*)PrimParPointer();
  *(DATA32*)PrimParPointer()  =  Tmp;
}


/*! \page cMath
 *  <hr size="1"/>
 *  <b>     opMATH (CMD, ....)  </b>
 *
 *- Math function entry\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   CMD               - \ref mathsubcode
 *
 *\n
 *  - CMD = EXP
 *\n  e^X (R = expf(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = POW
 *\n  Exponent (R = powf(X,Y))\n
 *    -  \param  (DATAF)    X           -
 *    -  \param  (DATAF)    Y           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = MOD8
 *\n  Modulo (R = X % Y)\n
 *    -  \param  (DATA8)    X           -
 *    -  \param  (DATA8)    Y           -
 *    -  \return (DATA8)    R           -
 *
 *\n
 *  - CMD = MOD16
 *\n  Modulo (R = X % Y)\n
 *    -  \param  (DATA16)   X           -
 *    -  \param  (DATA16)   Y           -
 *    -  \return (DATA16)   R           -
 *
 *\n
 *  - CMD = MOD32
 *\n  Modulo (R = X % Y)\n
 *    -  \param  (DATA32)   X           -
 *    -  \param  (DATA32)   Y           -
 *    -  \return (DATA32)   R           -
 *
 *\n
 *  - CMD = MOD
 *\n  Modulo (R = fmod(X,Y))\n
 *    -  \param  (DATAF)    X           -
 *    -  \param  (DATAF)    Y           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = FLOOR
 *\n  Floor (R = floor(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = CEIL
 *\n  Ceil (R = ceil(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = ROUND
 *\n  Round (R = round(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = ABS
 *\n  Absolut (R = fabs(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = NEGATE
 *\n  Negate (R = 0.0 - X)\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = TRUNC
 *\n  Truncate\n
 *    -  \param  (DATAF)    X           - Value
 *    -  \param  (DATA8)    P           - Precision [0..9]
 *    -  \return (DATAF)    R           - Result
 *
 *\n
 *  - CMD = SQRT
 *\n  Squareroot (R = sqrt(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = LOG
 *\n  Log (R = log10(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = LN
 *\n  Ln (R = log(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = SIN
 *\n  Sin (R = sinf(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = COS
 *\n  Cos (R = cos(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = TAN
 *\n  Tan (R = tanf(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = ASIN
 *\n  ASin (R = asinf(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = ACOS
 *\n  ACos (R = acos(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *  - CMD = ATAN
 *\n  ATan (R = atanf(X))\n
 *    -  \param  (DATAF)    X           -
 *    -  \return (DATAF)    R           -
 *
 *\n
 *
 */
/*! \brief  opMATH
 *
 *
 */
void      cMath(void)
{
  DATAF   X;
  DATAF   Y;
  DATA8   X8;
  DATA8   Y8;
  DATA16  X16;
  DATA16  Y16;
  DATA32  X32;
  DATA32  Y32;
  DATA8   Cmd;
  char    Buf[64];
  char    *pBuf;

  Cmd           =  *(DATA8*)PrimParPointer();

  switch (Cmd)
  {
    case EXP :
    {
      X  =  *(DATAF*)PrimParPointer();
      *(DATAF*)PrimParPointer()  =  expf(X);
    }
    break;

    case POW :
    {
      X  =  *(DATAF*)PrimParPointer();
      Y  =  *(DATAF*)PrimParPointer();
      *(DATAF*)PrimParPointer()  =  powf(X,Y);
    }
    break;

    case MOD8 :
    {
      X8    =  *(DATA8*)PrimParPointer();
      Y8    =  *(DATA8*)PrimParPointer();
      *(DATA8*)PrimParPointer()  =  X8 % Y8;
    }
    break;

    case MOD16 :
    {
      X16   =  *(DATA16*)PrimParPointer();
      Y16   =  *(DATA16*)PrimParPointer();
      *(DATA16*)PrimParPointer()  =  X16 % Y16;
    }
    break;

    case MOD32 :
    {
      X32   =  *(DATA32*)PrimParPointer();
      Y32   =  *(DATA32*)PrimParPointer();
      *(DATA32*)PrimParPointer()  =  X32 % Y32;
    }
    break;

    case MOD :
    {
      X  =  *(DATAF*)PrimParPointer();
      Y  =  *(DATAF*)PrimParPointer();
      *(DATAF*)PrimParPointer()  =  fmod(X,Y);
    }
    break;

    case FLOOR :
    {
      X  =  *(DATAF*)PrimParPointer();
      *(DATAF*)PrimParPointer()  =  floor(X);
    }
    break;

    case CEIL :
    {
      X  =  *(DATAF*)PrimParPointer();
      *(DATAF*)PrimParPointer()  =  ceil(X);
    }
    break;

    case ROUND :
    {
      X  =  *(DATAF*)PrimParPointer();
      if (X < (DATAF)0)
      {
        *(DATAF*)PrimParPointer()  =  ceil(X - (DATAF)0.5);
      }
      else
      {
        *(DATAF*)PrimParPointer()  =  floor(X + (DATAF)0.5);
      }
    }
    break;

    case ABS :
    {
      X  =  *(DATAF*)PrimParPointer();
      *(DATAF*)PrimParPointer()  =  fabs(X);
    }
    break;

    case NEGATE :
    {
      X  =  *(DATAF*)PrimParPointer();
      *(DATAF*)PrimParPointer()  =  (DATAF)0 - X;
    }
    break;

    case TRUNC :
    {
      X   =  *(DATAF*)PrimParPointer();
      Y8  =  *(DATA8*)PrimParPointer();

      if (Y8 > 9)
      {
        Y8  =  9;
      }
      if (Y8 < 0)
      {
        Y8  =  0;
      }

      snprintf(Buf,64,"%f",X);

      pBuf  =  strstr(Buf,".");
      if (pBuf != NULL)
      {
        pBuf[Y8 + 1]  =  0;
      }
      sscanf(Buf,"%f",&X);
      *(DATAF*)PrimParPointer()  =  X;
    }
    break;

    case SQRT :
    {
      X  =  *(DATAF*)PrimParPointer();
      *(DATAF*)PrimParPointer()  =  sqrt(X);
    }
    break;

    case LOG :
    {
      X  =  *(DATAF*)PrimParPointer();
      *(DATAF*)PrimParPointer()  =  log10(X);
    }
    break;

    case LN :
    {
      X  =  *(DATAF*)PrimParPointer();
      *(DATAF*)PrimParPointer()  =  log(X);
    }
    break;

    case SIN :
    {
      X  =  *(DATAF*)PrimParPointer();
      X  =  DegToRad(X);
      X  =  sinf(X);
      *(DATAF*)PrimParPointer()  =  X;
    }
    break;

    case COS :
    {
      X  =  *(DATAF*)PrimParPointer();
      X  =  DegToRad(X);
      X  =  cosf(X);
      *(DATAF*)PrimParPointer()  =  X;
    }
    break;

    case TAN :
    {
      X  =  *(DATAF*)PrimParPointer();
      X  =  DegToRad(X);
      X  =  tanf(X);
      *(DATAF*)PrimParPointer()  =  X;
    }
    break;

    case ASIN :
    {
      X  =  *(DATAF*)PrimParPointer();
      X  =  asinf(X);
      X  =  RadToDeg(X);
      *(DATAF*)PrimParPointer()  =  X;
    }
    break;

    case ACOS :
    {
      X  =  *(DATAF*)PrimParPointer();
      X  =  acosf(X);
      X  =  RadToDeg(X);
      *(DATAF*)PrimParPointer()  =  X;
    }
    break;

    case ATAN :
    {
      X  =  *(DATAF*)PrimParPointer();
      X  =  atanf(X);
      X  =  RadToDeg(X);
      *(DATAF*)PrimParPointer()  =  X;
    }
    break;

  }

}


//*****************************************************************************

