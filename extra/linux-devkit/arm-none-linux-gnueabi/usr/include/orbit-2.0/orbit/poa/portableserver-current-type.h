#ifndef PORTABLESERVER_CURRENT_TYPE_H
#define PORTABLESERVER_CURRENT_TYPE_H 1

#include <glib.h>

G_BEGIN_DECLS

#ifdef ORBIT2_INTERNAL_API

struct PortableServer_Current_type {
        struct ORBit_RootObject_struct parent;

        CORBA_ORB                      orb;
};

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
