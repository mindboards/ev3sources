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

#ifndef C_I2C_H_
#define C_I2C_H_


#include  "lms2012.h"


enum
{
  MODE2_SUCCESS       = 0,
  MODE2_BOOTING       = 1,
  MODE2_FALIURE       = 2
};

RESULT    I2cInit(READBUF *pBuf, WRITEBUF *pWriteBuf, char *pBundleId, char *pBundleSeedId);

void      I2cExit(void);

void      I2cStart(void);
void      I2cStop(void);

UWORD     DataToMode2Decoding(UBYTE *pBuf, UWORD Length);
UBYTE     I2cGetBootStatus(void);


#endif /* C_I2C_H_ */
