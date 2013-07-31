/* This stays outside */
#include <orbit/orbit-config.h>

#ifdef ORBIT_IDL_SERIAL
#if ORBIT_IDL_SERIAL < ORBIT_CONFIG_SERIAL
#error "You need to rerun 'orbit-idl' on the .idl file whose stubs you are using. These stubs were generated with an older version of ORBit, and need to be regenerated."
#endif
#endif

#ifndef ORBIT_H
#define ORBIT_H 1

#include <orbit/orbit-types.h>
#ifdef ORBIT2_INTERNAL_API
#  include <orbit/GIOP/giop.h>
#endif
#include <orbit/orb-core/orb-core.h>
#include <orbit/poa/poa.h>
#include <orbit/dynamic/dynamic.h>

G_BEGIN_DECLS

extern const char  *orbit_version;
extern unsigned int orbit_major_version;
extern unsigned int orbit_minor_version;
extern unsigned int orbit_micro_version;

G_END_DECLS

#endif
