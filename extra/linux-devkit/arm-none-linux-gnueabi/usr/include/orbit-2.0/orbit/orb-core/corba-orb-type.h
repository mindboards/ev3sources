#ifndef CORBA_ORB_TYPE
#define CORBA_ORB_TYPE 1

#include <orbit/orb-core/orb-types.h>
#include <orbit/orb-core/orbit-object.h>

G_BEGIN_DECLS

#if !defined(ORBIT_DECL_CORBA_ORB) && !defined(_CORBA_ORB_defined)
#define ORBIT_DECL_CORBA_ORB 1
#define _CORBA_ORB_defined 1
typedef struct CORBA_ORB_type *CORBA_ORB;
#endif

#if !defined(_CORBA_ORB_ObjectId_defined)
#define _CORBA_ORB_ObjectId_defined 1
   typedef CORBA_string CORBA_ORB_ObjectId;
#define CORBA_ORB_ObjectId_marshal(x,y,z) CORBA_string_marshal((x),(y),(z))
#define CORBA_ORB_ObjectId_demarshal(x,y,z,i) CORBA_string_demarshal((x),(y),(z),(i))
#if !defined(TC_IMPL_TC_CORBA_ORB_ObjectId_0)
#define TC_IMPL_TC_CORBA_ORB_ObjectId_0 'c'
#define TC_IMPL_TC_CORBA_ORB_ObjectId_1 'o'
#define TC_IMPL_TC_CORBA_ORB_ObjectId_2 'r'
#define TC_IMPL_TC_CORBA_ORB_ObjectId_3 'b'
#define TC_IMPL_TC_CORBA_ORB_ObjectId_4 'a'
#define TC_IMPL_TC_CORBA_ORB_ObjectId_5 '_'
#define TC_IMPL_TC_CORBA_ORB_ObjectId_6 'd'
#define TC_IMPL_TC_CORBA_ORB_ObjectId_7 'e'
#define TC_IMPL_TC_CORBA_ORB_ObjectId_8 'f'
#define TC_IMPL_TC_CORBA_ORB_ObjectId_9 's'
   extern ORBIT2_MAYBE_CONST struct CORBA_TypeCode_struct TC_CORBA_ORB_ObjectId_struct;
#define TC_CORBA_ORB_ObjectId ((CORBA_TypeCode)&TC_CORBA_ORB_ObjectId_struct)
#endif
#define CORBA_ORB_ObjectId__freekids CORBA_string__freekids
#endif

#if !defined(_CORBA_InterfaceDef_defined)
#define _CORBA_InterfaceDef_defined 1
#define CORBA_InterfaceDef__freekids CORBA_Object__freekids
   typedef CORBA_Object CORBA_InterfaceDef;
   extern CORBA_unsigned_long CORBA_InterfaceDef__classid;
#if !defined(TC_IMPL_TC_CORBA_InterfaceDef_0)
#define TC_IMPL_TC_CORBA_InterfaceDef_0 'c'
#define TC_IMPL_TC_CORBA_InterfaceDef_1 'o'
#define TC_IMPL_TC_CORBA_InterfaceDef_2 'r'
#define TC_IMPL_TC_CORBA_InterfaceDef_3 'b'
#define TC_IMPL_TC_CORBA_InterfaceDef_4 'a'
#define TC_IMPL_TC_CORBA_InterfaceDef_5 '_'
#define TC_IMPL_TC_CORBA_InterfaceDef_6 'd'
#define TC_IMPL_TC_CORBA_InterfaceDef_7 'e'
#define TC_IMPL_TC_CORBA_InterfaceDef_8 'f'
#define TC_IMPL_TC_CORBA_InterfaceDef_9 's'
   extern ORBIT2_MAYBE_CONST struct CORBA_TypeCode_struct TC_CORBA_InterfaceDef_struct;
#define TC_CORBA_InterfaceDef ((CORBA_TypeCode)&TC_CORBA_InterfaceDef_struct)
#endif
#endif

#ifdef ORBIT2_INTERNAL_API

struct CORBA_ORB_type {
	struct ORBit_RootObject_struct  root_object;

	GMutex                         *lock;

	GIOPVersion                     default_giop_version;

	GSList                         *servers;
	GSList                         *profiles;
	GPtrArray                      *adaptors;
	GHashTable                     *forw_binds;
	GSList                         *current_invocations;
	gpointer                        default_ctx;
	GHashTable                     *initial_refs;
	guint                           life_flags;

	/*
	 * HashTable of Object Adaptor generated refs that have 
	 * been externalised and refs that we have received.
	 */
	GHashTable		       *objrefs;
};

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
