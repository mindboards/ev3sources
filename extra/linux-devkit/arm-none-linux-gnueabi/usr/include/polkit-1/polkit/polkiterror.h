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

#ifndef __POLKIT_ERROR_H
#define __POLKIT_ERROR_H

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * POLKIT_ERROR:
 *
 * Error domain for errors when using PolicyKit. Errors in this domain will be from the #PolkitError
 * enumeration. See #GError for information on error domains
 */
#define POLKIT_ERROR polkit_error_quark()

GQuark polkit_error_quark (void);

GType polkit_error_get_type (void) G_GNUC_CONST;

#define POLKIT_TYPE_ERROR (polkit_error_get_type ())

/**
 * PolkitError:
 * @POLKIT_ERROR_FAILED: The operation failed.
 * @POLKIT_ERROR_CANCELLED: The operation was cancelled.
 * @POLKIT_ERROR_NOT_SUPPORTED: Operation is not supported.
 * @POLKIT_ERROR_NOT_AUTHORIZED: Not authorized to perform operation.
 *
 * Possible error when using PolicyKit.
 */
typedef enum
{
  POLKIT_ERROR_FAILED = 0,
  POLKIT_ERROR_CANCELLED = 1,
  POLKIT_ERROR_NOT_SUPPORTED = 2,
  POLKIT_ERROR_NOT_AUTHORIZED = 3,
} PolkitError;

G_END_DECLS

#endif /* __POLKIT_ERROR_H */
