/*
 * Copyright (C) 2008 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: David Zeuthen <davidz@redhat.com>
 */

#if !defined (_EGG_DBUS_COMPILATION) && !defined(_EGG_DBUS_INSIDE_EGG_DBUS_H)
#error "Only <eggdbus/eggdbus.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __EGG_DBUS_ERROR_H
#define __EGG_DBUS_ERROR_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

/**
 * EGG_DBUS_ERROR:
 *
 * Error domain for EggDBus. Errors in this domain will be from the
 * #EggDBusError enumeration. See #GError for information on error
 * domains.
 */
#define EGG_DBUS_ERROR egg_dbus_error_quark()

GQuark egg_dbus_error_quark (void);

gboolean egg_dbus_error_get_remote_exception (GError *error,
                                              gchar **out_name,
                                              gchar **out_message);

/**
 * EggDBusError:
 * @EGG_DBUS_ERROR_FAILED: The operation failed.
 * @EGG_DBUS_ERROR_CANCELLED: The operation was cancelled.
 * @EGG_DBUS_ERROR_REMOTE_EXCEPTION: A remote exception that couldn't be
 * mapped to a proper #GError. Use egg_dbus_error_get_remote_exception()
 * to extract the D-Bus error name.
 * @EGG_DBUS_ERROR_DBUS_FAILED:
 * A generic error; "something went wrong" - see the error message for
 * more.
 * @EGG_DBUS_ERROR_NO_MEMORY:
 * There was not enough memory to complete an operation.
 * @EGG_DBUS_ERROR_SERVICE_UNKNOWN:
 * The bus doesn't know how to launch a service to supply the bus name
 * you wanted.
 * @EGG_DBUS_ERROR_NAME_HAS_NO_OWNER:
 * The bus name you referenced doesn't exist (i.e. no application owns
 * it).
 * @EGG_DBUS_ERROR_NO_REPLY:
 * No reply to a message expecting one, usually means a timeout occurred.
 * @EGG_DBUS_ERROR_IO_ERROR:
 * Something went wrong reading or writing to a socket, for example.
 * @EGG_DBUS_ERROR_BAD_ADDRESS:
 * A D-Bus bus address was malformed.
 * @EGG_DBUS_ERROR_NOT_SUPPORTED:
 * Requested operation isn't supported (like ENOSYS on UNIX).
 * @EGG_DBUS_ERROR_LIMITS_EXCEEDED:
 * Some limited resource is exhausted.
 * @EGG_DBUS_ERROR_ACCESS_DENIED:
 * Security restrictions don't allow doing what you're trying to do.
 * @EGG_DBUS_ERROR_AUTH_FAILED:
 * Authentication didn't work.
 * @EGG_DBUS_ERROR_NO_SERVER:
 * Unable to connect to server (probably caused by ECONNREFUSED on a
 * socket).
 * @EGG_DBUS_ERROR_TIMEOUT:
 * Certain timeout errors, possibly ETIMEDOUT on a socket.  Note that
 * #EGG_DBUS_ERROR_NO_REPLY is used for message reply timeouts. Warning:
 * this is confusingly-named given that #EGG_DBUS_ERROR_TIMED_OUT also
 * exists. We can't fix it for compatibility reasons so just be
 * careful.
 * @EGG_DBUS_ERROR_NO_NETWORK:
 * No network access (probably ENETUNREACH on a socket).
 * @EGG_DBUS_ERROR_ADDRESS_IN_USE:
 * Can't bind a socket since its address is in use (i.e. EADDRINUSE).
 * @EGG_DBUS_ERROR_DISCONNECTED:
 * The connection is disconnected and you're trying to use it.
 * @EGG_DBUS_ERROR_INVALID_ARGS:
 * Invalid arguments passed to a method call.
 * @EGG_DBUS_ERROR_FILE_NOT_FOUND:
 * Missing file.
 * @EGG_DBUS_ERROR_FILE_EXISTS:
 * Existing file and the operation you're using does not silently overwrite.
 * @EGG_DBUS_ERROR_UNKNOWN_METHOD:
 * Method name you invoked isn't known by the object you invoked it on.
 * @EGG_DBUS_ERROR_TIMED_OUT:
 * Certain timeout errors, e.g. while starting a service. Warning: this is
 * confusingly-named given that #EGG_DBUS_ERROR_TIMEOUT also exists. We
 * can't fix it for compatibility reasons so just be careful.
 * @EGG_DBUS_ERROR_MATCH_RULE_NOT_FOUND:
 * Tried to remove or modify a match rule that didn't exist.
 * @EGG_DBUS_ERROR_MATCH_RULE_INVALID:
 * The match rule isn't syntactically valid.
 * @EGG_DBUS_ERROR_SPAWN_EXEC_FAILED:
 * While starting a new process, the exec() call failed.
 * @EGG_DBUS_ERROR_SPAWN_FORK_FAILED:
 * While starting a new process, the fork() call failed.
 * @EGG_DBUS_ERROR_SPAWN_CHILD_EXITED:
 * While starting a new process, the child exited with a status code.
 * @EGG_DBUS_ERROR_SPAWN_CHILD_SIGNALED:
 * While starting a new process, the child exited on a signal.
 * @EGG_DBUS_ERROR_SPAWN_FAILED:
 * While starting a new process, something went wrong.
 * @EGG_DBUS_ERROR_SPAWN_SETUP_FAILED:
 * We failed to setup the environment correctly.
 * @EGG_DBUS_ERROR_SPAWN_CONFIG_INVALID:
 * We failed to setup the config parser correctly.
 * @EGG_DBUS_ERROR_SPAWN_SERVICE_INVALID:
 * Bus name was not valid.
 * @EGG_DBUS_ERROR_SPAWN_SERVICE_NOT_FOUND:
 * Service file not found in system-services directory.
 * @EGG_DBUS_ERROR_SPAWN_PERMISSIONS_INVALID:
 * Permissions are incorrect on the setuid helper.
 * @EGG_DBUS_ERROR_SPAWN_FILE_INVALID:
 * Service file invalid (Name, User or Exec missing).
 * @EGG_DBUS_ERROR_SPAWN_NO_MEMORY:
 * Tried to get a UNIX process ID and it wasn't available.
 * @EGG_DBUS_ERROR_UNIX_PROCESS_ID_UNKNOWN:
 * Tried to get a UNIX process ID and it wasn't available.
 * @EGG_DBUS_ERROR_INVALID_SIGNATURE:
 * A type signature is not valid.
 * @EGG_DBUS_ERROR_INVALID_FILE_CONTENT:
 * A file contains invalid syntax or is otherwise broken.
 * @EGG_DBUS_ERROR_SELINUX_SECURITY_CONTEXT_UNKNOWN:
 * Asked for SELinux security context and it wasn't available.
 * @EGG_DBUS_ERROR_ADT_AUDIT_DATA_UNKNOWN:
 * Asked for ADT audit data and it wasn't available.
 * @EGG_DBUS_ERROR_OBJECT_PATH_IN_USE:
 * There's already an object with the requested object path.
 *
 * Error codes.
 */
