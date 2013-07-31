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

#ifndef __POLKIT_AGENT_H
#define __POLKIT_AGENT_H

#if !defined (POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE) && !defined (_POLKIT_AGENT_COMPILATION)
#error "libpolkitagent is unstable API and subject to change. You must define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE to acknowledge this."
#endif

#define _POLKIT_AGENT_INSIDE_POLKIT_AGENT_H 1
#include <polkitagent/polkitagenttypes.h>
#include <polkitagent/polkitagentlistener.h>
#include <polkitagent/polkitagentsession.h>
#undef _POLKIT_AGENT_INSIDE_POLKIT_AGENT_H

#endif /* __POLKIT_AGENT_H */
