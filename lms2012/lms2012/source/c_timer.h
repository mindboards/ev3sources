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


#ifndef C_TIMER_H_
#define C_TIMER_H_

#if (HARDWARE != SIMULATION)
ULONG     cTimerGetuS(void);
ULONG     cTimerGetmS(void);
#endif

void      cTimerWait(void);

void      cTimerReady(void);

void      cTimerRead(void);

void      cTimerReaduS(void);

#endif /* C_TIMER_H_ */
