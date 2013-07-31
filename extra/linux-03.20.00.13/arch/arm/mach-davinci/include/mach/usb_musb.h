/*
 * This file contains the architecture specific USB definitions
 * of the TI DaVinci platforms.
 *
 * Copyright (C) 2009 Texas Instruments.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef USB_MUSB_H_
#define USB_MUSB_H_

#include <linux/platform_device.h>
#include <linux/usb/musb.h>

struct plat_res_data {
	struct musb_hdrc_platform_data	*plat_data;
	struct resource			*res_data;
	u8				num_res;
};

struct usb_plat_data {
	struct plat_res_data	*prdata;
	u8			num_inst;
};

/* VBUS control fuction */
extern void setup_usb(struct usb_plat_data *pdata);
#endif
