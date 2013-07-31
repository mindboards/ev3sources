#ifndef CORBA_ANY_H
#define CORBA_ANY_H 1

#include <glib.h>

G_BEGIN_DECLS

void       CORBA_any__copy     (CORBA_any       *out,
				const CORBA_any *in);
CORBA_any *CORBA_any__alloc    (void);

#define    CORBA_any_alloc     CORBA_any__alloc

gpointer   CORBA_any__freekids (gpointer mem,
				gpointer data);

#define CORBA_any_set_release(a, r) (a)->_release = r
#define CORBA_any_get_release(a)    (a)->_release

gpointer ORBit_copy_value       (gconstpointer  value,
				 CORBA_TypeCode tc);

CORBA_boolean
	 ORBit_any_equivalent   (CORBA_any         *obj,
				 CORBA_any         *any,
				 CORBA_Environment *ev);

#ifdef ORBIT2_INTERNAL_API

void     ORBit_marshal_arg      (GIOPSendBuffer *buf,
				 gconstpointer   val,
				 CORBA_TypeCode  tc);

void     ORBit_marshal_any      (GIOPSendBuffer  *buf,
				 const CORBA_any *val);

gpointer ORBit_demarshal_arg    (GIOPRecvBuffer *buf,
				 CORBA_TypeCode  tc,
				 CORBA_ORB       orb);

gboolean ORBit_demarshal_any    (GIOPRecvBuffer *buf,
				 CORBA_any      *retval,
				 CORBA_ORB       orb);

gboolean ORBit_demarshal_value  (CORBA_TypeCode  tc,
				 gpointer       *val,
				 GIOPRecvBuffer *buf,
				 CORBA_ORB       orb);

void     ORBit_marshal_value    (GIOPSendBuffer *buf,
				 gconstpointer  *val,
				 CORBA_TypeCode  tc);

CORBA_boolean
	 ORBit_value_equivalent (gpointer          *a,
				 gpointer          *b,
				 CORBA_TypeCode     tc,
				 CORBA_Environment *ev);

size_t  ORBit_gather_alloc_info (CORBA_TypeCode tc);

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
