/*
 * drivers/video/davincifb.c
 *
 * Framebuffer driver for Texas Instruments DaVinci display controller.
 *
 * Copyright (C) 2006 Texas Instruments, Inc.
 * Rishi Bhattacharya <support@ti.com>
 *
 * Leveraged from the framebuffer driver for OMAP24xx
 * written by Andy Lowe (source@mvista.com)
 * Copyright (C) 2004 MontaVista Software, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

#include <asm/irq.h>
#include <asm/uaccess.h>

#include <video/davincifb.h>
#include <asm/system.h>

#define MODULE_NAME "davincifb"

/* Output Format Selection  */
#define MULTIPLE_BUFFERING	1

#ifdef MULTIPLE_BUFFERING
#define DOUBLE_BUF	2
#define TRIPLE_BUF	3
#else
#define DOUBLE_BUF	1
#define TRIPLE_BUF	1
#endif

/*
 * display controller register I/O routines
 */
static __inline__ u32 dispc_reg_in(u32 reg)
{
	return ioread32(IO_ADDRESS(reg));
}
static __inline__ u32 dispc_reg_out(u32 reg, u32 val)
{
	iowrite32(val, IO_ADDRESS(reg));
	return (val);
}
static __inline__ u32 dispc_reg_merge(u32 reg, u32 val, u32 mask)
{
	u32 new_val = (ioread32(IO_ADDRESS(reg)) & ~mask) | (val & mask);

	iowrite32(new_val, IO_ADDRESS(reg));
	return (new_val);
}

/* There are 4 framebuffers, each represented by an fb_info and
 * a dm_win_info structure */
#define OSD0_FBNAME	"dm_osd0_fb"
#define OSD1_FBNAME	"dm_osd1_fb"
#define VID0_FBNAME	"dm_vid0_fb"
#define VID1_FBNAME	"dm_vid1_fb"

/* usage:	if (is_win(info->fix.id, OSD0)) ... */
#define is_win(name, x) ((strcmp(name, x ## _FBNAME) == 0) ? 1 : 0)

struct dm_win_info {
	struct fb_info info;

	/* X and Y position */
	unsigned int x, y;

	/* framebuffer area */
	dma_addr_t fb_base_phys;
	unsigned long fb_base;
	unsigned long fb_size;

	u32 pseudo_palette[17];

	/* flag to identify if framebuffer area is fixed already or not */
	int alloc_fb_mem;
	unsigned long sdram_address;
	struct dm_info *dm;
};

static struct dm_info {
	struct dm_win_info *osd0;
	struct dm_win_info *osd1;
	struct dm_win_info *vid0;
	struct dm_win_info *vid1;

	/* to map the registers */
	dma_addr_t mmio_base_phys;
	unsigned long mmio_base;
	unsigned long mmio_size;

	wait_queue_head_t vsync_wait;
	unsigned long vsync_cnt;
	int timeout;

	/* this is the function that configures the output device (NTSC/PAL/LCD)
	 * for the required output format (composite/s-video/component/rgb)
	 */
	void (*output_device_config) (int on);

	struct device *dev;
} dm_static;
static struct dm_info *dm = &dm_static;

static struct fb_ops davincifb_ops;

#define BASEX		0x80
#define BASEY		0x12

#define DISP_XRES	720
#define DISP_YRES	480
#define DISP_MEMY	576

/* Random value chosen for now. Should be within the panel's supported range */
#define LCD_PANEL_CLOCK	180000

/* All window widths have to be rounded up to a multiple of 32 bytes */

/* The OSD0 window has to be always within VID0. Plus, since it is in RGB565
 * mode, it _cannot_ overlap with VID1.
 * For defaults, we are setting the OSD0 window to be displayed in the top
 * left quadrant of the screen, and the VID1 in the bottom right quadrant.
 * So the default 'xres' and 'yres' are set to  half of the screen width and
 * height respectively. Note however that the framebuffer size is allocated
 * for the full screen size so the user can change the 'xres' and 'yres' by
 * using the FBIOPUT_VSCREENINFO ioctl within the limits of the screen size.
 */
#define round_32(width)	((((width) + 31) / 32) * 32 )

#define OSD0_XRES	round_32((DISP_XRES)*16/8) * 8/16	/* pixels */
#define OSD0_YRES	DISP_YRES
#define OSD0_FB_PHY	0
#define OSD0_FB_SIZE	(round_32((DISP_XRES)*16/8) * DISP_MEMY * DOUBLE_BUF)

			/* 16 bpp, Double buffered */
static struct fb_var_screeninfo osd0_default_var = {
	.xres = OSD0_XRES,
	.yres = OSD0_YRES,
	.xres_virtual = OSD0_XRES,
	.yres_virtual = OSD0_YRES * DOUBLE_BUF,
	.xoffset = 0,
	.yoffset = 0,
	.bits_per_pixel = 16,
	.grayscale = 0,
	.red = {11, 5, 0},
	.green = {5, 6, 0},
	.blue = {0, 5, 0},
	.transp = {0, 0, 0},
	.nonstd = 0,
	.activate = FB_ACTIVATE_NOW,
	.height = -1,
	.width = -1,
	.accel_flags = 0,
	.pixclock = LCD_PANEL_CLOCK,	/* picoseconds */
	.left_margin = 40,	/* pixclocks */
	.right_margin = 4,	/* pixclocks */
	.upper_margin = 8,	/* line clocks */
	.lower_margin = 2,	/* line clocks */
	.hsync_len = 4,		/* pixclocks */
	.vsync_len = 2,		/* line clocks */
	.sync = 0,
	.vmode = FB_VMODE_INTERLACED,
};

/* Using the full screen for OSD1 by default */
#define OSD1_XRES	round_32(DISP_XRES*4/8) * 8/4	/* pixels */
#define OSD1_YRES	DISP_YRES
#define OSD1_FB_PHY	0
#define OSD1_FB_SIZE	(round_32(DISP_XRES*4/8) * DISP_MEMY * DOUBLE_BUF)

static struct fb_var_screeninfo osd1_default_var = {
	.xres = DISP_XRES,
	.yres = OSD1_YRES,
	.xres_virtual = OSD1_XRES,
	.yres_virtual = OSD1_YRES * DOUBLE_BUF,
	.xoffset = 0,
	.yoffset = 0,
	.bits_per_pixel = 4,
	.activate = FB_ACTIVATE_NOW,
	.accel_flags = 0,
	.pixclock = LCD_PANEL_CLOCK,	/* picoseconds */
	.vmode = FB_VMODE_INTERLACED,
};

/* Using the full screen for OSD0 by default */
#define VID0_XRES	round_32(DISP_XRES*16/8) * 8/16	/* pixels */
#define VID0_YRES	DISP_YRES
#define VID0_FB_PHY	0
#define VID0_FB_SIZE	(round_32(DISP_XRES*16/8) * DISP_MEMY * TRIPLE_BUF)
static struct fb_var_screeninfo vid0_default_var = {
	.xres = VID0_XRES,
	.yres = VID0_YRES,
	.xres_virtual = VID0_XRES,
	.yres_virtual = VID0_YRES * TRIPLE_BUF,
	.xoffset = 0,
	.yoffset = 0,
	.bits_per_pixel = 16,
	.activate = FB_ACTIVATE_NOW,
	.accel_flags = 0,
	.pixclock = LCD_PANEL_CLOCK,	/* picoseconds */
	.vmode = FB_VMODE_INTERLACED,
};

/* Using the bottom right quadrant of the screen screen for VID1 by default,
 * but keeping the framebuffer allocated for the full screen, so the user can
 * change the 'xres' and 'yres' later using the FBIOPUT_VSCREENINFO ioctl.
 */
