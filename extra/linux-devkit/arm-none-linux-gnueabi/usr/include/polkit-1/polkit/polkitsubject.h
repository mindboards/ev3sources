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

#ifndef __POLKIT_SUBJECT_H
#define __POLKIT_SUBJECT_H

#include <glib-object.h>
#include <gio/gio.h>
#include <polkit/polkittypes.h>

G_BEGIN_DECLS

#define POLKIT_TYPE_SUBJECT         (polkit_subject_get_type())
#define POLKIT_SUBJECT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_TYPE_SUBJECT, PolkitSubject))
#define POLKIT_IS_SUBJECT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_TYPE_SUBJECT))
#define POLKIT_SUBJECT_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE((o), POLKIT_TYPE_SUBJECT, PolkitSubjectIface))

#if 0
/**
 * PolkitSubject:
 *
 * Generic type for all objects that can be used as subjects.
 */
typedef struct _PolkitSubject PolkitSubject; /* Dummy typedef */
#endif
typedef struct _PolkitSubjectIface PolkitSubjectIface;

/**
 * PolkitSubjectIface:
 * @parent_iface: The parent interface.
 * @hash: Gets a hash value for a #PolkitSubject.
 * @equal: Checks if two #PolkitSubject<!-- -->s are equal.
 * @to_string: Serializes a #PolkitSubject to a string that can be
 * used in polkit_subject_from_string().
 * @exists: Asynchronously check if a #PolkitSubject exists.
 * @exists_finish: Finishes checking if a #PolkitSubject exists.
 * @exists_sync: Synchronously check if a #PolkitSubject exists.
 *
 * An interface for subjects.
 */
struct _PolkitSubjectIface
{
  GTypeInterface parent_iface;

  guint    (*hash)          (PolkitSubject       *subject);

  gboolean (*equal)         (PolkitSubject       *a,
                             PolkitSubject       *b);

  gchar *  (*to_string)     (PolkitSubject       *subject);

  void     (*exists)        (PolkitSubject       *subject,
                             GCancellable        *cancellable,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data);

  gboolean (*exists_finish) (PolkitSubject       *subject,
                             GAsyncResult        *res,
                             GError             **error);

  gboolean (*exists_sync)   (PolkitSubject       *subject,
                             GCancellable        *cancellable,
                             GError             **error);
};

GType          polkit_subject_get_type      (void) G_GNUC_CONST;
guint          polkit_subject_hash          (PolkitSubject       *subject);
gboolean       polkit_subject_equal         (PolkitSubject       *a,
                                             PolkitSubject       *b);
gchar         *polkit_subject_to_string     (PolkitSubject       *subject);
PolkitSubject *polkit_subject_from_string   (const gchar         *str,
                                             GError             **error);
void           polkit_subject_exists        (PolkitSubject       *subject,
                                             GCancellable        *cancellable,
                                             GAsyncReadyCallback  callback,
                                             gpointer             user_data);
gboolean       polkit_subject_exists_finish (PolkitSubject       *subject,
                                             GAsyncResult        *res,
                                             GError             **error);
gboolean       polkit_subject_exists_sync   (PolkitSubject       *subject,
                                             GCancellable        *cancellable,
                                             GError             **error);

G_END_DECLS

#endif /* __POLKIT_SUBJECT_H */
