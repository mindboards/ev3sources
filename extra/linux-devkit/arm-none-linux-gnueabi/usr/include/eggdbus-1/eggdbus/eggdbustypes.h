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

#if !defined (_EGG_DBUS_COMPILATION) && !defined(_EGG_DBUS_INSIDE_EGG_DBUS_H)
#error "Only <eggdbus/eggdbus.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __EGG_DBUS_TYPES_H
#define __EGG_DBUS_TYPES_H

#include <glib-object.h>
#include <gio/gio.h>
#include <eggdbus/eggdbusenumtypes.h>

G_BEGIN_DECLS

typedef struct _EggDBusConnection EggDBusConnection;

typedef struct _EggDBusObjectProxy EggDBusObjectProxy;

typedef struct _EggDBusInterfaceProxy EggDBusInterfaceProxy;

typedef struct _EggDBusMessage EggDBusMessage;

typedef struct _EggDBusVariant EggDBusVariant;

typedef struct _EggDBusStructure EggDBusStructure; /* Dummy typedef */

typedef struct _EggDBusAnonymousStructure EggDBusAnonymousStructure;

typedef struct _EggDBusInterface EggDBusInterface; /* Dummy typedef */
typedef struct _EggDBusInterfaceIface EggDBusInterfaceIface;

typedef struct _EggDBusInterfaceAnnotationInfo  EggDBusInterfaceAnnotationInfo;
typedef struct _EggDBusInterfaceArgInfo         EggDBusInterfaceArgInfo;
typedef struct _EggDBusInterfacePropertyInfo    EggDBusInterfacePropertyInfo;
typedef struct _EggDBusInterfaceMethodInfo      EggDBusInterfaceMethodInfo;
typedef struct _EggDBusInterfaceSignalInfo      EggDBusInterfaceSignalInfo;
typedef struct _EggDBusInterfaceInfo            EggDBusInterfaceInfo;
typedef struct _EggDBusInterfaceNodeInfo        EggDBusInterfaceNodeInfo;

typedef struct _EggDBusBus EggDBusBus;                                   /* Dummy typedef */
typedef struct _EggDBusPeer EggDBusPeer;                                 /* Dummy typedef */
typedef struct _EggDBusProperties EggDBusProperties;                     /* Dummy typedef */
typedef struct _EggDBusIntrospectable EggDBusIntrospectable;             /* Dummy typedef */

typedef struct _EggDBusBusNameTracker EggDBusBusNameTracker;

typedef struct _EggDBusMethodInvocation EggDBusMethodInvocation;

typedef struct _EggDBusParamSpecInt16      EggDBusParamSpecInt16;
typedef struct _EggDBusParamSpecUInt16     EggDBusParamSpecUInt16;


/**
 * EggDBusObjectPath:
 *
 * A C representable type for #EGG_DBUS_TYPE_OBJECT_PATH.
 */
typedef gchar* EggDBusObjectPath;


/**
 * EggDBusObjectPathArray:
 *
 * A C representable type for #EGG_DBUS_TYPE_OBJECT_PATH_ARRAY.
 */
typedef GStrv EggDBusObjectPathArray;

/**
 * EggDBusSignature:
 *
 * A C representable type for #EGG_DBUS_TYPE_SIGNATURE.
 */
typedef gchar* EggDBusSignature;

/**
 * EggDBusSignatureArray:
 *
 * A C representable type for #EGG_DBUS_TYPE_SIGNATURE_ARRAY.
 */
typedef GStrv EggDBusSignatureArray;

gpointer egg_dbus_structure_type_check_instance_cast (gpointer     instance,
                                                      const gchar *signature,
                                                      const gchar *c_type_name);

gboolean egg_dbus_structure_type_check_instance_type (gpointer     instance,
                                                      const gchar *signature,
                                                      const gchar *c_type_name);

typedef struct _EggDBusArraySeq EggDBusArraySeq;
typedef struct _EggDBusHashMap EggDBusHashMap;

/* ---------------------------------------------------------------------------------------------------- */

G_END_DECLS

#endif /* __EGG_DBUS_TYPES_H */
