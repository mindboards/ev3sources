#ifndef PORTABLESERVER_POA_TYPE_H
#define PORTABLESERVER_POA_TYPE_H 1

#include <orbit/poa/orbit-adaptor.h>

G_BEGIN_DECLS

#ifdef ORBIT2_INTERNAL_API

struct PortableServer_POAManager_type {
	struct ORBit_RootObject_struct    parent;

	GSList                           *poa_collection;
	PortableServer_POAManager_State   state;
	CORBA_ORB                         orb;
};

struct PortableServer_POA_type {
	struct ORBit_ObjectAdaptor_type base;

	guint16                         use_cnt;
	guint16                         life_flags;
	int                             poa_id;
	int                             next_sysid;
	char                           *name;
	CORBA_ORB                       orb;
	PortableServer_POA              parent_poa;
	PortableServer_POAManager       poa_manager;
	PortableServer_AdapterActivator the_activator;
	PortableServer_ServantManager   servant_manager;
	PortableServer_Servant          default_servant;

	GHashTable                     *oid_to_obj_map;
	CORBA_unsigned_long             next_id;

	GSList                         *held_requests;
	GHashTable                     *child_poas;

	PortableServer_ThreadPolicyValue             p_thread;
	PortableServer_LifespanPolicyValue           p_lifespan;
	PortableServer_IdUniquenessPolicyValue       p_id_uniqueness;
	PortableServer_IdAssignmentPolicyValue       p_id_assignment;
	PortableServer_ImplicitActivationPolicyValue p_implicit_activation;
	PortableServer_ServantRetentionPolicyValue   p_servant_retention;
	PortableServer_RequestProcessingPolicyValue  p_request_processing;
};

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
