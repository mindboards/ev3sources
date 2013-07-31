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

#ifndef __POLKIT_H
#define __POLKIT_H

#define _POLKIT_INSIDE_POLKIT_H 1

#include <polkit/polkittypes.h>
#include <polkit/polkitenumtypes.h>
#include <polkit/polkitimplicitauthorization.h>
#include <polkit/polkitactiondescription.h>
#include <polkit/polkitauthorityfeatures.h>
#include <polkit/polkiterror.h>
#include <polkit/polkitidentity.h>
#include <polkit/polkitunixuser.h>
#include <polkit/polkitunixgroup.h>
#include <polkit/polkitsubject.h>
#include <polkit/polkitunixprocess.h>
#include <polkit/polkitunixsession.h>
#include <polkit/polkitsystembusname.h>
#include <polkit/polkitauthorizationresult.h>
#include <polkit/polkitcheckauthorizationflags.h>
#include <polkit/polkitauthority.h>
#include <polkit/polkitdetails.h>
#include <polkit/polkittemporaryauthorization.h>

#undef _POLKIT_INSIDE_POLKIT_H

#endif /* __POLKIT_H */
