#ifndef CORBA_CONTEXT_H
#define CORBA_CONTEXT_H 1

#include <glib.h>

G_BEGIN_DECLS

#ifdef ORBIT2_INTERNAL_API

struct CORBA_Context_type {
	struct ORBit_RootObject_struct parent;

	GHashTable    *mappings;
	GSList        *children;

	gchar         *the_name;

	CORBA_Context  parent_ctx;
};

void     ORBit_Context_marshal     (CORBA_Context                   ctx,
				    const ORBit_ContextMarshalItem *mlist,
				    CORBA_unsigned_long             nitems,
				    GIOPSendBuffer                 *buf);

gboolean ORBit_Context_demarshal   (CORBA_Context   parent,
				    CORBA_Context   initme,
				    GIOPRecvBuffer *recv_buffer);

void     ORBit_Context_server_free (CORBA_Context ctx);

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
