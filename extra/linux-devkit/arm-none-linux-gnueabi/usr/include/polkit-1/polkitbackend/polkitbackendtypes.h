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

#ifndef __POLKIT_BACKEND_TYPES_H
#define __POLKIT_BACKEND_TYPES_H

#include <glib-object.h>

struct _PolkitBackendSessionMonitor;
typedef struct _PolkitBackendSessionMonitor PolkitBackendSessionMonitor;

struct _PolkitBackendConfigSource;
typedef struct _PolkitBackendConfigSource PolkitBackendConfigSource;

struct _PolkitBackendActionLookup;
typedef struct _PolkitBackendActionLookup PolkitBackendActionLookup; /* Dummy typedef */

struct _PolkitBackendAuthority;
typedef struct _PolkitBackendAuthority PolkitBackendAuthority;

struct _PolkitBackendInteractiveAuthority;
typedef struct _PolkitBackendInteractiveAuthority PolkitBackendInteractiveAuthority;

struct _PolkitBackendLocalAuthority;
typedef struct _PolkitBackendLocalAuthority PolkitBackendLocalAuthority;

struct _PolkitBackendLocalAuthorizationStore;
typedef struct _PolkitBackendLocalAuthorizationStore PolkitBackendLocalAuthorizationStore;

#endif /* __POLKIT_BACKEND_TYPES_H */

