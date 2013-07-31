
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

#ifndef GCONF_GCONF_VALUE_H
#define GCONF_GCONF_VALUE_H

#include <glib.h>
#include "gconf-error.h"

G_BEGIN_DECLS

/* 
 * A GConfValue is used to pass configuration values around
 */

typedef enum {
  GCONF_VALUE_INVALID,
  GCONF_VALUE_STRING,
  GCONF_VALUE_INT,
  GCONF_VALUE_FLOAT,
  GCONF_VALUE_BOOL,
  GCONF_VALUE_SCHEMA,

  /* unfortunately these aren't really types; we want list_of_string,
     list_of_int, etc.  but it's just too complicated to implement.
     instead we'll complain in various places if you do something
     moronic like mix types in a list or treat pair<string,int> and
     pair<float,bool> as the same type. */
  GCONF_VALUE_LIST,
  GCONF_VALUE_PAIR
  
} GConfValueType;

#define GCONF_VALUE_TYPE_VALID(x) (((x) > GCONF_VALUE_INVALID) && ((x) <= GCONF_VALUE_PAIR))

/* Forward declaration */
typedef struct _GConfSchema GConfSchema;

typedef struct _GConfValue GConfValue;

struct _GConfValue {
  GConfValueType type;
};

const char*    gconf_value_get_string    (const GConfValue *value);
int            gconf_value_get_int       (const GConfValue *value);
double         gconf_value_get_float     (const GConfValue *value);
GConfValueType gconf_value_get_list_type (const GConfValue *value);
GSList*        gconf_value_get_list      (const GConfValue *value);
GConfValue*    gconf_value_get_car       (const GConfValue *value);
GConfValue*    gconf_value_get_cdr       (const GConfValue *value);
gboolean       gconf_value_get_bool      (const GConfValue *value);
GConfSchema*   gconf_value_get_schema    (const GConfValue *value);

GConfValue* gconf_value_new                  (GConfValueType type);

/* doesn't work on complicated types (only string, int, bool, float) */
GConfValue* gconf_value_new_from_string      (GConfValueType type,
                                              const gchar* str,
                                              GError** err);

GConfValue* gconf_value_copy                 (const GConfValue* src);
void        gconf_value_free                 (GConfValue* value);

void        gconf_value_set_int              (GConfValue* value,
                                              gint the_int);
void        gconf_value_set_string           (GConfValue* value,
                                              const gchar* the_str);
void        gconf_value_set_float            (GConfValue* value,
                                              gdouble the_float);
void        gconf_value_set_bool             (GConfValue* value,
                                              gboolean the_bool);
void        gconf_value_set_schema           (GConfValue* value,
                                              const GConfSchema* sc);
void        gconf_value_set_schema_nocopy    (GConfValue* value,
                                              GConfSchema* sc);
void        gconf_value_set_car              (GConfValue* value,
                                              const GConfValue* car);
void        gconf_value_set_car_nocopy       (GConfValue* value,
                                              GConfValue* car);
void        gconf_value_set_cdr              (GConfValue* value,
                                              const GConfValue* cdr);
void        gconf_value_set_cdr_nocopy       (GConfValue* value,
                                              GConfValue* cdr);
/* Set a list of GConfValue, NOT lists or pairs */
void        gconf_value_set_list_type        (GConfValue* value,
                                              GConfValueType type);
void        gconf_value_set_list_nocopy      (GConfValue* value,
                                              GSList* list);
void        gconf_value_set_list             (GConfValue* value,
                                              GSList* list);

gchar*      gconf_value_to_string            (const GConfValue* value);

int         gconf_value_compare              (const GConfValue* value_a,
                                              const GConfValue* value_b);

GConfValue* gconf_value_decode               (const gchar *encoded);
gchar*      gconf_value_encode               (GConfValue  *val);

/* Meta-information about a key. Not the same as a schema; this is
 * information stored on the key, the schema is a specification
 * that may apply to this key.
 */

/* FIXME GConfMetaInfo is basically deprecated in favor of stuffing this
 * info into GConfEntry, though the transition isn't complete.
 */

typedef struct _GConfMetaInfo GConfMetaInfo;

struct _GConfMetaInfo {
  gchar* schema;
  gchar* mod_user; /* user owning the daemon that made the last modification */
  GTime  mod_time; /* time of the modification */
};

const char* gconf_meta_info_get_schema   (GConfMetaInfo *gcmi);
const char* gconf_meta_info_get_mod_user (GConfMetaInfo *gcmi);
GTime       gconf_meta_info_mod_time     (GConfMetaInfo *gcmi);

GConfMetaInfo* gconf_meta_info_new          (void);
void           gconf_meta_info_free         (GConfMetaInfo *gcmi);
void           gconf_meta_info_set_schema   (GConfMetaInfo *gcmi,
                                             const gchar   *schema_name);
void           gconf_meta_info_set_mod_user (GConfMetaInfo *gcmi,
                                             const gchar   *mod_user);
void           gconf_meta_info_set_mod_time (GConfMetaInfo *gcmi,
                                             GTime          mod_time);



/* Key-value pairs; used to list the contents of
 *  a directory
 */  

typedef enum
{
  GCONF_UNSET_INCLUDING_SCHEMA_NAMES = 1 << 0
} GConfUnsetFlags;

typedef struct _GConfEntry GConfEntry;

struct _GConfEntry {
  char *key;
  GConfValue *value;
};

const char* gconf_entry_get_key         (const GConfEntry *entry);
GConfValue* gconf_entry_get_value       (const GConfEntry *entry);
const char* gconf_entry_get_schema_name (const GConfEntry *entry);
gboolean    gconf_entry_get_is_default  (const GConfEntry *entry);
gboolean    gconf_entry_get_is_writable (const GConfEntry *entry);

GConfEntry* gconf_entry_new              (const gchar *key,
                                          const GConfValue  *val);
GConfEntry* gconf_entry_new_nocopy       (gchar       *key,
                                          GConfValue  *val);

GConfEntry* gconf_entry_copy             (const GConfEntry *src);
#ifndef GCONF_DISABLE_DEPRECATED
void        gconf_entry_free             (GConfEntry  *entry);
#endif
void        gconf_entry_ref   (GConfEntry *entry);
void        gconf_entry_unref (GConfEntry *entry);

/* Transfer ownership of value to the caller. */
GConfValue* gconf_entry_steal_value      (GConfEntry  *entry);
void        gconf_entry_set_value        (GConfEntry  *entry,
                                          const GConfValue  *val);
void        gconf_entry_set_value_nocopy (GConfEntry  *entry,
                                          GConfValue  *val);
void        gconf_entry_set_schema_name  (GConfEntry  *entry,
                                          const gchar *name);
void        gconf_entry_set_is_default   (GConfEntry  *entry,
                                          gboolean     is_default);
void        gconf_entry_set_is_writable  (GConfEntry  *entry,
                                          gboolean     is_writable);

gboolean    gconf_entry_equal            (const GConfEntry *a,
                                          const GConfEntry *b);

G_END_DECLS

#endif


