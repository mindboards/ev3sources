/*
 * linux/include/video/st586fb.h -- FB driver for ST7586 LCD controller
 *
 * Copyright (C) 2011, Matt Porter <matt@ohporter.com>
 * Copyright (C) 2011, Texas Instruments
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file COPYING in the main directory of this archive for
 * more details.
 */

#define DRVNAME		"st7586fb"
#define WIDTH		178
#define HEIGHT		128

#define FB_ST7586_MAGIC 'S'                                    // defines the magic number
#define FB_ST7586_INIT_DISPLAY _IOW(FB_ST7586_MAGIC,0, int)    // defines ioctl call for .
#define FB_ST7586_START_DISPLAY _IOW(FB_ST7586_MAGIC,1, int)   // defines our ioctl call.

/* Supported display modules */
#define ST7586_DISPLAY_LMS2012_LCD	0	/* LMS2012 LCD */

/* Init script function */
struct st7586_function {
	u16 cmd;
	u16 data;
};

/* Init script commands */
enum st7586_cmd {
	ST7586_START,
	ST7586_END,
	ST7586_CMD,
	ST7586_DATA,
	ST7586_DELAY,
	ST7586_CLR,
};

struct st7586fb_par {
	struct spi_device *spi;
	struct fb_info *info;
	int rst;
	int a0;
	int cs;
	u8 *buf;
};

struct st7586fb_platform_data {
	int rst_gpio;
	int a0_gpio;
	int cs_gpio;
};

/* ST7586 Commands */
#define ST7586_NOP	0x00
#define ST7586_SWRESET	0x01
#define ST7586_SLPIN	0x10
#define ST7586_SLPOUT	0x11
#define ST7586_PTLON	0x12
#define ST7586_NORON	0x13
#define ST7586_INVOFF	0x20
#define ST7586_INVON	0x21
#define ST7586_APOFF	0x22
#define ST7586_APON	0x23
#define ST7586_DISPOFF	0x28
#define ST7586_DISPON	0x29
#define ST7586_CASET	0x2A
#define ST7586_RASET	0x2B
#define ST7586_RAMWR	0x2C
#define ST7586_RAMRD	0x2E
#define ST7586_PARAREA	0x30
#define ST7586_DSPCTL	0x36
#define ST7586_DSPGRAY	0x38
#define ST7586_DSPMONO	0x39
#define ST7586_RAMENB	0x3A
#define ST7586_DSPDUTY	0xB0
#define ST7586_PARDISP	0xB4
#define ST7586_NLNINV	0xB5
#define ST7586_VOP	0xC0
#define ST7586_BIAS	0xC3
#define ST7586_BOOST	0xC4
#define ST7586_VOPOFF	0xC7
#define ST7586_ANCTL	0xD0
#define ST7586_ARDCTL	0xD7
#define ST7586_OTPRWCTL	0xE0
#define ST7586_OTPCOUT	0xE1
#define ST7586_OTPWR	0xE2
#define ST7586_OTPRD	0xE3
