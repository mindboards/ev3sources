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

#if !defined (_EGG_DBUS_COMPILATION) && !defined(_EGG_DBUS_INSIDE_EGG_DBUS_H)
#error "Only <eggdbus/eggdbus.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __EGG_DBUS_OBJECT_PROXY_H
#define __EGG_DBUS_OBJECT_PROXY_H

#include <eggdbus/eggdbustypes.h>
#include <eggdbus/eggdbusconnection.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_OBJECT_PROXY         (egg_dbus_object_proxy_get_type())
#define EGG_DBUS_OBJECT_PROXY(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_OBJECT_PROXY, EggDBusObjectProxy))
#define EGG_DBUS_OBJECT_PROXY_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), EGG_DBUS_TYPE_OBJECT_PROXY, EggDBusObjectProxyClass))
#define EGG_DBUS_OBJECT_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), EGG_DBUS_TYPE_OBJECT_PROXY, EggDBusObjectProxyClass))
#define EGG_DBUS_IS_OBJECT_PROXY(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_OBJECT_PROXY))
#define EGG_DBUS_IS_OBJECT_PROXY_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), EGG_DBUS_TYPE_OBJECT_PROXY))

#if 0
typedef struct _EggDBusObjectProxy EggDBusObjectProxy;
#endif
typedef struct _EggDBusObjectProxyClass EggDBusObjectProxyClass;

struct _EggDBusObjectProxy
{
  GObject parent_instance;
};

struct _EggDBusObjectProxyClass
{
  GObjectClass parent_class;
};


GType                     egg_dbus_object_proxy_get_type              (void) G_GNUC_CONST;
EggDBusConnection        *egg_dbus_object_proxy_get_connection        (EggDBusObjectProxy  *object_proxy);
const gchar              *egg_dbus_object_proxy_get_name              (EggDBusObjectProxy  *object_proxy);
const gchar              *egg_dbus_object_proxy_get_object_path       (EggDBusObjectProxy  *object_proxy);
gchar                    *egg_dbus_object_proxy_get_name_owner        (EggDBusObjectProxy  *object_proxy);

guint                     egg_dbus_object_proxy_introspect            (EggDBusObjectProxy  *object_proxy,
                                                                       EggDBusCallFlags     call_flags,
                                                                       GCancellable        *cancellable,
                                                                       GAsyncReadyCallback  callback,
                                                                       gpointer             user_data);

EggDBusInterfaceNodeInfo *egg_dbus_object_proxy_introspect_finish     (EggDBusObjectProxy  *object_proxy,
                                                                       GAsyncResult        *res,
                                                                       GError             **error);

EggDBusInterfaceNodeInfo *egg_dbus_object_proxy_introspect_sync       (EggDBusObjectProxy  *object_proxy,
                                                                       EggDBusCallFlags     call_flags,
                                                                       GCancellable        *cancellable,
                                                                       GError             **error);

void                      egg_dbus_object_proxy_invalidate_properties (EggDBusObjectProxy  *object_proxy);

EggDBusInterfaceProxy    *egg_dbus_object_proxy_query_interface       (EggDBusObjectProxy  *object_proxy,
                                                                       GType                interface_type);

/* ---------------------------------------------------------------------------------------------------- */

G_END_DECLS

#endif /* __EGG_DBUS_OBJECT_PROXY_H */
