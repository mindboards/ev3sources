/*
 * orbit-imodule.h:
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
#ifndef __ORBIT_IMODULE_H__
#define __ORBIT_IMODULE_H__

#include <glib.h>
#include <orbit/orb-core/orbit-interface.h>
#include <libIDL/IDL.h>

G_BEGIN_DECLS

ORBit_IInterfaces *ORBit_iinterfaces_from_file (const char                     *path,
						const char                     *cpp_args,
						CORBA_sequence_CORBA_TypeCode **typecodes_ret);

ORBit_IInterfaces *ORBit_iinterfaces_from_tree (IDL_tree                        tree,
						CORBA_sequence_CORBA_TypeCode **typecodes_ret);

G_END_DECLS

#endif /* __ORBIT_IMODULE_H__ */
