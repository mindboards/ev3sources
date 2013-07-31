/*
 * Platform data for TI DaVinci flash devices.
 *
 * Copyright (C) 2009 Texas Instruments, Inc. http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ARCH_ARM_MACH_DAVINCI_FLASH_H
#define __ARCH_ARM_MACH_DAVINCI_FLASH_H

#include <linux/mtd/mtd.h>

#include <mach/aemif.h>

/*
 * map_name:	the map probe function name
 * name:	flash device name (eg, as used with mtdparts=)
 * width:	width of mapped device
 * parts:	optional array of mtd_partitions for static partitioning
 * nr_parts:	number of mtd_partitions for static partitoning
 * timing:	AEMIF timimg values for NOR access
 */
struct davinciflash_pdata {
	const char	*map_name;
	const char	*name;
	unsigned int	width;
	struct mtd_partition *parts;
	unsigned int	nr_parts;
	struct davinci_aemif_timing *timing;
};

#endif
