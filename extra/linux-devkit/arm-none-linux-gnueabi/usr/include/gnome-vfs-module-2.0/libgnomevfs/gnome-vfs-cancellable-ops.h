/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gnome-vfs-private-ops.h - Private synchronous operations for the GNOME
   Virtual File System.

   Copyright (C) 1999 Free Software Foundation

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

   Author: Ettore Perazzoli <ettore@gnu.org> */

#ifndef GNOME_VFS_CANCELLABLE_OPS_H
#define GNOME_VFS_CANCELLABLE_OPS_H

#include <libgnomevfs/gnome-vfs-directory.h>
#include <libgnomevfs/gnome-vfs-find-directory.h>
#include <libgnomevfs/gnome-vfs-handle.h>
#include <libgnomevfs/gnome-vfs-xfer.h>

G_BEGIN_DECLS

GnomeVFSResult gnome_vfs_open_uri_cancellable
					(GnomeVFSHandle **handle,
					 GnomeVFSURI *uri,
					 GnomeVFSOpenMode open_mode,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_create_uri_cancellable
					(GnomeVFSHandle **handle,
					 GnomeVFSURI *uri,
					 GnomeVFSOpenMode open_mode,
					 gboolean exclusive,
					 guint perm,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_close_cancellable
					(GnomeVFSHandle *handle,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_read_cancellable
					(GnomeVFSHandle *handle,
					 gpointer buffer,
					 GnomeVFSFileSize bytes,
					 GnomeVFSFileSize *bytes_written,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_write_cancellable
					(GnomeVFSHandle *handle,
					 gconstpointer buffer,
					 GnomeVFSFileSize bytes,
					 GnomeVFSFileSize *bytes_written,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_seek_cancellable
					(GnomeVFSHandle *handle,
					 GnomeVFSSeekPosition whence,
					 GnomeVFSFileOffset offset,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_get_file_info_uri_cancellable
					(GnomeVFSURI *uri,
					 GnomeVFSFileInfo *info,
					 GnomeVFSFileInfoOptions options,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_get_file_info_from_handle_cancellable
					(GnomeVFSHandle *handle,
					 GnomeVFSFileInfo *info,
					 GnomeVFSFileInfoOptions options,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_truncate_uri_cancellable (GnomeVFSURI *uri,
						   GnomeVFSFileSize length,
						   GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_truncate_handle_cancellable (GnomeVFSHandle *handle,
						      GnomeVFSFileSize length,
						      GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_make_directory_for_uri_cancellable
					(GnomeVFSURI *uri,
					 guint perm,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_find_directory_cancellable (GnomeVFSURI *near_uri,
						     GnomeVFSFindDirectoryKind kind,
						     GnomeVFSURI **result_uri,
						     gboolean create_if_needed,
		  				     gboolean find_if_needed,
						     guint permissions,
						     GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_remove_directory_from_uri_cancellable
					(GnomeVFSURI *uri,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_unlink_from_uri_cancellable
					(GnomeVFSURI *uri,
					 GnomeVFSContext *context);
GnomeVFSResult gnome_vfs_create_symbolic_link_cancellable
                                        (GnomeVFSURI *uri,
					 const gchar *target_reference,
					 GnomeVFSContext *context);
GnomeVFSResult gnome_vfs_move_uri_cancellable
					(GnomeVFSURI *old,
					 GnomeVFSURI *new,
					 gboolean force_replace,
					 GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_check_same_fs_uris_cancellable
					 (GnomeVFSURI *a,
					  GnomeVFSURI *b,
					  gboolean *same_fs_return,
					  GnomeVFSContext *context);

GnomeVFSResult gnome_vfs_set_file_info_cancellable
					 (GnomeVFSURI *a,
					  const GnomeVFSFileInfo *info,
					  GnomeVFSSetFileInfoMask mask,
					  GnomeVFSContext *context);

GnomeVFSResult _gnome_vfs_xfer_private   (const GList *source_uri_list,
					 const GList *target_uri_list,
					 GnomeVFSXferOptions xfer_options,
					 GnomeVFSXferErrorMode error_mode,
					 GnomeVFSXferOverwriteMode overwrite_mode,
					 GnomeVFSXferProgressCallback progress_callback,
					 gpointer data,
					 GnomeVFSXferProgressCallback sync_progress_callback,
					 gpointer sync_progress_data);

GnomeVFSResult	gnome_vfs_directory_read_next_cancellable
					(GnomeVFSDirectoryHandle *handle,
					 GnomeVFSFileInfo *info,
					 GnomeVFSContext *context);

GnomeVFSResult  gnome_vfs_directory_open_from_uri_cancellable
					(GnomeVFSDirectoryHandle **handle,
					 GnomeVFSURI *uri,
					 GnomeVFSFileInfoOptions options,
					 GnomeVFSContext *context);

GnomeVFSResult  gnome_vfs_file_control_cancellable
                                        (GnomeVFSHandle *handle,
					 const char *operation,
					 gpointer operation_data,
					 GnomeVFSContext *context);

G_END_DECLS

#endif /* GNOME_VFS_CANCELLABLE_OPS_H */
