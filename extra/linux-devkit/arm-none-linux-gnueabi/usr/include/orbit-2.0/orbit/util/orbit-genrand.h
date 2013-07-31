
#ifndef ORBIT_GENRAND_H
#define ORBIT_GENRAND_H 1

#include <glib.h>

G_BEGIN_DECLS

#ifdef ORBIT2_INTERNAL_API

typedef enum {
	ORBIT_GENUID_STRONG,
	ORBIT_GENUID_SIMPLE
} ORBitGenUidType;

typedef enum {
	ORBIT_GENUID_COOKIE,
	ORBIT_GENUID_OBJECT_ID
} ORBitGenUidRole;

gboolean ORBit_genuid_init   (ORBitGenUidType type);
void     ORBit_genuid_fini   (void);
void     ORBit_genuid_buffer (guint8         *buffer,
			      int             length,
			      ORBitGenUidRole role);

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif /* ORBIT_GENRAND_H */
