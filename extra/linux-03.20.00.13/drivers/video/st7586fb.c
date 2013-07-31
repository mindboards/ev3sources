/*
 * linux/drivers/video/st7856fb.c -- FB driver for ST7856 display controller
 *
 * Based on st7735fb.c by Matt Porter
 * Layout is based on skeletonfb.c by James Simmons and Geert Uytterhoeven.
 *
 * Copyright (C) 2011 Matt Porter <matt@ohporter.com>
 * Copyright (C) 2011 Texas Instruments
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file COPYING in the main directory of this archive for
 * more details.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/uaccess.h>

#include <video/st7586fb.h>

static struct st7586_function st7586_cfg_script[] = {
	{ ST7586_START, ST7586_START},
	{ ST7586_CMD, ST7586_ARDCTL},
	{ ST7586_DATA, 0x9f},
	{ ST7586_CMD, ST7586_OTPRWCTL},
	{ ST7586_DATA, 0x00},
	{ ST7586_DELAY, 10},
	{ ST7586_CMD, ST7586_OTPRD},
	{ ST7586_DELAY, 20},
	{ ST7586_CMD, ST7586_OTPCOUT},
	{ ST7586_CMD, ST7586_SLPOUT},
	{ ST7586_CMD, ST7586_DISPOFF},
	{ ST7586_DELAY, 50},
	{ ST7586_CMD, ST7586_VOPOFF},
	{ ST7586_DATA, 0x00},
	{ ST7586_CMD, ST7586_VOP},
	{ ST7586_DATA, 0xE3},
	{ ST7586_DATA, 0x00},
	{ ST7586_CMD, ST7586_BIAS},
	{ ST7586_DATA, 0x02},
	{ ST7586_CMD, ST7586_BOOST},
	{ ST7586_DATA, 0x04},
	{ ST7586_CMD, ST7586_ANCTL},
	{ ST7586_DATA, 0x1d},
	{ ST7586_CMD, ST7586_NLNINV},
	{ ST7586_DATA, 0x00},
	{ ST7586_CMD, ST7586_DSPMONO},
	{ ST7586_CMD, ST7586_RAMENB},
	{ ST7586_DATA, 0x02},
	{ ST7586_CMD, ST7586_DSPCTL},
	{ ST7586_DATA, 0x00},
	{ ST7586_CMD, ST7586_DSPDUTY},
	{ ST7586_DATA, 0x7f},
	{ ST7586_CMD, ST7586_PARDISP},
	{ ST7586_DATA, 0xa0},
	{ ST7586_CMD, ST7586_PARAREA},
	{ ST7586_DATA, 0x00},
	{ ST7586_DATA, 0x00},
	{ ST7586_DATA, 0x00},
	{ ST7586_DATA, 0x77},
	{ ST7586_CMD, ST7586_INVOFF},
	{ ST7586_CMD, ST7586_CASET},
	{ ST7586_DATA, 0x00},
	{ ST7586_DATA, 0x00},
	{ ST7586_DATA, 0x00},
	{ ST7586_DATA, 0x7f},
	{ ST7586_CMD, ST7586_RASET},
	{ ST7586_DATA, 0x00},
	{ ST7586_DATA, 0x00},
	{ ST7586_DATA, 0x00},
	{ ST7586_DATA, 0x9f},
	{ ST7586_CLR, ST7586_CLR},
	{ ST7586_DELAY, 100},
	{ ST7586_END, ST7586_END},
};

static bool driver_inited = false;

static struct fb_fix_screeninfo st7586fb_fix __devinitdata = {
	.id		= "ST7586",
	.type		= FB_TYPE_PACKED_PIXELS,
	.visual		= FB_VISUAL_PSEUDOCOLOR,
	.xpanstep	= 0,
	.ypanstep	= 0,
	.ywrapstep	= 0,
	.line_length	= (WIDTH+2)/3,
	.accel		= FB_ACCEL_NONE,
};

static struct fb_var_screeninfo st7586fb_var __devinitdata = {
	.xres		= WIDTH,
	.yres		= HEIGHT,
	.xres_virtual	= WIDTH,
	.yres_virtual	= HEIGHT,
	.bits_per_pixel	= 2,
	.nonstd		= 1,
};

static int st7586_write(struct st7586fb_par *par, u8 data)
{
	int ret = 0;

	/* Assert out-of-band CS */
	if (par->cs)
		gpio_set_value(par->cs, 0);

	par->buf[0] = data;

	ret = spi_write(par->spi, par->buf, 1);

	/* Deassert out-of-band CS */
	if (par->cs)
		gpio_set_value(par->cs, 1);

	return ret;
}

