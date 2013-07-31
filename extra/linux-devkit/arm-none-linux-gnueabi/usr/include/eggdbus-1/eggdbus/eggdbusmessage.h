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

#ifndef __EGG_DBUS_MESSAGE_H
#define __EGG_DBUS_MESSAGE_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_MESSAGE         (egg_dbus_message_get_type())
#define EGG_DBUS_MESSAGE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_MESSAGE, EggDBusMessage))
#define EGG_DBUS_MESSAGE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), EGG_DBUS_TYPE_MESSAGE, EggDBusMessageClass))
#define EGG_DBUS_MESSAGE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), EGG_DBUS_TYPE_MESSAGE, EggDBusMessageClass))
#define EGG_DBUS_IS_MESSAGE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_MESSAGE))
#define EGG_DBUS_IS_MESSAGE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), EGG_DBUS_TYPE_MESSAGE))

typedef struct _EggDBusMessageClass EggDBusMessageClass;

struct _EggDBusMessage
{
  GObject parent_instance;
};

struct _EggDBusMessageClass
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

typedef enum {
  EGG_DBUS_MESSAGE_TYPE_METHOD_CALL,
  EGG_DBUS_MESSAGE_TYPE_METHOD_REPLY,
  EGG_DBUS_MESSAGE_TYPE_METHOD_ERROR_REPLY,
  EGG_DBUS_MESSAGE_TYPE_SIGNAL,
} EggDBusMessageType;

/* ---------------------------------------------------------------------------------------------------- */

GType            egg_dbus_message_get_type             (void) G_GNUC_CONST;

EggDBusMessage    *egg_dbus_message_new_for_method_reply (EggDBusMessage          *reply_to);

EggDBusMessage    *egg_dbus_message_new_for_method_error_reply  (EggDBusMessage         *reply_to,
                                                             const gchar          *error_name,
                                                             const gchar          *error_message);

EggDBusConnection *egg_dbus_message_get_connection       (EggDBusMessage          *message);

EggDBusMessageType egg_dbus_message_get_message_type     (EggDBusMessage          *message);

const gchar     *egg_dbus_message_get_object_path      (EggDBusMessage          *message);

const gchar     *egg_dbus_message_get_interface_name   (EggDBusMessage          *message);

const gchar     *egg_dbus_message_get_method_name      (EggDBusMessage          *message);

const gchar     *egg_dbus_message_get_signal_name      (EggDBusMessage          *message);

EggDBusMessage    *egg_dbus_message_get_in_reply_to      (EggDBusMessage          *message);

const gchar     *egg_dbus_message_get_sender           (EggDBusMessage          *message);

const gchar     *egg_dbus_message_get_destination      (EggDBusMessage          *message);

const gchar     *egg_dbus_message_get_signature        (EggDBusMessage          *message);

/* ---------------------------------------------------------------------------------------------------- */

gboolean       egg_dbus_message_append_string        (EggDBusMessage        *message,
                                                      const gchar           *value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_string_array  (EggDBusMessage        *message,
                                                      gchar                **value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_object_path   (EggDBusMessage        *message,
                                                      const gchar           *value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_object_path_array (EggDBusMessage          *message,
                                                          gchar                  **value,
                                                          GError                 **error);

gboolean       egg_dbus_message_append_signature   (EggDBusMessage        *message,
                                                    const gchar           *value,
                                                    GError               **error);

gboolean       egg_dbus_message_append_signature_array (EggDBusMessage          *message,
                                                        gchar                  **value,
                                                        GError                 **error);

gboolean       egg_dbus_message_append_byte          (EggDBusMessage        *message,
                                                      guchar                 value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_int16         (EggDBusMessage        *message,
                                                      gint16                 value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_uint16        (EggDBusMessage        *message,
                                                      guint16                value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_int           (EggDBusMessage        *message,
                                                      gint                   value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_uint          (EggDBusMessage        *message,
                                                      guint                  value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_int64         (EggDBusMessage        *message,
                                                      gint64                 value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_uint64        (EggDBusMessage        *message,
                                                      guint64                value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_boolean       (EggDBusMessage        *message,
                                                      gboolean               value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_double        (EggDBusMessage        *message,
                                                      gdouble                value,
                                                      GError               **error);

gboolean       egg_dbus_message_append_seq           (EggDBusMessage        *message,
                                                      EggDBusArraySeq       *seq,
                                                      const gchar           *element_signature,
                                                      GError               **error);

gboolean       egg_dbus_message_append_map           (EggDBusMessage        *message,
                                                      EggDBusHashMap        *map,
                                                      const gchar           *key_signature,
                                                      const gchar           *value_signature,
                                                      GError               **error);

gboolean       egg_dbus_message_append_structure     (EggDBusMessage        *message,
                                                      EggDBusStructure      *structure,
                                                      GError               **error);

gboolean       egg_dbus_message_append_variant       (EggDBusMessage        *message,
                                                      EggDBusVariant        *variant,
                                                      GError               **error);

gboolean       egg_dbus_message_append_gvalue        (EggDBusMessage        *message,
                                                      const GValue          *value,
                                                      const gchar           *signature,
                                                      GError               **error);

/* ---------------------------------------------------------------------------------------------------- */

gboolean       egg_dbus_message_extract_string        (EggDBusMessage         *message,
                                                       gchar                 **out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_string_array  (EggDBusMessage         *message,
                                                       gchar                ***out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_object_path   (EggDBusMessage         *message,
                                                       gchar                 **out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_object_path_array   (EggDBusMessage          *message,
                                                             gchar                 ***out_value,
                                                             GError                 **error);

gboolean       egg_dbus_message_extract_signature   (EggDBusMessage         *message,
                                                     gchar                 **out_value,
                                                     GError                **error);

gboolean       egg_dbus_message_extract_signature_array   (EggDBusMessage          *message,
                                                           gchar                 ***out_value,
                                                           GError                 **error);

gboolean       egg_dbus_message_extract_byte          (EggDBusMessage         *message,
                                                       guchar                 *out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_int16         (EggDBusMessage         *message,
                                                       gint16                  *out_value,
                                                       GError                 **error);

gboolean       egg_dbus_message_extract_uint16        (EggDBusMessage         *message,
                                                       guint16                *out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_int           (EggDBusMessage         *message,
                                                       gint                   *out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_uint          (EggDBusMessage         *message,
                                                       guint                  *out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_int64         (EggDBusMessage         *message,
                                                       gint64                 *out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_uint64        (EggDBusMessage         *message,
                                                       guint64                *out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_boolean       (EggDBusMessage         *message,
                                                       gboolean               *out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_double        (EggDBusMessage         *message,
                                                       gdouble                *out_value,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_seq           (EggDBusMessage         *message,
                                                       EggDBusArraySeq       **out_seq,
                                                       GError                **error);

gboolean       egg_dbus_message_extract_map           (EggDBusMessage          *message,
                                                       EggDBusHashMap         **out_map,
                                                       GError                 **error);

gboolean       egg_dbus_message_extract_structure     (EggDBusMessage          *message,
                                                       EggDBusStructure       **out_structure,
                                                       GError                 **error);

gboolean       egg_dbus_message_extract_variant       (EggDBusMessage          *message,
                                                       EggDBusVariant         **out_variant,
                                                       GError                 **error);

gboolean       egg_dbus_message_extract_gvalue        (EggDBusMessage          *message,
                                                       GValue                  *out_value,
                                                       GError                 **error);

/* ---------------------------------------------------------------------------------------------------- */

G_END_DECLS

#endif /* __EGG_DBUS_MESSAGE_H */