#define VID1_BPP	16	/* Video1 can be in YUV or RGB888 format */
#define VID1_XRES round_32(DISP_XRES*16/8) * 8/16	/* pixels */
#define VID1_YRES DISP_YRES
#define VID1_FB_PHY	0
#define VID1_FB_SIZE (round_32(DISP_XRES*16/8) * DISP_MEMY * TRIPLE_BUF)
static struct fb_var_screeninfo vid1_default_var = {
	.xres = VID1_XRES,
	.yres = VID1_YRES,
	.xres_virtual = VID1_XRES,
	.yres_virtual = VID1_YRES * TRIPLE_BUF,
	.xoffset = 0,
	.yoffset = 0,
	.bits_per_pixel = VID1_BPP,
	.activate = FB_ACTIVATE_NOW,
	.accel_flags = 0,
	.pixclock = LCD_PANEL_CLOCK,	/* picoseconds */
	.vmode = FB_VMODE_INTERLACED,
};

#define	x_pos(w)	((w)->x)
#define	y_pos(w)	((w)->y)

static struct dmparams_t {
	u8 output;
	u8 format;
	u8 windows;		/* bitmap flag based on VID0, VID1, OSD0, OSD1
				 * definitions in header file */
	u32 vid0_xres;
	u32 vid0_yres;
	u32 vid0_xpos;
	u32 vid0_ypos;

	u32 vid1_xres;
	u32 vid1_yres;
	u32 vid1_xpos;
	u32 vid1_ypos;

	u32 osd0_xres;
	u32 osd0_yres;
	u32 osd0_xpos;
	u32 osd0_ypos;

	u32 osd1_xres;
	u32 osd1_yres;
	u32 osd1_xpos;
	u32 osd1_ypos;
} dmparams = {
	NTSC,		/* output */
	    COMPOSITE,		/* format */
	    (1 << VID0) | (1 << VID1) | (1 << OSD0) | (1 << OSD1),
	    /* windows registered */
	    720, 480, 0, 0,	/* vid0 size and position */
	    720, 480, 0, 0,	/* vid1 size and position */
	    720, 480, 0, 0,	/* osd0 size and position */
	    720, 480, 0, 0,	/* osd1 size and position */
};

/* Must do checks against the limits of the output device */
static int davincifb_venc_check_mode(const struct dm_win_info *w,
				     const struct fb_var_screeninfo *var)
{
	return 0;
}

static void set_sdram_params(char *id, u32 addr, u32 line_length);
static irqreturn_t davincifb_isr(int irq, void *arg)
{
	struct dm_info *dm = (struct dm_info *)arg;
	unsigned long addr=0;

	if ((dispc_reg_in(VENC_VSTAT) & 0x00000010) == 0x10) {
		xchg(&addr, dm->osd0->sdram_address);
		if (addr) {
			set_sdram_params(dm->osd0->info.fix.id,
					 dm->osd0->sdram_address,
					 dm->osd0->info.fix.line_length);
			dm->osd0->sdram_address = 0;
		}
		addr = 0;
		xchg(&addr, dm->osd1->sdram_address);
		if (addr) {
			set_sdram_params(dm->osd1->info.fix.id,
					 dm->osd1->sdram_address,
					 dm->osd1->info.fix.line_length);
			dm->osd1->sdram_address = 0;
		}
		addr = 0;
		xchg(&addr, dm->vid0->sdram_address);
		if (addr) {
			set_sdram_params(dm->vid0->info.fix.id,
					 dm->vid0->sdram_address,
					 dm->vid0->info.fix.line_length);
			dm->vid0->sdram_address = 0;
		}
		addr = 0;
		xchg(&addr, dm->vid1->sdram_address);
		if (addr) {
			set_sdram_params(dm->vid1->info.fix.id,
					 dm->vid1->sdram_address,
					 dm->vid1->info.fix.line_length);
			dm->vid1->sdram_address = 0;
		}
		return IRQ_HANDLED;
	} else {
		++dm->vsync_cnt;
		wake_up_interruptible(&dm->vsync_wait);
		return IRQ_HANDLED;
  	}

	return IRQ_HANDLED;
}

/* Wait for a vsync interrupt.  This routine sleeps so it can only be called
 * from process context.
 */
static int davincifb_wait_for_vsync(struct dm_win_info *w)
{
	struct dm_info *dm = w->dm;
	wait_queue_t wq;
	unsigned long cnt;
	int ret;

	init_waitqueue_entry(&wq, current);

	cnt = dm->vsync_cnt;
	ret = wait_event_interruptible_timeout(dm->vsync_wait,
					       cnt != dm->vsync_cnt,
					       dm->timeout);
	if (ret < 0)
		return (ret);
	if (ret == 0)
		return (-ETIMEDOUT);

	return (0);
}

/* Sets a uniform attribute value over a rectangular area on the attribute
 * window. The attribute value (0 to 7) is passed through the fb_fillrect's
 * color parameter.
 */
static int davincifb_set_attr_blend(struct fb_fillrect *r)
{
	struct fb_info *info = &dm->osd1->info;
	struct fb_var_screeninfo *var = &dm->osd1->info.var;
	unsigned long start = 0;
	u8 blend;
	u32 width_bytes;

	if (r->dx + r->width > var->xres_virtual)
		return -EINVAL;
	if (r->dy + r->height > var->yres_virtual)
		return -EINVAL;
	if (r->color < 0 || r->color > 7)
		return -EINVAL;

	/* since bits_per_pixel = 4, this will truncate the width if it is
	 * not even. Similarly r->dx will be rounded down to an even pixel.
	 * ... Do we want to return an error otherwise?
	 */
	width_bytes = r->width * var->bits_per_pixel / 8;
	start = dm->osd1->fb_base + r->dy * info->fix.line_length
	    + r->dx * var->bits_per_pixel / 8;

	blend = (((u8) r->color & 0xf) << 4) | ((u8) r->color);
	while (r->height--) {
		start += info->fix.line_length;
		memset((void *)start, blend, width_bytes);
	}

	return 0;
}

/* These position parameters are given through fb_var_screeninfo.
 * xp = var.reserved[0], yp = var.reserved[1],
 * xl = var.xres, yl = var.yres
 */
static void set_win_position(char *id, u32 xp, u32 yp, u32 xl, u32 yl)
{
	int i = 0;

	if (is_win(id, VID0)) {
		i = 0;
	} else if (is_win(id, VID1)) {
		i = 1;
	} else if (is_win(id, OSD0)) {
		i = 2;
	} else if (is_win(id, OSD1)) {
		i = 3;
	}

	dispc_reg_out(OSD_WINXP(i), xp);
	dispc_reg_out(OSD_WINYP(i), yp);
	dispc_reg_out(OSD_WINXL(i), xl);
	dispc_reg_out(OSD_WINYL(i), yl);
}

static inline void get_win_position(struct dm_win_info *w,
				    u32 * xp, u32 * yp, u32 * xl, u32 * yl)
{
	struct fb_var_screeninfo *v = &w->info.var;

	*xp = x_pos(w);
	*yp = y_pos(w);
	*xl = v->xres;
	*yl = v->yres;
}

/* Returns 1 if the windows overlap, 0 otherwise */
static int window_overlap(struct dm_win_info *w, u32 xp, u32 yp, u32 xl, u32 yl)
{
	u32 _xp = 0, _yp = 0, _xl = 0, _yl = 0;

#define OVERLAP(x1, y1, x2, y2, x3, y3, x4, y4)		\
(!(	((x1)<(x3) && (x2)<(x3)) || ((x1)>(x4) && (x2)>(x4)) ||	\
	((y1)<(y3) && (y2)<(y3)) || ((y1)>(y4) && (y2)>(y4)) )	\
)

	if (!w)
		return (0);

	get_win_position(w, &_xp, &_yp, &_xl, &_yl);

	return (OVERLAP(xp, yp, xp + xl, yp + yl,
		       _xp, _yp, _xp + _xl, _yp + _yl));
#undef OVERLAP
}

/* Returns 1 if the window parameters are within VID0, 0 otherwise */
static int within_vid0_limits(u32 xp, u32 yp, u32 xl, u32 yl)
{
	u32 vid0_xp = 0, vid0_yp = 0, vid0_xl = 0, vid0_yl = 0;

	if (!dm->vid0)
		return (1);
	get_win_position(dm->vid0, &vid0_xp, &vid0_yp, &vid0_xl, &vid0_yl);
	if ((xp >= vid0_xp) && (yp >= vid0_yp) &&
	    (xp + xl <= vid0_xp + vid0_xl) && (yp + yl <= vid0_yp + vid0_yl))
		return (1);
	return (0);
}

