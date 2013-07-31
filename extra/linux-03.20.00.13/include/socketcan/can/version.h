/*
 * socketcan/can/version.h
 *
 * Version information for the CAN network layer implementation

 * Author: Urs Thuermann   <urs.thuermann@volkswagen.de>
 * Copyright (c) 2002-2007 Volkswagen Group Electronic Research
 * All rights reserved.
 *
 * Send feedback to <socketcan-users@lists.berlios.de>
 *
 */

#ifndef CAN_VERSION_H
#define CAN_VERSION_H

#define RCSID(s) asm(".section .rodata.str1.1,\"aMS\",@progbits,1\n\t" \
		     ".string \"" s "\"\n\t.previous\n")

RCSID("$Id: version.h 1038 2009-08-21 10:00:21Z hartkopp $");

#endif /* CAN_VERSION_H */
