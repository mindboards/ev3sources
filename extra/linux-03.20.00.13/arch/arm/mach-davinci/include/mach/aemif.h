/*
 * TI DaVinci AEMIF support
 *
 * 2009 (C) Texas Instruments, Inc. http://www.ti.com/
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#ifndef _MACH_DAVINCI_AEMIF_H
#define _MACH_DAVINCI_AEMIF_H

#define NRCSR_OFFSET		0x00
#define AWCCR_OFFSET		0x04
#define A1CR_OFFSET		0x10

#define ACR_ASIZE_MASK		0x3
#define ACR_EW_MASK		BIT(30)
#define ACR_SS_MASK		BIT(31)

/* Timing value configuration */
#define TA(x)			((x) << 2)
#define RHOLD(x)		((x) << 4)
#define RSTROBE(x)		((x) << 7)
#define RSETUP(x)		((x) << 13)
#define WHOLD(x)		((x) << 17)
#define WSTROBE(x)		((x) << 20)
#define WSETUP(x)		((x) << 26)

#define TA_MAX			0x3
#define RHOLD_MAX		0x7
#define RSTROBE_MAX		0x3f
#define RSETUP_MAX		0xf
#define WHOLD_MAX		0x7
#define WSTROBE_MAX		0x3f
#define WSETUP_MAX		0xf

#define TIMING_MASK		(TA(TA_MAX) | \
					RHOLD(RHOLD_MAX) | \
					RSTROBE(RSTROBE_MAX) |	\
					RSETUP(RSETUP_MAX) | \
					WHOLD(WHOLD_MAX) | \
					WSTROBE(WSTROBE_MAX) | \
					WSETUP(WSETUP_MAX))


/* All timings in nanoseconds */
struct davinci_aemif_timing {
	u8	wsetup;
	u8	wstrobe;
	u8	whold;

	u8	rsetup;
	u8	rstrobe;
	u8	rhold;

	u8	ta;
};

int davinci_aemif_setup_timing(struct davinci_aemif_timing *t,
					void __iomem *base, unsigned cs);

#endif