/* VID0 must be large enough to hold all other windows */
static int check_new_vid0_size(u32 xp0, u32 yp0, u32 xl0, u32 yl0)
{
	u32 _xp = 0, _yp = 0, _xl = 0, _yl = 0;
#define WITHIN_LIMITS 				\
	((_xp >= xp0) && (_yp >= yp0) &&	\
	(_xp + _xl <= xp0 + xl0) && (_yp + _yl <= yp0 + yl0))

	if (dm->osd0) {
		get_win_position(dm->osd0, &_xp, &_yp, &_xl, &_yl);
		if (!WITHIN_LIMITS)
			return (-EINVAL);
	}
	if (dm->osd1) {
		get_win_position(dm->osd1, &_xp, &_yp, &_xl, &_yl);
		if (!WITHIN_LIMITS)
			return (-EINVAL);
	}
	if (dm->vid1) {
		get_win_position(dm->vid1, &_xp, &_yp, &_xl, &_yl);
		if (!WITHIN_LIMITS)
			return (-EINVAL);
	}
	return (0);

#undef WITHIN_LIMITS
}

/**
 *      davincifb_check_var - Validates a var passed in.
 *      @var: frame buffer variable screen structure
 *      @info: frame buffer structure that represents a single frame buffer
 *
 *	Checks to see if the hardware supports the state requested by
 *	var passed in. This function does not alter the hardware state!!!
 *	This means the data stored in struct fb_info and struct xxx_par do
 *      not change. This includes the var inside of struct fb_info.
 *	Do NOT change these. This function can be called on its own if we
 *	intent to only test a mode and not actually set it.
 *	If the var passed in is slightly off by what the hardware can support
 *	then we alter the var PASSED in to what we can do.
 *
 *	Returns negative errno on error, or zero on success.
 */
static int davincifb_check_var(struct fb_var_screeninfo *var,
			       struct fb_info *info)
{
	const struct dm_win_info *w = (const struct dm_win_info *)info->par;
	struct fb_var_screeninfo v;

/* Rules:
 * 1) Vid1, OSD0, OSD1 and Cursor must be fully contained inside of Vid0.
 * 2) Vid0 and Vid1 are both set to accept YUV 4:2:2 (for now).
 * 3) OSD window data is always packed into 32-bit words and left justified.
 * 4) Each horizontal line of window data must be a multiple of 32 bytes.
 *    32 bytes = 32 bytes / 2 bytes per pixel = 16 pixels.
 *    This implies that 'xres' must be a multiple of 32 bytes.
 * 5) The offset registers hold the distance between the start of one line and
 *    the start of the next. This offset value must be a multiple of 32 bytes.
 *    This implies that 'xres_virtual' is also a multiple of 32 bytes. Note
 *    that 'xoffset' needn't be a multiple of 32 bytes.
 * 6) OSD0 is set to accept RGB565.
 * 	dispc_reg_merge(OSD_OSDWIN0ND, OSD_OSDWIN0ND_RGB0E, OSD_OSDWIN0ND_RGB0E)
 * 7) OSD1 is set to be the attribute window.
 * 8) Vid1 startX = Vid0 startX + N * 16 pixels (32 bytes)
 * 9) Vid1 width = (16*N - 8) pixels
 * 10) When one of the OSD windows is in RGB565, it cannot overlap with Vid1.
 * 11) Vid1 start X position must be offset a multiple of 16 pixels from the
 * left edge of Vid0.
 */

	memcpy(&v, var, sizeof(v));
	return (0);

	/* do board-specific checks on the var */
	if (davincifb_venc_check_mode(w, &v))
		return (-EINVAL);

	if (v.xres_virtual < v.xres || v.yres_virtual < v.yres)
		return (-EINVAL);
	if (v.xoffset > v.xres_virtual - v.xres)
		return (-EINVAL);
	if (v.yoffset > v.yres_virtual - v.yres)
		return (-EINVAL);
	if ((v.xres * v.bits_per_pixel / 8) % 32 || (v.xres_virtual * v.bits_per_pixel / 8) % 32)	/* Rules 4, 5 */
		return (-EINVAL);
	if (v.xres_virtual * v.yres_virtual * v.bits_per_pixel / 8 > w->fb_size)
		return (-EINVAL);

	if (!is_win(info->fix.id, VID0)) {
		/* Rule 1 */
		if (!within_vid0_limits(x_pos(w), y_pos(w), v.xres, v.yres))
			return (-EINVAL);
	}
	if (is_win(info->fix.id, OSD0)) {
		/* Rule 10 */
		if (window_overlap(w->dm->vid1,
				   x_pos(w), y_pos(w), v.xres, v.yres))
			return (-EINVAL);
		/* Rule 5 */
		v.bits_per_pixel = 16;
		v.red.offset = 11;
		v.green.offset = 5;
		v.blue.offset = 0;
		v.red.length = 5;
		v.green.length = 6;
		v.blue.length = 5;
		v.transp.offset = v.transp.length = 0;
		v.red.msb_right = v.green.msb_right = v.blue.msb_right
		    = v.transp.msb_right = 0;
		v.nonstd = 0;
		v.accel_flags = 0;
	} else if (is_win(info->fix.id, OSD1)) {
		v.bits_per_pixel = 4;
	} else if (is_win(info->fix.id, VID0)) {
		if (check_new_vid0_size(x_pos(w), y_pos(w), v.xres, v.yres))
			return (-EINVAL);
		v.bits_per_pixel = 16;
	} else if (is_win(info->fix.id, VID1)) {
		/* Rule 11 */
		if ((x_pos(w->dm->vid0) - x_pos(w)) % 16)
			return (-EINVAL);
		/* Video1 may be in YUV or RGB888 format */
		if ((v.bits_per_pixel != 16) && (v.bits_per_pixel != 32))
			return (-EINVAL);
	} else
		return (-EINVAL);

	memcpy(var, &v, sizeof(v));
	return (0);
}

/* Interlaced = Frame mode, Non-interlaced = Field mode */
static void set_interlaced(char *id, unsigned int on)
{
	on = (on == 0) ? 0 : ~0;

	if (is_win(id, VID0))
		dispc_reg_merge(OSD_VIDWINMD, on, OSD_VIDWINMD_VFF0);
	else if (is_win(id, VID1))
		dispc_reg_merge(OSD_VIDWINMD, on, OSD_VIDWINMD_VFF1);
	else if (is_win(id, OSD0))
		dispc_reg_merge(OSD_OSDWIN0MD, on, OSD_OSDWIN0MD_OFF0);
	else if (is_win(id, OSD1))
		dispc_reg_merge(OSD_OSDWIN1MD, on, OSD_OSDWIN1MD_OFF1);
}

/* For zooming, we just have to set the start of framebuffer, the zoom factors
 * and the display size. The hardware will then read only
 * (display size / zoom factor) area of the framebuffer and  zoom and
 * display it. In the following function, we assume that the start of
 * framebuffer and the display size parameters are set already.
 */
static void set_zoom(int WinID, int h_factor, int v_factor)
{
	switch (WinID) {
	case 0:		//VID0
		dispc_reg_merge(OSD_VIDWINMD,
				h_factor << OSD_VIDWINMD_VHZ0_SHIFT,
				OSD_VIDWINMD_VHZ0);
		dispc_reg_merge(OSD_VIDWINMD,
				v_factor << OSD_VIDWINMD_VVZ0_SHIFT,
				OSD_VIDWINMD_VVZ0);
		break;
	case 1:		//VID1
		dispc_reg_merge(OSD_VIDWINMD,
				h_factor << OSD_VIDWINMD_VHZ1_SHIFT,
				OSD_VIDWINMD_VHZ1);
		dispc_reg_merge(OSD_VIDWINMD,
				v_factor << OSD_VIDWINMD_VVZ1_SHIFT,
				OSD_VIDWINMD_VVZ1);
		break;
	case 2:		//OSD0
		dispc_reg_merge(OSD_OSDWIN0MD,
				h_factor << OSD_OSDWIN0MD_OHZ0_SHIFT,
				OSD_OSDWIN0MD_OHZ0);
		dispc_reg_merge(OSD_OSDWIN0MD,
				v_factor << OSD_OSDWIN0MD_OVZ0_SHIFT,
				OSD_OSDWIN0MD_OVZ0);
		break;
	case 3:
		dispc_reg_merge(OSD_OSDWIN1MD,
				h_factor << OSD_OSDWIN1MD_OHZ1_SHIFT,
				OSD_OSDWIN1MD_OHZ1);
		dispc_reg_merge(OSD_OSDWIN1MD,
				v_factor << OSD_OSDWIN1MD_OVZ1_SHIFT,
				OSD_OSDWIN1MD_OVZ1);
		break;
	}
}

