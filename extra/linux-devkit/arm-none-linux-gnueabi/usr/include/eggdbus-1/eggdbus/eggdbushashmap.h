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

#ifndef __EGG_DBUS_HASH_MAP_H
#define __EGG_DBUS_HASH_MAP_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_HASH_MAP         (egg_dbus_hash_map_get_type())
#define EGG_DBUS_HASH_MAP(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_HASH_MAP, EggDBusHashMap))
#define EGG_DBUS_HASH_MAP_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), EGG_DBUS_TYPE_HASH_MAP, EggDBusHashMapClass))
#define EGG_DBUS_HASH_MAP_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), EGG_DBUS_TYPE_HASH_MAP, EggDBusHashMapClass))
#define EGG_DBUS_IS_HASH_MAP(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_HASH_MAP))
#define EGG_DBUS_IS_HASH_MAP_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), EGG_DBUS_TYPE_HASH_MAP))

typedef struct _EggDBusHashMapClass EggDBusHashMapClass;

/**
 * EggDBusHashMapForeachFunc:
 * @hash_map: The map being iterated over.
 * @key: A key.
 * @value: Value corresponding to @key.
 * @user_data: User data passed to egg_dbus_hash_map_foreach().
 *
 * Type of callback function passed to egg_dbus_hash_map_foreach().
 *
 * Returns: %TRUE to short-circuit the iteration; %FALSE to continue iterating.
 */
typedef gboolean (*EggDBusHashMapForeachFunc) (EggDBusHashMap            *hash_map,
                                               gpointer                   key,
                                               gpointer                   value,
                                               gpointer                   user_data);


/**
 * EggDBusHashMap:
 * @size: Number of elements in the hash map.
 * @key_type: The #GType of the keys in the hash map.
 * @value_type: The #GType of the values in the hash map.
 * @data: Underlying #GHashTable.
 *
 * The #EggDBusHashMap instance structure should normally not be accessed directly, use accessor
 * functions instead. Certain instance members are provided <emphasis>only</emphasis> for efficient
 * and convenient access when using the C programming language. Use with caution.
 */
struct _EggDBusHashMap
{
  GObject               parent_instance;

  /*< public >*/
  GType                 key_type;
  GType                 value_type;
  GHashTable           *data;
};

