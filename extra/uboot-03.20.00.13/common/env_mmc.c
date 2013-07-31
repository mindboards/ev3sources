/*
 * (C) Copyright 2008-2009 Freescale Semiconductor, Inc.

 * (C) Copyright 2000-2006
 * Wolfgang Denk, DENX Software Engineering, w...@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <ahep...@sysgo.de>

 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.         See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* #define DEBUG */

#include <common.h>

#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>
#include <mmc.h>

#if defined(CONFIG_CMD_ENV) && defined(CONFIG_CMD_MMC)
#define CMD_SAVEENV
#elif defined(CONFIG_ENV_OFFSET_REDUND)
#error Cannot use CONFIG_ENV_OFFSET_REDUND without CONFIG_CMD_ENV & CONFIG_CMD_MMC
#endif

#if defined(CONFIG_ENV_SIZE_REDUND) && (CONFIG_ENV_SIZE_REDUND < CONFIG_ENV_SIZE)
#error CONFIG_ENV_SIZE_REDUND should not be less then CONFIG_ENV_SIZE
#endif

/* references to names in env_common.c */
extern uchar default_environment[];

char *env_name_spec = "MMC";

#ifdef ENV_IS_EMBEDDED
extern uchar environment[];
env_t *env_ptr = (env_t *)(&environment[0]);
#else /* ! ENV_IS_EMBEDDED */
env_t *env_ptr;
#endif /* ENV_IS_EMBEDDED */

/* local functions */
#if !defined(ENV_IS_EMBEDDED)
static void use_default(void);
#endif

DECLARE_GLOBAL_DATA_PTR;

uchar env_get_char_spec(int index)
{
       return *((uchar *)(gd->env_addr + index));
}

int env_init(void)
{
       /* use default */
       gd->env_addr = (ulong)&default_environment[0];
       gd->env_valid = 1;

       return 0;
}

inline int init_mmc_for_env(struct mmc *mmc)
{
       if (!mmc) {
               puts("No MMC card found\n");
               return -1;
       }

       if (mmc_init(mmc)) {
               puts("MMC init failed\n");
               return  -1;
       }

       return 0;
}

#ifdef CMD_SAVEENV

inline int write_env(struct mmc *mmc, unsigned long size,
                                               unsigned long offset, const void *buffer)
{
       uint blk_start = 0, blk_cnt = 0, n = 0;

       blk_start = (offset % 512) ? ((offset / 512) + 1) : (offset / 512);
       blk_cnt = (size % 512) ? ((size / 512) + 1) : (size / 512);
       n = mmc->block_dev.block_write(0, blk_start , blk_cnt, (u_char *)buffer);

       return (n == blk_cnt) ? 0 : -1;
}

int saveenv(void)
{
       struct mmc *mmc = find_mmc_device(0);

       if (init_mmc_for_env(mmc))
               return 1;

       puts("Writing to MMC... ");
       if (write_env(mmc, CONFIG_ENV_SIZE, \
                               CONFIG_ENV_OFFSET, env_ptr)) {
               puts("failed\n");
               return 1;
       }

       puts("done\n");
       return 0;
}
#endif /* CMD_SAVEENV */

inline int read_env(struct mmc *mmc, unsigned long size,
                                               unsigned long offset, const void *buffer)
{
       uint blk_start = 0, blk_cnt = 0, n = 0;

       blk_start = (offset % 512) ? ((offset / 512) + 1) : (offset / 512);
       blk_cnt = (size % 512) ? ((size / 512) + 1) : (size / 512);

       n = mmc->block_dev.block_read(0, blk_start, blk_cnt, (uchar *)buffer);

       return (n == blk_cnt) ? 0 : -1;
}

void env_relocate_spec(void)
{
#if !defined(ENV_IS_EMBEDDED)
       struct mmc *mmc = find_mmc_device(0);

       if (init_mmc_for_env(mmc))
               return;

       if (read_env(mmc, CONFIG_ENV_SIZE, CONFIG_ENV_OFFSET, env_ptr))
               return use_default();

       if (crc32(0, env_ptr->data, ENV_SIZE) != env_ptr->crc)
               return use_default();

#endif /* ! ENV_IS_EMBEDDED */
}

#if !defined(ENV_IS_EMBEDDED)
static void use_default()
{
       puts ("*** Warning - bad CRC or MMC, using default environment\n\n");
       set_default_env();
}
#endif

