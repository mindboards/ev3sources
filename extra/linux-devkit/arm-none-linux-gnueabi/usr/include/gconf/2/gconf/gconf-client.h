/* -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* GConf
 * Copyright (C) 1999, 2000 Red Hat Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef GCONF_GCONF_CLIENT_H
#define GCONF_GCONF_CLIENT_H

#include <glib-object.h>
#include "gconf/gconf.h"
#include "gconf/gconf-listeners.h"
#include "gconf/gconf-changeset.h"

G_BEGIN_DECLS

/*
 * This is a wrapper for the client-side GConf API which provides several
 * convenient features.
 *
 *  - It (recursively) caches the contents of certain directories on
 *    the client side, such as your application's configuration
 *    directory
 *
 *  - It allows you to register per-key callbacks within these directories,
 *    without having to register multiple server-side callbacks
 *    (gconf_engine_notify_add() adds a request-for-notify to the server,
 *    this wrapper adds a notify to the server for the whole directory
 *    and keeps your per-key notify requests on the client side).
 *
 *  - It has some error-handling features
 *
 * This class is heavily specialized for per-user desktop applications -
 * even more so than GConf itself.
 */

/*
 * IMPORTANT: you can't mix GConfClient with direct GConfEngine access,
 * or you will have a mess because the client won't know what you're doing
 * underneath it.
 */

typedef enum { /*< prefix=GCONF_CLIENT >*/
  GCONF_CLIENT_PRELOAD_NONE,     /* don't preload anything */
  GCONF_CLIENT_PRELOAD_ONELEVEL, /* load entries directly under the directory. */
  GCONF_CLIENT_PRELOAD_RECURSIVE /* recurse the directory tree; possibly quite expensive! */
} GConfClientPreloadType;

typedef enum { /*< prefix=GCONF_CLIENT >*/
  GCONF_CLIENT_HANDLE_NONE,
  GCONF_CLIENT_HANDLE_UNRETURNED,
  GCONF_CLIENT_HANDLE_ALL
} GConfClientErrorHandlingMode;


typedef struct _GConfClient       GConfClient;
typedef struct _GConfClientClass  GConfClientClass;


typedef void (*GConfClientNotifyFunc)(GConfClient* client,
                                      guint cnxn_id,
                                      GConfEntry *entry,
                                      gpointer user_data);

typedef void (*GConfClientErrorHandlerFunc) (GConfClient* client,
                                             GError* error);

#define GCONF_TYPE_CLIENT                  (gconf_client_get_type ())
#define GCONF_CLIENT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GCONF_TYPE_CLIENT, GConfClient))
#define GCONF_CLIENT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GCONF_TYPE_CLIENT, GConfClientClass))
#define GCONF_IS_CLIENT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GCONF_TYPE_CLIENT))
#define GCONF_IS_CLIENT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GCONF_TYPE_CLIENT))
#define GCONF_CLIENT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GCONF_TYPE_CLIENT, GConfClientClass))

struct _GConfClient
{
  GObject object;

  /*< private >*/

  GConfEngine* engine;
  GConfClientErrorHandlingMode error_mode;
  GHashTable* dir_hash;
  GHashTable* cache_hash;
  GConfListeners* listeners;
  GSList *notify_list;
  guint notify_handler;
  int pending_notify_count;
  GHashTable *cache_dirs;
  int pad2;
};

struct _GConfClientClass
{
  GObjectClass parent_class;

  /* emitted whenever a value changes. Often, you should use a notify
     function instead; the problem with this signal is that you
     probably have to do an expensive chain of strcmp() to
     determine how to respond to it.
  */

  void (* value_changed) (GConfClient* client,
                          const gchar* key,
                          GConfValue* value);

  /* General note about error handling: AVOID DIALOG DELUGES.
     That is, if lots of errors could happen in a row you need
     to collect those and put them in _one_ dialog, maybe using
     an idle function. gconf_client_set_error_handling()
     is provided and it does this using GnomeDialog.  */

  /* emitted when you pass NULL for the error return location to a
     GConfClient function and an error occurs. This allows you to
     ignore errors when your generic handler will work, and handle
     them specifically when you need to */
  void (* unreturned_error) (GConfClient* client,
                             GError* error);

