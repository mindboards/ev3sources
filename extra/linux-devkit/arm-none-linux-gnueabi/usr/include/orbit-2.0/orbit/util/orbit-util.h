#ifndef ORBIT_UTIL_H
#define ORBIT_UTIL_H 1

#include <glib.h>
#include <orbit/orbit-config.h>
#include <orbit/util/basic_types.h>
#include <orbit/util/orbit-genrand.h>

G_BEGIN_DECLS

/* Align a value upward to a boundary, expressed as a number of bytes.
   E.g. align to an 8-byte boundary with argument of 8.  */

/*
 *   (this + boundary - 1)
 *          &
 *    ~(boundary - 1)
 */

#define ALIGN_VALUE(this, boundary) \
  (( ((gulong)(this)) + (((gulong)(boundary)) -1)) & (~(((gulong)(boundary))-1)))

#define ALIGN_ADDRESS(this, boundary) \
  ((gpointer)ALIGN_VALUE(this, boundary))

#ifdef ORBIT2_INTERNAL_API

gulong ORBit_wchar_strlen(CORBA_wchar *wstr);

#define num2hexdigit(n) (((n)>9)?((n)+'a'-10):((n)+'0'))

#endif /* ORBIT2_INTERNAL_API */

G_END_DECLS

#endif
