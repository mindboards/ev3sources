#ifndef CORBA_POLICY_TYPE_H
#define CORBA_POLICY_TYPE_H 1

#include <glib.h>

G_BEGIN_DECLS

#ifdef ORBIT2_INTERNAL_API

struct CORBA_Policy_type {
	struct ORBit_RootObject_struct parent;

	CORBA_unsigned_long            type;
	CORBA_unsigned_long            value; 
};

CORBA_Policy        ORBit_Policy_new (CORBA_unsigned_long type,
				      CORBA_unsigned_long value);

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