/* Chooses the ROM CLUT for now. Can be extended later. */
static void set_bg_color(u8 clut, u8 color_offset)
{
	clut = 0;		/* 0 = ROM, 1 = RAM */

	dispc_reg_merge(OSD_MODE, OSD_MODE_BCLUT & clut, OSD_MODE_BCLUT);
	dispc_reg_merge(OSD_MODE, color_offset << OSD_MODE_CABG_SHIFT,
			OSD_MODE_CABG);
}

static void set_sdram_params(char *id, u32 addr, u32 line_length)
{
	/* The parameters to be written to the registers should be in
	 * multiple of 32 bytes
	 */
	addr = addr;		/* div by 32 */
	line_length = line_length / 32;

	if (is_win(id, VID0)) {
		dispc_reg_out(OSD_VIDWIN0ADR, addr);
		dispc_reg_out(OSD_VIDWIN0OFST, line_length);
	} else if (is_win(id, VID1)) {
		dispc_reg_out(OSD_VIDWIN1ADR, addr);
		dispc_reg_out(OSD_VIDWIN1OFST, line_length);
	} else if (is_win(id, OSD0)) {
		dispc_reg_out(OSD_OSDWIN0ADR, addr);
		dispc_reg_out(OSD_OSDWIN0OFST, line_length);
	} else if (is_win(id, OSD1)) {
		dispc_reg_out(OSD_OSDWIN1ADR, addr);
		dispc_reg_out(OSD_OSDWIN1OFST, line_length);
	}
}

static void set_win_enable(char *id, unsigned int on)
{
	on = (on == 0) ? 0 : ~0;

	if (is_win(id, VID0))
		/* Turning off VID0 use due to field inversion issue */
		dispc_reg_merge(OSD_VIDWINMD, 0, OSD_VIDWINMD_ACT0);
	else if (is_win(id, VID1))
		dispc_reg_merge(OSD_VIDWINMD, on, OSD_VIDWINMD_ACT1);
	else if (is_win(id, OSD0))
		dispc_reg_merge(OSD_OSDWIN0MD, on, OSD_OSDWIN0MD_OACT0);
	else if (is_win(id, OSD1)) {
		/* The OACT1 bit is applicable only if OSD1 is not used as
		 * the attribute window
		 */
		if (!(dispc_reg_in(OSD_OSDWIN1MD) & OSD_OSDWIN1MD_OASW))
			dispc_reg_merge(OSD_OSDWIN1MD, on, OSD_OSDWIN1MD_OACT1);
	}
}

static void set_win_mode(char *id)
{
	if (is_win(id, VID0)) ;
	else if (is_win(id, VID1)) {
		if (dm->vid1->info.var.bits_per_pixel == 32)
			dispc_reg_merge(OSD_MISCCT, ~0,
					OSD_MISCCT_RGBWIN | OSD_MISCCT_RGBEN);
	} else if (is_win(id, OSD0))
		/* Set RGB565 mode */
		dispc_reg_merge(OSD_OSDWIN0MD, OSD_OSDWIN0MD_RGB0E,
				OSD_OSDWIN0MD_RGB0E);
	else if (is_win(id, OSD1)) {
		/* Set as attribute window */
		dispc_reg_merge(OSD_OSDWIN1MD, OSD_OSDWIN1MD_OASW,
				OSD_OSDWIN1MD_OASW);
	}

}

/**
 *      davincifb_set_par - Optional function. Alters the hardware state.
 *      @info: frame buffer structure that represents a single frame buffer
 *
 *	Using the fb_var_screeninfo in fb_info we set the resolution of the
 *	this particular framebuffer. This function alters the par AND the
 *	fb_fix_screeninfo stored in fb_info. It doesn't not alter var in
 *	fb_info since we are using that data. This means we depend on the
 *	data in var inside fb_info to be supported by the hardware.
 *	davincifb_check_var is always called before dmfb_set_par to ensure this.
 *	Again if you can't can't the resolution you don't need this function.
 *
 */
static int davincifb_set_par(struct fb_info *info)
{
	struct dm_win_info *w = (struct dm_win_info *)info->par;
	struct fb_var_screeninfo *v = &info->var;
	u32 start = 0, offset = 0;

	info->fix.line_length = v->xres_virtual * v->bits_per_pixel / 8;

	offset = v->yoffset * info->fix.line_length +
	    v->xoffset * v->bits_per_pixel / 8;
	start = (u32) w->fb_base_phys + offset;
	set_sdram_params(info->fix.id, start, info->fix.line_length);

	set_interlaced(info->fix.id, 1);
	set_win_position(info->fix.id,
			 x_pos(w), y_pos(w), v->xres, v->yres / 2);
	set_win_mode(info->fix.id);
	set_win_enable(info->fix.id, 1);

	return (0);
}

/**
 *	davincifb_ioctl - handler for private ioctls.
 */
static int davincifb_ioctl(struct fb_info *info, unsigned int cmd,
			   unsigned long arg)
{
	struct dm_win_info *w = (struct dm_win_info *)info->par;
	void __user *argp = (void __user *)arg;
	struct fb_fillrect rect;
	struct zoom_params zoom;
	long std = 0;

	switch (cmd) {
	case FBIO_WAITFORVSYNC:
		/* This ioctl accepts an integer argument to specify a
		 * display.  We only support one display, so we will
		 * simply ignore the argument.
		 */
		return (davincifb_wait_for_vsync(w));
		break;
	case FBIO_SETATTRIBUTE:
		if (copy_from_user(&rect, argp, sizeof(rect)))
			return -EFAULT;
		return (davincifb_set_attr_blend(&rect));
		break;
	case FBIO_SETPOSX:
		if (arg >= 0 && arg <= DISP_XRES) {
			w->x = arg;
			davincifb_check_var(&w->info.var, &w->info);
			davincifb_set_par(&w->info);
			return 0;
		} else
			return -EINVAL;
		break;
	case FBIO_SETPOSY:
		if (arg >= 0 && arg <= DISP_YRES) {
			w->y = arg;
			davincifb_check_var(&w->info.var, &w->info);
			davincifb_set_par(&w->info);
			return 0;
		} else
			return -EINVAL;
		break;
	case FBIO_SETZOOM:
		if (copy_from_user(&zoom, argp, sizeof(zoom)))
			return -EFAULT;
		if ((zoom.zoom_h == 2) || (zoom.zoom_h == 0)
		    || (zoom.zoom_h == 1) || (zoom.zoom_v == 2)
		    || (zoom.zoom_v == 0) || (zoom.zoom_v == 1)) {
			set_zoom(zoom.window_id, zoom.zoom_h, zoom.zoom_v);
			return 0;
		} else {
			return -EINVAL;
		}
		break;
	case FBIO_GETSTD:
		std = ((dmparams.output << 16) | (dmparams.format));	//(NTSC <<16) | (COPOSITE);
		if (copy_to_user(argp, &std, sizeof(u_int32_t)))
			return -EFAULT;
		return 0;
		break;
	}
	return (-EINVAL);
}

