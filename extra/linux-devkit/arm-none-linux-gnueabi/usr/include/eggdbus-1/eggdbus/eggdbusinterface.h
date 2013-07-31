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

#ifndef __EGG_DBUS_INTERFACE_H
#define __EGG_DBUS_INTERFACE_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_INTERFACE         (egg_dbus_interface_get_type())
#define EGG_DBUS_INTERFACE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_INTERFACE, EggDBusInterface))
#define EGG_DBUS_IS_INTERFACE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_INTERFACE))
#define EGG_DBUS_INTERFACE_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE((o), EGG_DBUS_TYPE_INTERFACE, EggDBusInterfaceIface))

#if 0
typedef struct _EggDBusInterface EggDBusInterface; /* Dummy typedef */
#endif

#if 0
typedef struct _EggDBusInterfaceIface EggDBusInterfaceIface;
#endif

/**
 * EggDBusInterfacePropertyInfoFlags:
 * @EGG_DBUS_INTERFACE_PROPERTY_INFO_FLAGS_NONE: No flags set.
 * @EGG_DBUS_INTERFACE_PROPERTY_INFO_FLAGS_READABLE: Property is readable.
 * @EGG_DBUS_INTERFACE_PROPERTY_INFO_FLAGS_WRITABLE: Property is writable.
 *
 * Flags describing the access control of a D-Bus property.
 */
typedef enum
{
  EGG_DBUS_INTERFACE_PROPERTY_INFO_FLAGS_NONE = 0,
  EGG_DBUS_INTERFACE_PROPERTY_INFO_FLAGS_READABLE = (1<<0),
  EGG_DBUS_INTERFACE_PROPERTY_INFO_FLAGS_WRITABLE = (1<<1),
} EggDBusInterfacePropertyInfoFlags;

/**
 * EggDBusInterfaceAnnotationInfo:
 * @key: The name of the annotation, e.g. <literal>org.freedesktop.DBus.Deprecated</literal>
 * @value: The value of the annotation.
 * @annotations: A pointer to an array of annotations for the annotation or %NULL if there are no annotations.
 *
 * Information about an annotation.
 *
 * By convention, an array of annotations is always terminated by an element
 * where @key is %NULL.
 */
struct _EggDBusInterfaceAnnotationInfo
{
  const gchar                           *key;
  const gchar                           *value;
  const EggDBusInterfaceAnnotationInfo  *annotations;
};

/**
 * EggDBusInterfaceArgInfo:
 * @name: Name of the argument, e.g. @unix_user_id.
 * @signature: D-Bus signature of the argument (a single complete type).
 * @annotations: A pointer to an array of annotations for the argument or %NULL if there are no annotations.
 *
 * Information about an argument for a method or a signal.
 */
struct _EggDBusInterfaceArgInfo
{
  const gchar                           *name;
  const gchar                           *signature;
  const EggDBusInterfaceAnnotationInfo  *annotations;
};

/**
 * EggDBusInterfaceMethodInfo:
 * @name: The name of the D-Bus method, e.g. @RequestName.
 * @in_signature: The combined D-Bus signature of all arguments passed to the method (@in_num_args complete types).
 * @in_num_args: Number of arguments passed to the method.
 * @in_args: A pointer to an array of @in_num_args #EggDBusInterfaceArgInfo structures or %NULL if @in_num_args is 0.
 * @out_signature: The combined D-Bus signature of all arguments the method returns (@out_num_args complete types).
 * @out_num_args: Number of arguments the method returns.
 * @out_args: A pointer to an array of @out_num_args #EggDBusInterfaceArgInfo structures or %NULL if @out_num_args is 0.
 * @annotations: A pointer to an array of annotations for the method or %NULL if there are no annotations.
 *
 * Information about a method on an D-Bus interface.
 */
struct _EggDBusInterfaceMethodInfo
{
  const gchar                           *name;

  const gchar                           *in_signature;
  guint                                  in_num_args;
  const EggDBusInterfaceArgInfo         *in_args;

  const gchar                           *out_signature;
  guint                                  out_num_args;
  const EggDBusInterfaceArgInfo         *out_args;

  const EggDBusInterfaceAnnotationInfo  *annotations;
};

