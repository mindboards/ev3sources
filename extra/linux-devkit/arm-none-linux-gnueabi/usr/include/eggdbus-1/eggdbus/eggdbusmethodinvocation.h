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

#ifndef __EGG_DBUS_METHOD_INVOCATION_H
#define __EGG_DBUS_METHOD_INVOCATION_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_METHOD_INVOCATION         (egg_dbus_method_invocation_get_type())
#define EGG_DBUS_METHOD_INVOCATION(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_METHOD_INVOCATION, EggDBusMethodInvocation))
#define EGG_DBUS_METHOD_INVOCATION_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), EGG_DBUS_TYPE_METHOD_INVOCATION, EggDBusMethodInvocationClass))
#define EGG_DBUS_METHOD_INVOCATION_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), EGG_DBUS_TYPE_METHOD_INVOCATION, EggDBusMethodInvocationClass))
#define EGG_DBUS_IS_METHOD_INVOCATION(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_METHOD_INVOCATION))
#define EGG_DBUS_IS_METHOD_INVOCATION_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), EGG_DBUS_TYPE_METHOD_INVOCATION))

typedef struct _EggDBusMethodInvocationClass EggDBusMethodInvocationClass;

struct _EggDBusMethodInvocation
{
  GObject parent_instance;
};

struct _EggDBusMethodInvocationClass
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

GType                  egg_dbus_method_invocation_get_type (void) G_GNUC_CONST;

EggDBusMethodInvocation *egg_dbus_method_invocation_new                 (EggDBusMessage         *request_message,
                                                                         gpointer                source_tag);

gpointer               egg_dbus_method_invocation_get_source_tag       (EggDBusMethodInvocation *method_invocation);

EggDBusMessage        *egg_dbus_method_invocation_create_reply_message (EggDBusMethodInvocation *method_invocation);

EggDBusConnection     *egg_dbus_method_invocation_get_connection       (EggDBusMethodInvocation *method_invocation);

const gchar           *egg_dbus_method_invocation_get_caller           (EggDBusMethodInvocation *method_invocation);

void                   egg_dbus_method_invocation_add_destroy_notify   (EggDBusMethodInvocation *method_invocation,
                                                                        gpointer                 data,
                                                                        GDestroyNotify           func);

void                   egg_dbus_method_invocation_return_error         (EggDBusMethodInvocation *method_invocation,
                                                                        GQuark                   domain,
                                                                        gint                     code,
                                                                        const gchar             *format,
                                                                        ...) G_GNUC_PRINTF (4, 5);

void                   egg_dbus_method_invocation_return_error_valist  (EggDBusMethodInvocation *method_invocation,
                                                                        GQuark                   domain,
                                                                        gint                     code,
                                                                        const gchar             *format,
                                                                        va_list                  var_args);

void                   egg_dbus_method_invocation_return_error_literal (EggDBusMethodInvocation *method_invocation,
                                                                        GQuark                   domain,
                                                                        gint                     code,
                                                                        const gchar             *message);

void                   egg_dbus_method_invocation_return_gerror        (EggDBusMethodInvocation *method_invocation,
                                                                        GError                  *error);

void                   egg_dbus_method_invocation_return_dbus_error    (EggDBusMethodInvocation *method_invocation,
                                                                        const gchar             *name,
                                                                        const gchar             *format,
                                                                        ...) G_GNUC_PRINTF (3, 4);

void                   egg_dbus_method_invocation_return_dbus_error_valist (EggDBusMethodInvocation *method_invocation,
                                                                            const gchar             *name,
                                                                            const gchar             *format,
                                                                            va_list                  var_args);

void                   egg_dbus_method_invocation_return_dbus_error_literal (EggDBusMethodInvocation *method_invocation,
                                                                             const gchar             *name,
                                                                             const gchar             *message);


G_END_DECLS

#endif /* __EGG_DBUS_METHOD_INVOCATION_H */