/**
 *  	davincifb_setcolreg - Optional function. Sets a color register.
 *      @regno: Which register in the CLUT we are programming
 *      @red: The red value which can be up to 16 bits wide
 *	@green: The green value which can be up to 16 bits wide
 *	@blue:  The blue value which can be up to 16 bits wide.
 *	@transp: If supported the alpha value which can be up to 16 bits wide.
 *      @info: frame buffer info structure
 *
 *  	Set a single color register. The values supplied have a 16 bit
 *  	magnitude which needs to be scaled in this function for the hardware.
 *	Things to take into consideration are how many color registers, if
 *	any, are supported with the current color visual. With truecolor mode
 *	no color palettes are supported. Here a psuedo palette is created
 *	which we store the value in pseudo_palette in struct fb_info. For
 *	pseudocolor mode we have a limited color palette. To deal with this
 *	we can program what color is displayed for a particular pixel value.
 *	DirectColor is similar in that we can program each color field. If
 *	we have a static colormap we don't need to implement this function.
 *
 *	Returns negative errno on error, or zero on success.
 */
static int davincifb_setcolreg(unsigned regno, unsigned red, unsigned green,
			       unsigned blue, unsigned transp,
			       struct fb_info *info)
{
	/* only pseudo-palette (16 bpp) allowed */
	if (regno >= 16)	/* maximum number of palette entries */
		return (1);

	if (info->var.grayscale) {
		/* grayscale = 0.30*R + 0.59*G + 0.11*B */
		red = green = blue = (red * 77 + green * 151 + blue * 28) >> 8;
	}

	/* Truecolor has hardware-independent 16-entry pseudo-palette */
	if (info->fix.visual == FB_VISUAL_TRUECOLOR) {
		u32 v;

		if (regno >= 16)
			return (1);

		red >>= (16 - info->var.red.length);
		green >>= (16 - info->var.green.length);
		blue >>= (16 - info->var.blue.length);

		v = (red << info->var.red.offset) |
		    (green << info->var.green.offset) |
		    (blue << info->var.blue.offset);

		switch (info->var.bits_per_pixel) {
		case 16:
			((u16 *) (info->pseudo_palette))[regno] = v;
			break;
		default:
			return (1);
		}
		return (0);
	}
	return (0);
}

/**
 *      davincifb_pan_display - NOT a required function. Pans the display.
 *      @var: frame buffer variable screen structure
 *      @info: frame buffer structure that represents a single frame buffer
 *
 *	Pan (or wrap, depending on the `vmode' field) the display using the
 *  	`xoffset' and `yoffset' fields of the `var' structure.
 *  	If the values don't fit, return -EINVAL.
 *
 *      Returns negative errno on error, or zero on success.
 */
static int davincifb_pan_display(struct fb_var_screeninfo *var,
				 struct fb_info *info)
{
	struct dm_win_info *w = (struct dm_win_info *)info->par;
	u32 start = 0, offset = 0;

	if (var->xoffset > var->xres_virtual - var->xres)
		return (-EINVAL);
	if (var->yoffset > var->yres_virtual - var->yres)
		return (-EINVAL);
	if ((var->xres_virtual * var->bits_per_pixel / 8) % 32)
		return (-EINVAL);

	offset = var->yoffset * info->fix.line_length +
	    var->xoffset * var->bits_per_pixel / 8;
	start = (u32) w->fb_base_phys + offset;

	if ((dispc_reg_in(VENC_VSTAT) & 0x00000010)==0x10)
		set_sdram_params(info->fix.id, start, info->fix.line_length);
	else
		w->sdram_address = start;

	return (0);
}

/**
 *      davincifb_blank - NOT a required function. Blanks the display.
 *      @blank_mode: the blank mode we want.
 *      @info: frame buffer structure that represents a single frame buffer
 *
 *      Blank the screen if blank_mode != 0, else unblank. Return 0 if
 *      blanking succeeded, != 0 if un-/blanking failed due to e.g. a
 *      video mode which doesn't support it. Implements VESA suspend
 *      and powerdown modes on hardware that supports disabling hsync/vsync:
 *      blank_mode == 2: suspend vsync
 *      blank_mode == 3: suspend hsync
 *      blank_mode == 4: powerdown
 *
 *      Returns negative errno on error, or zero on success.
 *
 */
static int davincifb_blank(int blank_mode, struct fb_info *info)
{
	return 0;
}

static int parse_win_params(char *wp,
			    int *xres, int *yres, int *xpos, int *ypos)
{
	char *s;

	if ((s = strsep(&wp, "x")) == NULL)
		return -EINVAL;
	*xres = simple_strtoul(s, NULL, 0);

	if ((s = strsep(&wp, "@")) == NULL)
		return -EINVAL;
	*yres = simple_strtoul(s, NULL, 0);

	if ((s = strsep(&wp, ",")) == NULL)
		return -EINVAL;
	*xpos = simple_strtoul(s, NULL, 0);

	if ((s = strsep(&wp, ":")) == NULL)
		return -EINVAL;
	*ypos = simple_strtoul(s, NULL, 0);

	return 0;
}

/*
 * Pass boot-time options by adding the following string to the boot params:
 * 	video=davincifb:[option[:option]]
 * Valid options:
 * 	output=[lcd|ntsc|pal]
 * 	format=[composite|s-video|component|rgb]
 * 	vid0=[off|MxN@X,Y]
 * 	vid1=[off|MxN@X,Y]
 * 	osd0=[off|MxN@X,Y]
 * 	osd1=[off|MxN@X,Y]
 * 		MxN specify the window resolution (displayed size)
 * 		X,Y specify the window position
 * 		M, N, X, Y are integers
 * 		M, X should be multiples of 16
 */

