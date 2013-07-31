#ifndef CORBA_NVLIST_H
#define CORBA_NVLIST_H 1

#include <orbit/orb-core/orb-types.h>
#include <orbit/orb-core/corba-typecode-type.h>

G_BEGIN_DECLS

void CORBA_NVList_add_item(CORBA_NVList list,
			   const CORBA_Identifier item_name,
			   const CORBA_TypeCode item_type,
			   const CORBA_OpaqueValue value,
			   const CORBA_long value_len,
			   const CORBA_Flags item_flags,
			   CORBA_Environment * ev);
void CORBA_NVList_free(CORBA_NVList list, CORBA_Environment * ev);
void CORBA_NVList_free_memory(CORBA_NVList list, CORBA_Environment * ev);
void CORBA_NVList_get_count(CORBA_NVList list, CORBA_long * count,
			    CORBA_Environment * ev);

G_END_DECLS

#endif
