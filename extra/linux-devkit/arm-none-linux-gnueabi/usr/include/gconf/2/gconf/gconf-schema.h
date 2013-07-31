
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

#ifndef GCONF_GCONF_SCHEMA_H
#define GCONF_GCONF_SCHEMA_H

#include <glib.h>

#include "gconf/gconf-value.h"

G_BEGIN_DECLS

/*
 *  A "schema" is a value that describes a key-value pair.
 *  It might include the type of the pair, documentation describing 
 *  the pair, the name of the application creating the pair, 
 *  etc.
 */

GConfSchema* gconf_schema_new  (void);
void         gconf_schema_free (GConfSchema *sc);
GConfSchema* gconf_schema_copy (const GConfSchema *sc);

void gconf_schema_set_type                 (GConfSchema    *sc,
                                            GConfValueType  type);
void gconf_schema_set_list_type            (GConfSchema    *sc,
                                            GConfValueType  type);
void gconf_schema_set_car_type             (GConfSchema    *sc,
                                            GConfValueType  type);
void gconf_schema_set_cdr_type             (GConfSchema    *sc,
                                            GConfValueType  type);
void gconf_schema_set_locale               (GConfSchema    *sc,
                                            const gchar    *locale);
void gconf_schema_set_short_desc           (GConfSchema    *sc,
                                            const gchar    *desc);
void gconf_schema_set_long_desc            (GConfSchema    *sc,
                                            const gchar    *desc);
void gconf_schema_set_owner                (GConfSchema    *sc,
                                            const gchar    *owner);
void gconf_schema_set_default_value        (GConfSchema    *sc,
                                            const GConfValue     *val);
void gconf_schema_set_default_value_nocopy (GConfSchema    *sc,
                                            GConfValue     *val);


GConfValueType gconf_schema_get_type          (const GConfSchema *schema);
GConfValueType gconf_schema_get_list_type     (const GConfSchema *schema);
GConfValueType gconf_schema_get_car_type      (const GConfSchema *schema);
GConfValueType gconf_schema_get_cdr_type      (const GConfSchema *schema);
const char*    gconf_schema_get_locale        (const GConfSchema *schema);
const char*    gconf_schema_get_short_desc    (const GConfSchema *schema);
const char*    gconf_schema_get_long_desc     (const GConfSchema *schema);
const char*    gconf_schema_get_owner         (const GConfSchema *schema);
GConfValue*    gconf_schema_get_default_value (const GConfSchema *schema);


G_END_DECLS

#endif


