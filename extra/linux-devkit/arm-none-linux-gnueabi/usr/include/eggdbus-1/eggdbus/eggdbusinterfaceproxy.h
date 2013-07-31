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

#ifndef __EGG_DBUS_INTERFACE_PROXY_H
#define __EGG_DBUS_INTERFACE_PROXY_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_INTERFACE_PROXY         (egg_dbus_interface_proxy_get_type())
#define EGG_DBUS_INTERFACE_PROXY(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_INTERFACE_PROXY, EggDBusInterfaceProxy))
#define EGG_DBUS_INTERFACE_PROXY_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), EGG_DBUS_TYPE_INTERFACE_PROXY, EggDBusInterfaceProxyClass))
#define EGG_DBUS_INTERFACE_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), EGG_DBUS_TYPE_INTERFACE_PROXY, EggDBusInterfaceProxyClass))
#define EGG_DBUS_IS_INTERFACE_PROXY(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_INTERFACE_PROXY))
#define EGG_DBUS_IS_INTERFACE_PROXY_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), EGG_DBUS_TYPE_INTERFACE_PROXY))

#if 0
typedef struct _EggDBusInterfaceProxy EggDBusInterfaceProxy;
#endif
typedef struct _EggDBusInterfaceProxyClass EggDBusInterfaceProxyClass;

struct _EggDBusInterfaceProxy
{
  GObject parent_instance;
};

struct _EggDBusInterfaceProxyClass
{
  GObjectClass parent_class;

  EggDBusObjectProxy *    (*get_object_proxy)    (EggDBusInterfaceProxy *interface_proxy);
  EggDBusInterfaceIface * (*get_interface_iface) (EggDBusInterfaceProxy *interface_proxy);

  /*< private >*/
  /* Padding for future expansion */
  void (*_g_reserved0) (void);
  void (*_g_reserved1) (void);
  void (*_g_reserved2) (void);
  void (*_g_reserved3) (void);
  void (*_g_reserved4) (void);
  void (*_g_reserved5) (void);
  void (*_g_reserved6) (void);
  void (*_g_reserved7) (void);
};


GType                  egg_dbus_interface_proxy_get_type            (void) G_GNUC_CONST;
EggDBusObjectProxy    *egg_dbus_interface_proxy_get_object_proxy    (EggDBusInterfaceProxy *interface_proxy);
EggDBusInterfaceIface *egg_dbus_interface_proxy_get_interface_iface (EggDBusInterfaceProxy *interface_proxy);

/* ---------------------------------------------------------------------------------------------------- */

G_END_DECLS

#endif /* __EGG_DBUS_INTERFACE_PROXY_H */
