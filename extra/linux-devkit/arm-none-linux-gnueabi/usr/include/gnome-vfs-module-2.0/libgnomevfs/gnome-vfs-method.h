/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gnome-vfs-method.h - Virtual class for access methods in the GNOME
   Virtual File System.

   Copyright (C) 1999, 2001 Free Software Foundation

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

   Author: Ettore Perazzoli <ettore@comm2000.it> 
           Seth Nickell <snickell@stanford.edu>
*/

#ifndef GNOME_VFS_METHOD_H
#define GNOME_VFS_METHOD_H

/*
 * The following include helps Solaris copy with its own headers.  (With 64-
 * bit stuff enabled they like to #define open open64, etc.)
 * See http://bugzilla.gnome.org/show_bug.cgi?id=71184 for details.
 */
#ifndef _WIN32
#include <unistd.h>
#endif

#include <libgnomevfs/gnome-vfs-context.h>
#include <libgnomevfs/gnome-vfs-file-info.h>
#include <libgnomevfs/gnome-vfs-find-directory.h>
#include <libgnomevfs/gnome-vfs-handle.h>
#include <libgnomevfs/gnome-vfs-transform.h>
#include <libgnomevfs/gnome-vfs-monitor.h>

G_BEGIN_DECLS

#define _GNOME_VFS_METHOD_PARAM_CHECK(expression)			\
	g_return_val_if_fail ((expression), GNOME_VFS_ERROR_BAD_PARAMETERS);

typedef struct GnomeVFSMethod GnomeVFSMethod;

typedef GnomeVFSMethod * (* GnomeVFSMethodInitFunc)(const char *method_name, const char *config_args);
typedef void (*GnomeVFSMethodShutdownFunc)(GnomeVFSMethod *method);

typedef GnomeVFSResult (* GnomeVFSMethodOpenFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle
			       	 	**method_handle_return,
					 GnomeVFSURI *uri,
					 GnomeVFSOpenMode mode,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodCreateFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle
			       	 	**method_handle_return,
					 GnomeVFSURI *uri,
					 GnomeVFSOpenMode mode,
					 gboolean exclusive,
					 guint perm,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodCloseFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle *method_handle,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodReadFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle *method_handle,
					 gpointer buffer,
					 GnomeVFSFileSize num_bytes,
					 GnomeVFSFileSize *bytes_read_return,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodWriteFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle *method_handle,
					 gconstpointer buffer,
					 GnomeVFSFileSize num_bytes,
					 GnomeVFSFileSize *bytes_written_return,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodSeekFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle *method_handle,
					 GnomeVFSSeekPosition  whence,
					 GnomeVFSFileOffset    offset,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodTellFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle *method_handle,
					 GnomeVFSFileSize *offset_return);

typedef GnomeVFSResult (* GnomeVFSMethodOpenDirectoryFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle **method_handle,
					 GnomeVFSURI *uri,
					 GnomeVFSFileInfoOptions options,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodCloseDirectoryFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle *method_handle,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodReadDirectoryFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle *method_handle,
					 GnomeVFSFileInfo *file_info,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodGetFileInfoFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSURI *uri,
					 GnomeVFSFileInfo *file_info,
					 GnomeVFSFileInfoOptions options,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodGetFileInfoFromHandleFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSMethodHandle *method_handle,
					 GnomeVFSFileInfo *file_info,
					 GnomeVFSFileInfoOptions options,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodTruncateFunc) (GnomeVFSMethod *method,
						       GnomeVFSURI *uri,
						       GnomeVFSFileSize length,
						       GnomeVFSContext *context);
typedef GnomeVFSResult (* GnomeVFSMethodTruncateHandleFunc) (GnomeVFSMethod *method,
							     GnomeVFSMethodHandle *handle,
							     GnomeVFSFileSize length,
							     GnomeVFSContext *context);

typedef gboolean       (* GnomeVFSMethodIsLocalFunc)
					(GnomeVFSMethod *method,
					 const GnomeVFSURI *uri);

typedef GnomeVFSResult (* GnomeVFSMethodMakeDirectoryFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSURI *uri,
					 guint perm,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodFindDirectoryFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSURI *find_near_uri,
					 GnomeVFSFindDirectoryKind kind,
					 GnomeVFSURI **result_uri,
					 gboolean create_if_needed,
					 gboolean find_if_needed,
					 guint perm,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodRemoveDirectoryFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSURI *uri,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodMoveFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSURI *old_uri,
					 GnomeVFSURI *new_uri,
					 gboolean force_replace,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodUnlinkFunc)
                                        (GnomeVFSMethod *method,
					 GnomeVFSURI *uri,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodCheckSameFSFunc)
					(GnomeVFSMethod *method,
					 GnomeVFSURI *a,
					 GnomeVFSURI *b,
					 gboolean *same_fs_return,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodSetFileInfo)
					(GnomeVFSMethod *method,
					 GnomeVFSURI *a,
					 const GnomeVFSFileInfo *info,
					 GnomeVFSSetFileInfoMask mask,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodCreateSymbolicLinkFunc)
                                        (GnomeVFSMethod *method,
                                         GnomeVFSURI *uri,
                                         const gchar *target_reference,
                                         GnomeVFSContext *context);
