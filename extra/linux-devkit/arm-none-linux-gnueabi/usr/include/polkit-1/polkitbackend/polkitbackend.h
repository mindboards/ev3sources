/*
 * Copyright (C) 2008 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: David Zeuthen <davidz@redhat.com>
 */

#ifndef __POLKIT_BACKEND_H
#define __POLKIT_BACKEND_H

#include <polkit/polkit.h>

#if !defined (POLKIT_BACKEND_I_KNOW_API_IS_SUBJECT_TO_CHANGE) && !defined (POLKIT_BACKEND_COMPILATION)
#error "libpolkitbackend is unstable API and subject to change. You must define POLKIT_BACKEND_I_KNOW_API_IS_SUBJECT_TO_CHANGE to acknowledge this."
#endif

#define _POLKIT_BACKEND_INSIDE_POLKIT_BACKEND_H 1
#include <polkitbackend/polkitbackendtypes.h>
#include <polkitbackend/polkitbackendauthority.h>
#include <polkitbackend/polkitbackendinteractiveauthority.h>
#include <polkitbackend/polkitbackendlocalauthority.h>
#include <polkitbackend/polkitbackendactionpool.h>
#include <polkitbackend/polkitbackendsessionmonitor.h>
#include <polkitbackend/polkitbackendconfigsource.h>
#include <polkitbackend/polkitbackendactionlookup.h>
#include <polkitbackend/polkitbackendlocalauthorizationstore.h>
#undef _POLKIT_BACKEND_INSIDE_POLKIT_BACKEND_H

#endif /* __POLKIT_BACKEND_H */


