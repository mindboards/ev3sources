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

#ifndef __POLKIT_TYPES_H
#define __POLKIT_TYPES_H

#include <glib-object.h>

struct _PolkitAuthority;
typedef struct _PolkitAuthority PolkitAuthority;

struct _PolkitActionDescription;
typedef struct _PolkitActionDescription PolkitActionDescription;

typedef struct _PolkitSubject PolkitSubject; /* Dummy typedef */

struct _PolkitUnixProcess;
typedef struct _PolkitUnixProcess PolkitUnixProcess;

struct _PolkitUnixSession;
typedef struct _PolkitUnixSession PolkitUnixSession;

struct _PolkitSystemBusName;
typedef struct _PolkitSystemBusName PolkitSystemBusName;

typedef struct _PolkitIdentity PolkitIdentity; /* Dummy typedef */

struct _PolkitUnixUser;
typedef struct _PolkitUnixUser PolkitUnixUser;

struct _PolkitUnixGroup;
typedef struct _PolkitUnixGroup PolkitUnixGroup;

struct _PolkitAuthorizationResult;
typedef struct _PolkitAuthorizationResult PolkitAuthorizationResult;

struct _PolkitDetails;
typedef struct _PolkitDetails PolkitDetails;

struct _PolkitTemporaryAuthorization;
typedef struct _PolkitTemporaryAuthorization PolkitTemporaryAuthorization;

#endif /* __POLKIT_TYPES_H */