/**
 * EggDBusInterfaceSignalInfo:
 * @name: The name of the D-Bus signal, e.g. @NameOwnerChanged.
 * @g_name: The GObject name of the D-Bus signal (in lower case with hyphens), e.g. <emphasis>name-owner-changed</emphasis>.
 * @signature: The combined D-Bus signature of all arguments of the signal (@num_args complete types).
 * @num_args: Number of arguments of the signal.
 * @args: A pointer to an array of @num_args #EggDBusInterfaceArgInfo structures or %NULL if @num_args is 0.
 * @annotations: A pointer to an array of annotations for the signal or %NULL if there are no annotations.
 *
 * Information about a signal on a D-Bus interface.
 */
struct _EggDBusInterfaceSignalInfo
{
  const gchar                           *name;
  const gchar                           *g_name;

  const gchar                           *signature;
  guint                                  num_args;
  const EggDBusInterfaceArgInfo         *args;

  const EggDBusInterfaceAnnotationInfo  *annotations;
};

/**
 * EggDBusInterfacePropertyInfo:
 * @name: The name of the D-Bus property, e.g. @SupportedFilesystems.
 * @g_name: The GObject name of the D-Bus property (in lower case with hyphens), e.g. <emphasis>supported-filesystems</emphasis>.
 * @signature: The D-Bus signature of the property (a single complete type).
 * @flags: Access control flags for the property.
 * @annotations: A pointer to an array of annotations for the property or %NULL if there are no annotations.
 *
 * Information about a D-Bus property on a D-Bus interface.
 */
struct _EggDBusInterfacePropertyInfo
{
  const gchar                           *name;
  const gchar                           *g_name;
  const gchar                           *signature;
  EggDBusInterfacePropertyInfoFlags      flags;
  const EggDBusInterfaceAnnotationInfo  *annotations;
};

/**
 * EggDBusInterfaceInfo:
 * @name: The name of the D-Bus interface, e.g. <literal>org.freedesktop.DBus.Properties</literal>.
 * @num_methods: Number of methods on the interface.
 * @methods: A pointer to an array of @num_methods #EggDBusInterfaceMethodInfo structures or %NULL if @num_methods is 0.
 * @num_signals: Number of signals on the interface.
 * @signals: A pointer to an array of @num_signals #EggDBusInterfaceSignalInfo structures or %NULL if @num_signals is 0.
 * @num_properties: Number of properties on the interface.
 * @properties: A pointer to an array of @num_properties #EggDBusInterfacePropertyInfo structures or %NULL if @num_properties is 0.
 * @annotations: A pointer to an array of annotations for the interface or %NULL if there are no annotations.
 *
 * Information about a D-Bus interface.
 */
struct _EggDBusInterfaceInfo
{
  const gchar                           *name;

  guint                                  num_methods;
  const EggDBusInterfaceMethodInfo      *methods;

  guint                                  num_signals;
  const EggDBusInterfaceSignalInfo      *signals;

  guint                                  num_properties;
  const EggDBusInterfacePropertyInfo    *properties;

  const EggDBusInterfaceAnnotationInfo  *annotations;
};

/**
 * EggDBusInterfaceNodeInfo:
 * @path: The path of the node or %NULL if omitted. Note that this may be a relative path. See the D-Bus specification for more details.
 * @num_interfaces: Number of interfaces of the node.
 * @interfaces: A pointer to an array of @num_interfaces #EggDBusInterfaceInfo structures or %NULL if @num_interfaces is 0.
 * @num_nodes: Number of child nodes.
 * @nodes: A pointer to an array of @num_nodes #EggDBusInterfaceNodeInfo structures or %NULL if @num_nodes is 0.
 * @annotations: A pointer to an array of annotations for the node or %NULL if there are no annotations.
 *
 * Information about nodes in a remote object hierarchy.
 */
struct _EggDBusInterfaceNodeInfo
{
  const gchar                           *path;

  guint                                  num_interfaces;
  const EggDBusInterfaceInfo            *interfaces;

  guint                                  num_nodes;
  const EggDBusInterfaceNodeInfo        *nodes;

  const EggDBusInterfaceAnnotationInfo  *annotations;
};