  /* emitted unconditionally anytime there's an error, whether you ask
     for that error or not. Useful for creating an error log or
     something. */
  void (* error)            (GConfClient* client,
                             GError* error);

  GFunc pad1;
  GFunc pad2;
  GFunc pad3;
};


GType             gconf_client_get_type        (void);

/* use the default engine */
GConfClient*      gconf_client_get_default             (void);

/* refcount on engine is incremented, you still own your ref */
GConfClient*      gconf_client_get_for_engine (GConfEngine* engine);

/* Add a directory to monitor and emit the value_changed signal and
   key notifications for.  Optionally pre-load the contents of this
   directory, much faster if you plan to access most of the directory
   contents.
*/

void              gconf_client_add_dir     (GConfClient* client,
                                            const gchar* dir,
                                            GConfClientPreloadType preload,
                                            GError** err);


/* This removes any notifications associated with the dir */
void              gconf_client_remove_dir  (GConfClient* client,
                                            const gchar* dir,
                                            GError** err);

/*
 *  The notification facility allows you to attach a callback to a single
 *  key or directory, which is more convenient most of the time than
 *  the value_changed signal. The key you're monitoring must be inside one
 *  of the GConfClient's directories.
 */


/* Returns ID of the notification */
/* returns 0 on error, 0 is an invalid ID */
guint        gconf_client_notify_add(GConfClient* client,
                                     const gchar* namespace_section, /* dir or key to listen to */
                                     GConfClientNotifyFunc func,
                                     gpointer user_data,
                                     GFreeFunc destroy_notify,
                                     GError** err);

void         gconf_client_notify_remove  (GConfClient* client,
                                          guint cnxn);
void         gconf_client_notify (GConfClient* client, const char* key);

/*
 * Error handling convenience; if you don't want the default handler,
 * set the error handling to GCONF_CLIENT_HANDLE_NONE
 */

/* 
 * Error handling happens in the default signal handler, so you can
 * selectively override the default handling by connecting to the error
 * signal and calling g_signal_stop_emission()
 */

void              gconf_client_set_error_handling(GConfClient* client,
                                                  GConfClientErrorHandlingMode mode);


/* Intended for use by gnome-libs */
void              gconf_client_set_global_default_error_handler(GConfClientErrorHandlerFunc func);

/*
 * If you know you're done reading values for a while,
 * you can blow away the cache. Note that this nullifies the effect of
 * any preloading you may have done. However it frees some memory.
 */
void              gconf_client_clear_cache(GConfClient* client);

/*
 * Preload a directory; the directory must have been added already.
 * This is only useful as an optimization if you clear the cache,
 * then later want to do a lot of reads again. It's not useful
 * unless you clear the cache, because you can preload when you
 * call gconf_client_add_dir()
 */
void              gconf_client_preload    (GConfClient* client,
                                           const gchar* dirname,
                                           GConfClientPreloadType type,
                                           GError** err);

/*
 * Basic key-manipulation facilities; these keys do _not_ have to be in the
 * client's directory list, but they won't be cached unless they are.
 */

void              gconf_client_set             (GConfClient* client,
                                                const gchar* key,
                                                const GConfValue* val,
                                                GError** err);

GConfValue*       gconf_client_get             (GConfClient* client,
                                                const gchar* key,
                                                GError** err);

GConfValue*       gconf_client_get_without_default  (GConfClient* client,
                                                     const gchar* key,
                                                     GError** err);

GConfEntry*       gconf_client_get_entry        (GConfClient* client,
                                                 const gchar* key,
                                                 const gchar* locale,
                                                 gboolean use_schema_default,
                                                 GError** err);

GConfValue*       gconf_client_get_default_from_schema (GConfClient* client,
                                                        const gchar* key,
                                                        GError** err);

gboolean     gconf_client_unset          (GConfClient* client,
                                          const gchar* key, GError** err);

gboolean     gconf_client_recursive_unset (GConfClient *client,
                                           const char     *key,
                                           GConfUnsetFlags flags,
                                           GError        **err);

GSList*      gconf_client_all_entries    (GConfClient* client,
                                          const gchar* dir, GError** err);

