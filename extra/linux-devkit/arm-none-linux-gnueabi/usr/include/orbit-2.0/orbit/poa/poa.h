#ifndef POA_H
#define POA_H 1

#include <orbit/poa/poa-defs.h>
#include <orbit/poa/poa-types.h>

G_BEGIN_DECLS

typedef struct {
	void *_private;
} POA_PortableServer_ServantManager__epv;

typedef struct {
	void *_private;
      
	PortableServer_Servant(*incarnate) (PortableServer_Servant _servant,
					    const PortableServer_ObjectId *
					    oid,
					    const PortableServer_POA adapter,
					    CORBA_Environment * ev);
	void (*etherealize) (PortableServer_Servant _servant,
			     const PortableServer_ObjectId * oid,
			     const PortableServer_POA adapter,
			     const PortableServer_Servant serv,
			     const CORBA_boolean cleanup_in_progress,
			     const CORBA_boolean remaining_activations,
			     CORBA_Environment * ev);
} POA_PortableServer_ServantActivator__epv;
typedef struct {
	PortableServer_ServantBase__epv *_base_epv;
	POA_PortableServer_ServantManager__epv
		*PortableServer_ServantManager_epv;
	POA_PortableServer_ServantActivator__epv
		*PortableServer_ServantActivator_epv;
} POA_PortableServer_ServantActivator__vepv;

typedef struct {
	void *_private;
	POA_PortableServer_ServantActivator__vepv *vepv;
} POA_PortableServer_ServantActivator;

typedef struct {
	void *_private;
      
	PortableServer_Servant(*preinvoke) (PortableServer_Servant _servant,
					    const PortableServer_ObjectId *
					    oid,
					    const PortableServer_POA adapter,
					    const CORBA_Identifier operation,
					    PortableServer_ServantLocator_Cookie
					    * the_cookie,
					    CORBA_Environment * ev);
	void (*postinvoke) (PortableServer_Servant _servant,
			    const PortableServer_ObjectId * oid,
			    const PortableServer_POA adapter,
			    const CORBA_Identifier operation,
			    const PortableServer_ServantLocator_Cookie
			    the_cookie,
			    const PortableServer_Servant the_servant,
			    CORBA_Environment * ev);
} POA_PortableServer_ServantLocator__epv;
typedef struct {
	PortableServer_ServantBase__epv *_base_epv;
	POA_PortableServer_ServantManager__epv
		*PortableServer_ServantManager_epv;
	POA_PortableServer_ServantLocator__epv
		*PortableServer_ServantLocator_epv;
} POA_PortableServer_ServantLocator__vepv;
typedef struct {
	void *_private;
	POA_PortableServer_ServantLocator__vepv *vepv;
} POA_PortableServer_ServantLocator;

#include <orbit/poa/portableserver-poa-type.h>
#include <orbit/poa/portableserver-current-type.h>

void PortableServer_ServantBase__init          (PortableServer_Servant  servant,
						CORBA_Environment      *ev);
void PortableServer_ServantBase__fini          (PortableServer_Servant  servant,
						CORBA_Environment      *ev);

void PortableServer_RefCountServantBase__init  (PortableServer_Servant  servant,
						CORBA_Environment      *ev);
void PortableServer_RefCountServantBase__fini  (PortableServer_Servant  servant,
						CORBA_Environment      *ev);

void PortableServer_ServantBase__add_ref       (PortableServer_Servant  servant,
						CORBA_Environment      *ev);
void PortableServer_ServantBase__remove_ref    (PortableServer_Servant  servant,
						CORBA_Environment      *ev);

void PortableServer_RefCountServantBase__add_ref    (PortableServer_Servant  servant,
						     CORBA_Environment      *ev);
void PortableServer_RefCountServantBase__remove_ref (PortableServer_Servant  servant,
						     CORBA_Environment      *ev);

PortableServer_POA 
     PortableServer_ServantBase__default_POA   (PortableServer_Servant  servant,
						CORBA_Environment      *ev);
CORBA_InterfaceDef 
     PortableServer_ServantBase__get_interface (PortableServer_Servant  servant,
						CORBA_Environment      *ev);
CORBA_boolean
     PortableServer_ServantBase__is_a          (PortableServer_Servant  servant,
						const CORBA_char       *logical_type_id,
						CORBA_Environment      *ev);

#define PortableServer_RefCount_ServantBase__default_POA		\
			PortableServer_ServantBase__default_POA
#define PortableServer_RefCount_ServantBase__get_interface		\
			PortableServer_ServantBase__get_interface
#define PortableServer_RefCount_ServantBase__is_a 			\
			PortableServer_ServantBase__is_a

PortableServer_ObjectId *PortableServer_string_to_ObjectId(CORBA_char *str,
                                                           CORBA_Environment *nv);

PortableServer_ObjectId *PortableServer_wstring_to_ObjectId(CORBA_wchar *str,
                                                            CORBA_Environment *ev);

CORBA_char *PortableServer_ObjectId_to_string(PortableServer_ObjectId *oid,
                                              CORBA_Environment *ev);

CORBA_wchar *PortableServer_ObjectId_to_wstring(PortableServer_ObjectId *oid,
                                                CORBA_Environment *ev);


PortableServer_POA ORBit_POA_new_from (CORBA_ORB                  orb,
				       PortableServer_POA         parent,
				       const CORBA_char          *adaptor_name,
				       const CORBA_PolicyList    *opt_policies,
				       CORBA_Environment         *ev);

#if defined(ORBIT2_INTERNAL_API) || defined (ORBIT2_STUBS_API)

void ORBit_skel_class_register (PortableServer_ClassInfo   *ci,
				PortableServer_ServantBase *servant,
				void                      (*opt_finalize) (PortableServer_Servant,
									   CORBA_Environment *),
				CORBA_unsigned_long         class_offset,
				CORBA_unsigned_long         first_parent_id,
				...);

/* DEPRECATED */
void ORBit_classinfo_register  (PortableServer_ClassInfo   *ci);
#define ORBIT_SERVANT_SET_CLASSINFO(servant,ci) G_STMT_START {	       \
  ((PortableServer_ServantBase *)(servant))->vepv[0]->_private = (ci); \
} G_STMT_END

#endif /* defined(ORBIT2_INTERNAL_API) || defined (ORBIT2_STUBS_API) */

#ifdef ORBIT2_INTERNAL_API

PortableServer_ClassInfo *ORBit_classinfo_lookup                 (const char *type_id);
void                      ORBit_POAObject_post_invoke            (ORBit_POAObject obj);
gboolean                  ORBit_poa_allow_cross_thread_call      (ORBit_POAObject   pobj,
								  ORBit_IMethodFlag method_flags);
void                      ORBit_recv_buffer_return_sys_exception (GIOPRecvBuffer    *buf,
								  CORBA_Environment *ev);
void                      ORBit_poa_init (void);

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
