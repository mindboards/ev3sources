#ifndef CORBA_OBJECT_TYPE_H
#define CORBA_OBJECT_TYPE_H 1

#include <glib.h>

G_BEGIN_DECLS

#if defined(ORBIT2_INTERNAL_API)

/* This ABI is now purely internal and can be hacked around */

typedef CORBA_sequence_CORBA_octet ORBit_ObjectKey;

struct CORBA_Object_type {
	struct ORBit_RootObject_struct parent;

	GIOPConnection                *connection;        /* l */
	GQuark                         type_qid;
	GSList                        *profile_list;      /* l */
	GSList                        *forward_locations; /* l */
	ORBit_ObjectKey               *object_key;        /* l */
	struct _ORBitPolicy           *invoke_policy;     /* l */

	CORBA_ORB                      orb;
	ORBit_OAObject                 adaptor_obj;
};

#endif /* defined(ORBIT2_INTERNAL_API) */

G_END_DECLS

#endif
