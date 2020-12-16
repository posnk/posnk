/**
 * kernel/address.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 */

#ifndef __KERNEL_ADDRESS_H__
#define __KERNEL_ADDRESS_H__

#include <stddef.h>
#include <stdint.h>

#include "config.h"

#ifdef ARCH_I386
#	define PAGE_SIZE         (4096u)
#	define PAGE_ADDR_MASK    (PAGE_SIZE - 1u)

	/**
	 * Type guaranteed to be able to contain a physical address
	 */
	typedef uint32_t	physaddr_t;
#endif

#ifdef ARCH_ARMV7
#	define PAGE_SIZE         (4096u)
#	define PAGE_ADDR_MASK    (PAGE_SIZE - 1u)

	/**
	 * Type guaranteed to be able to contain a physical address
	 */
	typedef uint32_t	physaddr_t;
#endif

#define PAGE_ROUND_DOWN( a ) ( (a) & ~PAGE_ADDR_MASK )
#define PAGE_ROUND_UP( a )   PAGE_ROUND_DOWN( (a) + PAGE_SIZE - 1 )

#endif
