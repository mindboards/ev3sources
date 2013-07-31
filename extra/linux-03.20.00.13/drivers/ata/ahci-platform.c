/*
 * TI AHCI SATA controler 
 *
 * TI SATA controller on OMAPL138 platform.  AHCI compliant controller. 
 *
 * Copyright (C) 2009 Texas Instruments.
 *
 * ---------------------------------------------------------------------------
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ---------------------------------------------------------------------------
 * History:
 * 0-1 Swaminathan Subbarathnam : Initial creation of AHCI platform specific
 *     interface for non-PCI based SATA AHCI implementation. 
 */
#ifdef CONFIG_ARCH_DAVINCI_DA850
#include "ahci-ti.c"
#endif
