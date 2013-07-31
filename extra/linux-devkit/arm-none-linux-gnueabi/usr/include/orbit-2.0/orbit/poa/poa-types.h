#ifndef POA_TYPES_H
#define POA_TYPES_H 1

#include <orbit/poa/orbit-adaptor.h>

G_BEGIN_DECLS

#if defined(ORBIT2_INTERNAL_API) || defined (ORBIT2_STUBS_API)

typedef struct {
	void (*dummy_padding) (void);
	ORBit_impl_finder        impl_finder;
	const char              *class_name;
	CORBA_unsigned_long     *class_id;
	ORBit_VepvIdx           *vepvmap;
	ORBit_IInterface        *idata;
} PortableServer_ClassInfo;

#endif /* defined(ORBIT2_INTERNAL_API) || defined (ORBIT2_STUBS_API) */

#ifdef ORBIT2_INTERNAL_API

struct ORBit_POAObject_type {
	struct ORBit_OAObject_type     base;

	PortableServer_Servant         servant;
	PortableServer_POA             poa;
	PortableServer_ObjectId       *object_id;

	ORBit_VepvIdx                 *vepvmap_cache;

	guint16                        life_flags;
	guint16                        use_cnt;

	ORBit_POAObject                next;
};

#define ORBit_LifeF_NeedPostInvoke      (1<<0)
#define ORBit_LifeF_DoEtherealize       (1<<1)
#define ORBit_LifeF_IsCleanup           (1<<2)
#define ORBit_LifeF_DeactivateDo        (1<<4)
#define ORBit_LifeF_Deactivating        (1<<5)
#define ORBit_LifeF_Deactivated         (1<<6)
#define ORBit_LifeF_DestroyDo           (1<<8)
#define ORBit_LifeF_Destroying          (1<<9)
#define ORBit_LifeF_Destroyed           (1<<10)

#define ORBIT_SERVANT_TO_CLASSINFO(servant) (                          \
  (PortableServer_ClassInfo*)                                          \
  ( ((PortableServer_ServantBase *)(servant))->vepv[0]->_private )     \
)

#endif /* ORBIT2_INTERNAL_API */

#if defined(ORBIT2_INTERNAL_API) || defined (ORBIT2_STUBS_API)

void
ORBit_c_stub_invoke (CORBA_Object        obj,
		     ORBit_IMethods     *methods,
		     glong               method_index,
		     gpointer            ret,
		     gpointer            args,
		     CORBA_Context       ctx,
		     CORBA_Environment  *ev,
		     glong               class_id,
		     glong               method_offset,
		     ORBitSmallSkeleton  skel_impl);

#define ORBIT_VEPV_OFFSET(vepv_type,epv_member) \
       ((CORBA_unsigned_long) (G_STRUCT_OFFSET (vepv_type, epv_member)) / sizeof (GFunc))


/*
 * These macros are deprecated, they remain on the off-chance that
 * someone compiles a really, really old stub. Their functionality
 * is always short-circuited because ORBit_small_flags &
 * ORBIT_SMALL_FAST_LOCALS is now never true.
 */
#define ORBIT_STUB_IsBypass(obj, classid) FALSE
#define ORBIT_STUB_GetEpv(obj, clsid)     NULL
#define ORBIT_STUB_PreCall(x)
#define ORBIT_STUB_PostCall(x)

#endif /* defined(ORBIT2_INTERNAL_API) || defined (ORBIT2_STUBS_API) */

G_END_DECLS

#endif
