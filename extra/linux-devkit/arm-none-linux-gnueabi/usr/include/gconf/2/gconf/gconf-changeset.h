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

#ifndef GCONF_GCONF_CHANGESET_H
#define GCONF_GCONF_CHANGESET_H

#include "gconf/gconf.h"

G_BEGIN_DECLS

/*
 * A GConfChangeSet is basically a hash from keys to "changes in value,"
 * where a change in a value is either a new value or "unset this value."
 *
 * You can use this to collect changes then "commit" them as a group to
 * the GConf database.
 */

#define GCONF_TYPE_CHANGE_SET                  (gconf_change_set_get_type ())

typedef struct _GConfChangeSet GConfChangeSet;

typedef void (* GConfChangeSetForeachFunc) (GConfChangeSet* cs,
                                            const gchar* key,
                                            GConfValue* value,
                                            gpointer user_data);

gboolean        gconf_engine_commit_change_set   (GConfEngine* conf,
                                                  GConfChangeSet* cs,
                                                  /* remove all
                                                     successfully
                                                     committed changes
                                                     from the set */
                                                  gboolean remove_committed,
                                                  GError** err);

/* Create a change set that would revert the given change set
   for the given GConfEngine */
GConfChangeSet* gconf_engine_reverse_change_set  (GConfEngine* conf,
                                                  GConfChangeSet* cs,
                                                  GError** err);

/* Create a change set that would restore the current state of all the keys
   in the NULL-terminated array "keys" */
GConfChangeSet* gconf_engine_change_set_from_currentv (GConfEngine* conf,
                                                       const gchar** keys,
                                                       GError** err);

GConfChangeSet* gconf_engine_change_set_from_current (GConfEngine* conf,
                                                      GError** err,
                                                      const gchar* first_key,
                                                      ...) G_GNUC_NULL_TERMINATED;


GType           gconf_change_set_get_type (void);
GConfChangeSet* gconf_change_set_new      (void);
void            gconf_change_set_ref      (GConfChangeSet* cs);

void            gconf_change_set_unref    (GConfChangeSet* cs);

void            gconf_change_set_clear    (GConfChangeSet* cs);

guint           gconf_change_set_size     (GConfChangeSet* cs);

void            gconf_change_set_remove   (GConfChangeSet* cs,
                                           const gchar* key);

void            gconf_change_set_foreach  (GConfChangeSet* cs,
                                           GConfChangeSetForeachFunc func,
                                           gpointer user_data);

/* Returns TRUE if the change set contains the given key; if the key
   is in the set, either NULL (for unset) or a GConfValue is placed in
   *value_retloc; the value is not a copy and should not be
   freed. value_retloc can be NULL if you just want to check for a value,
   and don't care what it is. */
gboolean     gconf_change_set_check_value   (GConfChangeSet* cs, const gchar* key,
                                             GConfValue** value_retloc);

void         gconf_change_set_set         (GConfChangeSet* cs, const gchar* key,
                                           GConfValue* value);

void         gconf_change_set_set_nocopy  (GConfChangeSet* cs, const gchar* key,
                                           GConfValue* value);

void         gconf_change_set_unset      (GConfChangeSet* cs, const gchar* key);

void         gconf_change_set_set_float   (GConfChangeSet* cs, const gchar* key,
                                           gdouble val);

void         gconf_change_set_set_int     (GConfChangeSet* cs, const gchar* key,
                                           gint val);

void         gconf_change_set_set_string  (GConfChangeSet* cs, const gchar* key,
                                           const gchar* val);

void         gconf_change_set_set_bool    (GConfChangeSet* cs, const gchar* key,
                                           gboolean val);

void         gconf_change_set_set_schema  (GConfChangeSet* cs, const gchar* key,
                                           GConfSchema* val);

void         gconf_change_set_set_list    (GConfChangeSet* cs, const gchar* key,
                                           GConfValueType list_type,
                                           GSList* list);

void         gconf_change_set_set_pair    (GConfChangeSet* cs, const gchar* key,
                                           GConfValueType car_type, GConfValueType cdr_type,
                                           gconstpointer address_of_car,
                                           gconstpointer address_of_cdr);


/* For use by language bindings only */
void     gconf_change_set_set_user_data (GConfChangeSet *cs,
                                         gpointer        data,
                                         GDestroyNotify  dnotify);
gpointer gconf_change_set_get_user_data (GConfChangeSet *cs);



G_END_DECLS

#endif