static void st7586_write_data(struct st7586fb_par *par, u8 data)
{
	int ret = 0;

	/* Set data mode */
	gpio_set_value(par->a0, 1);

	ret = st7586_write(par, data);
	if (ret < 0)
		pr_err("%s: write data %02x failed with status %d\n",
			par->info->fix.id, data, ret);
}

static int st7586_write_data_buf(struct st7586fb_par *par,
					u8 *txbuf, int size)
{
	int ret = 0;

	/* Set data mode */
	gpio_set_value(par->a0, 1);

	/* Assert out-of-band CS */
	if (par->cs)
		gpio_set_value(par->cs, 0);

	/* Write entire buffer */
	ret = spi_write(par->spi, txbuf, size);

	/* Deassert out-of-band CS */
	if (par->cs)
		gpio_set_value(par->cs, 1);

	return ret;
}

static void st7586_write_cmd(struct st7586fb_par *par, u8 data)
{
	int ret = 0;

	/* Set command mode */
	gpio_set_value(par->a0, 0);

	ret = st7586_write(par, data);
	if (ret < 0)
		pr_err("%s: write command %02x failed with status %d\n",
			par->info->fix.id, data, ret);
}

static void st7586_clear_ddrram(struct st7586fb_par *par)
{
	u8 *buf;

	buf = kzalloc(128*128, GFP_KERNEL);
	st7586_write_cmd(par, ST7586_RAMWR);
	st7586_write_data_buf(par, buf, 128*128);
	kfree(buf);
}

static void st7586_run_cfg_script(struct st7586fb_par *par)
{
	int i = 0;
	int end_script = 0;

	do {
		switch (st7586_cfg_script[i].cmd)
		{
		case ST7586_START:
			break;
		case ST7586_CLR:
			st7586_clear_ddrram(par);
			break;
		case ST7586_CMD:
			st7586_write_cmd(par,
				st7586_cfg_script[i].data & 0xff);
			break;
		case ST7586_DATA:
			st7586_write_data(par,
				st7586_cfg_script[i].data & 0xff);
			break;
		case ST7586_DELAY:
			mdelay(st7586_cfg_script[i].data);
			break;
		case ST7586_END:
			end_script = 1;
		}
		i++;
	} while (!end_script);
}

static void st7586_set_addr_win(struct st7586fb_par *par,
				int xs, int ys, int xe, int ye)
{
	st7586_write_cmd(par, ST7586_CASET);
	st7586_write_data(par, 0x00);
	st7586_write_data(par, xs);
	st7586_write_data(par, 0x00);
	st7586_write_data(par, ((xe+2)/3)-1);
	st7586_write_cmd(par, ST7586_RASET);
	st7586_write_data(par, 0x00);
	st7586_write_data(par, ys);
	st7586_write_data(par, 0x00);
	st7586_write_data(par, ye-1);
}

static void st7586_reset(struct st7586fb_par *par)
{
	/* Reset controller */
	gpio_set_value(par->rst, 0);
	mdelay(10);
	gpio_set_value(par->rst, 1);
	mdelay(120);
}

static void st7586fb_update_display(struct st7586fb_par *par)
{
    int ret = 0, i=0;
	u8 *vmem = par->info->screen_base;

	// Check if any data has been put into screen buffer. 
	// In case data written - Select display as inited 
	if (!driver_inited) {
	  while ( (!vmem[i]) && (i <(WIDTH+2)/3*HEIGHT)) i++;
	  if (i != ((WIDTH+2)/3*HEIGHT)) driver_inited = true;
	}

	if (driver_inited) {
	  st7586_set_addr_win(par, 0, 0, WIDTH, HEIGHT);
	  st7586_write_cmd(par, ST7586_RAMWR);

	  /* Blast framebuffer to ST7586 internal display RAM */
	  ret = st7586_write_data_buf(par, vmem, (WIDTH+2)/3*HEIGHT);
	  
	  if (ret < 0)
	    pr_err("%s: spi_write failed to update display buffer\n",
		   par->info->fix.id);
	}
}

