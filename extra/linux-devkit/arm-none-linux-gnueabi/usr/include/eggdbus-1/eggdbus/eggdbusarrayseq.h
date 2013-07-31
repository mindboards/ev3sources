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

#ifndef __EGG_DBUS_ARRAY_SEQ_H
#define __EGG_DBUS_ARRAY_SEQ_H

#include <eggdbus/eggdbustypes.h>

G_BEGIN_DECLS

#define EGG_DBUS_TYPE_ARRAY_SEQ         (egg_dbus_array_seq_get_type())
#define EGG_DBUS_ARRAY_SEQ(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), EGG_DBUS_TYPE_ARRAY_SEQ, EggDBusArraySeq))
#define EGG_DBUS_ARRAY_SEQ_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), EGG_DBUS_TYPE_ARRAY_SEQ, EggDBusArraySeqClass))
#define EGG_DBUS_ARRAY_SEQ_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), EGG_DBUS_TYPE_ARRAY_SEQ, EggDBusArraySeqClass))
#define EGG_DBUS_IS_ARRAY_SEQ(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), EGG_DBUS_TYPE_ARRAY_SEQ))
#define EGG_DBUS_IS_ARRAY_SEQ_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), EGG_DBUS_TYPE_ARRAY_SEQ))

typedef struct _EggDBusArraySeqClass EggDBusArraySeqClass;

/**
 * EggDBusArraySeq:
 * @size: Number of elements in the array.
 * @element_type: The #GType of the elements in the array.
 * @element_size: The size, in bytes, of each element.
 * @data: Access data as a single memory block.
 * @ptr: Access data as an array of pointers. For use with boxed and #GObject derived types.
 * @v_byte: Access data as an array of bytes. Only use when @element_type is %G_TYPE_UCHAR.
 * @v_int16: Access data as an array of 16bit signed integers. Only use when @element_type is %EGG_DBUS_TYPE_INT16.
 * @v_uint16: Access data as an array of 16bit unsigned integers. Only use when @element_type is %EGG_DBUS_TYPE_UINT16.
 * @v_int: Access data as an array of signed integers. Only use when @element_type is %G_TYPE_INT.
 * @v_uint: Access data as an array of signed integers. Only use when @element_type is %G_TYPE_UINT.
 * @v_int64: Access data as an array of 64bit signed integers. Only use when @element_type is %G_TYPE_INT64.
 * @v_uint64: Access data as an array of 64bit unsigned integers. Only use when @element_type is %G_TYPE_UINT64.
 * @v_long: Access data as an array of signed longs. Only use when @element_type is %G_TYPE_LONG.
 * @v_ulong: Access data as an array of unsigned longs. Only use when @element_type is %G_TYPE_ULONG.
 * @v_boolean: Access data as an array of truth values. Only use when @element_type is %G_TYPE_BOOLEAN.
 * @v_float: Access data as an array of single precision floating point numbers. Only use when @element_type is %G_TYPE_FLOAT.
 * @v_double: Access data as an array of double precision floating point numbers. Only use when @element_type is %G_TYPE_DOUBLE.
 * @v_str: Access data as an array of strings. Only use when @element_type is %G_TYPE_STRING.
 * @v_strv: Access data as an array of string arrays. Only use when @element_type is a %G_TYPE_STRV.
 * @data: C union for accessing the data of the array in various ways.
 *
 * The #EggDBusArraySeq instance structure should normally not be accessed directly, use accessor
 * functions instead. Certain instance members are provided <emphasis>only</emphasis> for efficient
 * and convenient access when using the C programming language. Use with caution.
 */
struct _EggDBusArraySeq
{
  GObject               parent_instance;

  /*< public >*/
  guint                 size;
  GType                 element_type;
  gsize                 element_size;
  union
  {
    gpointer            data;
    gpointer           *v_ptr;
    guchar             *v_byte;
    gint16             *v_int16;
    guint16            *v_uint16;
    gint               *v_int;
    guint              *v_uint;
    gint64             *v_int64;
    guint64            *v_uint64;
    glong              *v_long;
    gulong             *v_ulong;
    gboolean           *v_boolean;
    gfloat             *v_float;
    gdouble            *v_double;
    gchar             **v_str;
    gchar            ***v_strv;
  } data;
};

struct _EggDBusArraySeqClass
{
  GObjectClass parent_class;

  /*< private >*/
  /* Padding for future expansion */
  void (*_g_reserved0) (void);
  void (*_g_reserved1) (void);
  void (*_g_reserved2) (void);
  void (*_g_reserved3) (void);
  void (*_g_reserved4) (void);
  void (*_g_reserved5) (void);
  void (*_g_reserved6) (void);
  void (*_g_reserved7) (void);
};


