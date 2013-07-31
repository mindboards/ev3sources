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

#ifndef __POLKIT_IMPLICIT_AUTHORIZATION_H
#define __POLKIT_IMPLICIT_AUTHORIZATION_H

#include <glib-object.h>

G_BEGIN_DECLS

GType polkit_implicit_authorization_get_type (void) G_GNUC_CONST;

#define POLKIT_TYPE_IMPLICIT_AUTHORIZATION (polkit_implicit_authorization_get_type ())

/**
 * PolkitImplicitAuthorization:
 * @POLKIT_IMPLICIT_AUTHORIZATION_UNKNOWN: Unknown whether the subject is authorized, never returned in any public API.
 * @POLKIT_IMPLICIT_AUTHORIZATION_NOT_AUTHORIZED: Subject is not authorized.
 * @POLKIT_IMPLICIT_AUTHORIZATION_AUTHENTICATION_REQUIRED: Authentication is required.
 * @POLKIT_IMPLICIT_AUTHORIZATION_ADMINISTRATOR_AUTHENTICATION_REQUIRED: Authentication as an administrator is required.
 * @POLKIT_IMPLICIT_AUTHORIZATION_AUTHENTICATION_REQUIRED_RETAINED: Authentication is required. If the authorization is obtained, it is retained.
 * @POLKIT_IMPLICIT_AUTHORIZATION_ADMINISTRATOR_AUTHENTICATION_REQUIRED_RETAINED: Authentication as an administrator is required. If the authorization is obtained, it is retained.
 * @POLKIT_IMPLICIT_AUTHORIZATION_AUTHORIZED: The subject is authorized
 *
 * Possible implicit authorizations.
 */
typedef enum
{
  POLKIT_IMPLICIT_AUTHORIZATION_UNKNOWN = -1,
  POLKIT_IMPLICIT_AUTHORIZATION_NOT_AUTHORIZED = 0,
  POLKIT_IMPLICIT_AUTHORIZATION_AUTHENTICATION_REQUIRED = 1,
  POLKIT_IMPLICIT_AUTHORIZATION_ADMINISTRATOR_AUTHENTICATION_REQUIRED = 2,
  POLKIT_IMPLICIT_AUTHORIZATION_AUTHENTICATION_REQUIRED_RETAINED = 3,
  POLKIT_IMPLICIT_AUTHORIZATION_ADMINISTRATOR_AUTHENTICATION_REQUIRED_RETAINED = 4,
  POLKIT_IMPLICIT_AUTHORIZATION_AUTHORIZED = 5,
} PolkitImplicitAuthorization;

const gchar                 *polkit_implicit_authorization_to_string   (PolkitImplicitAuthorization implicit_authorization);

gboolean                     polkit_implicit_authorization_from_string (const gchar *string,
                                                                        PolkitImplicitAuthorization *out_implicit_authorization);


G_END_DECLS

#endif /* __POLKIT_IMPLICIT_AUTHORIZATION_H */
