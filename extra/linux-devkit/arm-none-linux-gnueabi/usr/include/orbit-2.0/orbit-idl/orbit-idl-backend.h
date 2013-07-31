/*
 * orbit-idl-backend.h:
 *
 * Copyright (C) 2002 Sun Microsystems, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors:
 *	Mark McLoughlin <mark@skynet.ie>
 */

#ifndef __ORBIT_IDL_BACKEND_H__
#define __ORBIT_IDL_BACKEND_H__

#include <glib.h>
#include <libIDL/IDL.h>

G_BEGIN_DECLS

typedef struct {
	IDL_tree  tree;
	char     *filename;

	guint     do_stubs  : 1;
	guint     do_skels  : 1;
	guint     do_common : 1;
} ORBitIDLBackendContext;

/* Define a function with this signature and named "orbit_idl_backend_func"
 * in the module.
 *
 * The module should be named libORBit-idl-backend-$(language).so. $(language)
 * is defined with the --lang idl-compiler command line option.
 *
 * Modules are searched for in the following order:
 *   1. The directory specified by the --backenddir option.
 *   2. %(prefix)/lib/orbit-2.0/idl-backends/, where $(prefix) is the prefix
 *      ORBit2 was installed in.
 *   3. For each $(path) in the $ORBIT_BACKENDS_PATH environment variable,
 *      the module is searched for in $(path)/lib/orbit-2.0/idl-backends/
 *   4. For each $(path) in the $GNOME2_PATH environment variable,
 *      the module is searched for in $(path)/lib/orbit-2.0/idl-backends/
 */

typedef gboolean (*ORBitIDLBackendFunc) (ORBitIDLBackendContext *context);

G_END_DECLS

#endif