typedef enum
{
  EGG_DBUS_ERROR_FAILED,                           /*< nick=org.gtk.EggDBus.Error.Failed >*/
  EGG_DBUS_ERROR_CANCELLED,                        /*< nick=org.gtk.EggDBus.Error.Cancelled >*/
  EGG_DBUS_ERROR_REMOTE_EXCEPTION,                 /*< nick=org.gtk.EggDBus.Error.RemoteException >*/

  /* Well-known errors in the org.freedesktop.DBus.Error namespace */
  EGG_DBUS_ERROR_DBUS_FAILED,                      /*< nick=org.freedesktop.DBus.Error.Failed >*/
  EGG_DBUS_ERROR_NO_MEMORY,                        /*< nick=org.freedesktop.DBus.Error.NoMemory >*/
  EGG_DBUS_ERROR_SERVICE_UNKNOWN,                  /*< nick=org.freedesktop.DBus.Error.ServiceUnknown >*/
  EGG_DBUS_ERROR_NAME_HAS_NO_OWNER,                /*< nick=org.freedesktop.DBus.Error.NameHasNoOwner >*/
  EGG_DBUS_ERROR_NO_REPLY,                         /*< nick=org.freedesktop.DBus.Error.NoReply >*/
  EGG_DBUS_ERROR_IO_ERROR,                         /*< nick=org.freedesktop.DBus.Error.IOError >*/
  EGG_DBUS_ERROR_BAD_ADDRESS,                      /*< nick=org.freedesktop.DBus.Error.BadAddress >*/
  EGG_DBUS_ERROR_NOT_SUPPORTED,                    /*< nick=org.freedesktop.DBus.Error.NotSupported >*/
  EGG_DBUS_ERROR_LIMITS_EXCEEDED,                  /*< nick=org.freedesktop.DBus.Error.LimitsExceeded >*/
  EGG_DBUS_ERROR_ACCESS_DENIED,                    /*< nick=org.freedesktop.DBus.Error.AccessDenied >*/
  EGG_DBUS_ERROR_AUTH_FAILED,                      /*< nick=org.freedesktop.DBus.Error.AuthFailed >*/
  EGG_DBUS_ERROR_NO_SERVER,                        /*< nick=org.freedesktop.DBus.Error.NoServer >*/
  EGG_DBUS_ERROR_TIMEOUT,                          /*< nick=org.freedesktop.DBus.Error.Timeout >*/
  EGG_DBUS_ERROR_NO_NETWORK,                       /*< nick=org.freedesktop.DBus.Error.NoNetwork >*/
  EGG_DBUS_ERROR_ADDRESS_IN_USE,                   /*< nick=org.freedesktop.DBus.Error.AddressInUse >*/
  EGG_DBUS_ERROR_DISCONNECTED,                     /*< nick=org.freedesktop.DBus.Error.Disconnected >*/
  EGG_DBUS_ERROR_INVALID_ARGS,                     /*< nick=org.freedesktop.DBus.Error.InvalidArgs >*/
  EGG_DBUS_ERROR_FILE_NOT_FOUND,                   /*< nick=org.freedesktop.DBus.Error.FileNotFound >*/
  EGG_DBUS_ERROR_FILE_EXISTS,                      /*< nick=org.freedesktop.DBus.Error.FileExists >*/
  EGG_DBUS_ERROR_UNKNOWN_METHOD,                   /*< nick=org.freedesktop.DBus.Error.UnknownMethod >*/
  EGG_DBUS_ERROR_TIMED_OUT,                        /*< nick=org.freedesktop.DBus.Error.TimedOut >*/
  EGG_DBUS_ERROR_MATCH_RULE_NOT_FOUND,             /*< nick=org.freedesktop.DBus.Error.MatchRuleNotFound >*/
  EGG_DBUS_ERROR_MATCH_RULE_INVALID,               /*< nick=org.freedesktop.DBus.Error.MatchRuleInvalid >*/
  EGG_DBUS_ERROR_SPAWN_EXEC_FAILED,                /*< nick=org.freedesktop.DBus.Error.Spawn.ExecFailed >*/
  EGG_DBUS_ERROR_SPAWN_FORK_FAILED,                /*< nick=org.freedesktop.DBus.Error.Spawn.ForkFailed >*/
  EGG_DBUS_ERROR_SPAWN_CHILD_EXITED,               /*< nick=org.freedesktop.DBus.Error.Spawn.ChildExited >*/
  EGG_DBUS_ERROR_SPAWN_CHILD_SIGNALED,             /*< nick=org.freedesktop.DBus.Error.Spawn.ChildSignaled >*/
  EGG_DBUS_ERROR_SPAWN_FAILED,                     /*< nick=org.freedesktop.DBus.Error.Spawn.Failed >*/
  EGG_DBUS_ERROR_SPAWN_SETUP_FAILED,               /*< nick=org.freedesktop.DBus.Error.Spawn.FailedToSetup >*/
  EGG_DBUS_ERROR_SPAWN_CONFIG_INVALID,             /*< nick=org.freedesktop.DBus.Error.Spawn.ConfigInvalid >*/
  EGG_DBUS_ERROR_SPAWN_SERVICE_INVALID,            /*< nick=org.freedesktop.DBus.Error.Spawn.ServiceNotValid >*/
  EGG_DBUS_ERROR_SPAWN_SERVICE_NOT_FOUND,          /*< nick=org.freedesktop.DBus.Error.Spawn.ServiceNotFound >*/
  EGG_DBUS_ERROR_SPAWN_PERMISSIONS_INVALID,        /*< nick=org.freedesktop.DBus.Error.Spawn.PermissionsInvalid >*/
  EGG_DBUS_ERROR_SPAWN_FILE_INVALID,               /*< nick=org.freedesktop.DBus.Error.Spawn.FileInvalid >*/
  EGG_DBUS_ERROR_SPAWN_NO_MEMORY,                  /*< nick=org.freedesktop.DBus.Error.Spawn.NoMemory >*/
  EGG_DBUS_ERROR_UNIX_PROCESS_ID_UNKNOWN,          /*< nick=org.freedesktop.DBus.Error.UnixProcessIdUnknown >*/
  EGG_DBUS_ERROR_INVALID_SIGNATURE,                /*< nick=org.freedesktop.DBus.Error.InvalidSignature >*/
  EGG_DBUS_ERROR_INVALID_FILE_CONTENT,             /*< nick=org.freedesktop.DBus.Error.InvalidFileContent >*/
  EGG_DBUS_ERROR_SELINUX_SECURITY_CONTEXT_UNKNOWN, /*< nick=org.freedesktop.DBus.Error.SELinuxSecurityContextUnknown >*/
  EGG_DBUS_ERROR_ADT_AUDIT_DATA_UNKNOWN,           /*< nick=org.freedesktop.DBus.Error.AdtAuditDataUnknown >*/
  EGG_DBUS_ERROR_OBJECT_PATH_IN_USE,               /*< nick=org.freedesktop.DBus.Error.ObjectPathInUse >*/
} EggDBusError;

G_END_DECLS

#endif /* __EGG_DBUS_ERROR_H */