struct _EggDBusHashMapClass
{
  GObjectClass parent_class;

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


GType                  egg_dbus_hash_map_get_type           (void) G_GNUC_CONST;

EggDBusHashMap        *egg_dbus_hash_map_new                (GType               key_type,
                                                             GDestroyNotify      key_free_func,
                                                             GType               value_type,
                                                             GDestroyNotify      value_free_func) G_GNUC_WARN_UNUSED_RESULT;

EggDBusHashMap        *egg_dbus_hash_map_new_full           (GType               key_type,
                                                             GHashFunc           key_hash_func,
                                                             GEqualFunc          key_equal_func,
                                                             GDestroyNotify      key_free_func,
                                                             GBoxedCopyFunc      key_copy_func,
                                                             GType               value_type,
                                                             GDestroyNotify      value_free_func,
                                                             GBoxedCopyFunc      value_copy_func,
                                                             GEqualFunc          value_equal_func) G_GNUC_WARN_UNUSED_RESULT;

guint                  egg_dbus_hash_map_get_size           (EggDBusHashMap     *hash_map) G_GNUC_WARN_UNUSED_RESULT;

GType                  egg_dbus_hash_map_get_key_type       (EggDBusHashMap     *hash_map) G_GNUC_WARN_UNUSED_RESULT;

GType                  egg_dbus_hash_map_get_value_type     (EggDBusHashMap     *hash_map) G_GNUC_WARN_UNUSED_RESULT;

void                   egg_dbus_hash_map_clear              (EggDBusHashMap     *hash_map);

void                   egg_dbus_hash_map_insert             (EggDBusHashMap     *hash_map,
                                                             gconstpointer       key,
                                                             gconstpointer       value);

gboolean               egg_dbus_hash_map_contains           (EggDBusHashMap     *hash_map,
                                                             gconstpointer       key) G_GNUC_WARN_UNUSED_RESULT;

gpointer               egg_dbus_hash_map_lookup             (EggDBusHashMap     *hash_map,
                                                             gconstpointer       key) G_GNUC_WARN_UNUSED_RESULT;

gpointer               egg_dbus_hash_map_lookup_copy        (EggDBusHashMap     *hash_map,
                                                             gconstpointer       key) G_GNUC_WARN_UNUSED_RESULT;

gboolean               egg_dbus_hash_map_remove             (EggDBusHashMap     *hash_map,
                                                             gconstpointer       key);

gboolean               egg_dbus_hash_map_foreach            (EggDBusHashMap            *hash_map,
                                                             EggDBusHashMapForeachFunc  func,
                                                             gpointer                   user_data);

/* C convenience functions */

gboolean               egg_dbus_hash_map_contains_fixed     (EggDBusHashMap     *hash_map,
                                                             guint64             key) G_GNUC_WARN_UNUSED_RESULT;

gboolean               egg_dbus_hash_map_contains_float     (EggDBusHashMap     *hash_map,
                                                             gdouble             key) G_GNUC_WARN_UNUSED_RESULT;

/* --- */

gpointer               egg_dbus_hash_map_lookup_fixed       (EggDBusHashMap     *hash_map,
                                                             guint64             key) G_GNUC_WARN_UNUSED_RESULT;

gpointer               egg_dbus_hash_map_lookup_float       (EggDBusHashMap     *hash_map,
                                                             gdouble             key) G_GNUC_WARN_UNUSED_RESULT;

gpointer               egg_dbus_hash_map_lookup_fixed_copy  (EggDBusHashMap     *hash_map,
                                                             guint64             key) G_GNUC_WARN_UNUSED_RESULT;

gpointer               egg_dbus_hash_map_lookup_float_copy  (EggDBusHashMap     *hash_map,
                                                             gdouble             key) G_GNUC_WARN_UNUSED_RESULT;

/* --- */

guint64                egg_dbus_hash_map_lookup_ptr_fixed   (EggDBusHashMap     *hash_map,
                                                             gconstpointer       key) G_GNUC_WARN_UNUSED_RESULT;

gdouble                egg_dbus_hash_map_lookup_ptr_float   (EggDBusHashMap     *hash_map,
                                                             gconstpointer       key) G_GNUC_WARN_UNUSED_RESULT;

guint64                egg_dbus_hash_map_lookup_fixed_fixed (EggDBusHashMap     *hash_map,
                                                             guint64             key) G_GNUC_WARN_UNUSED_RESULT;

gdouble                egg_dbus_hash_map_lookup_fixed_float (EggDBusHashMap     *hash_map,
                                                             guint64             key) G_GNUC_WARN_UNUSED_RESULT;

guint64                egg_dbus_hash_map_lookup_float_fixed (EggDBusHashMap     *hash_map,
                                                             gdouble             key) G_GNUC_WARN_UNUSED_RESULT;

gdouble                egg_dbus_hash_map_lookup_float_float (EggDBusHashMap     *hash_map,
                                                             gdouble             key) G_GNUC_WARN_UNUSED_RESULT;


/* --- */

gboolean               egg_dbus_hash_map_remove_fixed       (EggDBusHashMap     *hash_map,
                                                             guint64             key) G_GNUC_WARN_UNUSED_RESULT;

gboolean               egg_dbus_hash_map_remove_float       (EggDBusHashMap     *hash_map,
                                                             gdouble             key) G_GNUC_WARN_UNUSED_RESULT;

/* --- */

void                   egg_dbus_hash_map_insert_ptr_fixed   (EggDBusHashMap     *hash_map,
                                                             gconstpointer       key,
                                                             guint64             value);

void                   egg_dbus_hash_map_insert_ptr_float   (EggDBusHashMap     *hash_map,
                                                             gconstpointer       key,
                                                             gdouble             value);

void                   egg_dbus_hash_map_insert_fixed_ptr   (EggDBusHashMap     *hash_map,
                                                             guint64             key,
                                                             gconstpointer       value);

void                   egg_dbus_hash_map_insert_fixed_fixed (EggDBusHashMap     *hash_map,
                                                             guint64             key,
                                                             guint64             value);

void                   egg_dbus_hash_map_insert_fixed_float (EggDBusHashMap     *hash_map,
                                                             guint64             key,
                                                             gdouble             value);

void                   egg_dbus_hash_map_insert_float_ptr   (EggDBusHashMap     *hash_map,
                                                             gdouble             key,
                                                             gconstpointer       value);

void                   egg_dbus_hash_map_insert_float_fixed (EggDBusHashMap     *hash_map,
                                                             gdouble             key,
                                                             guint64             value);

void                   egg_dbus_hash_map_insert_float_float (EggDBusHashMap     *hash_map,
                                                             gdouble             key,
                                                             gdouble             value);

G_END_DECLS

#endif /* __EGG_DBUS_HASH_MAP_H */
