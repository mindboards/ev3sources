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
#include  "c_branch.h"


//******* BYTE CODE SNIPPETS **************************************************


/*! \page cBranch Branch
 *  <hr size="1"/>
 *  <b>     opJR (OFFSET)  </b>
 *
 *- Branch unconditionally relative\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR byte code
 *
 *
 */
void      cBranchJr(void)
{
  AdjustObjectIp(*(IMOFFS*)PrimParPointer());
}

#include  <stdio.h>
/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_FALSE (FLAG, OFFSET)  </b>
 *
 *- Branch relative if FLAG is FALSE (zero)\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   FLAG
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_FALSE byte code
 *
 *
 */
void      cBranchJrFalse(void)
{
  if (*(DATA8*)PrimParPointer() == (DATA8)0)
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_TRUE (FLAG, OFFSET)  </b>
 *
 *- Branch relative if FLAG is TRUE (non zero)\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   FLAG
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_TRUE byte code
 *
 *
 */
void      cBranchJrTrue(void)
{
  if (*(DATA8*)PrimParPointer() != (DATA8)0)
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


#include  <math.h>
/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_NAN (VALUE, OFFSET)  </b>
 *
 *- Branch relative if VALUE is NAN (not a number)\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   VALUE
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_NAN byte code
 *
 *
 */
void      cBranchJrNan(void)
{
  DATAF   Tmp;

  Tmp  =  *(DATAF*)PrimParPointer();

  if (isnan(Tmp))
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_LT8 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is less than RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_LT8 byte code
 *
 *
 */
void      cBranchJrLt8(void)
{
  DATA8   Tmp;

  Tmp      =  *(DATA8*)PrimParPointer();
  if (Tmp  <  *(DATA8*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_LT16 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is less than RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_LT16 byte code
 *
 *
 */
void      cBranchJrLt16(void)
{
  DATA16  Tmp;

  Tmp      =  *(DATA16*)PrimParPointer();
  if (Tmp  <  *(DATA16*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_LT32 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is less than RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_LT16 byte code
 *
 *
 */
void      cBranchJrLt32(void)
{
  DATA32  Tmp;

  Tmp      =  *(DATA32*)PrimParPointer();
  if (Tmp  <  *(DATA32*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_LTF (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is less than RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_LTF byte code
 *
 *
 */
void      cBranchJrLtF(void)
{
  DATAF   Tmp;

  Tmp      =  *(DATAF*)PrimParPointer();
  if (Tmp  <  *(DATAF*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_GT8 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is greater than RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_GT8 byte code
 *
 *
 */
void      cBranchJrGt8(void)
{
  DATA8   Tmp;

  Tmp      =  *(DATA8*)PrimParPointer();
  if (Tmp  >  *(DATA8*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_GT16 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is greater than RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_GT16 byte code
 *
 *
 */
void      cBranchJrGt16(void)
{
  DATA16  Tmp;

  Tmp      =  *(DATA16*)PrimParPointer();
  if (Tmp  >  *(DATA16*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_GT32 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is greater than RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_GT32 byte code
 *
 *
 */
void      cBranchJrGt32(void)
{
  DATA32  Tmp;

  Tmp      =  *(DATA32*)PrimParPointer();
  if (Tmp  >  *(DATA32*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_GTF (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is greater than RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_GTF byte code
 *
 *
 */
void      cBranchJrGtF(void)
{
  DATAF   Tmp;

  Tmp      =  *(DATAF*)PrimParPointer();
  if (Tmp  >  *(DATAF*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_EQ8 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_EQ8 byte code
 *
 *
 */
void      cBranchJrEq8(void)
{
  DATA8   Tmp;

  Tmp      =  *(DATA8*)PrimParPointer();
  if (Tmp ==  *(DATA8*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_EQ16 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_EQ16 byte code
 *
 *
 */
void      cBranchJrEq16(void)
{
  DATA16  Tmp;

  Tmp      =  *(DATA16*)PrimParPointer();
  if (Tmp ==  *(DATA16*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_EQ32 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_EQ32 byte code
 *
 *
 */
void      cBranchJrEq32(void)
{
  DATA32  Tmp;

  Tmp      =  *(DATA32*)PrimParPointer();
  if (Tmp ==  *(DATA32*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_EQF (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_EQF byte code
 *
 *
 */
void      cBranchJrEqF(void)
{
  DATAF   Tmp;

  Tmp      =  *(DATAF*)PrimParPointer();
  if (Tmp ==  *(DATAF*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_NEQ8 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is not equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_NEQ8 byte code
 *
 *
 */
void      cBranchJrNEq8(void)
{
  DATA8   Tmp;

  Tmp      =  *(DATA8*)PrimParPointer();
  if (Tmp !=  *(DATA8*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_NEQ16 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is not equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_NEQ16 byte code
 *
 *
 */
void      cBranchJrNEq16(void)
{
  DATA16  Tmp;

  Tmp      =  *(DATA16*)PrimParPointer();
  if (Tmp !=  *(DATA16*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_NEQ32 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is not equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_NEQ32 byte code
 *
 *
 */
void      cBranchJrNEq32(void)
{
  DATA32  Tmp;

  Tmp      =  *(DATA32*)PrimParPointer();
  if (Tmp !=  *(DATA32*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_NEQF (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is not equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_NEQF byte code
 *
 *
 */
void      cBranchJrNEqF(void)
{
  DATAF   Tmp;

  Tmp      =  *(DATAF*)PrimParPointer();
  if (Tmp !=  *(DATAF*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_LTEQ8 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is less than or equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_LTEQ8 byte code
 *
 *
 */
void      cBranchJrLtEq8(void)
{
  DATA8   Tmp;

  Tmp      =  *(DATA8*)PrimParPointer();
  if (Tmp <=  *(DATA8*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_LTEQ16 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is less than or equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  LEFT
 *  \param  (DATA16)  RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_LTEQ16 byte code
 *
 *
 */
void      cBranchJrLtEq16(void)
{
  DATA16  Tmp;

  Tmp      =  *(DATA16*)PrimParPointer();
  if (Tmp  <= *(DATA16*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_LTEQ32 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is less than or equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  LEFT
 *  \param  (DATA32)  RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_LTEQ32 byte code
 *
 *
 */
void      cBranchJrLtEq32(void)
{
  DATA32  Tmp;

  Tmp      =  *(DATA32*)PrimParPointer();
  if (Tmp <=  *(DATA32*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_LTEQF (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is less than or equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_LTEQF byte code
 *
 *
 */
void      cBranchJrLtEqF(void)
{
  DATAF   Tmp;

  Tmp      =  *(DATAF*)PrimParPointer();
  if (Tmp <=  *(DATAF*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_GTEQ8 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is greater than or equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   LEFT
 *  \param  (DATA8)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_GTEQ8 byte code
 *
 *
 */
void      cBranchJrGtEq8(void)
{
  DATA8   Tmp;

  Tmp      =  *(DATA8*)PrimParPointer();
  if (Tmp >=  *(DATA8*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_GTEQ16 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is greater than or equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)   LEFT
 *  \param  (DATA16)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_GTEQ16 byte code
 *
 *
 */
void      cBranchJrGtEq16(void)
{
  DATA16  Tmp;

  Tmp      =  *(DATA16*)PrimParPointer();
  if (Tmp >=  *(DATA16*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_GTEQ32 (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is greater than or equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)   LEFT
 *  \param  (DATA32)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_GTEQ32 byte code
 *
 *
 */
void      cBranchJrGtEq32(void)
{
  DATA32  Tmp;

  Tmp      =  *(DATA32*)PrimParPointer();
  if (Tmp >=  *(DATA32*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


/*! \page cBranch
 *  <hr size="1"/>
 *  <b>     opJR_GTEQF (LEFT, RIGHT, OFFSET)  </b>
 *
 *- Branch relative OFFSET if LEFT is greater than or equal to RIGHT\n
 *- Dispatch status unchanged
 *
 *  \param  (DATAF)   LEFT
 *  \param  (DATAF)   RIGHT
 *  \param  (DATA32)  OFFSET
 */
/*! \brief  opJR_GTEQF byte code
 *
 *
 */
void      cBranchJrGtEqF(void)
{
  DATAF   Tmp;

  Tmp      =  *(DATAF*)PrimParPointer();
  if (Tmp >=  *(DATAF*)PrimParPointer())
  {
    AdjustObjectIp(*(IMOFFS*)PrimParPointer());
  }
  else
  {
    PrimParAdvance();
  }
}


