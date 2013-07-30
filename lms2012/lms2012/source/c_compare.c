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
#include  "c_compare.h"


//******* BYTE CODE SNIPPETS **************************************************


/*! \page cCompare Compare
 *  <hr size="1"/>
 *  <b>     opCP_LT8 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is less than RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_LT8 byte code
 *
 *
 */
void      cCompareLt8(void)
{
  DATA8   Tmp;

  Tmp      =  *(DATA8*)PrimParPointer();
  if (Tmp  <  *(DATA8*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_LT16 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is less than RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_LT16 byte code
 *
 *
 */
void      cCompareLt16(void)
{
  DATA16  Tmp;

  Tmp      =  *(DATA16*)PrimParPointer();
  if (Tmp  <  *(DATA16*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_LT32 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is less than RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_LT32 byte code
 *
 *
 */
void      cCompareLt32(void)
{
  DATA32  Tmp;

  Tmp      =  *(DATA32*)PrimParPointer();
  if (Tmp  <  *(DATA32*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare Compare
 *  <hr size="1"/>
 *  <b>     opCP_LTF (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is less than RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_LTF byte code
 *
 *
 */
void      cCompareLtF(void)
{
  DATAF   Tmp;

  Tmp      =  *(DATAF*)PrimParPointer();
  if (Tmp  <  *(DATAF*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_GT8 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is greater than RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_GT8 byte code
 *
 *
 */
void      cCompareGt8(void)
{
  DATA8   Tmp;

  Tmp      =  *(DATA8*)PrimParPointer();
  if (Tmp  >  *(DATA8*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_GT16 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is greater than RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_GT16 byte code
 *
 *
 */
void      cCompareGt16(void)
{
  DATA16  Tmp;

  Tmp      =  *(DATA16*)PrimParPointer();
  if (Tmp  >  *(DATA16*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_GT32 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is greater than RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_GT32 byte code
 *
 *
 */
void      cCompareGt32(void)
{
  DATA32  Tmp;

  Tmp      =  *(DATA32*)PrimParPointer();
  if (Tmp  >  *(DATA32*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare Compare
 *  <hr size="1"/>
 *  <b>     opCP_GTF (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is greater than RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_GTF byte code
 *
 *
 */
void      cCompareGtF(void)
{
  DATAF   Tmp;

  Tmp      =  *(DATAF*)PrimParPointer();
  if (Tmp  >  *(DATAF*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_EQ8 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_EQ8 byte code
 *
 *
 */
void      cCompareEq8(void)
{
  DATA8   Tmp;

  Tmp      =   *(DATA8*)PrimParPointer();
  if (Tmp  ==  *(DATA8*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_EQ16 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_EQ16 byte code
 *
 *
 */
void      cCompareEq16(void)
{
  DATA16  Tmp;

  Tmp      =   *(DATA16*)PrimParPointer();
  if (Tmp  ==  *(DATA16*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_EQ32 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_EQ32 byte code
 *
 *
 */
void      cCompareEq32(void)
{
  DATA32  Tmp;

  Tmp      =   *(DATA32*)PrimParPointer();
  if (Tmp  ==  *(DATA32*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_EQF (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_EQF byte code
 *
 *
 */
void      cCompareEqF(void)
{
  DATAF   Tmp;

  Tmp      =   *(DATAF*)PrimParPointer();
  if (Tmp  ==  *(DATAF*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_NEQ8 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is not equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_NEQ8 byte code
 *
 *
 */
void      cCompareNEq8(void)
{
  DATA8   Tmp;

  Tmp      =   *(DATA8*)PrimParPointer();
  if (Tmp  !=  *(DATA8*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_NEQ16 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is not equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_NEQ16 byte code
 *
 *
 */
void      cCompareNEq16(void)
{
  DATA16  Tmp;

  Tmp      =   *(DATA16*)PrimParPointer();
  if (Tmp  !=  *(DATA16*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_NEQ32 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is not equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_NEQ32 byte code
 *
 *
 */
void      cCompareNEq32(void)
{
  DATA32  Tmp;

  Tmp      =   *(DATA32*)PrimParPointer();
  if (Tmp  !=  *(DATA32*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_NEQF (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is not equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_NEQF byte code
 *
 *
 */
void      cCompareNEqF(void)
{
  DATAF   Tmp;

  Tmp      =   *(DATAF*)PrimParPointer();
  if (Tmp  !=  *(DATAF*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_LTEQ8 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is less than or equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_LTEQ8 byte code
 *
 *
 */
void      cCompareLtEq8(void)
{
  DATA8   Tmp;

  Tmp      =  *(DATA8*)PrimParPointer();
  if (Tmp <=  *(DATA8*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_LTEQ16 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is less than or equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_LTEQ16 byte code
 *
 *
 */
void      cCompareLtEq16(void)
{
  DATA16  Tmp;

  Tmp      =  *(DATA16*)PrimParPointer();
  if (Tmp <=  *(DATA16*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_LTEQ32 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is less than or equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_LTEQ32 byte code
 *
 *
 */
void      cCompareLtEq32(void)
{
  DATA32  Tmp;

  Tmp      =  *(DATA32*)PrimParPointer();
  if (Tmp <=  *(DATA32*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare Compare
 *  <hr size="1"/>
 *  <b>     opCP_LTEQF (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is less than or equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_LTEQF byte code
 *
 *
 */
void      cCompareLtEqF(void)
{
  DATAF   Tmp;

  Tmp      =  *(DATAF*)PrimParPointer();
  if (Tmp <=  *(DATAF*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_GTEQ8 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is greater than or equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_GTEQ8 byte code
 *
 *
 */
void      cCompareGtEq8(void)
{
  DATA8   Tmp;

  Tmp      =  *(DATA8*)PrimParPointer();
  if (Tmp >=  *(DATA8*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_GTEQ16 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is greater than or equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_GTEQ16 byte code
 *
 *
 */
void      cCompareGtEq16(void)
{
  DATA16  Tmp;

  Tmp      =  *(DATA16*)PrimParPointer();
  if (Tmp >=  *(DATA16*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare
 *  <hr size="1"/>
 *  <b>     opCP_GTEQ32 (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is greater than or equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_GTEQ32 byte code
 *
 *
 */
void      cCompareGtEq32(void)
{
  DATA32  Tmp;

  Tmp      =  *(DATA32*)PrimParPointer();
  if (Tmp >=  *(DATA32*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page cCompare Compare
 *  <hr size="1"/>
 *  <b>     opCP_GTEQF (LEFT, RIGHT, FLAG)  </b>
 *
 *- If LEFT is greater than or equal to RIGTH - set FLAG \n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \return (DATA8)   FLAG
 */
/*! \brief  opCP_GTEQF byte code
 *
 *
 */
void      cCompareGtEqF(void)
{
  DATAF   Tmp;

  Tmp      =  *(DATAF*)PrimParPointer();
  if (Tmp >=  *(DATAF*)PrimParPointer())
  {
    *(DATA8*)PrimParPointer()  =  1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  0;
  }
}


/*! \page Select Select
 *  <hr size="1"/>
 *  <b>     opSELECT8 (FLAG, SOURCE1, SOURCE2, *RESULT)  </b>
 *
 *- If FLAG is set move SOURCE1 to RESULT else move SOURCE2 to RESULT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   FLAG
 *  \param  (DATA8)   SOURCE1
 *  \param  (DATA8)   SOURCE2
 *  \return (DATA8)   *RESULT
 */
/*! \brief  opSELECT8 byte code
 *
 *
 */
void      cCompareSelect8(void)
{
  DATA8   Flag;
  DATA8   Source1;
  DATA8   Source2;

  Flag     =  *(DATA8*)PrimParPointer();
  Source1  =  *(DATA8*)PrimParPointer();
  Source2  =  *(DATA8*)PrimParPointer();
  if (Flag)
  {
    *(DATA8*)PrimParPointer()  =  Source1;
  }
  else
  {
    *(DATA8*)PrimParPointer()  =  Source2;
  }
}


/*! \page Select
 *  <hr size="1"/>
 *  <b>     opSELECT16 (FLAG, SOURCE1, SOURCE2, *RESULT)  </b>
 *
 *- If FLAG is set move SOURCE1 to RESULT else move SOURCE2 to RESULT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   FLAG
 *  \param  (DATA16)  SOURCE1
 *  \param  (DATA16)  SOURCE2
 *  \return (DATA16)  *RESULT
 */
/*! \brief  opSELECT16 byte code
 *
 *
 */
void      cCompareSelect16(void)
{
  DATA8   Flag;
  DATA16  Source1;
  DATA16  Source2;


  Flag     =  *(DATA8*)PrimParPointer();
  Source1  =  *(DATA16*)PrimParPointer();
  Source2  =  *(DATA16*)PrimParPointer();
  if (Flag)
  {
    *(DATA16*)PrimParPointer()  =  Source1;
  }
  else
  {
    *(DATA16*)PrimParPointer()  =  Source2;
  }
}


/*! \page Select
 *  <hr size="1"/>
 *  <b>     opSELECT32 (FLAG, SOURCE1, SOURCE2, *RESULT)  </b>
 *
 *- If FLAG is set move SOURCE1 to RESULT else move SOURCE2 to RESULT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   FLAG
 *  \param  (DATA32)  SOURCE1
 *  \param  (DATA32)  SOURCE2
 *  \return (DATA32)  *RESULT
 */
/*! \brief  opSELECT32 byte code
 *
 *
 */
void      cCompareSelect32(void)
{
  DATA8   Flag;
  DATA32  Source1;
  DATA32  Source2;

  Flag     =  *(DATA8*)PrimParPointer();
  Source1  =  *(DATA32*)PrimParPointer();
  Source2  =  *(DATA32*)PrimParPointer();
  if (Flag)
  {
    *(DATA32*)PrimParPointer()  =  Source1;
  }
  else
  {
    *(DATA32*)PrimParPointer()  =  Source2;
  }
}


/*! \page Select
 *  <hr size="1"/>
 *  <b>     opSELECTF (FLAG, SOURCE1, SOURCE2, *RESULT)  </b>
 *
 *- If FLAG is set move SOURCE1 to RESULT else move SOURCE2 to RESULT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   FLAG
 *  \param  (DATAF)   SOURCE1
 *  \param  (DATAF)   SOURCE2
 *  \return (DATAF)   *RESULT
 */
/*! \brief  opSELECTF byte code
 *
 *
 */
void      cCompareSelectF(void)
{
  DATA8   Flag;
  DATAF   Source1;
  DATAF   Source2;

  Flag     =  *(DATA8*)PrimParPointer();
  Source1  =  *(DATAF*)PrimParPointer();
  Source2  =  *(DATAF*)PrimParPointer();
  if (Flag)
  {
    *(DATAF*)PrimParPointer()  =  Source1;
  }
  else
  {
    *(DATAF*)PrimParPointer()  =  Source2;
  }
}


