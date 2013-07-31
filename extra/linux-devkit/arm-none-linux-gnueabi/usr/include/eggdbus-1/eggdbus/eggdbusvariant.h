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

#ifndef __EGG_DBUS_VARIANT_H
#define __EGG_DBUS_VARIANT_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_VARIANT         (egg_dbus_variant_get_type())
#define EGG_DBUS_VARIANT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_VARIANT, EggDBusVariant))
#define EGG_DBUS_VARIANT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), EGG_DBUS_TYPE_VARIANT, EggDBusVariantClass))
#define EGG_DBUS_VARIANT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), EGG_DBUS_TYPE_VARIANT, EggDBusVariantClass))
#define EGG_DBUS_IS_VARIANT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_VARIANT))
#define EGG_DBUS_IS_VARIANT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), EGG_DBUS_TYPE_VARIANT))

typedef struct _EggDBusVariantClass EggDBusVariantClass;

struct _EggDBusVariant
{
  GObject parent_instance;
};

struct _EggDBusVariantClass
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

GType           egg_dbus_variant_get_type             (void) G_GNUC_CONST;

void            egg_dbus_variant_print                (EggDBusVariant *variant,
                                                       guint         indent);

EggDBusVariant *egg_dbus_variant_new                  (void);

EggDBusVariant *egg_dbus_variant_new_for_gvalue       (const GValue *value,
                                                       const gchar  *signature);

const GValue   *egg_dbus_variant_get_gvalue           (EggDBusVariant *variant);

const gchar    *egg_dbus_variant_get_variant_signature (EggDBusVariant *variant);

/* ---------------------------------------------------------------------------------------------------- */

EggDBusVariant *egg_dbus_variant_new_for_string       (const gchar             *value);
EggDBusVariant *egg_dbus_variant_new_for_object_path  (const gchar             *value);
EggDBusVariant *egg_dbus_variant_new_for_signature    (const gchar             *value);
EggDBusVariant *egg_dbus_variant_new_for_string_array (gchar                  **value);
EggDBusVariant *egg_dbus_variant_new_for_object_path_array  (gchar            **value);
EggDBusVariant *egg_dbus_variant_new_for_signature_array (gchar               **value);
EggDBusVariant *egg_dbus_variant_new_for_byte         (guchar                   value);
EggDBusVariant *egg_dbus_variant_new_for_int16        (gint16                   value);
EggDBusVariant *egg_dbus_variant_new_for_uint16       (guint16                  value);
EggDBusVariant *egg_dbus_variant_new_for_int          (gint                     value);
EggDBusVariant *egg_dbus_variant_new_for_uint         (guint                    value);
EggDBusVariant *egg_dbus_variant_new_for_int64        (gint64                   value);
EggDBusVariant *egg_dbus_variant_new_for_uint64       (guint64                  value);
EggDBusVariant *egg_dbus_variant_new_for_boolean      (gboolean                 value);
EggDBusVariant *egg_dbus_variant_new_for_double       (gdouble                  value);
EggDBusVariant *egg_dbus_variant_new_for_seq          (EggDBusArraySeq         *seq,
                                                       const gchar             *element_signature);
EggDBusVariant *egg_dbus_variant_new_for_map          (EggDBusHashMap          *map,
                                                       const gchar             *key_signature,
                                                       const gchar             *value_signature);
EggDBusVariant *egg_dbus_variant_new_for_structure    (EggDBusStructure        *structure);

/* ---------------------------------------------------------------------------------------------------- */

void         egg_dbus_variant_set_string            (EggDBusVariant           *variant,
                                                     const gchar              *value);
void         egg_dbus_variant_set_object_path       (EggDBusVariant           *variant,
                                                     const gchar              *value);
void         egg_dbus_variant_set_signature         (EggDBusVariant           *variant,
                                                     const gchar              *value);
void         egg_dbus_variant_set_string_array      (EggDBusVariant           *variant,
                                                     gchar                   **value);
