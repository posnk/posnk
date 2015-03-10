/* 
 * arch/armv7/bootargs.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 08-03-2015 - Created
 */

#ifndef _ARCH_ARMV7_BOOTARGS_
#define _ARCH_ARMV7_BOOTARGS_

#include <stdint.h>

#define ARMV7_BOOTARGS_MAGIC	(0xCAFEB007)

#define ARMV7_BA_KMAP_READ	(0x00000001)
#define ARMV7_BA_KMAP_WRITE	(0x00000002)
#define ARMV7_BA_KMAP_EXEC	(0x00000004)
#define ARMV7_BA_KMAP_DEVICE	(0x00000008)


/**
 * Boot Arguments structure for ARMV7
 */

typedef struct {
	/** Magic number, must be equal to ARMV7_BOOTARGS_MAGIC */
	uint32_t	 ba_magic;
	/** Physical address of initrd image */
	uint32_t	 ba_initrd_pa;
	/** Size of initrd image */
	uint32_t	 ba_initrd_sz;	
	/** Physical memory availability bitmap */
	uint32_t	 ba_pm_bitmap[32768];
	/** Pointer to the command line */
	uint8_t		*ba_cmd;
	/** Pointer to the kernel mapping list */
	armv7_kmap_t	*ba_kmap;
} armv7_bootargs_t;

/**
 * Boot Kernel Mapping structure for ARMV7
 */
typedef struct {
	/** Phyisical Address */
	uint32_t	ba_kmap_pa;
	/** Virtual Address */
	uint32_t	ba_kmap_va;
	/** Size (in bytes) */
	uint32_t	ba_kmap_sz;
	/** Flags */
	uint32_t	ba_kmap_fl;
} armv7_ba_kmap_t;



#endif
