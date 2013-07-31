/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gnome-vfs-method.h - All the VFS bits a module needs to include in one
   place.

   Copyright (C) 1999 Free Software Foundation

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Michael Meeks <michael@imaginator.com> */

#ifndef GNOME_VFS_MODULE_H
#define GNOME_VFS_MODULE_H

#include <libgnomevfs/gnome-vfs-method.h>

G_BEGIN_DECLS


/**
 * vfs_module_init:
 * @method_name: name of the method that invoked this module (e.g. "http", "ftp", "file").
 * @args: not used by most modules, but potential arguments for creating the module (could
 * be a file to point at, for example)
 *
 * Standard extern call implemented by each filesystem module. This is called
 * to initialize the module and setup any basic structures / connections the
 * method requires. It also allows the module to identify the URI method it is
 * associated with in this instance.
 *
 * Return value: the module symbol table, pointing to the appropriate calls for
 * this module.
 **/
extern GnomeVFSMethod    *vfs_module_init	(const char *method_name, const char *args);

/**
 * vfs_module_transform:
 * @method_name: name of the method that invoked this module (e.g. "http", "ftp", "file").
 * @args: not used by most modules, but potential arguments for creating the module (could
 * be a file to point at, for example)
 *
 * Shift an already instanced module to a new method name. This call is not implemented
 * by most modules and is optional.
 *
 * Return value: the module symbol table, pointing to the appropriate calls for
 * this module.
 **/
extern GnomeVFSTransform *vfs_module_transform	(const char *method_name, const char *args);

/**
 * vfs_module_shutdown:
 * @method: the symbol table of the module being shut down
 *
 * Called to tell a module to end any active operations, free all used memory,
 * and close any connections (as appropriate) or resources.
 *
 **/
extern void               vfs_module_shutdown	(GnomeVFSMethod *method);

G_END_DECLS

#endif /* GNOME_VFS_MODULE_H */
