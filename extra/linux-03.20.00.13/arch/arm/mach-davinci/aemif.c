/*
 * AEMIF support for DaVinci SoCs
 *
 * Copyright (C) 2009 Texas Instruments Incorporated. http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/module.h>

#include <mach/aemif.h>

/* timing calculations */

#define NS_IN_KHZ 1000000

/*
 * aemif_calc_rate - calculate timing data.
 * @wanted: The cycle time in nanoseconds.
 * @clk: The clock rate in kHz.
 * @max: The maximum divider value.
 *
 * Calculate the timing value from the given parameters.
 */
static int aemif_calc_rate(int wanted, unsigned long clk, int max)
{
	int result;

	result = DIV_ROUND_UP((wanted * clk), NS_IN_KHZ);

	pr_debug("%s: result %d from %ld, %d\n", __func__, result, clk, wanted);

	if (result > max) {
		pr_err("%s: %d ns is too big for current clock rate %ld\n",
							__func__, wanted, clk);
		return -EINVAL;
	}

	if (result < 1)
		result = 1;

	return result;
}

int davinci_aemif_setup_timing(struct davinci_aemif_timing *t,
					void __iomem *base, unsigned cs)
{
	unsigned set, val;
	unsigned ta, rhold, rstrobe, rsetup, whold, wstrobe, wsetup;
	unsigned offset = A1CR_OFFSET + cs * 4;
	struct clk *aemif_clk;
	unsigned long clkrate;

	if (!t)
		return 0;	/* Nothing to do */

	aemif_clk = clk_get(NULL, "aemif");
	if (IS_ERR(aemif_clk))
		return PTR_ERR(aemif_clk);

	clkrate = clk_get_rate(aemif_clk);

	clkrate /= 1000;	/* turn clock into kHz for ease of use */

	ta	= aemif_calc_rate(t->ta, clkrate, TA_MAX);
	rhold	= aemif_calc_rate(t->rhold, clkrate, RHOLD_MAX);
	rstrobe	= aemif_calc_rate(t->rstrobe, clkrate, RSTROBE_MAX);
	rsetup	= aemif_calc_rate(t->rsetup, clkrate, RSETUP_MAX);
	whold	= aemif_calc_rate(t->whold, clkrate, WHOLD_MAX);
	wstrobe	= aemif_calc_rate(t->wstrobe, clkrate, WSTROBE_MAX);
	wsetup	= aemif_calc_rate(t->wsetup, clkrate, WSETUP_MAX);

	if (ta < 0 || rhold < 0 || rstrobe < 0 || rsetup < 0 ||
			whold < 0 || wstrobe < 0 || wsetup < 0) {
		pr_err("%s: cannot get suitable timings\n", __func__);
		return -EINVAL;
	}

	set = TA(ta - 1) |
		RHOLD(rhold - 1) |
		RSTROBE(rstrobe - 1) |
		RSETUP(rsetup - 1) |
		WHOLD(whold - 1) |
		WSTROBE(wstrobe - 1) |
		WSETUP(wsetup - 1);

	val = __raw_readl(base + offset);
	val &= ~TIMING_MASK;
	val |= set;
	__raw_writel(val, base + offset);

	return 0;
}
EXPORT_SYMBOL(davinci_aemif_setup_timing);
