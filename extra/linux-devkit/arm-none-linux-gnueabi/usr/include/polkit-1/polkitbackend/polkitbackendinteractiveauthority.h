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

#ifndef __POLKIT_BACKEND_INTERACTIVE_AUTHORITY_H
#define __POLKIT_BACKEND_INTERACTIVE_AUTHORITY_H

#include <glib-object.h>
#include <polkitbackend/polkitbackendtypes.h>
#include <polkitbackend/polkitbackendauthority.h>

G_BEGIN_DECLS

#define POLKIT_BACKEND_TYPE_INTERACTIVE_AUTHORITY         (polkit_backend_interactive_authority_get_type ())
#define POLKIT_BACKEND_INTERACTIVE_AUTHORITY(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), POLKIT_BACKEND_TYPE_INTERACTIVE_AUTHORITY, PolkitBackendInteractiveAuthority))
#define POLKIT_BACKEND_INTERACTIVE_AUTHORITY_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), POLKIT_BACKEND_TYPE_INTERACTIVE_AUTHORITY, PolkitBackendInteractiveAuthorityClass))
#define POLKIT_BACKEND_INTERACTIVE_AUTHORITY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), POLKIT_BACKEND_TYPE_INTERACTIVE_AUTHORITY,PolkitBackendInteractiveAuthorityClass))
#define POLKIT_BACKEND_IS_INTERACTIVE_AUTHORITY(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), POLKIT_BACKEND_TYPE_INTERACTIVE_AUTHORITY))
#define POLKIT_BACKEND_IS_INTERACTIVE_AUTHORITY_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), POLKIT_BACKEND_TYPE_INTERACTIVE_AUTHORITY))

typedef struct _PolkitBackendInteractiveAuthorityClass    PolkitBackendInteractiveAuthorityClass;

/**
 * PolkitBackendInteractiveAuthority:
 *
 * The #PolkitBackendInteractiveAuthority struct should not be accessed directly.
 */
struct _PolkitBackendInteractiveAuthority
{
  /*< private >*/
  PolkitBackendAuthority parent_instance;
};

/**
 * PolkitBackendInteractiveAuthorityClass:
 * @parent_class: The parent class.
 * @get_admin_identities: Returns list of identities for administrator authentication or %NULL to use the default
 *   implementation. See polkit_backend_interactive_authority_get_admin_identities() for details.
 * @check_authorization_sync: Checks for an authorization or %NULL to use the default implementation.
 *  See polkit_backend_interactive_authority_check_authorization_sync() for details.
 *
 * Class structure for #PolkitBackendInteractiveAuthority.
 */
struct _PolkitBackendInteractiveAuthorityClass
{
  /*< public >*/
  PolkitBackendAuthorityClass parent_class;

  /* VTable */
  GList *                     (*get_admin_identities)          (PolkitBackendInteractiveAuthority *authority,
                                                                PolkitSubject                     *caller,
                                                                PolkitSubject                     *subject,
                                                                PolkitIdentity                    *user_for_subject,
                                                                const gchar                       *action_id,
                                                                PolkitDetails                     *details);

  PolkitImplicitAuthorization (*check_authorization_sync) (PolkitBackendInteractiveAuthority *authority,
                                                           PolkitSubject                     *caller,
                                                           PolkitSubject                     *subject,
                                                           PolkitIdentity                    *user_for_subject,
                                                           gboolean                           subject_is_local,
                                                           gboolean                           subject_is_active,
                                                           const gchar                       *action_id,
                                                           PolkitDetails                     *details,
                                                           PolkitImplicitAuthorization        implicit,
                                                           PolkitDetails                     *out_details);

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
  void (*_polkit_reserved9) (void);
  void (*_polkit_reserved10) (void);
  void (*_polkit_reserved11) (void);
  void (*_polkit_reserved12) (void);
  void (*_polkit_reserved13) (void);
  void (*_polkit_reserved14) (void);
  void (*_polkit_reserved15) (void);
  void (*_polkit_reserved16) (void);
  void (*_polkit_reserved17) (void);
  void (*_polkit_reserved18) (void);
  void (*_polkit_reserved19) (void);
  void (*_polkit_reserved20) (void);
  void (*_polkit_reserved21) (void);
  void (*_polkit_reserved22) (void);
  void (*_polkit_reserved23) (void);
  void (*_polkit_reserved24) (void);
  void (*_polkit_reserved25) (void);
  void (*_polkit_reserved26) (void);
  void (*_polkit_reserved27) (void);
  void (*_polkit_reserved28) (void);
  void (*_polkit_reserved29) (void);
  void (*_polkit_reserved30) (void);
  void (*_polkit_reserved31) (void);
  void (*_polkit_reserved32) (void);
};

GType   polkit_backend_interactive_authority_get_type            (void) G_GNUC_CONST;
GList  *polkit_backend_interactive_authority_get_admin_identities (PolkitBackendInteractiveAuthority *authority,
                                                                   PolkitSubject                     *caller,
                                                                   PolkitSubject                     *subject,
                                                                   PolkitIdentity                    *user_for_subject,
                                                                   const gchar                       *action_id,
                                                                   PolkitDetails                     *details);

PolkitImplicitAuthorization polkit_backend_interactive_authority_check_authorization_sync (
                                                          PolkitBackendInteractiveAuthority *authority,
                                                          PolkitSubject                     *caller,
                                                          PolkitSubject                     *subject,
                                                          PolkitIdentity                    *user_for_subject,
                                                          gboolean                           subject_is_local,
                                                          gboolean                           subject_is_active,
                                                          const gchar                       *action_id,
                                                          PolkitDetails                     *details,
                                                          PolkitImplicitAuthorization        implicit,
                                                          PolkitDetails                     *out_details);

G_END_DECLS

#endif /* __POLKIT_BACKEND_INTERACTIVE_AUTHORITY_H */

