/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
/*
 *  ======== tistdtypes.h ========
 *
 */

/*
 *  These types are also defined by other TI components.  They are bracketed
 *  with _TI_STD_TYPES to avoid warnings for duplicate definition.  The
 *  The Uint16, etc. definitions were provided in early 2.x versions of CSL
 *  that did not have _TI_STD_TYPES protection.
 *
 *  You may get warnings about duplicate type definitions when using this
 *  header file with 2.x CSL.  You can use the '-pds303' compiler option to
 *  suppress these warnings.
 */
#ifndef _TISTDTYPES_H_
#define _TISTDTYPES_H_

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif

/*
 * Null declaration 
 */
#ifndef NULL
#define NULL    (0)
#endif

/*
 * Return type defines 
 */
#ifndef E_PASS
#define E_PASS    (0x00000000u)
#endif
#ifndef E_FAIL
#define E_FAIL    (0x00000001u)
#endif
#ifndef E_TIMEOUT
#define E_TIMEOUT (0x00000002u)
#endif

// Aliases for standard C types
	typedef int Int;
	typedef unsigned Uns;
	typedef char Char;
	typedef char *String;	/* pointer to null-terminated character sequence */
	typedef void *Ptr;	/* pointer to arbitrary type */
	typedef unsigned short Bool;	/* boolean */

// Boolean declarations
#ifndef TRUE
#define TRUE    ((Bool) 1)
#endif
#ifndef FALSE
#define FALSE   ((Bool) 0)
#endif

// GENERIC BUS width defines
#ifndef BUS_8BIT
#define BUS_8BIT            (0x01)
#endif
#ifndef BUS_16BIT
#define BUS_16BIT           (0x02)
#endif
#ifndef BUS_32BIT
#define BUS_32BIT           (0x04)
#endif
#ifndef BUS_64BIT
#define BUS_64BIT           (0x08)
#endif

// Generic bit mask defines
#ifndef BIT0
#define BIT0    (0x00000001)
#define BIT1    (0x00000002)
#define BIT2    (0x00000004)
#define BIT3    (0x00000008)
#define BIT4    (0x00000010)
#define BIT5    (0x00000020)
#define BIT6    (0x00000040)
#define BIT7    (0x00000080)
#define BIT8    (0x00000100)
#define BIT9    (0x00000200)
#define BIT10   (0x00000400)
#define BIT11   (0x00000800)
#define BIT12   (0x00001000)
#define BIT13   (0x00002000)
#define BIT14   (0x00004000)
#define BIT15   (0x00008000)
#define BIT16   (0x00010000)
#define BIT17   (0x00020000)
#define BIT18   (0x00040000)
#define BIT19   (0x00080000)
#define BIT20   (0x00100000)
#define BIT21   (0x00200000)
#define BIT22   (0x00400000)
#define BIT23   (0x00800000)
#define BIT24   (0x01000000)
#define BIT25   (0x02000000)
#define BIT26   (0x04000000)
#define BIT27   (0x08000000)
#define BIT28   (0x10000000)
#define BIT29   (0x20000000)
#define BIT30   (0x40000000)
#define BIT31   (0x80000000)
#endif

/*
 * Uint8, Uint16, Uint32, etc are defined to be "smallest unit of
 * available storage that is large enough to hold unsigned or integer
 * of specified size".
 */

/* Handle the 6x ISA */
#if defined(_TMS320C6X)
	/* Unsigned integer definitions (32bit, 16bit, 8bit) follow... */
	typedef unsigned int Uint32;
	typedef unsigned short Uint16;
	typedef unsigned char Uint8;

	/* Signed integer definitions (32bit, 16bit, 8bit) follow... */
	typedef int Int32;
	typedef short Int16;
	typedef char Int8;

	/* Volatile unsigned integer definitions (32bit, 16bit, 8bit) follow... */
	typedef volatile unsigned int VUint32;
	typedef volatile unsigned short VUint16;
	typedef volatile unsigned char VUint8;

	/* Volatile signed integer definitions (32bit, 16bit, 8bit) follow... */
	typedef volatile int VInt32;
	typedef volatile short VInt16;
	typedef volatile char VInt8;

#define __FAR__                      far
#define __NEAR__                     near

/* Handle the 54x, 55x ISAs */
#elif defined(_TMS320C5XX) || defined(__TMS320C55X__)
	/* Unsigned integer definitions (32bit, 16bit, 8bit) follow... */
	typedef unsigned long Uint32;
	typedef unsigned short Uint16;
	typedef unsigned char Uint8;

	/* Signed integer definitions (32bit, 16bit, 8bit) follow... */
	typedef long Int32;
	typedef short Int16;
	typedef char Int8;
/* Handle 28x ISA */
#elif defined(_TMS320C28X)
	/* Unsigned integer definitions (32bit, 16bit, 8bit) follow... */
	typedef unsigned long Uint32;
	typedef unsigned int Uint16;
	typedef unsigned char Uint8;

	/* Signed integer definitions (32bit, 16bit, 8bit) follow... */
	typedef long Int32;
	typedef int Int16;
	typedef char Int8;
/* Handle ARM TMS470 ISA (GNU compiler is assumed to exist for ARM only)*/
#elif (defined(__TMS470__) || defined(__GNUC__))
	/* Unsigned integer definitions (32bit, 16bit, 8bit) follow... */
	typedef unsigned int Uint32;
	typedef unsigned short Uint16;
	typedef unsigned char Uint8;

	/* Signed integer definitions (32bit, 16bit, 8bit) follow... */
	typedef int Int32;
	typedef short Int16;
	typedef char Int8;

	/* Volatile unsigned integer definitions (32bit, 16bit, 8bit) follow... */
	typedef volatile unsigned int VUint32;
	typedef volatile unsigned short VUint16;
	typedef volatile unsigned char VUint8;

	/* Volatile signed integer definitions (32bit, 16bit, 8bit) follow... */
	typedef volatile int VInt32;
	typedef volatile short VInt16;
	typedef volatile char VInt8;

	/* empty defines */
#define __FAR__
#define __NEAR__
#else
	/* Other ISAs not supported */
#error <tistdtypes.h> is not supported for this target
#endif

#ifdef __cplusplus
}
#endif
#endif				/* _TISTDTYPES_H_ */
