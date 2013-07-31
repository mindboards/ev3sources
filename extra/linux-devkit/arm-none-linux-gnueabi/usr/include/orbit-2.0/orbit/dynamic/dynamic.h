#ifndef ORBIT_DYNAMIC_H
#define ORBIT_DYNAMIC_H 1

#include <orbit/dynamic/dynamic-defs.h>

G_BEGIN_DECLS

#ifdef ORBIT2_INTERNAL_API

gpointer ORBit_dynany_new_default (const CORBA_TypeCode tc);

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
