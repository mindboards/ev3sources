/*
 *  linux/include/linux/ti_omapl_pru_suart.h
 */
#ifndef _LINUX_SUART_OMAPL_PRU_H
#define _LINUX_SUART_OMAPL_PRU_H

#include <linux/serial_core.h>
#include <linux/platform_device.h>

/*
 * TI OMAPL PRU SUART Emulation device driver
 *
 * This driver supports TI's PRU SUART Emulation and the
 * specs for the same is available at <http://www.ti.com>
 * 
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 * 
 * This program is distributed as is WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * struct ti_pru_suart_platform_data - SUART Platform Data
 *
 * @version:            version for future use
 *
 * Platform data structure to get all platform specific settings.
 */

struct ti_pru_suart_platform_data {
	u32 version;
};
#endif
