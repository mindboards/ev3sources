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

#if !defined (_POLKIT_BACKEND_COMPILATION) && !defined(_POLKIT_BACKEND_INSIDE_POLKIT_BACKEND_H)
#error "Only <polkitbackend/polkitbackend.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __POLKIT_BACKEND_ACTION_LOOKUP_H
#define __POLKIT_BACKEND_ACTION_LOOKUP_H

#include <glib-object.h>
#include "polkitbackendtypes.h"

G_BEGIN_DECLS

/**
 * POLKIT_BACKEND_ACTION_LOOKUP_EXTENSION_POINT_NAME:
 *
 * Extension point name for looking up action information.
 */
#define POLKIT_BACKEND_ACTION_LOOKUP_EXTENSION_POINT_NAME "polkit-backend-action-lookup-1"

#define POLKIT_BACKEND_TYPE_ACTION_LOOKUP         (polkit_backend_action_lookup_get_type())
#define POLKIT_BACKEND_ACTION_LOOKUP(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_BACKEND_TYPE_ACTION_LOOKUP, PolkitBackendActionLookup))
#define POLKIT_BACKEND_IS_ACTION_LOOKUP(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_BACKEND_TYPE_ACTION_LOOKUP))
#define POLKIT_BACKEND_ACTION_LOOKUP_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE((o), POLKIT_BACKEND_TYPE_ACTION_LOOKUP, PolkitBackendActionLookupIface))

#if 0
/**
 * PolkitBackendActionLookup:
 *
 * Opaque object for #PolkitBackendActionLookupIface.
 */
typedef struct _PolkitBackendActionLookup PolkitBackendActionLookup; /* Dummy typedef */
#endif
typedef struct _PolkitBackendActionLookupIface PolkitBackendActionLookupIface;

/**
 * PolkitBackendActionLookupIface:
 * @get_message: See polkit_backend_action_lookup_get_message().
 * @get_icon_name: See polkit_backend_action_lookup_get_icon_name().
 * @get_details: See polkit_backend_action_lookup_get_details().
 *
 * Interface that is used by backends to provide localized data shown
 * in authentication dialogs.
 */
struct _PolkitBackendActionLookupIface
{
  /*< private >*/
  GTypeInterface parent_iface;

  /*< public >*/
  /* VTable */

  gchar *           (*get_message)   (PolkitBackendActionLookup *lookup,
                                      const gchar               *action_id,
                                      PolkitDetails             *details,
                                      PolkitActionDescription   *action_description);

  gchar *           (*get_icon_name) (PolkitBackendActionLookup *lookup,
                                      const gchar               *action_id,
                                      PolkitDetails             *details,
                                      PolkitActionDescription   *action_description);

  PolkitDetails *   (*get_details)   (PolkitBackendActionLookup *lookup,
                                      const gchar               *action_id,
                                      PolkitDetails             *details,
                                      PolkitActionDescription   *action_description);
};

GType          polkit_backend_action_lookup_get_type      (void) G_GNUC_CONST;
gchar         *polkit_backend_action_lookup_get_message   (PolkitBackendActionLookup *lookup,
                                                           const gchar               *action_id,
                                                           PolkitDetails             *details,
                                                           PolkitActionDescription   *action_description);
gchar         *polkit_backend_action_lookup_get_icon_name (PolkitBackendActionLookup *lookup,
                                                           const gchar               *action_id,
                                                           PolkitDetails             *details,
                                                           PolkitActionDescription   *action_description);
PolkitDetails *polkit_backend_action_lookup_get_details   (PolkitBackendActionLookup *lookup,
                                                           const gchar               *action_id,
                                                           PolkitDetails             *details,
                                                           PolkitActionDescription   *action_description);

G_END_DECLS

#endif /* __POLKIT_BACKEND_ACTION_LOOKUP_H */