#ifndef MODULE
int __init davincifb_setup(char *options)
{
	char *this_opt;
	u32 xres, yres, xpos, ypos;
	int format_yres = 480;

	pr_debug("davincifb: Options \"%s\"\n", options);

	if (!options || !*options)
		return 0;

	while ((this_opt = strsep(&options, ":")) != NULL) {

		if (!*this_opt)
			continue;

		if (!strncmp(this_opt, "output=", 7)) {
			if (!strncmp(this_opt + 7, "lcd", 3)) {
				dmparams.output = LCD;
				dmparams.format = 0;
			} else if (!strncmp(this_opt + 7, "ntsc", 4))
				dmparams.output = NTSC;
			else if (!strncmp(this_opt + 7, "pal", 3))
				dmparams.output = PAL;
		} else if (!strncmp(this_opt, "format=", 7)) {
			if (dmparams.output == LCD)
				continue;
			if (!strncmp(this_opt + 7, "composite", 9))
				dmparams.format = COMPOSITE;
			else if (!strncmp(this_opt + 7, "s-video", 7))
				dmparams.format = SVIDEO;
			else if (!strncmp(this_opt + 7, "component", 9))
				dmparams.format = COMPONENT;
			else if (!strncmp(this_opt + 7, "rgb", 3))
				dmparams.format = RGB;
		} else if (!strncmp(this_opt, "vid0=", 5)) {
			if (!strncmp(this_opt + 5, "off", 3))
				dmparams.windows &= ~(1 << VID0);
			else if (!parse_win_params(this_opt + 5,
						   &xres, &yres, &xpos,
						   &ypos)) {
				dmparams.vid0_xres = xres;
				dmparams.vid0_yres = yres;
				dmparams.vid0_xpos = xpos;
				dmparams.vid0_ypos = ypos;
			}
		} else if (!strncmp(this_opt, "vid1=", 5)) {
			if (!strncmp(this_opt + 5, "off", 3))
				dmparams.windows &= ~(1 << VID1);
			else if (!parse_win_params(this_opt + 5,
						   &xres, &yres, &xpos,
						   &ypos)) {
				dmparams.vid1_xres = xres;
				dmparams.vid1_yres = yres;
				dmparams.vid1_xpos = xpos;
				dmparams.vid1_ypos = ypos;
			}
		} else if (!strncmp(this_opt, "osd0=", 5)) {
			if (!strncmp(this_opt + 5, "off", 3))
				dmparams.windows &= ~(1 << OSD0);
			else if (!parse_win_params(this_opt + 5,
						   &xres, &yres, &xpos,
						   &ypos)) {
				dmparams.osd0_xres = xres;
				dmparams.osd0_yres = yres;
				dmparams.osd0_xpos = xpos;
				dmparams.osd0_ypos = ypos;
			}
		} else if (!strncmp(this_opt, "osd1=", 5)) {
			if (!strncmp(this_opt + 5, "off", 3))
				dmparams.windows &= ~(1 << OSD1);
			else if (!parse_win_params(this_opt + 5,
						   &xres, &yres, &xpos,
						   &ypos)) {
				dmparams.osd1_xres = xres;
				dmparams.osd1_yres = yres;
				dmparams.osd1_xpos = xpos;
				dmparams.osd1_ypos = ypos;
			}
		}
	}
	printk(KERN_INFO "DaVinci: "
	       "Output on %s%s, Enabled windows: %s %s %s %s\n",
	       (dmparams.output == LCD) ? "LCD" :
	       (dmparams.output == NTSC) ? "NTSC" :
	       (dmparams.output == PAL) ? "PAL" : "unknown device!",
	       (dmparams.format == 0) ? "" :
	       (dmparams.format == COMPOSITE) ? " in COMPOSITE format" :
	       (dmparams.format == SVIDEO) ? " in SVIDEO format" :
	       (dmparams.format == COMPONENT) ? " in COMPONENT format" :
	       (dmparams.format == RGB) ? " in RGB format" : "",
	       (dmparams.windows & (1 << VID0)) ? "Video0" : "",
	       (dmparams.windows & (1 << VID1)) ? "Video1" : "",
	       (dmparams.windows & (1 << OSD0)) ? "OSD0" : "",
	       (dmparams.windows & (1 << OSD1)) ? "OSD1" : "");
	if (dmparams.output == NTSC) {
		format_yres = 480;
	} else if (dmparams.output == PAL) {
		format_yres = 576;
	} else {
		printk(KERN_INFO
		       "DaVinci:invalid format..defaulting width to 480\n");
	}
	dmparams.osd0_yres = osd0_default_var.yres = format_yres;
	dmparams.osd1_yres = osd1_default_var.yres = format_yres;
	dmparams.vid0_yres = vid0_default_var.yres = format_yres;
	dmparams.vid1_yres = vid1_default_var.yres = format_yres;

	osd0_default_var.yres_virtual = format_yres * DOUBLE_BUF;
	osd1_default_var.yres_virtual = format_yres * DOUBLE_BUF;
	vid0_default_var.yres_virtual = format_yres * TRIPLE_BUF;
	vid1_default_var.yres_virtual = format_yres * TRIPLE_BUF;

	if (dmparams.windows & (1 << VID0))
		printk(KERN_INFO "Setting Video0 size %dx%d, "
		       "position (%d,%d)\n",
		       dmparams.vid0_xres, dmparams.vid0_yres,
		       dmparams.vid0_xpos, dmparams.vid0_ypos);
	if (dmparams.windows & (1 << VID1))
		printk(KERN_INFO "Setting Video1 size %dx%d, "
		       "position (%d,%d)\n",
		       dmparams.vid1_xres, dmparams.vid1_yres,
		       dmparams.vid1_xpos, dmparams.vid1_ypos);
	if (dmparams.windows & (1 << OSD0))
		printk(KERN_INFO "Setting OSD0 size %dx%d, "
		       "position (%d,%d)\n",
		       dmparams.osd0_xres, dmparams.osd0_yres,
		       dmparams.osd0_xpos, dmparams.osd0_ypos);
	if (dmparams.windows & (1 << OSD1))
		printk(KERN_INFO "Setting OSD1 size %dx%d, "
		       "position (%d,%d)\n",
		       dmparams.osd1_xres, dmparams.osd1_yres,
		       dmparams.osd1_xpos, dmparams.osd1_ypos);
	return (0);
}
#endif

static int mem_release(struct dm_win_info *w)
{
	if (!w->alloc_fb_mem) {
		iounmap((void *)w->fb_base);
		release_mem_region(w->fb_base_phys, w->fb_size);
	} else
		dma_free_coherent(NULL, w->fb_size, (void *)w->fb_base,
				  w->fb_base_phys);
	kfree(w);
	return (0);
}

static int mem_alloc(struct dm_win_info **win, dma_addr_t fb_base_phys,
		     unsigned long fb_size, char *fbname)
{
	struct dm_win_info *w;
	struct device *dev = dm->dev;

	w = kmalloc(sizeof(struct dm_win_info), GFP_KERNEL);
	if (!w) {
		dev_err(dev, "%s: could not allocate memory\n", fbname);
		return (-ENOMEM);
	}
	memset(w, 0, sizeof(struct dm_win_info));

	w->fb_base_phys = fb_base_phys;
	w->fb_size = fb_size;

	/* A null base address indicates that the framebuffer memory should be
	 * dynamically allocated.
	 */
	if (!w->fb_base_phys)
		w->alloc_fb_mem = 1;

	if (!w->alloc_fb_mem) {
		if (!request_mem_region(w->fb_base_phys, w->fb_size, fbname)) {
			dev_err(dev, "%s: cannot reserve FB region\n", fbname);
			goto free_par;
		}
		w->fb_base =
		    (unsigned long)ioremap(w->fb_base_phys, w->fb_size);
		if (!w->fb_base) {
			dev_err(dev, "%s: cannot map framebuffer\n", fbname);
			goto release_fb;
		}
	} else {
		/* allocate coherent memory for the framebuffer */
		w->fb_base = (unsigned long)dma_alloc_coherent(dev,
							       w->fb_size,
							       &w->fb_base_phys,
							       GFP_KERNEL |
							       GFP_DMA);
		if (!w->fb_base) {
			dev_err(dev, "%s: cannot allocate framebuffer\n",
				fbname);
			goto free_par;
		}

		dev_dbg(dev, "Framebuffer allocated at 0x%x "
			"mapped to 0x%x, size %dk\n",
			(unsigned)w->fb_base_phys, (unsigned)w->fb_base,
			(unsigned)w->fb_size / 1024);
	}

	*win = w;
	return (0);

      release_fb:
	if (!w->alloc_fb_mem)
		release_mem_region(w->fb_base_phys, w->fb_size);
      free_par:
	kfree(w);
	return (-ENOMEM);
}

static struct fb_info *init_fb_info(struct dm_win_info *w,
				    struct fb_var_screeninfo *var, char *id)
{
	struct fb_info *info = &(w->info);
	struct dm_info *dm = w->dm;

	/* initialize the fb_info structure */
	info->flags = FBINFO_DEFAULT;
	info->fbops = &davincifb_ops;
	info->screen_base = (char *)(w->fb_base);
	info->pseudo_palette = w->pseudo_palette;
	info->par = w;

	/* Initialize variable screeninfo.
	 * The variable screeninfo can be directly specified by the user
	 * via an ioctl.
	 */
	memcpy(&info->var, var, sizeof(info->var));
	info->var.activate = FB_ACTIVATE_NOW;

	/* Initialize fixed screeninfo.
	 * The fixed screeninfo cannot be directly specified by the user, but
	 * it may change to reflect changes to the var info.
	 */
	strlcpy(info->fix.id, id, sizeof(info->fix.id));
	info->fix.smem_start = w->fb_base_phys;
	info->fix.line_length =
	    (info->var.xres_virtual * info->var.bits_per_pixel) / 8;
	info->fix.smem_len = w->fb_size;
	info->fix.type = FB_TYPE_PACKED_PIXELS;
	info->fix.visual = (info->var.bits_per_pixel <= 8) ?
	    FB_VISUAL_PSEUDOCOLOR : FB_VISUAL_TRUECOLOR;
	info->fix.xpanstep = 0;
	info->fix.ypanstep = 1;
	info->fix.ywrapstep = 0;
	info->fix.type_aux = 0;
	info->fix.mmio_start = dm->mmio_base_phys;
	info->fix.mmio_len = dm->mmio_size;
	info->fix.accel = FB_ACCEL_NONE;
	w->sdram_address = 0;

	return info;
}

