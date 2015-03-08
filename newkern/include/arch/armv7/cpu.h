/* 
 * arch/armv7/cpu.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-03-2015 - Created
 */

#ifndef _ARCH_ARMV7_CPU_
#define _ARCH_ARMV7_CPU_
/* Processor Status Register */

/* ------ MODE FIELD ------ */

/* User Mode */
#define	PSR_MODE_USR	(0x00000010)

/* Fast Interrupt Mode */
#define	PSR_MODE_FIQ	(0x00000011)

/* Interrupt Mode */
#define	PSR_MODE_IRQ	(0x00000012)

/* Supervisor Mode */
#define	PSR_MODE_SVC	(0x00000013)

/* Monitor Mode */
#define	PSR_MODE_MON	(0x00000016)

/* Abort Mode */
#define	PSR_MODE_ABT	(0x00000017)

/* Hypervisor Mode */
#define	PSR_MODE_HYP	(0x0000001A)

/* Undefined Mode */
#define	PSR_MODE_UND	(0x0000001B)

/* System Mode */
#define	PSR_MODE_SYS	(0x0000001F)

/* Mode field mask */
#define	PSR_MODE	(0x0000001F)

/* ------ BITS ------ */

/* Thumb execution state bit */
#define PSR_THUMB	(0x00000020)

/* Fast Interrupt Mask bit */
#define PSR_FIQ_MASK	(0x00000040)

/* Interrupt Mask bit */
#define PSR_IRQ_MASK	(0x00000080)

/* Async Abort Mask bit */
#define PSR_ABT_MASK	(0x00000100)

/* Endianness Select bit */
#define PSR_BIG_ENDIAN	(0x00000200)

/* ------  IF THEN ------ */

/* IF THEN field mask */
#define	PSR_IT		(0x0600FC00)

/* ------ GE FIELD ------ */

/* IF THEN field mask */
#define	PSR_GE		(0x000F0000)

/* ------  RESERVD ------ */

/* Reserved field mask */
#define	PSR_RESERVED	(0x00F00000)

/* ------ BITS ------ */

/* Jazelle execution state bit */
#define PSR_JAZELLE	(0x01000000)

/* Cumulative saturation bit */
#define PSR_SATURATION	(0x08000000)

/* Overflow condition flag */
#define PSR_OVERFLOW	(0x10000000)

/* Carry condition flag */
#define PSR_CARRY	(0x20000000)

/* Zero condition flag */
#define PSR_ZERO	(0x40000000)

/* Negative condition flag */
#define PSR_NEGATIVE	(0x80000000)


#endif