GSList*      gconf_client_all_dirs       (GConfClient* client,
                                          const gchar* dir, GError** err);

void         gconf_client_suggest_sync   (GConfClient* client,
                                          GError** err);

gboolean     gconf_client_dir_exists     (GConfClient* client,
                                          const gchar* dir, GError** err);

gboolean     gconf_client_key_is_writable(GConfClient* client,
                                          const gchar* key,
                                          GError**     err);

/* Get/Set convenience wrappers */

gdouble      gconf_client_get_float (GConfClient* client, const gchar* key,
                                     GError** err);

gint         gconf_client_get_int   (GConfClient* client, const gchar* key,
                                     GError** err);

/* free the retval, if non-NULL */
gchar*       gconf_client_get_string(GConfClient* client, const gchar* key,
                                     GError** err);

gboolean     gconf_client_get_bool  (GConfClient* client, const gchar* key,
                                     GError** err);

/* this one has no default since it would be expensive and make little
   sense; it returns NULL as a default, to indicate unset or error */
/* free the retval */
/* Note that this returns the schema stored at key, NOT
   the schema that key conforms to. */
GConfSchema* gconf_client_get_schema  (GConfClient* client,
                                       const gchar* key, GError** err);

GSList*      gconf_client_get_list    (GConfClient* client, const gchar* key,
                                       GConfValueType list_type, GError** err);

gboolean     gconf_client_get_pair    (GConfClient* client, const gchar* key,
                                       GConfValueType car_type, GConfValueType cdr_type,
                                       gpointer car_retloc, gpointer cdr_retloc,
                                       GError** err);

/* No convenience functions for lists or pairs, since there are too
   many combinations of types possible
*/

/* setters return TRUE on success; note that you still should suggest a sync */

gboolean     gconf_client_set_float   (GConfClient* client, const gchar* key,
                                       gdouble val, GError** err);

gboolean     gconf_client_set_int     (GConfClient* client, const gchar* key,
                                       gint val, GError** err);

gboolean     gconf_client_set_string  (GConfClient* client, const gchar* key,
                                       const gchar* val, GError** err);

gboolean     gconf_client_set_bool    (GConfClient* client, const gchar* key,
                                       gboolean val, GError** err);

gboolean     gconf_client_set_schema  (GConfClient* client, const gchar* key,
                                       const GConfSchema* val, GError** err);

/* List should be the same as the one gconf_client_get_list() would return */
gboolean     gconf_client_set_list    (GConfClient* client, const gchar* key,
                                       GConfValueType list_type,
                                       GSList* list,
                                       GError** err);

gboolean     gconf_client_set_pair    (GConfClient* client, const gchar* key,
                                       GConfValueType car_type, GConfValueType cdr_type,
                                       gconstpointer address_of_car,
                                       gconstpointer address_of_cdr,
                                       GError** err);

/*
 * Functions to emit signals
 */

/* these are useful to manually invoke the default error handlers */
void         gconf_client_error                  (GConfClient* client, GError* error);
void         gconf_client_unreturned_error       (GConfClient* client, GError* error);

/* useful to force an update */
void         gconf_client_value_changed          (GConfClient* client,
                                                  const gchar* key,
                                                  GConfValue* value);

/*
 * Change set stuff
 */

gboolean        gconf_client_commit_change_set   (GConfClient* client,
                                                  GConfChangeSet* cs,
                                                  /* remove all
                                                     successfully
                                                     committed changes
                                                     from the set */
                                                  gboolean remove_committed,
                                                  GError** err);

/* Create a change set that would revert the given change set
   for the given GConfClient */
GConfChangeSet* gconf_client_reverse_change_set  (GConfClient* client,
                                                         GConfChangeSet* cs,
                                                         GError** err);

GConfChangeSet* gconf_client_change_set_from_currentv (GConfClient* client,
                                                              const gchar** keys,
                                                              GError** err);

GConfChangeSet* gconf_client_change_set_from_current (GConfClient* client,
                                                             GError** err,
                                                             const gchar* first_key,
                                                             ...) G_GNUC_NULL_TERMINATED;

G_END_DECLS

#endif



