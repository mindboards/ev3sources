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

#if !defined(_POLKIT_AGENT_INSIDE_POLKIT_AGENT_H) && !defined (_POLKIT_AGENT_COMPILATION)
#error "Only <polkitagent/polkitagent.h> can be included directly, this file may disappear or change contents"
#endif

#ifndef __POLKIT_AGENT_LISTENER_H
#define __POLKIT_AGENT_LISTENER_H

#include <polkit/polkit.h>
#include <polkitagent/polkitagenttypes.h>

G_BEGIN_DECLS

#define POLKIT_AGENT_TYPE_LISTENER         (polkit_agent_listener_get_type ())
#define POLKIT_AGENT_LISTENER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_AGENT_TYPE_LISTENER, PolkitAgentListener))
#define POLKIT_AGENT_LISTENER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), POLKIT_AGENT_TYPE_LISTENER, PolkitAgentListenerClass))
#define POLKIT_AGENT_LISTENER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_AGENT_TYPE_LISTENER,PolkitAgentListenerClass))
#define POLKIT_AGENT_IS_LISTENER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_AGENT_TYPE_LISTENER))
#define POLKIT_AGENT_IS_LISTENER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_AGENT_TYPE_LISTENER))

struct _PolkitAgentListenerClass;
typedef struct _PolkitAgentListenerClass PolkitAgentListenerClass;

/**
 * PolkitAgentListener:
 *
 * The #PolkitAgentListener struct should not be accessed directly.
 */
struct _PolkitAgentListener
{
  GObject parent_instance;
};

/**
 * PolkitAgentListenerClass:
 * @parent_class: The parent class.
 * @initiate_authentication: Handle an authentication request, see polkit_agent_listener_initiate_authentication().
 * @initiate_authentication_finish: Finishes handling an authentication request, see polkit_agent_listener_initiate_authentication_finish().
 *
 * VFuncs that authentication agents needs to implement.
 */
struct _PolkitAgentListenerClass
{
  /*< public >*/
  GObjectClass parent_class;

  /* Vtable */
  void     (*initiate_authentication)        (PolkitAgentListener  *listener,
                                              const gchar          *action_id,
                                              const gchar          *message,
                                              const gchar          *icon_name,
                                              PolkitDetails        *details,
                                              const gchar          *cookie,
                                              GList                *identities,
                                              GCancellable         *cancellable,
                                              GAsyncReadyCallback   callback,
                                              gpointer              user_data);

  gboolean (*initiate_authentication_finish) (PolkitAgentListener  *listener,
                                              GAsyncResult         *res,
                                              GError              **error);

  /*< private >*/
  /* Padding for future expansion */
  void (*_polkit_reserved0) (void);
  void (*_polkit_reserved1) (void);
  void (*_polkit_reserved2) (void);
  void (*_polkit_reserved3) (void);
  void (*_polkit_reserved4) (void);
  void (*_polkit_reserved5) (void);
  void (*_polkit_reserved6) (void);
  void (*_polkit_reserved7) (void);
};

GType     polkit_agent_listener_get_type                        (void) G_GNUC_CONST;

void      polkit_agent_listener_initiate_authentication         (PolkitAgentListener  *listener,
                                                                 const gchar          *action_id,
                                                                 const gchar          *message,
                                                                 const gchar          *icon_name,
                                                                 PolkitDetails        *details,
                                                                 const gchar          *cookie,
                                                                 GList                *identities,
                                                                 GCancellable         *cancellable,
                                                                 GAsyncReadyCallback   callback,
                                                                 gpointer              user_data);

gboolean  polkit_agent_listener_initiate_authentication_finish  (PolkitAgentListener  *listener,
                                                                 GAsyncResult         *res,
                                                                 GError              **error);

gboolean  polkit_agent_register_listener                        (PolkitAgentListener  *listener,
                                                                 PolkitSubject        *subject,
                                                                 const gchar          *object_path,
                                                                 GError              **error);

G_END_DECLS

#endif /* __POLKIT_AGENT_LISTENER_H */
