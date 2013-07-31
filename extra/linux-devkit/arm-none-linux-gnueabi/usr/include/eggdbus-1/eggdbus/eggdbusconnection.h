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

#ifndef __EGG_DBUS_CONNECTION_H
#define __EGG_DBUS_CONNECTION_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_CONNECTION         (egg_dbus_connection_get_type())
#define EGG_DBUS_CONNECTION(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_CONNECTION, EggDBusConnection))
#define EGG_DBUS_CONNECTION_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), EGG_DBUS_TYPE_CONNECTION, EggDBusConnectionClass))
#define EGG_DBUS_CONNECTION_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), EGG_DBUS_TYPE_CONNECTION, EggDBusConnectionClass))
#define EGG_DBUS_IS_CONNECTION(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_CONNECTION))
#define EGG_DBUS_IS_CONNECTION_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), EGG_DBUS_TYPE_CONNECTION))

typedef struct _EggDBusConnectionClass EggDBusConnectionClass;

struct _EggDBusConnection
{
  GObject parent_instance;
};

struct _EggDBusConnectionClass
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

GType              egg_dbus_connection_get_type        (void) G_GNUC_CONST;

/**
 * EggDBusCallFlags:
 * @EGG_DBUS_CALL_FLAGS_NONE: No flags set.
 * @EGG_DBUS_CALL_FLAGS_BLOCK_IN_MAINLOOP: When doing a synchronous
 * call, block in a main loop instead of waiting for the reply
 * to arrive (which reorders and queues up other messages for delivery
 * <emphasis>after</emphasis> the reply have been processed). Be
 * careful using this option, it may mean that your call site will
 * have to handle reentrancy.
 * @EGG_DBUS_CALL_FLAGS_TIMEOUT_NONE: Wait indefinitely instead of using
 * the default timeout.
 *
 * Flags used for sending messages.
 *
 */
typedef enum {
  EGG_DBUS_CALL_FLAGS_NONE = 0,
  EGG_DBUS_CALL_FLAGS_BLOCK_IN_MAINLOOP = (1<<0),
  EGG_DBUS_CALL_FLAGS_TIMEOUT_NONE = (1<<1),
} EggDBusCallFlags;

typedef enum {
  EGG_DBUS_BUS_TYPE_NONE,
  EGG_DBUS_BUS_TYPE_SESSION,
  EGG_DBUS_BUS_TYPE_SYSTEM,
  EGG_DBUS_BUS_TYPE_STARTER,
} EggDBusBusType;

/* TODO: non-bus connections */

EggDBusConnection *egg_dbus_connection_get_for_bus                    (EggDBusBusType       bus_type);

EggDBusObjectProxy  *egg_dbus_connection_get_object_proxy             (EggDBusConnection   *connection,
                                                                       const gchar         *name,
                                                                       const gchar         *object_path);

EggDBusBus        *egg_dbus_connection_get_bus                        (EggDBusConnection   *connection);

const gchar       *egg_dbus_connection_get_unique_name                (EggDBusConnection   *connection);

void               egg_dbus_connection_register_interface             (EggDBusConnection   *connection,
                                                                       const gchar         *object_path,
                                                                       GType                interface_type,
                                                                       ...);

void               egg_dbus_connection_register_interface_valist      (EggDBusConnection   *connection,
                                                                       const gchar         *object_path,
                                                                       GType                interface_type,
                                                                       va_list              var_args);

void               egg_dbus_connection_unregister_interface           (EggDBusConnection   *connection,
                                                                       const gchar         *object_path,
                                                                       GType                interface_type,
                                                                       ...);

void               egg_dbus_connection_unregister_interface_valist    (EggDBusConnection   *connection,
                                                                       const gchar         *object_path,
                                                                       GType                interface_type,
                                                                       va_list              var_args);

guint              egg_dbus_connection_lookup_interface               (EggDBusConnection   *connection,
                                                                       const gchar         *object_path,
                                                                       GType              **out_interface_types,
                                                                       GObject           ***out_interface_stubs);

EggDBusMessage    *egg_dbus_connection_new_message_for_signal         (EggDBusConnection   *connection,
                                                                       const gchar         *sender,
                                                                       const gchar         *destination,
                                                                       const gchar         *object_path,
                                                                       const gchar         *interface_name,
                                                                       const gchar         *signal_name);

EggDBusMessage    *egg_dbus_connection_new_message_for_method_call    (EggDBusConnection   *connection,
                                                                       const gchar         *sender,
                                                                       const gchar         *destination,
                                                                       const gchar         *object_path,
                                                                       const gchar         *interface_name,
                                                                       const gchar         *method_name);

void               egg_dbus_connection_send_message                   (EggDBusConnection   *connection,
                                                                       EggDBusMessage      *message);

guint              egg_dbus_connection_send_message_with_reply        (EggDBusConnection   *connection,
                                                                       EggDBusCallFlags     call_flags,
                                                                       EggDBusMessage      *message,
                                                                       GType               *error_types,
                                                                       GCancellable        *cancellable,
                                                                       GAsyncReadyCallback  callback,
                                                                       gpointer             user_data);

EggDBusMessage    *egg_dbus_connection_send_message_with_reply_finish (EggDBusConnection   *connection,
                                                                       GAsyncResult        *res,
                                                                       GError             **error);

EggDBusMessage    *egg_dbus_connection_send_message_with_reply_sync   (EggDBusConnection   *connection,
                                                                       EggDBusCallFlags     call_flags,
                                                                       EggDBusMessage      *message,
                                                                       GType               *error_types,
                                                                       GCancellable        *cancellable,
                                                                       GError             **error);

void               egg_dbus_connection_pending_call_cancel            (EggDBusConnection   *connection,
                                                                       guint                pending_call_id);

void               egg_dbus_connection_pending_call_block             (EggDBusConnection   *connection,
                                                                       guint                pending_call_id);

/* ---------------------------------------------------------------------------------------------------- */

G_END_DECLS

#endif /* __EGG_DBUS_CONNECTION_H */
