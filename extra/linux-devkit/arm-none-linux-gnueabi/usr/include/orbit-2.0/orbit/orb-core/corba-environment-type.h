#ifndef CORBA_ENVIRONMENT_TYPE_H
#define CORBA_ENVIRONMENT_TYPE_H 1

#include <orbit/orb-core/orb-types.h>
#include <orbit/orb-core/corba-any-type.h>

G_BEGIN_DECLS

#if !defined(_CORBA_Environment_defined)
#define _CORBA_Environment_defined 1
	typedef struct CORBA_Environment_type CORBA_Environment;
#endif

struct CORBA_Environment_type {
	CORBA_char          *_id;
	CORBA_unsigned_long  _major;
	CORBA_any            _any;
};

G_END_DECLS

#endif
