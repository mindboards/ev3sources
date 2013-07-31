
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gnome-vfs-callbacks.h - various callback declarations

   Copyright (C) 2001 Free Software Foundation

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

   Authors: Maciej Stachowiak <mjs@noisehavoc.org>
            Seth Nickell <snickell@stanford.edu>
*/

#ifndef GNOME_VFS_MODULE_CALLBACK_MODULE_API_H
#define GNOME_VFS_MODULE_CALLBACK_MODULE_API_H

#include <glib.h>

gboolean gnome_vfs_module_callback_invoke (const char    *callback_name,
					   gconstpointer  in,
					   gsize          in_size,
					   gpointer       out,
					   gsize          out_size);

#endif
