/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gnome-vfs-transform.h - transform function declarations

   Copyright (C) 1999, 2001 Free Software Foundation

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Ettore Perazzoli <ettore@gnu.org>
           Seth Nickell <snickell@stanford.edu>
*/

#ifndef GNOME_VFS_TRANSFORM_H
#define GNOME_VFS_TRANSFORM_H

#include <libgnomevfs/gnome-vfs-result.h>
#include <libgnomevfs/gnome-vfs-context.h>

G_BEGIN_DECLS

typedef struct GnomeVFSTransform GnomeVFSTransform;

typedef GnomeVFSTransform * (* GnomeVFSTransformInitFunc) (const char        *method_name,
							   const char        *config_args);

typedef GnomeVFSResult (* GnomeVFSTransformFunc)          (GnomeVFSTransform *transform,
							   const char        *old_uri,
							   char             **new_uri,
							   GnomeVFSContext   *context);

struct GnomeVFSTransform {
	GnomeVFSTransformFunc transform;
};

G_END_DECLS

#endif
