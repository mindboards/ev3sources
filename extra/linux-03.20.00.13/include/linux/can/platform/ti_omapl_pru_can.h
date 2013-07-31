/*
 * TI OMAPL PRU CAN Emulation device driver
 * 
 * This driver supports TI's PRU CAN Emulation and the
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


/**
 * struct hecc_platform_data - HECC Platform Data
 * 
 * @version:            version for future use
 *
 * Platform data structure to get all platform specific settings.
 */

struct ti_pru_can_platform_data {
        u32 version;
};
