#ifndef __ORBIT_ADAPTOR_H__
#define __ORBIT_ADAPTOR_H__

#include <glib.h>
#include <stdarg.h>

G_BEGIN_DECLS

typedef enum {
	ORBIT_THREAD_HINT_NONE = 0,
	ORBIT_THREAD_HINT_PER_OBJECT,
	ORBIT_THREAD_HINT_PER_REQUEST,
	ORBIT_THREAD_HINT_PER_POA,
	ORBIT_THREAD_HINT_PER_CONNECTION,
	ORBIT_THREAD_HINT_ONEWAY_AT_IDLE,
	ORBIT_THREAD_HINT_ALL_AT_IDLE,
	ORBIT_THREAD_HINT_ON_CONTEXT
} ORBitThreadHint;

typedef struct ORBit_ObjectAdaptor_type *ORBit_ObjectAdaptor;

void            ORBit_ObjectAdaptor_set_thread_hint  (ORBit_ObjectAdaptor adaptor,
						      ORBitThreadHint     thread_hint,
						      ...);
void            ORBit_ObjectAdaptor_set_thread_hintv (ORBit_ObjectAdaptor adaptor,
						      ORBitThreadHint     thread_hint,
						      va_list             args);
ORBitThreadHint ORBit_ObjectAdaptor_get_thread_hint  (ORBit_ObjectAdaptor adaptor);

void            ORBit_ObjectAdaptor_object_bind_to_current_thread (CORBA_Object obj);


#ifdef ORBIT2_INTERNAL_API

void                ORBit_handle_locate_request     (CORBA_ORB          orb, 
						     GIOPRecvBuffer    *recv_buffer);

void                ORBit_handle_request            (CORBA_ORB          orb, 
						     GIOPRecvBuffer    *recv_buffer);

void                ORBit_small_handle_request      (ORBit_OAObject     adaptor_obj,
						     CORBA_Identifier   opname,
						     gpointer           ret,
						     gpointer          *args, 
						     CORBA_Context      ctx,
						     GIOPRecvBuffer    *recv_buffer,
						     CORBA_Environment *ev);

gboolean            ORBit_OAObject_is_active        (ORBit_OAObject     adaptor_obj);

ORBit_ObjectKey    *ORBit_OAObject_object_to_objkey (ORBit_OAObject     adaptor_obj);

void                ORBit_OAObject_invoke           (ORBit_OAObject     adaptor_obj,
						     gpointer           ret,
						     gpointer          *args,
						     CORBA_Context      ctx,
						     gpointer           data,
						     CORBA_Environment *ev);
/*
 * ORBit_OAObject
 */

typedef gboolean            (*ORBitStateCheckFunc) (ORBit_OAObject     adaptor_obj);

typedef ORBit_ObjectKey    *(*ORBitKeyGenFunc)     (ORBit_OAObject     adaptor_obj);

typedef void                (*ORBitInvokeFunc)     (ORBit_OAObject     adaptor_obj,
						    gpointer           ret,
						    gpointer          *args,
						    CORBA_Context      ctx,
						    gpointer           data, 
						    CORBA_Environment *ev);

typedef void                (*ORBitReqFunc)        (ORBit_OAObject     adaptor_obj,
						    CORBA_Identifier   opname,
						    gpointer           ret,
						    gpointer          *args,
						    CORBA_Context      ctx,
						    GIOPRecvBuffer    *recv_buffer,
						    CORBA_Environment *ev);

typedef enum {
	ORBIT_ADAPTOR_POA        = 1 << 0
} ORBit_Adaptor_type;

struct ORBit_OAObject_Interface_type {
	ORBit_Adaptor_type  adaptor_type;

	ORBitStateCheckFunc is_active;
	ORBitKeyGenFunc     object_to_objkey;
	ORBitInvokeFunc     invoke;
	ORBitReqFunc        handle_request;
};

typedef struct ORBit_OAObject_Interface_type *ORBit_OAObject_Interface;

struct ORBit_OAObject_type {
	struct ORBit_RootObject_struct parent;

	CORBA_Object                   objref;

	ORBit_OAObject_Interface       interface;
};

/*
 * ORBit_ObjectAdaptor
 */

typedef CORBA_sequence_CORBA_octet ORBit_AdaptorKey;

typedef void (*ORBitReqHandlerFunc) (ORBit_ObjectAdaptor         adaptor,
				     GIOPRecvBuffer             *recv_buffer,
				     ORBit_ObjectKey            *objkey);

struct ORBit_ObjectAdaptor_type {
	struct ORBit_RootObject_struct parent;

	GMutex                        *lock;

	ORBitReqHandlerFunc            handle_request;

	ORBit_AdaptorKey               adaptor_key;

	ORBitThreadHint                thread_hint;

	GMainContext                  *context;
};

int ORBit_adaptor_setup (ORBit_ObjectAdaptor adaptor, CORBA_ORB orb);

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif /* __ORBIT_ADAPTOR_H__ */
