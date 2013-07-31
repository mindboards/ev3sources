/*
 * DaVinci VPIF specific defines and structures
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed .as is. WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __DAVINCI_VPIF_H
#define __DAVINCI_VPIF_H

#include <linux/i2c.h>
#include <linux/videodev2.h>

enum vpif_if_type {
	VPIF_IF_BT656,
	VPIF_IF_BT1120,
	VPIF_IF_RAW_BAYER
};

struct vpif_interface {
	enum vpif_if_type if_type;
	unsigned hd_pol:1;
	unsigned vd_pol:1;
	unsigned fid_pol:1;
};

struct vpif_subdev_info {
	const char *name;
	struct i2c_board_info board_info;
	u32 input;
	u32 output;
	unsigned can_route:1;
	struct vpif_interface vpif_if;
};

struct vpif_display_config {
	int (*set_clock)(int, int);
	int (*intr_status)(void __iomem *vpif_base, int);
	struct vpif_subdev_info *subdevinfo;
	int subdev_count;
	const char **output;
	int output_count;
	const char *card_name;
};

struct vpif_input {
	struct v4l2_input input;
	const char *subdev_name;
};

#define VPIF_CAPTURE_MAX_CHANNELS	2

struct vpif_capture_chan_config {
	const struct vpif_input *inputs;
	int input_count;
};

struct vpif_capture_config {
	int (*setup_input_channel_mode)(int);
	int (*setup_input_path)(int, const char *);
	int (*intr_status)(void __iomem *vpif_base, int);
	struct vpif_capture_chan_config chan_config[VPIF_CAPTURE_MAX_CHANNELS];
	struct vpif_subdev_info *subdev_info;
	int subdev_count;
	const char *card_name;
};

#endif /* __DAVINCI_VPIF_H */