GType                  egg_dbus_array_seq_get_type          (void) G_GNUC_CONST;

EggDBusArraySeq        *egg_dbus_array_seq_new              (GType                 element_type,
                                                             GDestroyNotify        free_func,
                                                             GBoxedCopyFunc        copy_func,
                                                             GEqualFunc            equal_func) G_GNUC_WARN_UNUSED_RESULT;
gsize                   egg_dbus_array_seq_get_element_size (EggDBusArraySeq      *array_seq);

GType                   egg_dbus_array_seq_get_element_type (EggDBusArraySeq      *array_seq) G_GNUC_WARN_UNUSED_RESULT;

GEqualFunc              egg_dbus_array_seq_get_equal_func   (EggDBusArraySeq      *array_seq) G_GNUC_WARN_UNUSED_RESULT;

gboolean                egg_dbus_array_seq_have_copy_func   (EggDBusArraySeq      *array_seq) G_GNUC_WARN_UNUSED_RESULT;

guint                   egg_dbus_array_seq_get_size         (EggDBusArraySeq      *array_seq) G_GNUC_WARN_UNUSED_RESULT;

void                    egg_dbus_array_seq_set_size         (EggDBusArraySeq      *array_seq,
                                                             guint                 size);

void                    egg_dbus_array_seq_clear            (EggDBusArraySeq      *array_seq);

gpointer                egg_dbus_array_seq_get              (EggDBusArraySeq      *array_seq,
                                                             gint                  index) G_GNUC_WARN_UNUSED_RESULT;

gpointer                egg_dbus_array_seq_get_copy         (EggDBusArraySeq      *array_seq,
                                                             gint                  index) G_GNUC_WARN_UNUSED_RESULT;

void                    egg_dbus_array_seq_set              (EggDBusArraySeq      *array_seq,
                                                             gint                  index,
                                                             gconstpointer         value);

void                    egg_dbus_array_seq_insert           (EggDBusArraySeq      *array_seq,
                                                             gint                  index,
                                                             gconstpointer         value);

gboolean                egg_dbus_array_seq_add              (EggDBusArraySeq      *array_seq,
                                                             gconstpointer         value);

void                    egg_dbus_array_seq_remove_at        (EggDBusArraySeq      *array_seq,
                                                             gint                  index);

void                    egg_dbus_array_seq_remove_range_at  (EggDBusArraySeq      *array_seq,
                                                             gint                  index,
                                                             guint                 size);

gint                    egg_dbus_array_seq_index_of         (EggDBusArraySeq      *array_seq,
                                                             gconstpointer         value);

gboolean                egg_dbus_array_seq_contains         (EggDBusArraySeq      *array_seq,
                                                             gconstpointer         value);

gboolean                egg_dbus_array_seq_remove           (EggDBusArraySeq      *array_seq,
                                                             gconstpointer         value);

gboolean                egg_dbus_array_seq_add_all          (EggDBusArraySeq      *array_seq,
                                                             EggDBusArraySeq      *other_array_seq);

gboolean                egg_dbus_array_seq_steal_all        (EggDBusArraySeq      *array_seq,
                                                             EggDBusArraySeq      *other_array_seq);


/* C convenience functions */

gboolean                egg_dbus_array_seq_add_fixed        (EggDBusArraySeq      *array_seq,
                                                             guint64               value);
gboolean                egg_dbus_array_seq_add_float        (EggDBusArraySeq      *array_seq,
                                                             gdouble               value);

void                    egg_dbus_array_seq_set_fixed        (EggDBusArraySeq      *array_seq,
                                                             gint                  index,
                                                             guint64               value);
void                    egg_dbus_array_seq_set_float        (EggDBusArraySeq      *array_seq,
                                                             gint                  index,
                                                             gdouble               value);

void                    egg_dbus_array_seq_insert_fixed     (EggDBusArraySeq      *array_seq,
                                                             gint                  index,
                                                             guint64               value);
void                    egg_dbus_array_seq_insert_float     (EggDBusArraySeq      *array_seq,
                                                             gint                  index,
                                                             gdouble               value);

guint64                 egg_dbus_array_seq_get_fixed        (EggDBusArraySeq      *array_seq,
                                                             gint                  index) G_GNUC_WARN_UNUSED_RESULT;
gdouble                 egg_dbus_array_seq_get_float        (EggDBusArraySeq      *array_seq,
                                                             gint                  index) G_GNUC_WARN_UNUSED_RESULT;

G_END_DECLS

#endif /* __EGG_DBUS_ARRAY_SEQ_H */
