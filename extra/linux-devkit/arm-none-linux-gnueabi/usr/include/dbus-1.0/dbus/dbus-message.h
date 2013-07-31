/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* dbus-message.h DBusMessage object
 *
 * Copyright (C) 2002, 2003, 2005 Red Hat Inc.
 *
 * Licensed under the Academic Free License version 2.1
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#if !defined (DBUS_INSIDE_DBUS_H) && !defined (DBUS_COMPILATION)
#error "Only <dbus/dbus.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef DBUS_MESSAGE_H
#define DBUS_MESSAGE_H

#include <dbus/dbus-macros.h>
#include <dbus/dbus-types.h>
#include <dbus/dbus-arch-deps.h>
#include <dbus/dbus-memory.h>
#include <dbus/dbus-errors.h>
#include <stdarg.h>

DBUS_BEGIN_DECLS

/**
 * @addtogroup DBusMessage
 * @{
 */

typedef struct DBusMessage DBusMessage;
/** Opaque type representing a message iterator. Can be copied by value, and contains no allocated memory so never needs to be freed and can be allocated on the stack. */
typedef struct DBusMessageIter DBusMessageIter;

/**
 * DBusMessageIter struct; contains no public fields. 
 */
struct DBusMessageIter
{ 
  void *dummy1;         /**< Don't use this */
  void *dummy2;         /**< Don't use this */
  dbus_uint32_t dummy3; /**< Don't use this */
  int dummy4;           /**< Don't use this */
  int dummy5;           /**< Don't use this */
  int dummy6;           /**< Don't use this */
  int dummy7;           /**< Don't use this */
  int dummy8;           /**< Don't use this */
  int dummy9;           /**< Don't use this */
  int dummy10;          /**< Don't use this */
  int dummy11;          /**< Don't use this */
  int pad1;             /**< Don't use this */
  int pad2;             /**< Don't use this */
  void *pad3;           /**< Don't use this */
};

DBusMessage* dbus_message_new               (int          message_type);
DBusMessage* dbus_message_new_method_call   (const char  *bus_name,
                                             const char  *path,
                                             const char  *interface,
                                             const char  *method);
DBusMessage* dbus_message_new_method_return (DBusMessage *method_call);
DBusMessage* dbus_message_new_signal        (const char  *path,
                                             const char  *interface,
                                             const char  *name);
DBusMessage* dbus_message_new_error         (DBusMessage *reply_to,
                                             const char  *error_name,
                                             const char  *error_message);
DBusMessage* dbus_message_new_error_printf  (DBusMessage *reply_to,
                                             const char  *error_name,
                                             const char  *error_format,
					     ...);

DBusMessage* dbus_message_copy              (const DBusMessage *message);

DBusMessage*  dbus_message_ref              (DBusMessage   *message);
void          dbus_message_unref            (DBusMessage   *message);
int           dbus_message_get_type         (DBusMessage   *message);
dbus_bool_t   dbus_message_set_path         (DBusMessage   *message,
                                             const char    *object_path);
const char*   dbus_message_get_path         (DBusMessage   *message);
dbus_bool_t   dbus_message_has_path         (DBusMessage   *message, 
                                             const char    *object_path);  
dbus_bool_t   dbus_message_set_interface    (DBusMessage   *message,
                                             const char    *interface);       
const char*   dbus_message_get_interface    (DBusMessage   *message);
dbus_bool_t   dbus_message_has_interface    (DBusMessage   *message, 
                                             const char    *interface);
dbus_bool_t   dbus_message_set_member       (DBusMessage   *message,
                                             const char    *member);
const char*   dbus_message_get_member       (DBusMessage   *message);
dbus_bool_t   dbus_message_has_member       (DBusMessage   *message, 
                                             const char    *member);
dbus_bool_t   dbus_message_set_error_name   (DBusMessage   *message,
                                             const char    *name);
const char*   dbus_message_get_error_name   (DBusMessage   *message);
dbus_bool_t   dbus_message_set_destination  (DBusMessage   *message,
                                             const char    *destination);
const char*   dbus_message_get_destination  (DBusMessage   *message);
dbus_bool_t   dbus_message_set_sender       (DBusMessage   *message,
                                             const char    *sender);
const char*   dbus_message_get_sender       (DBusMessage   *message);
const char*   dbus_message_get_signature    (DBusMessage   *message);
void          dbus_message_set_no_reply     (DBusMessage   *message,
                                             dbus_bool_t    no_reply);
dbus_bool_t   dbus_message_get_no_reply     (DBusMessage   *message);
dbus_bool_t   dbus_message_is_method_call   (DBusMessage   *message,
                                             const char    *interface,
                                             const char    *method);
dbus_bool_t   dbus_message_is_signal        (DBusMessage   *message,
                                             const char    *interface,
                                             const char    *signal_name);
dbus_bool_t   dbus_message_is_error         (DBusMessage   *message,
                                             const char    *error_name);
