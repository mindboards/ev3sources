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

#ifndef __POLKIT_AUTHORIZATION_RESULT_H
#define __POLKIT_AUTHORIZATION_RESULT_H

#include <glib-object.h>
#include <polkit/polkittypes.h>

G_BEGIN_DECLS

#define POLKIT_TYPE_AUTHORIZATION_RESULT          (polkit_authorization_result_get_type())
#define POLKIT_AUTHORIZATION_RESULT(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_TYPE_AUTHORIZATION_RESULT, PolkitAuthorizationResult))
#define POLKIT_AUTHORIZATION_RESULT_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), POLKIT_TYPE_AUTHORIZATION_RESULT, PolkitAuthorizationResultClass))
#define POLKIT_AUTHORIZATION_RESULT_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_TYPE_AUTHORIZATION_RESULT, PolkitAuthorizationResultClass))
#define POLKIT_IS_AUTHORIZATION_RESULT(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_TYPE_AUTHORIZATION_RESULT))
#define POLKIT_IS_AUTHORIZATION_RESULT_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_TYPE_AUTHORIZATION_RESULT))

#if 0
typedef struct _PolkitAuthorizationResult PolkitAuthorizationResult;
#endif
typedef struct _PolkitAuthorizationResultClass PolkitAuthorizationResultClass;

GType              polkit_authorization_result_get_type                       (void) G_GNUC_CONST;
PolkitAuthorizationResult *polkit_authorization_result_new                    (gboolean                   is_authorized,
                                                                               gboolean                   is_challenge,
                                                                               PolkitDetails             *details);
PolkitDetails     *polkit_authorization_result_get_details                    (PolkitAuthorizationResult *result);
gboolean           polkit_authorization_result_get_is_authorized              (PolkitAuthorizationResult *result);
gboolean           polkit_authorization_result_get_is_challenge               (PolkitAuthorizationResult *result);
gboolean           polkit_authorization_result_get_retains_authorization      (PolkitAuthorizationResult *result);
const gchar       *polkit_authorization_result_get_temporary_authorization_id (PolkitAuthorizationResult *result);
gboolean           polkit_authorization_result_get_locked_down                (PolkitAuthorizationResult *result);

/* ---------------------------------------------------------------------------------------------------- */

G_END_DECLS

#endif /* __POLKIT_AUTHORIZATION_RESULT_H */
