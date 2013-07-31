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

#ifndef GCONF_GCONF_H
#define GCONF_GCONF_H

#include <glib.h>

G_BEGIN_DECLS

#include "gconf/gconf-schema.h"
#include "gconf/gconf-engine.h"
#include "gconf/gconf-error.h"
#include "gconf/gconf-enum-types.h"

typedef void (*GConfNotifyFunc) (GConfEngine* conf,
                                 guint cnxn_id,
                                 GConfEntry *entry,
                                 gpointer user_data);
  
/* Returns ID of the notification */
/* returns 0 on error, 0 is an invalid ID */
guint gconf_engine_notify_add    (GConfEngine      *conf,
                                  /* dir or key to listen to */
                                  const gchar      *namespace_section,
                                  GConfNotifyFunc   func,
                                  gpointer          user_data,
                                  GError      **err);

void  gconf_engine_notify_remove (GConfEngine      *conf,
                                  guint             cnxn);



/* Low-level interfaces */
GConfValue* gconf_engine_get                     (GConfEngine  *conf,
                                                  const gchar  *key,
                                                  GError  **err);

GConfValue* gconf_engine_get_without_default     (GConfEngine  *conf,
                                                  const gchar  *key,
                                                  GError  **err);

GConfEntry* gconf_engine_get_entry                (GConfEngine  *conf,
                                                   const gchar  *key,
                                                   const gchar  *locale,
                                                   gboolean      use_schema_default,
                                                   GError  **err);


/* Locale only matters if you are expecting to get a schema, or if you
   don't know what you are expecting and it might be a schema. Note
   that gconf_engine_get () automatically uses the current locale, which is
   normally what you want. */
GConfValue* gconf_engine_get_with_locale         (GConfEngine  *conf,
                                                  const gchar  *key,
                                                  const gchar  *locale,
                                                  GError  **err);


/* Get the default value stored in the schema associated with this key */
GConfValue* gconf_engine_get_default_from_schema (GConfEngine  *conf,
                                                  const gchar  *key,
                                                  GError  **err);
gboolean    gconf_engine_set                     (GConfEngine  *conf,
                                                  const gchar  *key,
                                                  const GConfValue *value,
                                                  GError  **err);
gboolean    gconf_engine_unset                   (GConfEngine  *conf,
                                                  const gchar  *key,
                                                  GError  **err);


/*
 * schema_key should have a schema (if key stores a value) or a dir
 * full of schemas (if key stores a directory name)
 */

gboolean gconf_engine_associate_schema (GConfEngine  *conf,
                                        const gchar  *key,
                                        const gchar  *schema_key,
                                        GError  **err);
GSList*  gconf_engine_all_entries      (GConfEngine  *conf,
                                        const gchar  *dir,
                                        GError  **err);
GSList*  gconf_engine_all_dirs         (GConfEngine  *conf,
                                        const gchar  *dir,
                                        GError  **err);
void     gconf_engine_suggest_sync     (GConfEngine  *conf,
                                        GError  **err);
gboolean gconf_engine_dir_exists       (GConfEngine  *conf,
                                        const gchar  *dir,
                                        GError  **err);
void     gconf_engine_remove_dir       (GConfEngine* conf,
                                        const gchar* dir,
                                        GError** err);

gboolean gconf_engine_key_is_writable  (GConfEngine *conf,
                                        const gchar *key,
                                        GError     **err);

/* if you pass non-NULL for why_invalid, it gives a user-readable
   explanation of the problem in g_malloc()'d memory
*/
gboolean gconf_valid_key          (const gchar  *key,
                                   gchar       **why_invalid);


/* return TRUE if the path "below" would be somewhere below the directory "above" */
gboolean gconf_key_is_below       (const gchar  *above,
                                   const gchar  *below);


/* Returns allocated concatenation of these two */
gchar*   gconf_concat_dir_and_key (const gchar  *dir,
                                   const gchar  *key);


/* Returns a different string every time (at least, the chances of
   getting a duplicate are like one in a zillion). The key is a
   legal gconf key name (a single element of one) */
gchar*   gconf_unique_key         (void);

/* Escape/unescape a string to create a valid key */
char*    gconf_escape_key         (const char *arbitrary_text,
                                   int         len);
char*    gconf_unescape_key       (const char *escaped_key,
                                   int         len);


/* 
 * Higher-level stuff 
 */


gdouble      gconf_engine_get_float  (GConfEngine     *conf,
                                      const gchar     *key,
                                      GError     **err);
gint         gconf_engine_get_int    (GConfEngine     *conf,
                                      const gchar     *key,
                                      GError     **err);


/* free the retval, retval can be NULL for "unset" */
gchar*       gconf_engine_get_string (GConfEngine     *conf,
                                      const gchar     *key,
                                      GError     **err);
