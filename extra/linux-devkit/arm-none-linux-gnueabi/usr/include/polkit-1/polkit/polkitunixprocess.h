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

#ifndef __POLKIT_UNIX_PROCESS_H
#define __POLKIT_UNIX_PROCESS_H

#include <unistd.h>
#include <sys/types.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <polkit/polkittypes.h>

G_BEGIN_DECLS

#define POLKIT_TYPE_UNIX_PROCESS          (polkit_unix_process_get_type())
#define POLKIT_UNIX_PROCESS(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_TYPE_UNIX_PROCESS, PolkitUnixProcess))
#define POLKIT_UNIX_PROCESS_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), POLKIT_TYPE_UNIX_PROCESS, PolkitUnixProcessClass))
#define POLKIT_UNIX_PROCESS_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_TYPE_UNIX_PROCESS, PolkitUnixProcessClass))
#define POLKIT_IS_UNIX_PROCESS(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_TYPE_UNIX_PROCESS))
#define POLKIT_IS_UNIX_PROCESS_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_TYPE_UNIX_PROCESS))

#if 0
typedef struct _PolkitUnixProcess PolkitUnixProcess;
#endif
typedef struct _PolkitUnixProcessClass PolkitUnixProcessClass;

GType           polkit_unix_process_get_type       (void) G_GNUC_CONST;
PolkitSubject  *polkit_unix_process_new            (gint pid);
PolkitSubject  *polkit_unix_process_new_full       (gint pid,
                                                    guint64 start_time);

gint            polkit_unix_process_get_pid        (PolkitUnixProcess *process);
guint64         polkit_unix_process_get_start_time (PolkitUnixProcess *process);
void            polkit_unix_process_set_pid        (PolkitUnixProcess *process,
                                                    gint               pid);
gint            polkit_unix_process_get_owner      (PolkitUnixProcess  *process,
                                                    GError            **error);

G_END_DECLS

#endif /* __POLKIT_UNIX_PROCESS_H */
