/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gnome-vfs-socket-buffer.h
 *
 * Copyright (C) 2001 Seth Nickell
 * Copyright (C) 2001 Maciej Stachowiak
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Gnome Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA. 
 *
 */
/*
 * Authors: Seth Nickell <snickell@stanford.edu>
 *          Maciej Stachowiak <mjs@noisehavoc.org>
 *          (reverse-engineered from code by Ian McKellar <yakk@yakk.net>)
 */

#ifndef GNOME_VFS_SOCKET_BUFFER_H
#define GNOME_VFS_SOCKET_BUFFER_H

#include "gnome-vfs-socket.h"

G_BEGIN_DECLS

/**
 * GnomeVFSSocketBuffer:
 *
 * A handle to a socket buffer. A socket buffer is a temporary in-memory storage for data
 * that is read from or written to a #GnomeVFSSocket.
 **/
typedef struct GnomeVFSSocketBuffer GnomeVFSSocketBuffer;


GnomeVFSSocketBuffer* gnome_vfs_socket_buffer_new      (GnomeVFSSocket       *socket);
GnomeVFSResult        gnome_vfs_socket_buffer_destroy  (GnomeVFSSocketBuffer *socket_buffer,
							gboolean              close_socket,
							GnomeVFSCancellation *cancellation);
GnomeVFSResult        gnome_vfs_socket_buffer_read     (GnomeVFSSocketBuffer *socket_buffer, 
							gpointer              buffer,
							GnomeVFSFileSize      bytes, 
							GnomeVFSFileSize     *bytes_read,
							GnomeVFSCancellation *cancellation);
GnomeVFSResult        gnome_vfs_socket_buffer_read_until (GnomeVFSSocketBuffer *socket_buffer,
							gpointer buffer,
							GnomeVFSFileSize bytes,
							gconstpointer boundary,
							GnomeVFSFileSize boundary_len,
							GnomeVFSFileSize *bytes_read,
							gboolean *got_boundary,
							GnomeVFSCancellation *cancellation);
GnomeVFSResult        gnome_vfs_socket_buffer_peekc    (GnomeVFSSocketBuffer *socket_buffer,
							char                 *character,
							GnomeVFSCancellation *cancellation);
GnomeVFSResult        gnome_vfs_socket_buffer_write    (GnomeVFSSocketBuffer *socket_buffer,
							gconstpointer         buffer,
							GnomeVFSFileSize      bytes,
							GnomeVFSFileSize     *bytes_written,
							GnomeVFSCancellation *cancellation);
GnomeVFSResult        gnome_vfs_socket_buffer_flush    (GnomeVFSSocketBuffer *socket_buffer,
							GnomeVFSCancellation *cancellation);

G_END_DECLS

#endif
