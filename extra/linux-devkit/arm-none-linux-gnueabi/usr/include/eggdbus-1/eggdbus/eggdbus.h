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

#ifndef __EGG_DBUS_H
#define __EGG_DBUS_H

#if !defined (EGG_DBUS_I_KNOW_API_IS_SUBJECT_TO_CHANGE) && !defined (_EGG_DBUS_COMPILATION)
#error "EggDBus is unstable API and subject to change. You must define EGG_DBUS_I_KNOW_API_IS_SUBJECT_TO_CHANGE to acknowledge this before using EggDBus."
#endif

#define _EGG_DBUS_INSIDE_EGG_DBUS_H 1
#include <eggdbus/eggdbustypes.h>
#include <eggdbus/eggdbusenumtypes.h>
#include <eggdbus/eggdbusmisctypes.h>
#include <eggdbus/eggdbusobjectpath.h>
#include <eggdbus/eggdbussignature.h>
#include <eggdbus/eggdbusutils.h>
#include <eggdbus/eggdbuserror.h>

#include <eggdbus/eggdbusarrayseq.h>
#include <eggdbus/eggdbushashmap.h>
#include <eggdbus/eggdbusstructure.h>
#include <eggdbus/eggdbusvariant.h>
#include <eggdbus/eggdbusinterface.h>

#include <eggdbus/eggdbusconnection.h>
#include <eggdbus/eggdbusmessage.h>
#include <eggdbus/eggdbusobjectproxy.h>
#include <eggdbus/eggdbusinterfaceproxy.h>
#include <eggdbus/eggdbusbusnametracker.h>
#include <eggdbus/eggdbusmethodinvocation.h>

#include <eggdbus/eggdbusbus.h>
#include <eggdbus/eggdbuspeer.h>
#include <eggdbus/eggdbusproperties.h>
#include <eggdbus/eggdbusintrospectable.h>

#undef _EGG_DBUS_INSIDE_EGG_DBUS_H

#endif /* __EGG_DBUS_H */


