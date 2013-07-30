/*
 * LEGO® MINDSTORMS EV3
 *
 * Copyright (C) 2010-2013 The LEGO Group
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include  "../../lms2012/source/lms2012.h"

#define   MODULE_NAME                   "iic_module"
#define   DEVICE1_NAME                  "none"

static    int  ModuleInit(void);
static    void ModuleExit(void);

#include  <linux/kernel.h>

#ifndef   PCASM
#include  <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("The LEGO Group");
MODULE_DESCRIPTION(MODULE_NAME);
MODULE_SUPPORTED_DEVICE(DEVICE1_NAME);

module_init(ModuleInit);
module_exit(ModuleExit);

#else
// Keep Eclipse happy
#endif

// MODULE *********************************************************************


static int ModuleInit(void)
{
  printk("%s init started\n",MODULE_NAME);

  return (0);
}


static void ModuleExit(void)
{
  printk("%s exit started\n",MODULE_NAME);
}
