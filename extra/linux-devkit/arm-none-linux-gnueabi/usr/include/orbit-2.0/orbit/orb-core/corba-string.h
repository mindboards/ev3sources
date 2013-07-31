#ifndef CORBA_STRING_H
#define CORBA_STRING_H 1

#include <glib.h>

G_BEGIN_DECLS

CORBA_char  *CORBA_string_alloc  (CORBA_unsigned_long len);
CORBA_wchar *CORBA_wstring_alloc (CORBA_unsigned_long len);

/*
 * ORBit extension.
 */
CORBA_char          *CORBA_string_dup  (const CORBA_char *str);

CORBA_unsigned_long  CORBA_wstring_len (CORBA_wchar *ws);

#ifdef ORBIT2_INTERNAL_API

gpointer CORBA_string__freekids         (gpointer mem,
					 gpointer data);

CORBA_sequence_CORBA_octet *
         ORBit_sequence_CORBA_octet_dup (const CORBA_sequence_CORBA_octet *seq);

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
