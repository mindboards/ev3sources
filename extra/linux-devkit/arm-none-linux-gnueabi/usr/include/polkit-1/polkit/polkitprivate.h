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

#ifndef __POLKIT_PRIVATE_H
#define __POLKIT_PRIVATE_H

#include "polkitimplicitauthorization.h"
#include "polkitactiondescription.h"
#include "polkitsubject.h"
#include "polkitauthorizationresult.h"
#include "polkittemporaryauthorization.h"
#include "_polkitbindings.h"

/* notes:
 *
 * - the _new_for_real() functions will ref the passed arg (you will still own the ref)
 * - the _get_real() functions will return a ref (you will own the ref)
 *
 */

PolkitActionDescription  *polkit_action_description_new_for_real (_PolkitActionDescription *real);
_PolkitActionDescription *polkit_action_description_get_real     (PolkitActionDescription  *action_description);

PolkitSubject  *polkit_subject_new_for_real (_PolkitSubject *real);
_PolkitSubject *polkit_subject_get_real     (PolkitSubject  *subject);

PolkitIdentity  *polkit_identity_new_for_real (_PolkitIdentity *real);
_PolkitIdentity *polkit_identity_get_real     (PolkitIdentity  *identity);

PolkitAuthorizationResult  *polkit_authorization_result_new_for_real (_PolkitAuthorizationResult *real);
_PolkitAuthorizationResult *polkit_authorization_result_get_real (PolkitAuthorizationResult *authorization_result);

_PolkitTemporaryAuthorization *polkit_temporary_authorization_get_real (PolkitTemporaryAuthorization *authorization);
PolkitTemporaryAuthorization *polkit_temporary_authorization_new_for_real (_PolkitTemporaryAuthorization *real);

PolkitDetails *polkit_details_new_for_hash (GHashTable *hash);
GHashTable *polkit_details_get_hash (PolkitDetails *details);

#endif /* __POLKIT_PRIVATE_H */
