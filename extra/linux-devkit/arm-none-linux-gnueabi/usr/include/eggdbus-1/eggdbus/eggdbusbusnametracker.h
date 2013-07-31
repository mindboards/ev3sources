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

#ifndef __EGG_DBUS_BUS_NAME_TRACKER_H
#define __EGG_DBUS_BUS_NAME_TRACKER_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_BUS_NAME_TRACKER         (egg_dbus_bus_name_tracker_get_type())
#define EGG_DBUS_BUS_NAME_TRACKER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_BUS_NAME_TRACKER, EggDBusBusNameTracker))
#define EGG_DBUS_BUS_NAME_TRACKER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), EGG_DBUS_TYPE_BUS_NAME_TRACKER, EggDBusBusNameTrackerClass))
#define EGG_DBUS_BUS_NAME_TRACKER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), EGG_DBUS_TYPE_BUS_NAME_TRACKER, EggDBusBusNameTrackerClass))
#define EGG_DBUS_IS_BUS_NAME_TRACKER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_BUS_NAME_TRACKER))
#define EGG_DBUS_IS_BUS_NAME_TRACKER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), EGG_DBUS_TYPE_BUS_NAME_TRACKER))

typedef struct _EggDBusBusNameTrackerClass EggDBusBusNameTrackerClass;

struct _EggDBusBusNameTracker
{
  GObject parent_instance;
};

struct _EggDBusBusNameTrackerClass
{
  GObjectClass parent_class;

  /* signals */
  void (*bus_name_has_info)     (EggDBusBusNameTracker *bus_name_tracker,
                                 const gchar         *bus_name);

  void (*bus_name_lost_owner)    (EggDBusBusNameTracker *bus_name_tracker,
                                  const gchar         *bus_name,
                                  const gchar         *old_owner);

  void (*bus_name_gained_owner)  (EggDBusBusNameTracker *bus_name_tracker,
                                  const gchar         *bus_name,
                                  const gchar         *new_owner);

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


GType                egg_dbus_bus_name_tracker_get_type                     (void) G_GNUC_CONST;

EggDBusBusNameTracker *egg_dbus_bus_name_tracker_new                          (EggDBusBus             *bus);

void                 egg_dbus_bus_name_tracker_watch_bus_name               (EggDBusBusNameTracker  *bus_name_tracker,
                                                                           const gchar          *bus_name);

void                 egg_dbus_bus_name_tracker_stop_watching_bus_name       (EggDBusBusNameTracker  *bus_name_tracker,
                                                                           const gchar          *bus_name);

gboolean             egg_dbus_bus_name_tracker_has_info_for_bus_name        (EggDBusBusNameTracker  *bus_name_tracker,
                                                                           const gchar          *bus_name);

gchar *              egg_dbus_bus_name_tracker_get_owner_for_bus_name       (EggDBusBusNameTracker  *bus_name_tracker,
                                                                           const gchar          *bus_name);

gchar              **egg_dbus_bus_name_tracker_get_known_well_known_bus_names_for_unique_bus_name (EggDBusBusNameTracker  *bus_name_tracker,
                                                                           const gchar          *unique_bus_name);

G_END_DECLS

#endif /* __EGG_DBUS_BUS_NAME_TRACKER_H */
