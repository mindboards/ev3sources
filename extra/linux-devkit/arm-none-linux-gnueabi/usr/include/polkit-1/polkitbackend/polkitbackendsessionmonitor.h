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

#if !defined (_POLKIT_BACKEND_COMPILATION) && !defined(_POLKIT_BACKEND_INSIDE_POLKIT_BACKEND_H)
#error "Only <polkitbackend/polkitbackend.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __POLKIT_BACKEND_SESSION_MONITOR_H
#define __POLKIT_BACKEND_SESSION_MONITOR_H

#include <glib-object.h>
#include <polkitbackend/polkitbackendtypes.h>

G_BEGIN_DECLS

#define POLKIT_BACKEND_TYPE_SESSION_MONITOR         (polkit_backend_session_monitor_get_type ())
#define POLKIT_BACKEND_SESSION_MONITOR(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_BACKEND_TYPE_SESSION_MONITOR, PolkitBackendSessionMonitor))
#define POLKIT_BACKEND_SESSION_MONITOR_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), POLKIT_BACKEND_TYPE_SESSION_MONITOR, PolkitBackendSessionMonitorClass))
#define POLKIT_BACKEND_SESSION_MONITOR_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_BACKEND_TYPE_SESSION_MONITOR,PolkitBackendSessionMonitorClass))
#define POLKIT_BACKEND_IS_SESSION_MONITOR(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_BACKEND_TYPE_SESSION_MONITOR))
#define POLKIT_BACKEND_IS_SESSION_MONITOR_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_BACKEND_TYPE_SESSION_MONITOR))

#if 0
typedef struct _PolkitBackendSessionMonitor         PolkitBackendSessionMonitor;
#endif
typedef struct _PolkitBackendSessionMonitorClass    PolkitBackendSessionMonitorClass;

GType                        polkit_backend_session_monitor_get_type     (void) G_GNUC_CONST;
PolkitBackendSessionMonitor *polkit_backend_session_monitor_new          (void);
GList                       *polkit_backend_session_monitor_get_sessions (PolkitBackendSessionMonitor *monitor);

PolkitIdentity              *polkit_backend_session_monitor_get_user_for_subject (PolkitBackendSessionMonitor *monitor,
                                                                                  PolkitSubject               *subject,
                                                                                  GError                     **error);

PolkitSubject               *polkit_backend_session_monitor_get_session_for_subject (PolkitBackendSessionMonitor *monitor,
                                                                                     PolkitSubject               *subject,
                                                                                     GError                     **error);

gboolean                     polkit_backend_session_monitor_is_session_local  (PolkitBackendSessionMonitor *monitor,
                                                                               PolkitSubject               *session);

gboolean                     polkit_backend_session_monitor_is_session_active (PolkitBackendSessionMonitor *monitor,
                                                                               PolkitSubject               *session);

G_END_DECLS

#endif /* __POLKIT_BACKEND_SESSION_MONITOR_H */

