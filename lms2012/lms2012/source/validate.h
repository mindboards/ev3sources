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


#ifndef VALIDATE_H_
#define VALIDATE_H_

RESULT    cValidateInit(void);

RESULT    cValidateExit(void);

RESULT    cValidateDisassemble(IP pI,IMINDEX *pIndex,LABEL *pLabel);

RESULT    cValidateProgram(PRGID PrgId,IP pI,LABEL *pLabel,DATA8 Disassemble);


typedef struct
{
  //*****************************************************************************
  // Validate Global variables
  //*****************************************************************************

  int       Row;
  IMINDEX   ValidateErrorIndex;
}
VALIDATE_GLOBALS;

#ifndef LEGO_SIMULATION
extern VALIDATE_GLOBALS ValidateInstance;
#else
extern VALIDATE_GLOBALS * gValidateInstance;
#define ValidateInstance (*gValidateInstance)

void setValidateInstance(VALIDATE_GLOBALS * _Instance);
VALIDATE_GLOBALS * getValidateInstance();
#endif


  #endif /* VALIDATE_H_ */
