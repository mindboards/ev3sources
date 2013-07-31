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

#ifndef __POLKIT_AGENT_SESSION_H
#define __POLKIT_AGENT_SESSION_H

#include <polkit/polkit.h>
#include <polkitagent/polkitagenttypes.h>

G_BEGIN_DECLS

#define POLKIT_AGENT_TYPE_SESSION          (polkit_agent_session_get_type())
#define POLKIT_AGENT_SESSION(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_AGENT_TYPE_SESSION, PolkitAgentSession))
#define POLKIT_AGENT_SESSION_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), POLKIT_AGENT_TYPE_SESSION, PolkitAgentSessionClass))
#define POLKIT_AGENT_SESSION_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_AGENT_TYPE_SESSION, PolkitAgentSessionClass))
#define POLKIT_AGENT_IS_SESSION(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_AGENT_TYPE_SESSION))
#define POLKIT_AGENT_IS_SESSION_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_AGENT_TYPE_SESSION))

struct _PolkitAgentSessionClass;
typedef struct _PolkitAgentSessionClass PolkitAgentSessionClass;

GType                polkit_agent_session_get_type    (void) G_GNUC_CONST;
PolkitAgentSession  *polkit_agent_session_new         (PolkitIdentity      *identity,
                                                       const gchar         *cookie);
void                 polkit_agent_session_initiate    (PolkitAgentSession  *session);
void                 polkit_agent_session_response    (PolkitAgentSession  *session,
                                                       const gchar         *response);
void                 polkit_agent_session_cancel      (PolkitAgentSession  *session);

G_END_DECLS

#endif /* __POLKIT_AGENT_SESSION_H */