gboolean     gconf_engine_get_bool   (GConfEngine     *conf,
                                      const gchar     *key,
                                      GError     **err);


/* this one has no default since it would be expensive and make little
   sense; it returns NULL as a default, to indicate unset or error */
/* free the retval */
/* Note that this returns the schema stored at key, NOT
   the schema associated with the key. */
GConfSchema* gconf_engine_get_schema (GConfEngine     *conf,
                                      const gchar     *key,
                                      GError     **err);


/*
  This automatically converts the list to the given list type;
  a list of int or bool stores values in the list->data field
  using GPOINTER_TO_INT(), a list of strings stores the gchar*
  in list->data, a list of float contains pointers to allocated
  gdouble (gotta love C!).
*/
GSList*      gconf_engine_get_list   (GConfEngine     *conf,
                                      const gchar     *key,
                                      GConfValueType   list_type,
                                      GError     **err);

/*
  The car_retloc and cdr_retloc args should be the address of the appropriate
  type:
  bool    gboolean*
  int     gint*
  string  gchar**
  float   gdouble*
  schema  GConfSchema**
*/
gboolean     gconf_engine_get_pair   (GConfEngine     *conf,
                                      const gchar     *key,
                                      GConfValueType   car_type,
                                      GConfValueType   cdr_type,
                                      gpointer         car_retloc,
                                      gpointer         cdr_retloc,
                                      GError     **err);


/* setters return TRUE on success; note that you still should suggest a sync */
gboolean gconf_engine_set_float  (GConfEngine     *conf,
                                  const gchar     *key,
                                  gdouble          val,
                                  GError     **err);
gboolean gconf_engine_set_int    (GConfEngine     *conf,
                                  const gchar     *key,
                                  gint             val,
                                  GError     **err);
gboolean gconf_engine_set_string (GConfEngine     *conf,
                                  const gchar     *key,
                                  const gchar     *val,
                                  GError     **err);
gboolean gconf_engine_set_bool   (GConfEngine     *conf,
                                  const gchar     *key,
                                  gboolean         val,
                                  GError     **err);
gboolean gconf_engine_set_schema (GConfEngine     *conf,
                                  const gchar     *key,
                                  const GConfSchema     *val,
                                  GError     **err);


/* List should be the same as the one gconf_engine_get_list() would return */
gboolean gconf_engine_set_list   (GConfEngine     *conf,
                                  const gchar     *key,
                                  GConfValueType   list_type,
                                  GSList          *list,
                                  GError     **err);
gboolean gconf_engine_set_pair   (GConfEngine     *conf,
                                  const gchar     *key,
                                  GConfValueType   car_type,
                                  GConfValueType   cdr_type,
                                  gconstpointer    address_of_car,
                                  gconstpointer    address_of_cdr,
                                  GError     **err);


/* Utility function converts enumerations to and from strings */
typedef struct _GConfEnumStringPair GConfEnumStringPair;

struct _GConfEnumStringPair {
  gint enum_value;
  const gchar* str;
};

gboolean     gconf_string_to_enum (GConfEnumStringPair  lookup_table[],
                                   const gchar         *str,
                                   gint                *enum_value_retloc);
const gchar* gconf_enum_to_string (GConfEnumStringPair  lookup_table[],
                                   gint                 enum_value);

int gconf_debug_shutdown (void);

#ifndef GCONF_DISABLE_DEPRECATED
gboolean     gconf_init           (int argc, char **argv, GError** err);
gboolean     gconf_is_initialized (void);
#endif /* GCONF_DISABLE_DEPRECATED */

/* No, you can't use this stuff. Bad application developer. Bad. */
#ifdef GCONF_ENABLE_INTERNALS

/* This stuff is only useful in GNOME 2.0, so isn't in this GConf
 * release.
 */

#ifndef GCONF_DISABLE_DEPRECATED
/* For use by the Gnome module system */
void gconf_preinit(gpointer app, gpointer mod_info);
void gconf_postinit(gpointer app, gpointer mod_info);

extern const char gconf_version[];

#ifdef HAVE_POPT_H
#include <popt.h>
#endif

#ifdef POPT_AUTOHELP
/* If people are using popt, then make the table available to them */
extern struct poptOption gconf_options[];
#endif
#endif /* GCONF_DISABLE_DEPRECATED */

void gconf_clear_cache(GConfEngine* conf, GError** err);
void gconf_synchronous_sync(GConfEngine* conf, GError** err);

GConfValue * gconf_engine_get_full (GConfEngine *conf,
                                    const gchar *key,
                                    const gchar *locale,
                                    gboolean use_schema_default,
                                    gboolean *is_default_p,
                                    gboolean *is_writable_p,
                                    GError **err);

#endif /* GCONF_ENABLE_INTERNALS */

G_END_DECLS

#endif
