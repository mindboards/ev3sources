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

#ifndef __POLKIT_TEMPORARY_AUTHORIZATION_H
#define __POLKIT_TEMPORARY_AUTHORIZATION_H

#include <glib-object.h>
#include <gio/gio.h>
#include <polkit/polkittypes.h>

G_BEGIN_DECLS

#define POLKIT_TYPE_TEMPORARY_AUTHORIZATION          (polkit_temporary_authorization_get_type())
#define POLKIT_TEMPORARY_AUTHORIZATION(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_TYPE_TEMPORARY_AUTHORIZATION, PolkitTemporaryAuthorization))
#define POLKIT_TEMPORARY_AUTHORIZATION_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), POLKIT_TYPE_TEMPORARY_AUTHORIZATION, PolkitTemporaryAuthorizationClass))
#define POLKIT_TEMPORARY_AUTHORIZATION_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_TYPE_TEMPORARY_AUTHORIZATION, PolkitTemporaryAuthorizationClass))
#define POLKIT_IS_TEMPORARY_AUTHORIZATION(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_TYPE_TEMPORARY_AUTHORIZATION))
#define POLKIT_IS_TEMPORARY_AUTHORIZATION_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_TYPE_TEMPORARY_AUTHORIZATION))

#if 0
typedef struct _PolkitTemporaryAuthorization PolkitTemporaryAuthorization;
#endif
typedef struct _PolkitTemporaryAuthorizationClass PolkitTemporaryAuthorizationClass;

GType          polkit_temporary_authorization_get_type              (void) G_GNUC_CONST;
PolkitTemporaryAuthorization *polkit_temporary_authorization_new    (const gchar                  *id,
                                                                     const gchar                  *action_id,
                                                                     PolkitSubject                *subject,
                                                                     guint64                       time_obtained,
                                                                     guint64                       time_expires);
const gchar   *polkit_temporary_authorization_get_id                (PolkitTemporaryAuthorization *authorization);
const gchar   *polkit_temporary_authorization_get_action_id         (PolkitTemporaryAuthorization *authorization);
PolkitSubject *polkit_temporary_authorization_get_subject           (PolkitTemporaryAuthorization *authorization);
guint64        polkit_temporary_authorization_get_time_obtained     (PolkitTemporaryAuthorization *authorization);
guint64        polkit_temporary_authorization_get_time_expires      (PolkitTemporaryAuthorization *authorization);

G_END_DECLS

#endif /* __POLKIT_TEMPORARY_AUTHORIZATION_H */
