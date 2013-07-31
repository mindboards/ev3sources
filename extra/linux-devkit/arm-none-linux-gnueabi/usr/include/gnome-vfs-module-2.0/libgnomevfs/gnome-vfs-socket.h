/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gnome-vfs-socket.h
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

#ifndef GNOME_VFS_SOCKET_H
#define GNOME_VFS_SOCKET_H

#include <glib.h>
#include <libgnomevfs/gnome-vfs-cancellation.h>
#include <libgnomevfs/gnome-vfs-file-size.h>
#include <libgnomevfs/gnome-vfs-result.h>

G_BEGIN_DECLS

/**
 * GnomeVFSSocket:
 *
 * An handle to a generic unbuffered socket connection established with
 * gnome_vfs_socket_new().
 *
 * The specifics of the underlying socket implementation are hidden
 * inside the #GnomeVFSSocketImpl passed on construction.
 *
 * If you need buffered I/O, you will also have to create a
 * #GnomeVFSSocketBuffer.
 **/
typedef struct GnomeVFSSocket GnomeVFSSocket;

/**
 * GnomeVFSSocketReadFunc:
 * @connection: The socket connection.
 * @buffer: A connection buffer.
 * @bytes: The bytes to read.
 * @bytes_read_out: The bytes that were read (out).
 * @cancellation: A cancellation handle that allows clients to cancel the read operation.
 *
 * This is a generic prototype for a function that reads from a socket.
 *
 * This function is implemented by a #GnomeVFSSocketImpl, and it defines how data
 * should be written to a buffer using the gnome_vfs_socket_read()
 * function which hides the socket implementation details.
 *
 * Returns: A #GnomeVFSResult signalling the result of the read operation.
 **/
typedef GnomeVFSResult (*GnomeVFSSocketReadFunc)  (gpointer connection,
						   gpointer buffer, 
						   GnomeVFSFileSize bytes, 
						   GnomeVFSFileSize *bytes_read_out,
						   GnomeVFSCancellation *cancellation);

/**
 * GnomeVFSSocketWriteFunc:
 * @connection: The socket connection.
 * @buffer: A connection buffer.
 * @bytes: The bytes to write.
 * @bytes_written_out: The bytes that were written.
 * @cancellation: A cancellation handle that allows clients to cancel the write operation.
 *
 * This is a generic prototype for a function that writes to a socket.
 *
 * This function is implemented by a #GnomeVFSSocketImpl, and it defines how data
 * should be written to a buffer using the gnome_vfs_socket_write()
 * function which hides the socket implementation details.
 *
 * Returns: A #GnomeVFSResult signalling the result of the write operation.
 **/
typedef GnomeVFSResult (*GnomeVFSSocketWriteFunc) (gpointer connection, 
						   gconstpointer buffer,
						   GnomeVFSFileSize bytes,
						   GnomeVFSFileSize *bytes_written_out,
						   GnomeVFSCancellation *cancellation);
/**
 * GnomeVFSSocketCloseFunc:
 * @cancellation: A cancellation handle that allows clients to cancel the write operation.
 *
 * This is a generic prototype for a function that closes a socket.
 *
 * This function is implemented by a #GnomeVFSSocketImpl, and it defines how an
 * open socket that was previously opened by gnome_vfs_socket_new()
 * should be closed using the gnome_vfs_socket_set_timeout() function which
 * hides the socket implementation details.
 **/
typedef void           (*GnomeVFSSocketCloseFunc) (gpointer connection,
						   GnomeVFSCancellation *cancellation);

/**
 * GnomeVFSSocketSetTimeoutFunc:
 * @cancellation: A cancellation handle that allows clients to cancel the write operation.
 *
 * This is a generic prototype for a function that sets a socket timeout.
 *
 * This function is implemented by a #GnomeVFSSocketImpl, and it defines how
 * a socket timeout should be set using 
 * should be closed by the gnome_vfs_socket_close() function which
 * hides the socket implementation details.
 *
 * Returns: A #GnomeVFSResult signalling the result of the write operation.
 **/
typedef GnomeVFSResult (*GnomeVFSSocketSetTimeoutFunc) (gpointer connection,
							GTimeVal *timeout,
							GnomeVFSCancellation *cancellation);

/**
 * GnomeVFSSocketImpl:
 * @read: A #GnomeVFSSocketReadFunc function used for reading from a socket.
 * @write: A #GnomeVFSSocketWriteFunc function used for writing to a socket.
 * @close: A #GnomeVFSSocketCloseFunc function used for closing an open socket.
 * @set_timeout: A #GnomeVFSSocketSetTimeoutFunc function used for setting a socket's timeout.
 *
 * An implementation of a generic socket (i.e. of #GnomeVFSSocket)
 * encapsulating the details of how socket I/O works.
 *
 * Please refer to #GnomeVFSSSL for a sample implementation of this interface.
 **/
typedef struct {
  GnomeVFSSocketReadFunc read;
  GnomeVFSSocketWriteFunc write;
  GnomeVFSSocketCloseFunc close;
  GnomeVFSSocketSetTimeoutFunc set_timeout;
} GnomeVFSSocketImpl;


GnomeVFSSocket* gnome_vfs_socket_new     (GnomeVFSSocketImpl *impl, 
					  void               *connection);
GnomeVFSResult  gnome_vfs_socket_write   (GnomeVFSSocket     *socket, 
					  gconstpointer       buffer,
					  int                 bytes, 
					  GnomeVFSFileSize   *bytes_written,
					  GnomeVFSCancellation *cancellation);
GnomeVFSResult  gnome_vfs_socket_close   (GnomeVFSSocket     *socket,
					  GnomeVFSCancellation *cancellation);
GnomeVFSResult  gnome_vfs_socket_read    (GnomeVFSSocket     *socket, 
					  gpointer            buffer, 
					  GnomeVFSFileSize    bytes, 
					  GnomeVFSFileSize   *bytes_read,
					  GnomeVFSCancellation *cancellation);
GnomeVFSResult  gnome_vfs_socket_set_timeout
					 (GnomeVFSSocket *socket,
					  GTimeVal *timeout,
					  GnomeVFSCancellation *cancellation);
void            gnome_vfs_socket_free   (GnomeVFSSocket *socket);
G_END_DECLS

#endif /* GNOME_VFS_SOCKET_H */