static void st7586fb_deferred_io(struct fb_info *info,
				struct list_head *pagelist)
{
	st7586fb_update_display(info->par);
}


static int st7586fb_request_gpios(struct st7586fb_par *par)
{
	/* TODO: Need some error checking on gpios */

        /* Request GPIOs and initialize to default values */
        gpio_request(par->rst, "ST7586 Reset Pin");
	gpio_direction_output(par->rst, 1);
        gpio_request(par->a0, "ST7586 A0 Pin");
	gpio_direction_output(par->a0, 0);
	if (par->cs) {
		gpio_request(par->cs, "ST7586 CS Pin");
		gpio_direction_output(par->cs, 1);
	}
	return 0;
}

static int st7586fb_init_display(struct st7586fb_par *par)
{
	st7586_reset(par);

	st7586_run_cfg_script(par);

	/* Set row/column data window */
	st7586_set_addr_win(par, 0, 0, WIDTH, HEIGHT);

	st7586_write_cmd(par, ST7586_DISPON);

	return 0;
}

void st7586fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
	struct st7586fb_par *par = info->par;

	sys_fillrect(info, rect);

	st7586fb_update_display(par);
}

void st7586fb_copyarea(struct fb_info *info, const struct fb_copyarea *area)
{
	struct st7586fb_par *par = info->par;

	sys_copyarea(info, area);

	st7586fb_update_display(par);
}

void st7586fb_imageblit(struct fb_info *info, const struct fb_image *image)
{
	struct st7586fb_par *par = info->par;

	sys_imageblit(info, image);

	st7586fb_update_display(par);
}

int st7586fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
	int retval = -ENOMEM;
	struct st7586fb_par *par = info->par;

	printk(KERN_ERR "fb%d: Initing display\n", info->node);

	switch (cmd) 
	  {
	  case FB_ST7586_INIT_DISPLAY: 
	    retval = st7586fb_init_display(par);
	    if (retval < 0) driver_inited = true;
	    break;
	  case FB_ST7586_START_DISPLAY: 
	    retval = 0;	  
	    driver_inited = true;
	  default: 
	    break;
	  }
	
	return retval;
}

