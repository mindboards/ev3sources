#ifndef ORBIT_OBJECT_H
#define ORBIT_OBJECT_H 1

#include <glib.h>

#ifdef G_PLATFORM_WIN32
#define ORBIT2_MAYBE_CONST
#else
#define ORBIT2_MAYBE_CONST const
#endif

G_BEGIN_DECLS

typedef enum {
	ORBIT_ROT_NULL,
	ORBIT_ROT_OBJREF,
	/* values below here all signify psuedo-objects */
	ORBIT_ROT_ORB,
	ORBIT_ROT_ADAPTOR,
	ORBIT_ROT_POAMANAGER,
	ORBIT_ROT_POLICY,
	ORBIT_ROT_TYPECODE,
	ORBIT_ROT_REQUEST,
	ORBIT_ROT_SERVERREQUEST,
	ORBIT_ROT_CONTEXT,
	ORBIT_ROT_DYNANY,
	ORBIT_ROT_OAOBJECT,
	ORBIT_ROT_ORBGROUP,
	ORBIT_ROT_POACURRENT,
	ORBIT_ROT_CLIENT_POLICY
} ORBit_RootObject_Type;

typedef struct ORBit_RootObject_struct *ORBit_RootObject;

typedef void (* ORBit_RootObject_DestroyFunc) (ORBit_RootObject obj);

typedef struct _ORBit_RootObject_Interface {
	ORBit_RootObject_Type        type;
	ORBit_RootObject_DestroyFunc destroy;
} ORBit_RootObject_Interface;

struct ORBit_RootObject_struct {
	const ORBit_RootObject_Interface *interface;
  int refs;
};

#define ORBIT_REFCOUNT_STATIC -10

void     ORBit_RootObject_init      (ORBit_RootObject obj,
				     const ORBit_RootObject_Interface *interface);

#ifdef ORBIT2_INTERNAL_API

/* Used to determine whether the refcount is valid or not */
#define ORBIT_REFCOUNT_MAX (1<<20)

#define ORBIT_ROOT_OBJECT(obj)      ((ORBit_RootObject)(obj))
#define ORBIT_ROOT_OBJECT_TYPE(obj) (((ORBit_RootObject)(obj))->interface->type)


gpointer ORBit_RootObject_duplicate   (gpointer obj);
gpointer ORBit_RootObject_duplicate_T (gpointer obj);
void     ORBit_RootObject_release     (gpointer obj);
void     ORBit_RootObject_release_T   (gpointer obj);

extern GMutex *ORBit_RootObject_lifecycle_lock;

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
