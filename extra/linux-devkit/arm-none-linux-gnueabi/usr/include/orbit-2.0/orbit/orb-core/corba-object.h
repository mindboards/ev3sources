#ifndef CORBA_OBJECT_H
#define CORBA_OBJECT_H 1

#include <glib-object.h>

G_BEGIN_DECLS

#ifdef ORBIT2_INTERNAL_API

GIOPConnection *ORBit_object_get_connection  (CORBA_Object    obj);
GIOPConnection *ORBit_object_peek_connection (CORBA_Object   obj);


void            ORBit_marshal_object        (GIOPSendBuffer *buf,
					     CORBA_Object    obj);
gboolean        ORBit_demarshal_object      (CORBA_Object   *obj,
					     GIOPRecvBuffer *buf,
					     CORBA_ORB       orb);

CORBA_Object    ORBit_objref_new            (CORBA_ORB       orb,
					     ORBit_OAObject  adaptor_obj,
					     GQuark          type_id);

#endif /* ORBIT2_INTERNAL_API */

/*
 * Client invocation policy API
 */
typedef struct _ORBitPolicy ORBitPolicy;

/* An extended policy - blocks re-enterancy by default */
#define ORBIT_TYPE_POLICY_EX (ORBit_policy_ex_get_type ())

GType        ORBit_policy_ex_get_type (void) G_GNUC_CONST;
ORBitPolicy *ORBit_policy_new         (GType        type,
				       const char  *first_prop,
				       ...);
ORBitPolicy *ORBit_policy_ref         (ORBitPolicy *p);
void         ORBit_policy_unref       (ORBitPolicy *p);
void         ORBit_object_set_policy  (CORBA_Object obj,
				       ORBitPolicy *p);
ORBitPolicy *ORBit_object_get_policy  (CORBA_Object obj);
void         ORBit_policy_push        (ORBitPolicy *p);
void         ORBit_policy_pop         (void);

/*
 * CORBA_Object interface type data.
 */
#include <orbit/orb-core/orbit-interface.h>

#define CORBA_OBJECT_SMALL_GET_TYPE_ID    12
#define CORBA_OBJECT_SMALL_GET_IINTERFACE 13

extern ORBit_IInterface CORBA_Object__iinterface;
extern ORBit_IMethod    CORBA_Object__imethods[];

#define CORBA_Object_IMETHODS_LEN 12

G_END_DECLS

#endif /* CORBA_OBJECT_H */
