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

#ifndef __POLKIT_IDENTITY_H
#define __POLKIT_IDENTITY_H

#include <glib-object.h>
#include <gio/gio.h>
#include <polkit/polkittypes.h>

G_BEGIN_DECLS

#define POLKIT_TYPE_IDENTITY         (polkit_identity_get_type())
#define POLKIT_IDENTITY(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_TYPE_IDENTITY, PolkitIdentity))
#define POLKIT_IS_IDENTITY(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_TYPE_IDENTITY))
#define POLKIT_IDENTITY_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE((o), POLKIT_TYPE_IDENTITY, PolkitIdentityIface))

#if 0
/**
 * PolkitIdentity:
 *
 * Generic type for all objects that can be used as identities.
 */
typedef struct _PolkitIdentity PolkitIdentity; /* Dummy typedef */
#endif
typedef struct _PolkitIdentityIface PolkitIdentityIface;

/**
 * PolkitIdentityIface:
 * @parent_iface: The parent interface.
 * @hash: Gets a hash value for a #PolkitIdentity.
 * @equal: Checks if two #PolkitIdentity<!-- -->s are equal.
 * @to_string: Serializes a #PolkitIdentity to a string that can be
 * used in polkit_identity_from_string().
 *
 * An interface for identities.
 */
struct _PolkitIdentityIface
{
  GTypeInterface parent_iface;

  guint    (*hash)      (PolkitIdentity *identity);

  gboolean (*equal)     (PolkitIdentity *a,
                         PolkitIdentity *b);

  gchar *  (*to_string) (PolkitIdentity *identity);
};

GType          polkit_identity_get_type      (void) G_GNUC_CONST;
guint          polkit_identity_hash          (PolkitIdentity *a);
gboolean       polkit_identity_equal         (PolkitIdentity *a,
                                              PolkitIdentity *b);
gchar          *polkit_identity_to_string    (PolkitIdentity *identity);
PolkitIdentity *polkit_identity_from_string  (const gchar   *str,
                                              GError       **error);

G_END_DECLS

#endif /* __POLKIT_IDENTITY_H */
