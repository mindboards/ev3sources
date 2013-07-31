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

#ifndef __EGG_DBUS_STRUCTURE_H
#define __EGG_DBUS_STRUCTURE_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_STRUCTURE         (egg_dbus_structure_get_type())
#define EGG_DBUS_STRUCTURE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_STRUCTURE, EggDBusStructure))
#define EGG_DBUS_STRUCTURE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), EGG_DBUS_TYPE_STRUCTURE, EggDBusStructureClass))
#define EGG_DBUS_STRUCTURE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), EGG_DBUS_TYPE_STRUCTURE, EggDBusStructureClass))
#define EGG_DBUS_IS_STRUCTURE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_STRUCTURE))
#define EGG_DBUS_IS_STRUCTURE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), EGG_DBUS_TYPE_STRUCTURE))

/**
 * EggDBusStructure:
 *
 * The #EggDBusStructure structure is an opaque structure and should
 * never be accessed directly.
 */
struct _EggDBusStructure
{
  GObject parent_instance;
};

struct _EggDBusStructureClass
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

/**
 * EGG_DBUS_STRUCTURE_TYPE_CHECK_INSTANCE_CAST:
 * @instance: A #EggDBusStructure instance.
 * @signature: A D-Bus signature.
 * @c_type: A corresponding C type for @signature.
 *
 * Checks that @instance is a #EggDBusStructure with @signature and issues a
 * warning if this is not the case. Returns @instance casted to a pointer
 * to @c_type.
 *
 * This macro should only be used in implementations of subclasses of
 * #EggDBusStructure.
 *
 * Returns: A pointer to @c_type.
 */
#define EGG_DBUS_STRUCTURE_TYPE_CHECK_INSTANCE_CAST(instance, signature, c_type) ((c_type *) egg_dbus_structure_type_check_instance_cast (instance, signature, G_STRINGIFY (c_type)))

/**
 * EGG_DBUS_STRUCTURE_TYPE_CHECK_INSTANCE_TYPE:
 * @instance: A #EggDBusStructure instance.
 * @signature: A D-Bus signature.
 * @c_type: A corresponding C type for @signature.
 *
 * Checks if @instance is a #EggDBusStructure with @signature.
 *
 * This macro should only be used in implementations of subclasses of
 * #EggDBusStructure.
 *
 * Returns: %TRUE only if @instance is a #EggDBusStructure with @signature.
 */
#define EGG_DBUS_STRUCTURE_TYPE_CHECK_INSTANCE_TYPE(instance, signature, c_type) (egg_dbus_structure_type_check_instance_type (instance, signature, G_STRINGIFY (c_type)))

typedef struct _EggDBusStructureClass EggDBusStructureClass;

GType           egg_dbus_structure_get_type (void) G_GNUC_CONST;

EggDBusStructure *egg_dbus_structure_new      (const gchar *signature,
                                               GValue      *elements);

guint        egg_dbus_structure_get_num_elements            (EggDBusStructure  *structure);

void         egg_dbus_structure_get_element                 (EggDBusStructure  *structure,
                                                           guint            first_structure_element_number,
                                                           ...);

void         egg_dbus_structure_get_element_valist          (EggDBusStructure  *structure,
                                                           guint            first_structure_element_number,
                                                           va_list          var_args);

void         egg_dbus_structure_get_element_as_gvalue       (EggDBusStructure  *structure,
                                                           guint            element_number,
                                                           GValue          *value);

void         egg_dbus_structure_set_element                 (EggDBusStructure  *structure,
                                                           guint            first_structure_element_number,
                                                           ...);

void         egg_dbus_structure_set_element_valist          (EggDBusStructure  *structure,
                                                           guint            first_structure_element_number,
                                                           va_list          var_args);

void         egg_dbus_structure_set_element_as_gvalue       (EggDBusStructure  *structure,
                                                           guint            element_number,
                                                           const GValue    *value);

const gchar *egg_dbus_structure_get_signature               (EggDBusStructure  *structure);

const gchar *egg_dbus_structure_get_signature_for_element  (EggDBusStructure   *structure,
                                                          guint             element_number);

void         egg_dbus_structure_print                       (EggDBusStructure  *structure,
                                                           guint            indent);

G_END_DECLS

#endif /* __EGG_DBUS_STRUCTURE_H */