static void davincifb_ntsc_composite_config(int on)
{
	if (on) {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);

		/* Enable Composite output and start video encoder */
		dispc_reg_out(VENC_VMOD, (VENC_VMOD_VIE | VENC_VMOD_VENC));

		/* Set REC656 Mode */
		dispc_reg_out(VENC_YCCCTL, 0x1);

		/* Enable output mode and NTSC  */
		dispc_reg_out(VENC_VMOD, 0x1003);

		/* Enable all DACs  */
		dispc_reg_out(VENC_DACTST, 0);
	} else {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);
	}
}

static void davincifb_ntsc_svideo_config(int on)
{
	if (on) {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);

		/* Enable Composite output and start video encoder */
		dispc_reg_out(VENC_VMOD, (VENC_VMOD_VIE | VENC_VMOD_VENC));

		/* Set REC656 Mode */
		dispc_reg_out(VENC_YCCCTL, 0x1);

		/* Enable output mode and NTSC  */
		dispc_reg_out(VENC_VMOD, 0x1003);

		/* Enable S-Video Output; DAC B: S-Video Y, DAC C: S-Video C  */
		dispc_reg_out(VENC_DACSEL, 0x210);

		/* Enable all DACs  */
		dispc_reg_out(VENC_DACTST, 0);
	} else {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);
	}
}

static void davincifb_ntsc_component_config(int on)
{
	if (on) {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);

		/* Enable Composite output and start video encoder */
		dispc_reg_out(VENC_VMOD, (VENC_VMOD_VIE | VENC_VMOD_VENC));

		/* Set REC656 Mode */
		dispc_reg_out(VENC_YCCCTL, 0x1);

		/* Enable output mode and NTSC  */
		dispc_reg_out(VENC_VMOD, 0x1003);

		/* Enable Component output; DAC A: Y, DAC B: Pb, DAC C: Pr  */
		dispc_reg_out(VENC_DACSEL, 0x543);

		/* Enable all DACs  */
		dispc_reg_out(VENC_DACTST, 0);
	} else {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);
	}
}

static void davincifb_pal_composite_config(int on)
{
	if (on) {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);

		/* Enable Composite output and start video encoder */
		dispc_reg_out(VENC_VMOD, (VENC_VMOD_VIE | VENC_VMOD_VENC));

		/* Set REC656 Mode */
		dispc_reg_out(VENC_YCCCTL, 0x1);

		/* Enable output mode and PAL  */
		dispc_reg_out(VENC_VMOD, 0x1043);

		/* Enable all DACs  */
		dispc_reg_out(VENC_DACTST, 0);
	} else {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);
	}
}

static void davincifb_pal_svideo_config(int on)
{
	if (on) {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);

		/* Enable Composite output and start video encoder */
		dispc_reg_out(VENC_VMOD, (VENC_VMOD_VIE | VENC_VMOD_VENC));

		/* Set REC656 Mode */
		dispc_reg_out(VENC_YCCCTL, 0x1);

		/* Enable output mode and PAL  */
		dispc_reg_out(VENC_VMOD, 0x1043);

		/* Enable S-Video Output; DAC B: S-Video Y, DAC C: S-Video C  */
		dispc_reg_out(VENC_DACSEL, 0x210);

		/* Enable all DACs  */
		dispc_reg_out(VENC_DACTST, 0);
	} else {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);
	}
}

static void davincifb_pal_component_config(int on)
{
	if (on) {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);

		/* Enable Composite output and start video encoder */
		dispc_reg_out(VENC_VMOD, (VENC_VMOD_VIE | VENC_VMOD_VENC));

		/* Set REC656 Mode */
		dispc_reg_out(VENC_YCCCTL, 0x1);

		/* Enable output mode and PAL  */
		dispc_reg_out(VENC_VMOD, 0x1043);

		/* Enable Component output; DAC A: Y, DAC B: Pb, DAC C: Pr  */
		dispc_reg_out(VENC_DACSEL, 0x543);

		/* Enable all DACs  */
		dispc_reg_out(VENC_DACTST, 0);
	} else {
		/* Reset video encoder module */
		dispc_reg_out(VENC_VMOD, 0);
	}
}

static inline void fix_default_var(struct dm_win_info *w,
				   u32 xres, u32 yres, u32 xpos, u32 ypos,
				   int n_buf)
{
	struct fb_var_screeninfo *v = &w->info.var;

	v->xres = xres;
	v->yres = yres;
	v->xres_virtual = v->xres;
	v->yres_virtual = v->yres * n_buf;
	x_pos(w) = xpos;
	y_pos(w) = ypos;
}

/*
 *  Cleanup
 */
static int davincifb_remove(struct platform_device *pdev)
{
	free_irq(IRQ_VENCINT, &dm);

	/* Cleanup all framebuffers */
	if (dm->osd0) {
		unregister_framebuffer(&dm->osd0->info);
		mem_release(dm->osd0);
	}
	if (dm->osd1) {
		unregister_framebuffer(&dm->osd1->info);
		mem_release(dm->osd1);
	}
	if (dm->vid0) {
		unregister_framebuffer(&dm->vid0->info);
		mem_release(dm->vid0);
	}
	if (dm->vid1) {
		unregister_framebuffer(&dm->vid1->info);
		mem_release(dm->vid1);
	}

	/* Turn OFF the output device */
	dm->output_device_config(0);

	if (dm->mmio_base)
		iounmap((void *)dm->mmio_base);
	release_mem_region(dm->mmio_base_phys, dm->mmio_size);

	return 0;
}

/*
 *  Initialization
 */