static ssize_t st7586fb_write(struct fb_info *info, const char __user *buf,
		size_t count, loff_t *ppos)
{
	struct st7586fb_par *par = info->par;
	unsigned long p = *ppos;
	void *dst;
	int err = 0;
	unsigned long total_size;

	if (info->state != FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size = info->fix.smem_len;

	if (p > total_size)
		return -EFBIG;

	if (count > total_size) {
		err = -EFBIG;
		count = total_size;
	}

	if (count + p > total_size) {
		if (!err)
			err = -ENOSPC;

		count = total_size - p;
	}

	dst = (void __force *) (info->screen_base + p);

	if (copy_from_user(dst, buf, count))
		err = -EFAULT;

	if  (!err)
		*ppos += count;

	st7586fb_update_display(par);

	return (err) ? err : count;
}

static struct fb_ops st7586fb_ops = {
	.owner		= THIS_MODULE,
	.fb_read	= fb_sys_read,
	.fb_write	= st7586fb_write,
	.fb_fillrect	= st7586fb_fillrect,
	.fb_copyarea	= st7586fb_copyarea,
	.fb_imageblit	= st7586fb_imageblit,
	.fb_ioctl       = st7586fb_ioctl
};

static struct fb_deferred_io st7586fb_defio = {
	.delay		= HZ / 20,
	.deferred_io	= st7586fb_deferred_io,
};

static int __devinit st7586fb_probe (struct spi_device *spi)
{
	int chip = spi_get_device_id(spi)->driver_data;
	struct st7586fb_platform_data *pdata = spi->dev.platform_data;
	int vmem_size = (WIDTH+2)/3*HEIGHT;
	u8 *vmem;
	struct fb_info *info;
	struct st7586fb_par *par;
	int retval = -ENOMEM;

	if (chip != ST7586_DISPLAY_LMS2012_LCD) {
		pr_err("%s: only the %s device is supported\n", DRVNAME,
			to_spi_driver(spi->dev.driver)->id_table->name);
		return -EINVAL;
	}

	if (!pdata) {
		pr_err("%s: platform data required for rst and a0 info\n",
			DRVNAME);
		return -EINVAL;
	}

	vmem = (u8 *)kmalloc(vmem_size, GFP_KERNEL);
	if (!vmem)
		return retval;

	// Zero memory for easy detection if any new data has been written to screenbuffer
	memset(vmem, 0, vmem_size);

	info = framebuffer_alloc(sizeof(struct st7586fb_par), &spi->dev);
	if (!info)
		goto fballoc_fail;

	info->screen_base = vmem;
	info->fbops = &st7586fb_ops;
	info->fix = st7586fb_fix;
	info->fix.smem_start = virt_to_phys(vmem);
	info->fix.smem_len = vmem_size;
	info->var = st7586fb_var;
	/* The ST7586 packed pixel format does not translate well here */
	/* FIXME - change to mono reporting */
	info->var.red.offset = 11;
	info->var.red.length = 5;
	info->var.green.offset = 5;
	info->var.green.length = 6;
	info->var.blue.offset = 0;
	info->var.blue.length = 5;
	info->var.transp.offset = 0;
	info->var.transp.length = 0;
	info->flags = FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;
	info->fbdefio = &st7586fb_defio;
	fb_deferred_io_init(info);

	par = info->par;
	par->info = info;
	par->spi = spi;
	par->rst = pdata->rst_gpio;
	par->a0 = pdata->a0_gpio;
	par->cs = pdata->cs_gpio;
	par->buf = kmalloc(1, GFP_KERNEL);

	retval = register_framebuffer(info);
	if (retval < 0)
		goto fbreg_fail;

	spi_set_drvdata(spi, info);

	retval = st7586fb_request_gpios(par);

	// Move initing of display to ioctl call to avoid dispaly flickering
	// as dispaly has already been inited in uboot
	// retval |= st7586fb_init_display(par);
	if (retval < 0)
		goto init_fail;

	printk(KERN_INFO
		"fb%d: %s frame buffer device, using %d KiB of video memory\n",
		info->node, info->fix.id, vmem_size/1024);

	return 0;

init_fail:
	spi_set_drvdata(spi, NULL);

fbreg_fail:
	kfree(par->buf);
	framebuffer_release(info);

fballoc_fail:
	kfree(vmem);

	return retval;
}

static int __devexit st7586fb_remove(struct spi_device *spi)
{
	struct fb_info *info = spi_get_drvdata(spi);

	spi_set_drvdata(spi, NULL);

	if (info) {
		struct st7586fb_par *par = info->par;

		unregister_framebuffer(info);
		kfree(info->screen_base);
		kfree(par->buf);
		gpio_free(par->rst);
		gpio_free(par->a0);
		if (par->cs)
			gpio_free(par->cs);
		framebuffer_release(info);
	}

	return 0;
}

static const struct spi_device_id st7586fb_ids[] = {
	{ "lms2012_lcd", ST7586_DISPLAY_LMS2012_LCD },
	{ },
};

MODULE_DEVICE_TABLE(spi, st7586fb_ids);

static struct spi_driver st7586fb_driver = {
	.driver = {
		.name   = "st7586fb",
		.owner  = THIS_MODULE,
	},
	.id_table = st7586fb_ids,
	.probe  = st7586fb_probe,
	.remove = __devexit_p(st7586fb_remove),
};

static int __init st7586fb_init(void)
{
	return spi_register_driver(&st7586fb_driver);
}

static void __exit st7586fb_exit(void)
{
	spi_unregister_driver(&st7586fb_driver);
}

/* ------------------------------------------------------------------------- */

module_init(st7586fb_init);
module_exit(st7586fb_exit);

MODULE_DESCRIPTION("FB driver for ST7586 display controller");
MODULE_AUTHOR("Matt Porter <mporter@ti.com>");
MODULE_LICENSE("GPL");
