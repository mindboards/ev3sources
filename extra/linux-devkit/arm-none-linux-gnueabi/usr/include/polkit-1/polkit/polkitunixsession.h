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

#if !defined (_POLKIT_COMPILATION) && !defined(_POLKIT_INSIDE_POLKIT_H)
#error "Only <polkit/polkit.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __POLKIT_UNIX_SESSION_H
#define __POLKIT_UNIX_SESSION_H

#include <glib-object.h>
#include <gio/gio.h>
#include <polkit/polkittypes.h>

G_BEGIN_DECLS

#define POLKIT_TYPE_UNIX_SESSION          (polkit_unix_session_get_type())
#define POLKIT_UNIX_SESSION(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_TYPE_UNIX_SESSION, PolkitUnixSession))
#define POLKIT_UNIX_SESSION_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), POLKIT_TYPE_UNIX_SESSION, PolkitUnixSessionClass))
#define POLKIT_UNIX_SESSION_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_TYPE_UNIX_SESSION, PolkitUnixSessionClass))
#define POLKIT_IS_UNIX_SESSION(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_TYPE_UNIX_SESSION))
#define POLKIT_IS_UNIX_SESSION_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_TYPE_UNIX_SESSION))

#if 0
typedef struct _PolkitUnixSession PolkitUnixSession;
#endif
typedef struct _PolkitUnixSessionClass PolkitUnixSessionClass;

GType           polkit_unix_session_get_type               (void) G_GNUC_CONST;
PolkitSubject  *polkit_unix_session_new                    (const gchar        *session_id);
void            polkit_unix_session_new_for_process        (gint                pid,
                                                            GCancellable       *cancellable,
                                                            GAsyncReadyCallback callback,
                                                            gpointer            user_data);
PolkitSubject  *polkit_unix_session_new_for_process_finish (GAsyncResult       *res,
                                                            GError            **error);
PolkitSubject  *polkit_unix_session_new_for_process_sync   (gint                pid,
                                                            GCancellable       *cancellable,
                                                            GError            **error);
const gchar    *polkit_unix_session_get_session_id         (PolkitUnixSession  *session);
void            polkit_unix_session_set_session_id         (PolkitUnixSession  *session,
                                                            const gchar        *session_id);

G_END_DECLS

#endif /* __POLKIT_UNIX_SESSION_H */
