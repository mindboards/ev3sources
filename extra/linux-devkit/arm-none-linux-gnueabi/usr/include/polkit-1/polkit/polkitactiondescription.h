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

#ifndef __POLKIT_ACTION_DESCRIPTION_H
#define __POLKIT_ACTION_DESCRIPTION_H

#include <glib-object.h>
#include <gio/gio.h>
#include <polkit/polkittypes.h>

G_BEGIN_DECLS

#define POLKIT_TYPE_ACTION_DESCRIPTION          (polkit_action_description_get_type())
#define POLKIT_ACTION_DESCRIPTION(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_TYPE_ACTION_DESCRIPTION, PolkitActionDescription))
#define POLKIT_ACTION_DESCRIPTION_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), POLKIT_TYPE_ACTION_DESCRIPTION, PolkitActionDescriptionClass))
#define POLKIT_ACTION_DESCRIPTION_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_TYPE_ACTION_DESCRIPTION, PolkitActionDescriptionClass))
#define POLKIT_IS_ACTION_DESCRIPTION(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_TYPE_ACTION_DESCRIPTION))
#define POLKIT_IS_ACTION_DESCRIPTION_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_TYPE_ACTION_DESCRIPTION))

#if 0
typedef struct _PolkitActionDescription PolkitActionDescription;
#endif
typedef struct _PolkitActionDescriptionClass PolkitActionDescriptionClass;

GType                polkit_action_description_get_type              (void) G_GNUC_CONST;
const gchar         *polkit_action_description_get_action_id         (PolkitActionDescription *action_description);
const gchar         *polkit_action_description_get_description       (PolkitActionDescription *action_description);
const gchar         *polkit_action_description_get_message           (PolkitActionDescription *action_description);
const gchar         *polkit_action_description_get_vendor_name       (PolkitActionDescription *action_description);
const gchar         *polkit_action_description_get_vendor_url        (PolkitActionDescription *action_description);
const gchar         *polkit_action_description_get_icon_name         (PolkitActionDescription *action_description);

PolkitImplicitAuthorization polkit_action_description_get_implicit_any (PolkitActionDescription *action_description);
PolkitImplicitAuthorization polkit_action_description_get_implicit_inactive (PolkitActionDescription *action_description);
PolkitImplicitAuthorization polkit_action_description_get_implicit_active (PolkitActionDescription *action_description);

const gchar         *polkit_action_description_get_annotation        (PolkitActionDescription *action_description,
                                                                      const gchar             *key);
const gchar * const *polkit_action_description_get_annotation_keys   (PolkitActionDescription *action_description);

G_END_DECLS

#endif /* __POLKIT_ACTION_DESCRIPTION_H */
