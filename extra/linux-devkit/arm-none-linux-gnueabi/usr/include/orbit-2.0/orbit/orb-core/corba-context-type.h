#ifndef CORBA_CONTEXT_TYPE_H
#define CORBA_CONTEXT_TYPE_H 1

#include <glib.h>

G_BEGIN_DECLS

#ifdef ORBIT2_INTERNAL_API

typedef struct {
	CORBA_unsigned_long  len;
	const CORBA_char    *str;
} ORBit_ContextMarshalItem;

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