/**
 * EggDBusInterfaceIface:
 * @g_iface: Parent interface.
 * @get_interface_info: Gets introspection information about the interface.
 * @handle_message: Handle an incoming message for an interface.  This is used for both dispatching
 * signals for proxies and for handling method calls invocations originating from remote processes.
 * @get_interface_proxy: Gets an interface proxy that can be used to access the D-Bus interface
 * for the remote object represented by the given #EggDBusObjectProxy object. The returned instance
 * must be derived from #EggDBusInterfaceProxy and must implement the given interface.
 *
 * VTable for #EggDBusInterface. This GInterface is used for both interface proxies (on
 * the client side) and when exporting D-Bus interfaces (on the server side).
 *
 * When exporting a D-Bus interface (using egg_dbus_connection_register_interface()), simply
 * provide #GObject derived instance that implement this interface.
 *
 * You need to implement the @get_interface_info vfunc to describe what the D-Bus interface
 * looks like (this is designed to be stored in read-only data) as well as how D-Bus signal and
 * property names (typically CamelCase) are mapped to GObject signal and property names (typically
 * lowercase using hyphens).
 *
 * Incoming method calls are dispatched through the @handle_message vfunc, D-Bus properties
 * are get/set through the get_property() / set_property() vfuncs on the #GObject class and
 * emitted GObject signals (that matches D-Bus signals) are broadcast on the #EggDBusConnection
 * instance(s) that the exported D-Bus interface have been registered with.
 *
 * Implementing the @get_interface_proxy vfunc is optional. Typically language bindings
 * for static languages (such as C or C++) wants to implement this vfunc to provide a
 * type-safe client-side API for accessing the D-Bus interface. If implemented, then
 * D-Bus signals received on the client-side will be propagated from #EggDBusConnection
 * through #EggDBusObjectProxy to the @handle_message vfunc. Typically, implementations
 * will want to turn the #EggDBusMessage into a GObject signal.
 */
struct _EggDBusInterfaceIface
{
  GTypeInterface g_iface;

  const EggDBusInterfaceInfo * (*get_interface_info)  (void);

  void                         (*handle_message)      (EggDBusInterface    *interface,
                                                       EggDBusMessage      *message);

  EggDBusInterfaceProxy *      (*get_interface_proxy) (EggDBusObjectProxy  *object_proxy);

  /*< private >*/

  /* Padding for future expansion */
  void (*_g_reserved0) (void);
  void (*_g_reserved1) (void);
  void (*_g_reserved2) (void);
  void (*_g_reserved3) (void);
  void (*_g_reserved4) (void);
  void (*_g_reserved5) (void);
  void (*_g_reserved6) (void);
  void (*_g_reserved7) (void);
};

GType egg_dbus_interface_get_type (void) G_GNUC_CONST;

const gchar                        *egg_dbus_interface_annotation_info_lookup          (const EggDBusInterfaceAnnotationInfo *annotations,
                                                                                        const gchar                          *annotation_name);

const EggDBusInterfaceSignalInfo   *egg_dbus_interface_info_lookup_signal_for_g_name   (const EggDBusInterfaceInfo           *info,
                                                                                        const gchar                          *g_name);

const EggDBusInterfacePropertyInfo *egg_dbus_interface_info_lookup_property_for_name   (const EggDBusInterfaceInfo           *info,
                                                                                        const gchar                          *name);

const EggDBusInterfacePropertyInfo *egg_dbus_interface_info_lookup_property_for_g_name (const EggDBusInterfaceInfo           *info,
                                                                                        const gchar                          *g_name);

EggDBusInterfaceNodeInfo           *egg_dbus_interface_new_node_info_from_xml          (const gchar                          *xml_data,
                                                                                        GError                              **error);

void                                egg_dbus_interface_node_info_free                  (EggDBusInterfaceNodeInfo             *node_info);

void                                egg_dbus_interface_info_to_xml                     (const EggDBusInterfaceInfo           *info,
                                                                                        guint                                 indent,
                                                                                        GString                              *string_builder);

void                                egg_dbus_interface_node_info_to_xml                (const EggDBusInterfaceNodeInfo       *node_info,
                                                                                        guint                                 indent,
                                                                                        GString                              *string_builder);


GType *egg_dbus_bindings_get_error_domain_types (void);

G_END_DECLS

#endif /* __EGG_DBUS_INTERFACE_H */
