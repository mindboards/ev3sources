/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gnome-vfs-inet-connection.h - Functions for creating and destroying Internet
   connections.

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

#ifndef GNOME_VFS_INET_CONNECTION_H
#define GNOME_VFS_INET_CONNECTION_H

#include <libgnomevfs/gnome-vfs-cancellation.h>
#include <libgnomevfs/gnome-vfs-socket.h>
#include <libgnomevfs/gnome-vfs-socket-buffer.h>
#include <libgnomevfs/gnome-vfs-address.h>

G_BEGIN_DECLS

typedef struct GnomeVFSInetConnection GnomeVFSInetConnection;

GnomeVFSResult	 gnome_vfs_inet_connection_create
					(GnomeVFSInetConnection **connection_return,
					 const gchar *host_name,
					 guint host_port,
					 GnomeVFSCancellation *cancellation);

GnomeVFSResult   gnome_vfs_inet_connection_create_from_address
                                        (GnomeVFSInetConnection **connection_return,
					 GnomeVFSAddress         *address,
					 guint                    host_port,
					 GnomeVFSCancellation    *cancellation);

/* free the connection structure and close the socket */
void		 gnome_vfs_inet_connection_destroy
					(GnomeVFSInetConnection *connection,
					 GnomeVFSCancellation *cancellation);

/* free the connection structure without closing the socket */
void		 gnome_vfs_inet_connection_free
					(GnomeVFSInetConnection *connection,
					 GnomeVFSCancellation *cancellation);

GnomeVFSSocket * gnome_vfs_inet_connection_to_socket 
					(GnomeVFSInetConnection *connection);

GnomeVFSSocketBuffer *gnome_vfs_inet_connection_to_socket_buffer 
                                        (GnomeVFSInetConnection *connection);

int gnome_vfs_inet_connection_get_fd    (GnomeVFSInetConnection *connection);

char *gnome_vfs_inet_connection_get_ip  (GnomeVFSInetConnection *connection);

GnomeVFSAddress *gnome_vfs_inet_connection_get_address
                                        (GnomeVFSInetConnection *connection);

G_END_DECLS

#endif /* GNOME_VFS_INET_CONNECTION_H */
