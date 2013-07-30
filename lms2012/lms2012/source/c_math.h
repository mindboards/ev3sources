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


#ifndef C_MATH_H_
#define C_MATH_H_

#define   DegToRad(D)           (D * 0.0174532925)

#define   RadToDeg(R)           (R * 57.2957795)


void      cMathAdd8(void);

void      cMathAdd16(void);

void      cMathAdd32(void);

void      cMathAddF(void);

void      cMathSub8(void);

void      cMathSub16(void);

void      cMathSub32(void);

void      cMathSubF(void);

void      cMathMul8(void);

void      cMathMul16(void);

void      cMathMul32(void);

void      cMathMulF(void);

void      cMathDiv8(void);

void      cMathDiv16(void);

void      cMathDiv32(void);

void      cMathDivF(void);

void      cMathOr8(void);

void      cMathOr16(void);

void      cMathOr32(void);

void      cMathAnd8(void);

void      cMathAnd16(void);

void      cMathAnd32(void);

void      cMathXor8(void);

void      cMathXor16(void);

void      cMathXor32(void);

void      cMathRl8(void);

void      cMathRl16(void);

void      cMathRl32(void);

void      cMath(void);

#endif /* C_MATH_H_ */
