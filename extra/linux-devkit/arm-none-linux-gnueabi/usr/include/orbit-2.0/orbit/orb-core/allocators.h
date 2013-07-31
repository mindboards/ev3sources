#ifndef _ALLOCATORS_H_
#define _ALLOCATORS_H_

#include <glib.h>

G_BEGIN_DECLS

#define CORBA_sequence_set_release(s,r) (s)->_release = r
#define CORBA_sequence_get_release(s) (s)->_release

void     CORBA_free          (gpointer mem);
gpointer ORBit_alloc_tcval   (CORBA_TypeCode tc,
			      guint          nelements);
gpointer ORBit_realloc_tcval (gpointer       old,
			      CORBA_TypeCode tc,
			      guint          old_num_elements,
			      guint          num_elements);

#ifdef ORBIT2_INTERNAL_API

#define PTR_TO_MEMINFO(x) (((ORBit_mem_info *)(x)) - 1)
#define MEMINFO_TO_PTR(x) ((gpointer)((x) + 1))

/**
    This function-type is used internally by the memory allocator
    as a callback.  When called, it must "free" anything contained
    in {mem}.  Normally, it does not free {mem} itself, only what is
    inside {mem}.  For example, if {mem} contains only integers, then
    nothing happens. Alternatively, if {mem} contains object references,
    then CORBA_Object_free (or ORBit_RootObject_release()) must be
    invoked on it.

    The callback func must return the "end" of {mem}. This is used
    when iterating through sequences, arrays and structs.

    Previously, older code supported the idea that the callback could
    return FALSE (or NULL?), in which case that meant that the callback
    itself had already free'd the memory. This convention is no longer
    used.

    Below, some magic values of the fnc ptr are defined.
**/
typedef gpointer (*ORBit_Mem_free_fn) (gpointer mem,
				       gpointer func_data);

#define ORBIT_MEMHOW_HOW(how)      ((how) & 0x3)
#define ORBIT_MEMHOW_ELEMENTS(how) ((how) >> 2)
#define ORBIT_MEMHOW_MAKE(how,elements) (((elements) << 2) | (how))

typedef enum {
	ORBIT_MEMHOW_NONE     = 0, /* don't free */
	ORBIT_MEMHOW_SIMPLE   = 1, /* plain g_free */
	ORBIT_MEMHOW_TYPECODE = 2, /* free via typecode */
	ORBIT_MEMHOW_FREEFNC  = 3  /* free via a method */
} ORBitMemHow;

typedef struct ORBit_Memprefix {
	union {
		CORBA_TypeCode    tc;
		ORBit_Mem_free_fn free_fn;
	} u;
	ORBitMemHow how;
} ORBit_MemPrefix;

void     ORBit_free   (gpointer mem);
void     ORBit_free_T (gpointer mem);

CORBA_char *ORBit_alloc_string       (size_t string_length);
gpointer    ORBit_alloc_simple       (size_t block_size);
gpointer    ORBit_alloc_by_tc        (CORBA_TypeCode tc);
gpointer    ORBit_alloc_with_free_fn (size_t element_size,
				      guint  num_elements,
				      ORBit_Mem_free_fn free_fn);
CORBA_TypeCode ORBit_alloc_get_tcval (gpointer mem);

gpointer CORBA_any__freekids         (gpointer mem, gpointer dat);
gpointer ORBit_freekids_via_TypeCode (CORBA_TypeCode tc, gpointer mem);

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif /* _ALLOCATORS_H_ */

