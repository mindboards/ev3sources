/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-glib-lowlevel.h GLib integration details that require dbus/dbus.h
 *
 * Copyright (C) 2002, 2003  CodeFactory AB
 * Copyright (C) 2003, 2004 Red Hat, Inc.
 *
 * Licensed under the Academic Free License version 2.1
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#ifndef DBUS_GLIB_LOWLEVEL_H
#define DBUS_GLIB_LOWLEVEL_H

#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>

G_BEGIN_DECLS

void dbus_set_g_error (GError   **gerror,
                       DBusError *derror);

#define DBUS_TYPE_CONNECTION      (dbus_connection_get_g_type ())
#define DBUS_TYPE_MESSAGE         (dbus_message_get_g_type ())
#define DBUS_TYPE_PENDING_CALL    (dbus_pending_call_get_g_type ())
GType dbus_connection_get_g_type   (void) G_GNUC_CONST;
GType dbus_message_get_g_type      (void) G_GNUC_CONST;
GType dbus_pending_call_get_g_type (void) G_GNUC_CONST;

void            dbus_connection_setup_with_g_main (DBusConnection  *connection,
                                                   GMainContext    *context);
void            dbus_server_setup_with_g_main     (DBusServer      *server,
                                                   GMainContext    *context);

void dbus_g_proxy_send (DBusGProxy    *proxy,
                        DBusMessage   *message,
                        dbus_uint32_t *client_serial);

DBusConnection*  dbus_g_connection_get_connection (DBusGConnection *gconnection);
DBusGConnection* dbus_connection_get_g_connection (DBusConnection  *connection);
DBusMessage*     dbus_g_message_get_message       (DBusGMessage    *gmessage);

/* dbus_g_pending_call_get_pending_call() deliberately skipped for now;
 * not sure it makes sense to use any of the DBusPendingCall functions
 * on the wrapped pending call (once we have the right exported
 * g-functions anyhow)
 */

gchar*            dbus_g_method_get_sender    (DBusGMethodInvocation *context);

DBusMessage*      dbus_g_method_get_reply     (DBusGMethodInvocation *context);

void              dbus_g_method_send_reply    (DBusGMethodInvocation *context, 
                                               DBusMessage *reply);

G_END_DECLS

#endif /* DBUS_GLIB_LOWLEVEL_H */



