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

#if !defined (_POLKIT_COMPILATION) && !defined(_POLKIT_INSIDE_POLKIT_H)
#error "Only <polkit/polkit.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __POLKIT_AUTHORITY_FEATURES_H
#define __POLKIT_AUTHORITY_FEATURES_H

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * PolkitAuthorityFeatures:
 * @POLKIT_AUTHORITY_FEATURES_NONE: No flags set.
 * @POLKIT_AUTHORITY_FEATURES_TEMPORARY_AUTHORIZATION: The authority supports temporary authorizations
 * that can be obtained through authentication.
 * @POLKIT_AUTHORITY_FEATURES_LOCKDOWN: The authority supports the XXX method.
 *
 * Flags describing features supported by the Authority implementation.
 */
typedef enum
{
  POLKIT_AUTHORITY_FEATURES_NONE                    = 0,
  POLKIT_AUTHORITY_FEATURES_TEMPORARY_AUTHORIZATION = (1<<0),
  POLKIT_AUTHORITY_FEATURES_LOCKDOWN                = (1<<1)
} PolkitAuthorityFeatures;

G_END_DECLS

#endif /* __POLKIT_AUTHORITY_FEATURES_H */
