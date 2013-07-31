/*
 * Flash memory support for TI DaVinci boards
 *
 * Copyright (C) 2009 Texas Instruments, Inc. http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/cpufreq.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

#include <mach/flash.h>
#include <mach/aemif.h>

struct davinciflash_info {
	struct mtd_info		*mtd;
	struct map_info		map;
	struct device		*dev;
	bool			partitioned;

	void __iomem		*ctlr;

	unsigned		cs;

	struct clk		*clk;
	struct davinci_aemif_timing	*timing;
};

static int __init davinciflash_probe(struct platform_device *pdev)
{
	struct davinciflash_pdata 	*pdata = pdev->dev.platform_data;
	struct davinciflash_info 	*info;
	struct resource 		*res1;
	struct resource 		*res2;
	int 				ret;
	unsigned			val;

	info = kzalloc(sizeof(struct davinciflash_info), GFP_KERNEL);
	if (!info) {
		dev_err(&pdev->dev, "unable to allocate memory\n");
		return -ENOMEM;
	}

	res1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	res2 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!res1 || !res2) {
		dev_err(&pdev->dev, "resource missing\n");
		ret = -EINVAL;
		goto err_get_resource;
	}

	info->map.virt = ioremap(res1->start, resource_size(res1));
	info->ctlr = ioremap(res2->start, resource_size(res2));
	if (!info->map.virt || !info->ctlr) {
		dev_err(&pdev->dev, "ioremap failed\n");
		ret = -ENOMEM;
		goto err_ioremap;
	}

	info->clk = clk_get(&pdev->dev, "aemif");
	if (IS_ERR(info->clk)) {
		ret = PTR_ERR(info->clk);
		dev_dbg(&pdev->dev, "unable to get AEMIF clock, error %d\n",
				ret);
		goto err_clk_get;
	}

	ret = clk_enable(info->clk);
	if (ret < 0) {
		dev_dbg(&pdev->dev, "unable to enable AEMIF clock, error %d\n",
				ret);
		goto err_clk_enable;
	}

	info->cs = pdev->id;
	info->timing = pdata->timing;

	val = __raw_readl(info->ctlr + A1CR_OFFSET + info->cs * 4);
	val &= ~(ACR_ASIZE_MASK | ACR_EW_MASK | ACR_SS_MASK);
	val |= pdata->width - 1;
	__raw_writel(val, info->ctlr + A1CR_OFFSET + info->cs * 4);

	ret = davinci_aemif_setup_timing(info->timing, info->ctlr, info->cs);
	if (ret < 0) {
		dev_dbg(&pdev->dev, "NOR flash timing values setup fail\n");
		goto err_timing;
	}

	info->map.name		= dev_name(&pdev->dev);
	info->map.phys		= res1->start;
	info->map.size		= resource_size(res1);
	info->map.bankwidth	= pdata->width;

	simple_map_init(&info->map);
	info->mtd = do_map_probe("cfi_probe", &info->map);
	if (!info->mtd) {
		ret = -EIO;
		goto err_map_probe;
	}
	info->mtd->owner = THIS_MODULE;

	info->mtd->dev.parent = &pdev->dev;

	if (mtd_has_partitions()) {
		struct mtd_partition	*mtd_parts = NULL;
		int			mtd_parts_nb = 0;

		if (mtd_has_cmdlinepart()) {
			static const char *probes[] __initconst = {
				"cmdlinepart",
				NULL,
			};

			mtd_parts_nb = parse_mtd_partitions(info->mtd, probes,
							    &mtd_parts, 0);
		}

		if (mtd_parts_nb <= 0) {
			mtd_parts = pdata->parts;
			mtd_parts_nb = pdata->nr_parts;
		}

		/* Register any partitions */
		if (mtd_parts_nb > 0) {
			ret = add_mtd_partitions(info->mtd, mtd_parts,
								mtd_parts_nb);
			if (ret == 0)
				info->partitioned = true;
		}

	} else if (pdata->nr_parts) {
		dev_warn(&pdev->dev, "ignoring %d default partitions on %s\n",
				pdata->nr_parts, info->mtd->name);
	}

	/*
	 * If there's no partition info, just package the whole chip
	 * as a single MTD device.
	 */
	if (!info->partitioned)
		ret = add_mtd_device(info->mtd) ? -ENODEV : 0;

	if (ret < 0)
		goto err_partition_mtd;

	info->dev = &pdev->dev;
	platform_set_drvdata(pdev, info);

	return 0;

err_partition_mtd:
	map_destroy(info->mtd);

err_map_probe:
err_timing:
	clk_disable(info->clk);

err_clk_enable:
	clk_put(info->clk);

err_clk_get:
	iounmap(info->map.virt);
	iounmap(info->ctlr);

err_ioremap:
err_get_resource:
	kfree(info);

	return ret;
}

static int __exit davinciflash_remove(struct platform_device *pdev)
{
	struct davinciflash_info *info = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);

	if (mtd_has_partitions() && info->partitioned)
		del_mtd_partitions(info->mtd);
	else
		del_mtd_device(info->mtd);

	map_destroy(info->mtd);

	iounmap(info->map.virt);
	iounmap(info->ctlr);

	clk_disable(info->clk);
	clk_put(info->clk);

	kfree(info);

	return 0;
}

#ifdef CONFIG_PM
static int davinciflash_suspend(struct platform_device *pdev,
							pm_message_t state)
{
	struct davinciflash_info *info = platform_get_drvdata(pdev);

	clk_disable(info->clk);

	return 0;
}

static int davinciflash_resume(struct platform_device *pdev)
{
	struct davinciflash_info *info = platform_get_drvdata(pdev);

	clk_enable(info->clk);

	return 0;
}

#else
# define davinciflash_suspend   NULL
# define davinciflash_resume    NULL
#endif	/* CONFIG_PM */


static struct platform_driver davinciflash_driver = {
	.remove	= __exit_p(davinciflash_remove),
	.driver = {
		.name	= "davinci-flash",
		.owner	= THIS_MODULE,
	},
	.suspend	= davinciflash_suspend,
	.resume		= davinciflash_resume,
};

static int __init davinciflash_init(void)
{
	return platform_driver_probe(&davinciflash_driver, davinciflash_probe);
}

static void __exit davinciflash_exit(void)
{
	platform_driver_unregister(&davinciflash_driver);
}

module_init(davinciflash_init);
module_exit(davinciflash_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MTD NOR map driver for TI DaVinci boards");
MODULE_ALIAS("platform:davinci-flash");
