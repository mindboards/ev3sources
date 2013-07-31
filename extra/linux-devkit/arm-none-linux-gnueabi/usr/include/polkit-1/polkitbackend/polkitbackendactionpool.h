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

#include <glib-object.h>

#ifndef __POLKIT_BACKEND_ACTION_POOL_H
#define __POLKIT_BACKEND_ACTION_POOL_H

G_BEGIN_DECLS

#define POLKIT_BACKEND_TYPE_ACTION_POOL         (polkit_backend_action_pool_get_type ())
#define POLKIT_BACKEND_ACTION_POOL(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_BACKEND_TYPE_ACTION_POOL, PolkitBackendActionPool))
#define POLKIT_BACKEND_ACTION_POOL_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), POLKIT_BACKEND_TYPE_ACTION_POOL, PolkitBackendActionPoolClass))
#define POLKIT_BACKEND_ACTION_POOL_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_BACKEND_TYPE_ACTION_POOL,PolkitBackendActionPoolClass))
#define POLKIT_BACKEND_IS_ACTION_POOL(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_BACKEND_TYPE_ACTION_POOL))
#define POLKIT_BACKEND_IS_ACTION_POOL_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_BACKEND_TYPE_ACTION_POOL))

typedef struct _PolkitBackendActionPool         PolkitBackendActionPool;
typedef struct _PolkitBackendActionPoolClass    PolkitBackendActionPoolClass;

struct _PolkitBackendActionPool
{
  GObject parent_instance;
};

struct _PolkitBackendActionPoolClass
{
  GObjectClass parent_class;

  /*< public >*/

  /*< private >*/
  /* Padding for future expansion */
  void (*_polkit_reserved1) (void);
  void (*_polkit_reserved2) (void);
  void (*_polkit_reserved3) (void);
  void (*_polkit_reserved4) (void);
  void (*_polkit_reserved5) (void);
  void (*_polkit_reserved6) (void);
  void (*_polkit_reserved7) (void);
  void (*_polkit_reserved8) (void);
};

GType                    polkit_backend_action_pool_get_type         (void) G_GNUC_CONST;
PolkitBackendActionPool *polkit_backend_action_pool_new              (GFile *directory);
GList                   *polkit_backend_action_pool_get_all_actions  (PolkitBackendActionPool  *pool,
                                                                      const gchar              *locale);

PolkitActionDescription *polkit_backend_action_pool_get_action       (PolkitBackendActionPool  *pool,
                                                                      const gchar              *action_id,
                                                                      const gchar              *locale);

G_END_DECLS

#endif /* __POLKIT_BACKEND_ACTION_POOL_H */

