/*
 * An attempt to shrink the beast to a managable size.
 */
#ifndef CORBA_SMALL_H
#define CORBA_SMALL_H 1

#include <glib-object.h>
#include <orbit/orb-core/orbit-interface.h>

G_BEGIN_DECLS

typedef struct {
	CORBA_unsigned_long           version;
	ORBit_IInterface            **interfaces;
	CORBA_sequence_CORBA_TypeCode types;
} ORBit_IModule;

const char    *ORBit_get_safe_tmp      (void);

/* Builtin allocators */
gpointer       ORBit_small_alloc       (CORBA_TypeCode      tc);
gpointer       ORBit_small_allocbuf    (CORBA_TypeCode      tc,
					CORBA_unsigned_long length);
void           ORBit_small_freekids    (CORBA_TypeCode      tc,
					gpointer            p,
					gpointer            d);

/* More friendly(?) sequence allocators */
gpointer       ORBit_sequence_alloc    (CORBA_TypeCode      sequence_tc,
					CORBA_unsigned_long length);
void           ORBit_sequence_append   (gpointer            sequence,
					gconstpointer       element);
void           ORBit_sequence_set_size (gpointer            sequence,
					CORBA_unsigned_long length);
#define        ORBit_sequence_index(sequence,idx) (sequence)->_buffer[(idx)]
void           ORBit_sequence_concat   (gpointer            sequence,
					gconstpointer       append);
void           ORBit_sequence_remove   (gpointer            sequence,
                                        guint               idx);

typedef enum {
	ORBIT_CONNECTION_CONNECTED,
	ORBIT_CONNECTION_CONNECTING,
	ORBIT_CONNECTION_DISCONNECTED,
	ORBIT_CONNECTION_IN_PROC
} ORBitConnectionStatus;

gpointer              ORBit_small_get_servant              (CORBA_Object obj);
ORBitConnectionStatus ORBit_small_get_connection_status    (CORBA_Object obj);
ORBitConnectionStatus ORBit_small_listen_for_broken        (CORBA_Object obj,
							    GCallback    fn,
							    gpointer     user_data);
ORBitConnectionStatus ORBit_small_unlisten_for_broken_full (CORBA_Object obj,
							    GCallback    fn,
							    gpointer     user_data);
/* old / stale */
ORBitConnectionStatus ORBit_small_unlisten_for_broken      (CORBA_Object obj,
							    GCallback    fn);

typedef struct _ORBitConnection ORBitConnection;

ORBitConnection      *ORBit_small_get_connection        (CORBA_Object     obj);
ORBitConnection      *ORBit_small_get_connection_ref    (CORBA_Object     obj);
void                  ORBit_small_connection_unref      (ORBitConnection *cnx);
void                  ORBit_connection_set_max_buffer   (ORBitConnection *cnx,
							 gulong           max_buffer_bytes);

#if defined(ORBIT2_INTERNAL_API) || defined (ORBIT2_STUBS_API)

#define ORBIT_SMALL_FAST_LOCALS           1
#define ORBIT_SMALL_FORCE_GENERIC_MARSHAL 2

extern int     ORBit_small_flags;

/* Deprecated - only for bin-compat with pre 2.4 stubs */
void           ORBit_small_invoke_stub (CORBA_Object        object,
					ORBit_IMethod      *m_data,
					gpointer            ret,
					gpointer           *args,
					CORBA_Context       ctx,
					CORBA_Environment  *ev);


void           ORBit_small_invoke_stub_n (CORBA_Object        object,
					  ORBit_IMethods     *methods,
					  glong               index,
					  gpointer            ret,
					  gpointer           *args,
					  CORBA_Context       ctx,
					  CORBA_Environment  *ev);

#endif /* defined(ORBIT2_INTERNAL_API) || defined (ORBIT2_STUBS_API) */

#ifdef ORBIT2_INTERNAL_API

#include <orbit/GIOP/giop.h>

void           ORBit_small_invoke_adaptor (ORBit_OAObject     adaptor_obj,
					   GIOPRecvBuffer    *recv_buffer,
					   ORBit_IMethod     *m_data,
					   gpointer           data,
					   CORBA_Environment *ev);

#endif /* ORBIT2_INTERNAL_API */

/* Type library work */
CORBA_char       *ORBit_small_get_type_id         (CORBA_Object       object,
						   CORBA_Environment *ev);
ORBit_IInterface *ORBit_small_get_iinterface      (CORBA_Object       opt_object,
					           const CORBA_char  *repo_id,
						   CORBA_Environment *ev);
gboolean          ORBit_small_load_typelib        (const char        *libname);
CORBA_sequence_CORBA_TypeCode *
                  ORBit_small_get_types           (const char        *name);
CORBA_sequence_ORBit_IInterface *
                  ORBit_small_get_iinterfaces     (const char        *name);

typedef struct _ORBitAsyncQueueEntry ORBitAsyncQueueEntry;

typedef void (*ORBitAsyncInvokeFunc) (CORBA_Object          object,
				      ORBit_IMethod        *m_data,
				      ORBitAsyncQueueEntry *aqe,
				      gpointer              user_data, 
				      CORBA_Environment    *ev);

/* Various bits for Async work */
void ORBit_small_invoke_async        (CORBA_Object           obj,
				      ORBit_IMethod         *m_data,
				      ORBitAsyncInvokeFunc   fn,
				      gpointer               user_data,
				      gpointer              *args,
				      CORBA_Context          ctx,
				      CORBA_Environment     *ev);

void ORBit_small_demarshal_async     (ORBitAsyncQueueEntry  *aqe,
				      gpointer               ret,
				      gpointer              *args,
				      CORBA_Environment     *ev);

G_END_DECLS

#endif /* CORBA_SMALL_H */
