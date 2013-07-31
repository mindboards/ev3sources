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

#ifndef __EGG_DBUS_OBJECT_PATH_H
#define __EGG_DBUS_OBJECT_PATH_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

/**
 * EGG_DBUS_TYPE_OBJECT_PATH:
 *
 * The #GType for a D-Bus object path.
 */
#define EGG_DBUS_TYPE_OBJECT_PATH               (egg_dbus_object_path_get_type())

/**
 * EGG_DBUS_TYPE_OBJECT_PATH_ARRAY:
 *
 * The #GType for holding a %NULL-terminated array of  D-Bus object paths.
 */
#define EGG_DBUS_TYPE_OBJECT_PATH_ARRAY         (egg_dbus_object_path_array_get_type())

GType egg_dbus_object_path_get_type       (void) G_GNUC_CONST;
GType egg_dbus_object_path_array_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __EGG_DBUS_OBJECT_PATH_H */
