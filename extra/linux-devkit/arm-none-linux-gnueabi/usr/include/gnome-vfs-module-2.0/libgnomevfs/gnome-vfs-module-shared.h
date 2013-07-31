/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gnome-vfs-module-shared.h - code shared between the different modules
   place.

   Copyright (C) 2001 Free Software Foundation

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

   Author: Seth Nickell <snickell@stanford.edu
 */

#ifndef GNOME_VFS_MODULE_SHARED_H
#define GNOME_VFS_MODULE_SHARED_H

/* This check is for Linux, but should be harmless on other platforms. */
#if !defined (_LARGEFILE64_SOURCE) || _FILE_OFFSET_BITS+0 != 64
#error configuration macros set inconsistently, gnome_vfs_stat_to_file_info will malfunction
#endif

#include <libgnomevfs/gnome-vfs-file-info.h>
#include <libgnomevfs/gnome-vfs-monitor.h>
#include <libgnomevfs/gnome-vfs-method.h>

G_BEGIN_DECLS

const char *    gnome_vfs_mime_type_from_mode            (mode_t             mode);
const char *    gnome_vfs_mime_type_from_mode_or_default (mode_t             mode,
							  const char        *defaultv);

void            gnome_vfs_stat_to_file_info	(GnomeVFSFileInfo  *file_info,
					    	 const struct stat *statptr);

GnomeVFSResult  gnome_vfs_set_meta          	(GnomeVFSFileInfo  *info,
					    	 const char        *file_name,
					    	 const char        *meta_key);

GnomeVFSResult  gnome_vfs_set_meta_for_list 	(GnomeVFSFileInfo  *info,
					    	 const char        *file_name,
					    	 const GList       *meta_keys);
	
const char     *gnome_vfs_get_special_mime_type (GnomeVFSURI       *uri);

void            gnome_vfs_monitor_callback (GnomeVFSMethodHandle *method_handle,
					    GnomeVFSURI *info_uri,
					    GnomeVFSMonitorEventType event_type);


G_END_DECLS

#endif