void         egg_dbus_variant_set_object_path_array (EggDBusVariant           *variant,
                                                     gchar                   **value);
void         egg_dbus_variant_set_signature_array   (EggDBusVariant           *variant,
                                                     gchar                   **value);
void         egg_dbus_variant_set_byte              (EggDBusVariant           *variant,
                                                     guchar                    value);
void         egg_dbus_variant_set_int16             (EggDBusVariant           *variant,
                                                     gint16                    value);
void         egg_dbus_variant_set_uint16            (EggDBusVariant           *variant,
                                                     guint16                   value);
void         egg_dbus_variant_set_int               (EggDBusVariant           *variant,
                                                     gint                      value);
void         egg_dbus_variant_set_uint              (EggDBusVariant           *variant,
                                                     guint                     value);
void         egg_dbus_variant_set_int64             (EggDBusVariant           *variant,
                                                     gint64                    value);
void         egg_dbus_variant_set_uint64            (EggDBusVariant           *variant,
                                                     guint64                   value);
void         egg_dbus_variant_set_boolean           (EggDBusVariant           *variant,
                                                     gboolean                  value);
void         egg_dbus_variant_set_double            (EggDBusVariant           *variant,
                                                     gdouble                   value);
void         egg_dbus_variant_set_seq               (EggDBusVariant           *variant,
                                                     EggDBusArraySeq          *seq,
                                                     const gchar              *element_signature);
void         egg_dbus_variant_set_map               (EggDBusVariant           *variant,
                                                     EggDBusHashMap           *map,
                                                     const gchar              *key_signature,
                                                     const gchar              *value_signature);
void         egg_dbus_variant_set_structure         (EggDBusVariant           *variant,
                                                     EggDBusStructure         *structure);

/* ---------------------------------------------------------------------------------------------------- */

const gchar            *egg_dbus_variant_get_string          (EggDBusVariant *variant);
const gchar            *egg_dbus_variant_get_object_path     (EggDBusVariant *variant);
const gchar            *egg_dbus_variant_get_signature       (EggDBusVariant *variant);
gchar                 **egg_dbus_variant_get_string_array    (EggDBusVariant *variant);
gchar                 **egg_dbus_variant_get_object_path_array (EggDBusVariant *variant);
gchar                 **egg_dbus_variant_get_signature_array (EggDBusVariant *variant);
guchar                  egg_dbus_variant_get_byte            (EggDBusVariant *variant);
gint16                  egg_dbus_variant_get_int16           (EggDBusVariant *variant);
guint16                 egg_dbus_variant_get_uint16          (EggDBusVariant *variant);
gint                    egg_dbus_variant_get_int             (EggDBusVariant *variant);
guint                   egg_dbus_variant_get_uint            (EggDBusVariant *variant);
gint64                  egg_dbus_variant_get_int64           (EggDBusVariant *variant);
guint64                 egg_dbus_variant_get_uint64          (EggDBusVariant *variant);
gboolean                egg_dbus_variant_get_boolean         (EggDBusVariant *variant);
gdouble                 egg_dbus_variant_get_double          (EggDBusVariant *variant);
EggDBusArraySeq        *egg_dbus_variant_get_seq             (EggDBusVariant *variant);
EggDBusHashMap         *egg_dbus_variant_get_map             (EggDBusVariant *variant);
EggDBusStructure       *egg_dbus_variant_get_structure       (EggDBusVariant *variant);

/* ---------------------------------------------------------------------------------------------------- */

gboolean     egg_dbus_variant_is_unset              (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_string             (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_object_path        (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_signature          (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_string_array       (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_object_path_array  (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_signature_array    (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_byte               (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_int16              (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_uint16             (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_int                (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_uint               (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_int64              (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_uint64             (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_boolean            (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_double             (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_seq                (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_map                (EggDBusVariant *variant);
gboolean     egg_dbus_variant_is_structure          (EggDBusVariant *variant);

/* ---------------------------------------------------------------------------------------------------- */

G_END_DECLS

#endif /* __EGG_DBUS_STRUCTURE_H */
