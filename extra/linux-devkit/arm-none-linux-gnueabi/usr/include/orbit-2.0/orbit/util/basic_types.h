#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H 1

#include <glib.h>

G_BEGIN_DECLS

enum {
	CORBA_FALSE = FALSE,
	CORBA_TRUE  = TRUE
};

typedef gint16    CORBA_short;
typedef gint32    CORBA_long;
typedef guint16   CORBA_unsigned_short;
typedef guint32   CORBA_unsigned_long;
typedef gfloat    CORBA_float;
typedef gdouble   CORBA_double;
typedef char      CORBA_char;
typedef gunichar2 CORBA_wchar;
typedef guchar    CORBA_boolean;
typedef guchar    CORBA_octet;
typedef gdouble   CORBA_long_double;

/*
 * Bad hack, oh well
 */
typedef CORBA_char  *CORBA_string;
typedef CORBA_wchar *CORBA_wstring;

#if !defined(ORBIT_DECL_CORBA_Object) && !defined(_CORBA_Object_defined)
#define ORBIT_DECL_CORBA_Object 1
#define _CORBA_Object_defined 1
typedef struct CORBA_Object_type *CORBA_Object;
#endif

#ifdef G_HAVE_GINT64
#define HAVE_CORBA_LONG_LONG

/*
 * According to the spec, these two are optional.
 * We support them if we can.
 */
typedef gint64  CORBA_long_long;
typedef guint64 CORBA_unsigned_long_long;

#else
#error ""
#error "You don't G_HAVE_GINT64 defined in glib."
#error "Please make sure you don't have an old glibconfig.h lying around."
#error ""
#endif

G_END_DECLS

#endif