dbus_bool_t   dbus_message_has_destination  (DBusMessage   *message,
                                             const char    *bus_name);
dbus_bool_t   dbus_message_has_sender       (DBusMessage   *message,
                                             const char    *unique_bus_name);
dbus_bool_t   dbus_message_has_signature    (DBusMessage   *message,
                                             const char    *signature);
dbus_uint32_t dbus_message_get_serial       (DBusMessage   *message);
void          dbus_message_set_serial       (DBusMessage   *message, 
                                             dbus_uint32_t  serial);
dbus_bool_t   dbus_message_set_reply_serial (DBusMessage   *message,
                                             dbus_uint32_t  reply_serial);
dbus_uint32_t dbus_message_get_reply_serial (DBusMessage   *message);

void          dbus_message_set_auto_start   (DBusMessage   *message,
                                             dbus_bool_t    auto_start);
dbus_bool_t   dbus_message_get_auto_start   (DBusMessage   *message);

dbus_bool_t   dbus_message_get_path_decomposed (DBusMessage   *message,
                                                char        ***path);

dbus_bool_t dbus_message_append_args          (DBusMessage     *message,
					       int              first_arg_type,
					       ...);
dbus_bool_t dbus_message_append_args_valist   (DBusMessage     *message,
					       int              first_arg_type,
					       va_list          var_args);
dbus_bool_t dbus_message_get_args             (DBusMessage     *message,
					       DBusError       *error,
					       int              first_arg_type,
					       ...);
dbus_bool_t dbus_message_get_args_valist      (DBusMessage     *message,
					       DBusError       *error,
					       int              first_arg_type,
					       va_list          var_args);


dbus_bool_t dbus_message_iter_init             (DBusMessage     *message,
                                                DBusMessageIter *iter);
dbus_bool_t dbus_message_iter_has_next         (DBusMessageIter *iter);
dbus_bool_t dbus_message_iter_next             (DBusMessageIter *iter);
char*       dbus_message_iter_get_signature    (DBusMessageIter *iter);
int         dbus_message_iter_get_arg_type     (DBusMessageIter *iter);
int         dbus_message_iter_get_element_type (DBusMessageIter *iter);
void        dbus_message_iter_recurse          (DBusMessageIter *iter,
                                                DBusMessageIter *sub);
void        dbus_message_iter_get_basic        (DBusMessageIter *iter,
                                                void            *value);
#ifndef DBUS_DISABLE_DEPRECATED
/* This function returns the wire protocol size of the array in bytes,
 * you do not want to know that probably
 */
DBUS_DEPRECATED int         dbus_message_iter_get_array_len    (DBusMessageIter *iter);
#endif
void        dbus_message_iter_get_fixed_array  (DBusMessageIter *iter,
                                                void            *value,
                                                int             *n_elements);


void        dbus_message_iter_init_append        (DBusMessage     *message,
                                                  DBusMessageIter *iter);
dbus_bool_t dbus_message_iter_append_basic       (DBusMessageIter *iter,
                                                  int              type,
                                                  const void      *value);
dbus_bool_t dbus_message_iter_append_fixed_array (DBusMessageIter *iter,
                                                  int              element_type,
                                                  const void      *value,
                                                  int              n_elements);
dbus_bool_t dbus_message_iter_open_container     (DBusMessageIter *iter,
                                                  int              type,
                                                  const char      *contained_signature,
                                                  DBusMessageIter *sub);
dbus_bool_t dbus_message_iter_close_container    (DBusMessageIter *iter,
                                                  DBusMessageIter *sub);
void        dbus_message_iter_abandon_container  (DBusMessageIter *iter,
                                                  DBusMessageIter *sub);

void dbus_message_lock    (DBusMessage  *message);

dbus_bool_t  dbus_set_error_from_message  (DBusError    *error,
                                           DBusMessage  *message);


dbus_bool_t dbus_message_allocate_data_slot (dbus_int32_t     *slot_p);
void        dbus_message_free_data_slot     (dbus_int32_t     *slot_p);
dbus_bool_t dbus_message_set_data           (DBusMessage      *message,
                                             dbus_int32_t      slot,
                                             void             *data,
                                             DBusFreeFunction  free_data_func);
void*       dbus_message_get_data           (DBusMessage      *message,
                                             dbus_int32_t      slot);

int         dbus_message_type_from_string (const char *type_str);
const char* dbus_message_type_to_string   (int type);

dbus_bool_t  dbus_message_marshal   (DBusMessage  *msg,
                                     char        **marshalled_data_p,
                                     int          *len_p);
DBusMessage* dbus_message_demarshal (const char *str,
                                     int         len,
                                     DBusError  *error);

int          dbus_message_demarshal_bytes_needed (const char *str, 
                                                  int len);

/** @} */

DBUS_END_DECLS

#endif /* DBUS_MESSAGE_H */
