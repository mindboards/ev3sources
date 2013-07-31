/* GConf
 * Copyright (C) 1999, 2000 Red Hat Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef GCONF_GCONF_LISTENERS_H
#define GCONF_GCONF_LISTENERS_H

#include <glib.h>

G_BEGIN_DECLS

/*
 * The listeners object is used to store listeners who want notification
 * of changes in a namespace section.
 *
 * It's shared between gconfd and the GObject convenience wrapper.
 * It's also a public API.
 */

typedef struct _GConfListeners GConfListeners;

typedef void (*GConfListenersCallback)(GConfListeners* listeners,
                                       const gchar*    all_above_key,
                                       guint           cnxn_id,
                                       gpointer        listener_data,
                                       gpointer        user_data);

typedef void (*GConfListenersForeach) (const gchar* location,
                                       guint        cnxn_id,
                                       gpointer     listener_data,
                                       gpointer     user_data);

typedef gboolean (*GConfListenersPredicate) (const gchar* location,
                                             guint        cnxn_id,
                                             gpointer     listener_data,
                                             gpointer     user_data);

GConfListeners*     gconf_listeners_new     (void);

void                gconf_listeners_free    (GConfListeners* listeners);

guint               gconf_listeners_add     (GConfListeners* listeners,
                                             const gchar* listen_point,
                                             gpointer listener_data,
                                             /* can be NULL */
                                             GFreeFunc destroy_notify);

/* Safe on nonexistent listeners, for robustness against broken
 * clients
 */

void     gconf_listeners_remove   (GConfListeners          *listeners,
                                   guint                    cnxn_id);
void     gconf_listeners_notify   (GConfListeners          *listeners,
                                   const gchar             *all_above,
                                   GConfListenersCallback   callback,
                                   gpointer                 user_data);
guint    gconf_listeners_count    (GConfListeners          *listeners);
void     gconf_listeners_foreach  (GConfListeners          *listeners,
                                   GConfListenersForeach    callback,
                                   gpointer                 user_data);
gboolean gconf_listeners_get_data (GConfListeners          *listeners,
                                   guint                    cnxn_id,
                                   gpointer                *listener_data_p,
                                   const gchar            **location_p);

void     gconf_listeners_remove_if (GConfListeners         *listeners,
                                    GConfListenersPredicate predicate,
                                    gpointer                user_data);

G_END_DECLS

#endif



