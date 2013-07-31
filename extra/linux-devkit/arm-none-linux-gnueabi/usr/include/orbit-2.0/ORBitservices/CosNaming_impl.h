#ifndef __CosNaming_impl_h__
#define __CosNaming_impl_h__

CosNaming_NamingContextExt 
ORBit_CosNaming_NamingContextExt_create (PortableServer_POA poa,
					 CORBA_Environment * ev);
/* Backward compatibilty */
#define impl_CosNaming_NamingContext__create \
        ORBit_CosNaming_NamingContextExt_create

#endif
