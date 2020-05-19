/**
 * @file driver/platform/platform.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch
 *
 */

#ifndef __driver_platform_legacypc_h__
#define __driver_platform_legacypc_h__

#include "driver/platform/platform.h"

#define INT_VECTOR_NMI		(0x2)
#define	INT_VECTOR_START	(0x20)
#define INT_VECTOR_PIC0_START	(INT_VECTOR_START)
#define INT_VECTOR_PIC0_LOWPRI	(INT_VECTOR_START + 7)
#define INT_VECTOR_PIC1_START	(INT_VECTOR_START + 8)
#define INT_VECTOR_PIC1_LOWPRI	(INT_VECTOR_START + 15)

#define INT_PIC0_START		(0)
#define INT_PIC1_START		(8)

#define	INT_PC_SYSTIMER     (0)
#define	INT_PC_KEYBOARD		(1)
#define	INT_PC_SERIAL2      (3)
#define	INT_PC_SERIAL1      (4)
#define	INT_PC_PARALLEL2    (5)
#define	INT_PC_DISKETTE	    (6)
#define	INT_PC_PARALLEL1    (7)
#define INT_PC_RTC          (8)
#define INT_PC_RETRACE      (9)
#define	INT_PC_AUXILIARY    (12)
#define INT_PC_COPROCESSOR  (13)
#define	INT_PC_ATA          (14)
#define INT_NMI				(16)

#endif