static int davincifb_probe(struct platform_device *pdev)
{
	struct fb_info *info;

	if (dmparams.windows == 0)
		return 0;	/* user disabled all windows through bootargs */
	dm->dev = &pdev->dev;
	dm->mmio_base_phys = OSD_REG_BASE;
	dm->mmio_size = OSD_REG_SIZE;

	if (!request_mem_region
	    (dm->mmio_base_phys, dm->mmio_size, MODULE_NAME)) {
		dev_err(dm->dev, ": cannot reserve MMIO region\n");
		return (-ENODEV);
	}

	/* map the regions */
	dm->mmio_base =
	    (unsigned long)ioremap(dm->mmio_base_phys, dm->mmio_size);
	if (!dm->mmio_base) {
		dev_err(dm->dev, ": cannot map MMIO\n");
		goto release_mmio;
	}

	/* initialize the vsync wait queue */
	init_waitqueue_head(&dm->vsync_wait);
	dm->timeout = HZ / 5;

	if ((dmparams.output == NTSC) && (dmparams.format == COMPOSITE))
		dm->output_device_config = davincifb_ntsc_composite_config;
	else if ((dmparams.output == NTSC) && (dmparams.format == SVIDEO))
		dm->output_device_config = davincifb_ntsc_svideo_config;
	else if ((dmparams.output == NTSC) && (dmparams.format == COMPONENT))
		dm->output_device_config = davincifb_ntsc_component_config;
	else if ((dmparams.output == PAL) && (dmparams.format == COMPOSITE))
		dm->output_device_config = davincifb_pal_composite_config;
	else if ((dmparams.output == PAL) && (dmparams.format == SVIDEO))
		dm->output_device_config = davincifb_pal_svideo_config;
	else if ((dmparams.output == PAL) && (dmparams.format == COMPONENT))
		dm->output_device_config = davincifb_pal_component_config;
	/* Add support for other displays here */
	else {
		printk(KERN_WARNING "Unsupported output device!\n");
		dm->output_device_config = NULL;
	}

	printk("Setting Up Clocks for DM420 OSD\n");

	/* Initialize the VPSS Clock Control register */
	dispc_reg_out(VPSS_CLKCTL, 0x18);

	/* Set Base Pixel X and Base Pixel Y */
	dispc_reg_out(OSD_BASEPX, BASEX);
	dispc_reg_out(OSD_BASEPY, BASEY);

	/* Reset OSD registers to default. */
	dispc_reg_out(OSD_MODE, 0);
	dispc_reg_out(OSD_OSDWIN0MD, 0);

	/* Set blue background color */
	set_bg_color(0, 162);

	/* Field Inversion Workaround */
	dispc_reg_out(OSD_MODE, 0x200);

	/* Setup VID0 framebuffer */
	if (!(dmparams.windows & (1 << VID0))) {
		printk(KERN_WARNING "No video/osd windows will be enabled "
		       "because Video0 is disabled\n");
		return 0;	/* background will still be shown */
	}
	/* Setup VID0 framebuffer */
	if (!mem_alloc(&dm->vid0, VID0_FB_PHY, VID0_FB_SIZE, VID0_FBNAME)) {
		dm->vid0->dm = dm;
		fix_default_var(dm->vid0,
				dmparams.vid0_xres, dmparams.vid0_yres,
				dmparams.vid0_xpos, dmparams.vid0_ypos,
				TRIPLE_BUF);
		info = init_fb_info(dm->vid0, &vid0_default_var, VID0_FBNAME);
		if (davincifb_check_var(&info->var, info)) {
			dev_err(dm->dev, ": invalid default video mode\n");
			goto exit;
		}
		memset((void *)dm->vid0->fb_base, 0x88, dm->vid0->fb_size);
	} else
		goto exit;

	/* Setup OSD0 framebuffer */
	if ((dmparams.windows & (1 << OSD0)) &&
	    (!mem_alloc(&dm->osd0, OSD0_FB_PHY, OSD0_FB_SIZE, OSD0_FBNAME))) {
		dm->osd0->dm = dm;
		fix_default_var(dm->osd0,
				dmparams.osd0_xres, dmparams.osd0_yres,
				dmparams.osd0_xpos, dmparams.osd0_ypos,
				DOUBLE_BUF);
		info = init_fb_info(dm->osd0, &osd0_default_var, OSD0_FBNAME);
		if (davincifb_check_var(&info->var, info)) {
			dev_err(dm->dev, ": invalid default video mode\n");
			mem_release(dm->osd0);
		} else
			memset((void *)dm->osd0->fb_base, 0, dm->osd0->fb_size);
	}

	/* Setup OSD1 framebuffer */
	if ((dmparams.windows & (1 << OSD1)) &&
	    (!mem_alloc(&dm->osd1, OSD1_FB_PHY, OSD1_FB_SIZE, OSD1_FBNAME))) {
		dm->osd1->dm = dm;
		fix_default_var(dm->osd1,
				dmparams.osd1_xres, dmparams.osd1_yres,
				dmparams.osd1_xpos, dmparams.osd1_ypos,
				DOUBLE_BUF);
		info = init_fb_info(dm->osd1, &osd1_default_var, OSD1_FBNAME);
		if (davincifb_check_var(&info->var, info)) {
			dev_err(dm->dev, ": invalid default video mode\n");
			mem_release(dm->osd1);
		} else
			/* Set blend factor to show OSD windows */
			memset((void *)dm->osd1->fb_base, 0xff,
			       dm->osd1->fb_size);
	}

	/* Setup VID1 framebuffer */
	if ((dmparams.windows & (1 << VID1)) &&
	    (!mem_alloc(&dm->vid1, VID1_FB_PHY, VID1_FB_SIZE, VID1_FBNAME))) {
		dm->vid1->dm = dm;
		fix_default_var(dm->vid1,
				dmparams.vid1_xres, dmparams.vid1_yres,
				dmparams.vid1_xpos, dmparams.vid1_ypos,
				TRIPLE_BUF);
		info = init_fb_info(dm->vid1, &vid1_default_var, VID1_FBNAME);
		if (davincifb_check_var(&info->var, info)) {
			dev_err(dm->dev,
				VID1_FBNAME ": invalid default video mode\n");
			mem_release(dm->vid1);
		} else
			memset((void *)dm->vid1->fb_base, 0x88,
			       dm->vid1->fb_size);
	}

	/* Register OSD0 framebuffer */
	if (dm->osd0) {
		info = &dm->osd0->info;
		if (register_framebuffer(info) < 0) {
			dev_err(dm->dev, OSD0_FBNAME
				"Unable to register OSD0 framebuffer\n");
			mem_release(dm->osd0);
		} else {
			printk(KERN_INFO "fb%d: %s frame buffer device\n",
			       info->node, info->fix.id);
			davincifb_set_par(info);
		}
	}

	/* Register VID0 framebuffer */
	info = &dm->vid0->info;
	if (register_framebuffer(info) < 0) {
		dev_err(dm->dev,
			VID0_FBNAME "Unable to register VID0 framebuffer\n");
		goto exit;
	} else {
		printk(KERN_INFO "fb%d: %s frame buffer device\n",
		       info->node, info->fix.id);
		davincifb_set_par(info);
	}

	/* Register OSD1 framebuffer */
	if (dm->osd1) {
		info = &dm->osd1->info;
		if (register_framebuffer(info) < 0) {
			dev_err(dm->dev, OSD1_FBNAME
				"Unable to register OSD1 framebuffer\n");
			mem_release(dm->osd1);
		} else {
			printk(KERN_INFO "fb%d: %s frame buffer device\n",
			       info->node, info->fix.id);
			davincifb_set_par(info);
		}
	}

	/* Register VID1 framebuffer */
	if (dm->vid1) {
		info = &dm->vid1->info;
		if (register_framebuffer(info) < 0) {
			mem_release(dm->vid1);
			dev_err(dm->dev, VID1_FBNAME
				"Unable to register VID1 framebuffer\n");
			mem_release(dm->vid1);
		} else {
			printk(KERN_INFO "fb%d: %s frame buffer device\n",
			       info->node, info->fix.id);
			davincifb_set_par(info);
		}
	}

	/* install our interrupt service routine */
	if (request_irq(IRQ_VENCINT, davincifb_isr, IRQF_SHARED, MODULE_NAME,
			dm)) {
		dev_err(dm->dev, MODULE_NAME
			": could not install interrupt service routine\n");
		goto exit;
	}

	/* Turn ON the output device */
	dm->output_device_config(1);

	return (0);

      exit:
	davincifb_remove(pdev);
	iounmap((void *)dm->mmio_base);
      release_mmio:
	release_mem_region(dm->mmio_base_phys, dm->mmio_size);
	return (-ENODEV);
}

/* ------------------------------------------------------------------------- */

    /*
     *  Frame buffer operations
     */
static struct fb_ops davincifb_ops = {
	.owner = THIS_MODULE,
	.fb_check_var = davincifb_check_var,
	.fb_set_par = davincifb_set_par,
	.fb_setcolreg = davincifb_setcolreg,
	.fb_blank = davincifb_blank,
	.fb_pan_display = davincifb_pan_display,
	.fb_fillrect = cfb_fillrect,
	.fb_copyarea = cfb_copyarea,
	.fb_imageblit = cfb_imageblit,
	.fb_rotate = NULL,
	.fb_sync = NULL,
	.fb_ioctl = davincifb_ioctl,
};

static struct platform_driver davincifb_driver = {
	.probe		= davincifb_probe,
	.remove		= davincifb_remove,
	.driver		= {
		.name	= MODULE_NAME,
		.owner	= THIS_MODULE,
	},
};

/* Register both the driver and the device */
int __init davincifb_init(void)
{
#ifndef MODULE
	/* boot-line options */
	/* handle options for "dm64xxfb" for backwards compatability */
	char *option;
	char *names[] = { "davincifb", "dm64xxfb" };
	int i, num_names = 2, done = 0;

	for (i = 0; i < num_names && !done; i++) {
		if (fb_get_options(names[i], &option)) {
			printk(MODULE_NAME
			       ": Disabled on command-line.\n");
			return -ENODEV;
		} else if (option) {
			davincifb_setup(option);
				done = 1;
		}
	}
#endif

	/* Register the driver with LDM */
	if (platform_driver_register(&davincifb_driver)) {
		pr_debug("failed to register omapfb driver\n");
		return -ENODEV;
	}

	return 0;
}

static void __exit davincifb_cleanup(void)
{
	platform_driver_unregister(&davincifb_driver);
}

module_init(davincifb_init);
module_exit(davincifb_cleanup);

MODULE_DESCRIPTION("Framebuffer driver for TI DaVinci");
MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