typedef GnomeVFSResult (* GnomeVFSMethodMonitorAddFunc)
     					(GnomeVFSMethod *method,
      					 GnomeVFSMethodHandle **method_handle_return,
      					 GnomeVFSURI *uri,
      					 GnomeVFSMonitorType monitor_type);

typedef GnomeVFSResult (* GnomeVFSMethodMonitorCancelFunc)
     					(GnomeVFSMethod *method,
      					 GnomeVFSMethodHandle *handle);

typedef GnomeVFSResult (* GnomeVFSMethodFileControlFunc)
     					(GnomeVFSMethod *method,
      					 GnomeVFSMethodHandle *method_handle,
					 const char *operation,
					 gpointer operation_data,
					 GnomeVFSContext *context);

typedef GnomeVFSResult (* GnomeVFSMethodForgetCacheFunc)
     					(GnomeVFSMethod *method,
      					 GnomeVFSMethodHandle *method_handle,
					 GnomeVFSFileOffset offset,
					 GnomeVFSFileSize size);

typedef GnomeVFSResult (* GnomeVFSMethodGetVolumeFreeSpaceFunc)
     					(GnomeVFSMethod *method,
					 const GnomeVFSURI *uri,
				 	 GnomeVFSFileSize *free_space);


/* Use this macro to test whether a given function is implemented in
 * a given GnomeVFSMethod.  Note that it checks the expected size of the structure
 * prior to testing NULL.
 */
 
#define VFS_METHOD_HAS_FUNC(method,func) ((((char *)&((method)->func)) - ((char *)(method)) < (method)->method_table_size) && method->func != NULL)

/* Structure defining an access method.	 This is also defined as an
   opaque type in `gnome-vfs-types.h'.	*/
struct GnomeVFSMethod {
	gsize method_table_size;			/* Used for versioning */
	GnomeVFSMethodOpenFunc open;
	GnomeVFSMethodCreateFunc create;
	GnomeVFSMethodCloseFunc close;
	GnomeVFSMethodReadFunc read;
	GnomeVFSMethodWriteFunc write;
	GnomeVFSMethodSeekFunc seek;
	GnomeVFSMethodTellFunc tell;
	GnomeVFSMethodTruncateHandleFunc truncate_handle;
	GnomeVFSMethodOpenDirectoryFunc open_directory;
	GnomeVFSMethodCloseDirectoryFunc close_directory;
	GnomeVFSMethodReadDirectoryFunc read_directory;
	GnomeVFSMethodGetFileInfoFunc get_file_info;
	GnomeVFSMethodGetFileInfoFromHandleFunc get_file_info_from_handle;
	GnomeVFSMethodIsLocalFunc is_local;
	GnomeVFSMethodMakeDirectoryFunc make_directory;
	GnomeVFSMethodRemoveDirectoryFunc remove_directory;
	GnomeVFSMethodMoveFunc move;
	GnomeVFSMethodUnlinkFunc unlink;
	GnomeVFSMethodCheckSameFSFunc check_same_fs;
	GnomeVFSMethodSetFileInfo set_file_info;
	GnomeVFSMethodTruncateFunc truncate;
	GnomeVFSMethodFindDirectoryFunc find_directory;
	GnomeVFSMethodCreateSymbolicLinkFunc create_symbolic_link;
	GnomeVFSMethodMonitorAddFunc monitor_add;
	GnomeVFSMethodMonitorCancelFunc monitor_cancel;
	GnomeVFSMethodFileControlFunc file_control;
	GnomeVFSMethodForgetCacheFunc forget_cache;
	GnomeVFSMethodGetVolumeFreeSpaceFunc get_volume_free_space;
};

gboolean	   gnome_vfs_method_init   (void);
GnomeVFSMethod    *gnome_vfs_method_get    (const gchar *name);
GnomeVFSTransform *gnome_vfs_transform_get (const gchar *name);

void              _gnome_vfs_method_shutdown (void);

G_END_DECLS

#endif /* GNOME_VFS_METHOD_H */
