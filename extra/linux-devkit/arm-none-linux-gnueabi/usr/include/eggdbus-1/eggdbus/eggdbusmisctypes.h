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

#ifndef __EGG_DBUS_MISC_TYPES_H
#define __EGG_DBUS_MISC_TYPES_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

/* ---------------------------------------------------------------------------------------------------- */

/**
 * EGG_DBUS_TYPE_INT16:
 *
 * A fundamental type corresponding to #gint16.
 */
#define EGG_DBUS_TYPE_INT16                     (egg_dbus_int16_get_type())

/**
 * EGG_DBUS_TYPE_UINT16:
 *
 * A fundamental type corresponding to #guint16.
 */
#define EGG_DBUS_TYPE_UINT16                    (egg_dbus_uint16_get_type())


/* ---------------------------------------------------------------------------------------------------- */

 /**
 * EggDBusParamSpecInt16:
 * @parent_instance: private #GParamSpec portion
 * @minimum: minimum value for the property specified
 * @maximum: maximum value for the property specified
 * @default_value: default value for the property specified
 *
 * A #GParamSpec derived structure that contains the meta data for 16bit integer properties.
 */
struct _EggDBusParamSpecInt16
{
  GParamSpec    parent_instance;

  gint16        minimum;
  gint16        maximum;
  gint16        default_value;
};

/**
 * EggDBusParamSpecUInt16:
 * @parent_instance: private #GParamSpec portion
 * @minimum: minimum value for the property specified
 * @maximum: maximum value for the property specified
 * @default_value: default value for the property specified
 *
 * A #GParamSpec derived structure that contains the meta data for unsigned 16bit integer properties.
 */
struct _EggDBusParamSpecUInt16
{
  GParamSpec    parent_instance;

  guint16       minimum;
  guint16       maximum;
  guint16       default_value;
};

/* ---------------------------------------------------------------------------------------------------- */

GParamSpec*     egg_dbus_param_spec_int16       (const gchar    *name,
                                                 const gchar    *nick,
                                                 const gchar    *blurb,
                                                 gint16          minimum,
                                                 gint16          maximum,
                                                 gint16          default_value,
                                                 GParamFlags     flags);

GParamSpec*     egg_dbus_param_spec_uint16      (const gchar    *name,
                                                 const gchar    *nick,
                                                 const gchar    *blurb,
                                                 guint16         minimum,
                                                 guint16         maximum,
                                                 guint16         default_value,
                                                 GParamFlags     flags);

/* ---------------------------------------------------------------------------------------------------- */

/**
 * EGG_DBUS_TYPE_PARAM_INT16:
 *
 * The #GType of #EggDBusParamSpecInt16.
 */
#define EGG_DBUS_TYPE_PARAM_INT16                 (egg_dbus_param_int16_get_type())

/**
 * EGG_DBUS_IS_PARAM_SPEC_INT16:
 * @pspec: a valid #GParamSpec instance
 *
 * Checks whether the given #GParamSpec is of type %EGG_DBUS_TYPE_PARAM_INT16.
 *
 * Returns: %TRUE on success.
 */
#define EGG_DBUS_IS_PARAM_SPEC_INT16(pspec)       (G_TYPE_CHECK_INSTANCE_TYPE ((pspec), EGG_DBUS_TYPE_PARAM_INT16))

/**
 * EGG_DBUS_PARAM_SPEC_INT16:
 * @pspec: a valid #GParamSpec instance
 *
 * Cast a #GParamSpec instance into a #EggDBusParamSpecInt16.
 */
#define EGG_DBUS_PARAM_SPEC_INT16(pspec)          (G_TYPE_CHECK_INSTANCE_CAST ((pspec), EGG_DBUS_TYPE_PARAM_INT16, EggDBusParamSpecInt16))

/**
 * EGG_DBUS_TYPE_PARAM_UINT16:
 *
 * The #GType of #EggDBusParamSpecUInt16.
 */
#define EGG_DBUS_TYPE_PARAM_UINT16                (egg_dbus_param_uint16_get_type())

/**
 * EGG_DBUS_IS_PARAM_SPEC_UINT16:
 * @pspec: a valid #GParamSpec instance
 *
 * Checks whether the given #GParamSpec is of type %EGG_DBUS_TYPE_PARAM_UINT16.
 *
 * Returns: %TRUE on success.
 */
#define EGG_DBUS_IS_PARAM_SPEC_UINT16(pspec)      (G_TYPE_CHECK_INSTANCE_TYPE ((pspec), EGG_DBUS_TYPE_PARAM_UINT16))

/**
 * EGG_DBUS_PARAM_SPEC_UINT16:
 * @pspec: a valid #GParamSpec instance
 *
 * Cast a #GParamSpec instance into a #EggDBusParamSpecUInt16.
 */
#define EGG_DBUS_PARAM_SPEC_UINT16(pspec)         (G_TYPE_CHECK_INSTANCE_CAST ((pspec), EGG_DBUS_TYPE_PARAM_UINT16, EggDBusParamSpecUInt16))

/* ---------------------------------------------------------------------------------------------------- */

/**
 * EGG_DBUS_VALUE_HOLDS_INT16:
 * @value: a valid #GValue structure
 *
 * Checks whether the given #GValue can hold values of type %EGG_DBUS_TYPE_INT16.
 *
 * Returns: %TRUE on success.
 */
#define EGG_DBUS_VALUE_HOLDS_INT16(value)       (G_TYPE_CHECK_VALUE_TYPE ((value), EGG_DBUS_TYPE_INT16))

/* -- */

/**
 * EGG_DBUS_VALUE_HOLDS_UINT16:
 * @value: a valid #GValue structure
 *
 * Checks whether the given #GValue can hold values of type %EGG_DBUS_TYPE_UINT16.
 *
 * Returns: %TRUE on success.
 */
#define EGG_DBUS_VALUE_HOLDS_UINT16(value)      (G_TYPE_CHECK_VALUE_TYPE ((value), EGG_DBUS_TYPE_UINT16))

/* ---------------------------------------------------------------------------------------------------- */

void                  egg_dbus_value_set_int16    (GValue       *value,
                                                   gint16        v_int16);

gint16                egg_dbus_value_get_int16    (const GValue *value);

void                  egg_dbus_value_set_uint16   (GValue       *value,
                                                   guint16       v_uint16);

guint16               egg_dbus_value_get_uint16   (const GValue *value);

/* ---------------------------------------------------------------------------------------------------- */

GType egg_dbus_int16_get_type             (void) G_GNUC_CONST;
GType egg_dbus_uint16_get_type            (void) G_GNUC_CONST;
GType egg_dbus_param_int16_get_type       (void) G_GNUC_CONST;
GType egg_dbus_param_uint16_get_type      (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __EGG_DBUS_MISC_TYPES_H */
