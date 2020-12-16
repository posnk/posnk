/**
 * kernel/physmm.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 29-03-2014 - Created
 */

#ifndef __KERNEL_PHYSMM_H__
#define __KERNEL_PHYSMM_H__

#include <stddef.h>
#include <stdint.h>

#include "config.h"
#include "kernel/address.h"

#ifdef ARCH_I386
	#define PHYSMM_BITMAP_SIZE          (32768)
#endif

#ifdef ARCH_ARMV7
	#define PHYSMM_BITMAP_SIZE          (32768)
#endif

#define PHYSMM_PAGE_ADDRESS_MASK    PAGE_ADDR_MASK
#define PHYSMM_PAGE_SIZE            PAGE_SIZE

#define PHYSMM_NO_FRAME	(1)

extern uint32_t physmm_bitmap[PHYSMM_BITMAP_SIZE];

void physmm_free_range(physaddr_t start, physaddr_t end);

void physmm_claim_range(physaddr_t start, physaddr_t end);

physaddr_t physmm_count_free(void);
physaddr_t physmm_alloc_bmcopy() ;
/**
 * Allocates a physical frame
 */
physaddr_t physmm_alloc_frame(void);

/**
 * Allocates four physical frames
 */
physaddr_t physmm_alloc_quadframe(void);

/**
 * Releases a frame to be used again
 */
void physmm_free_frame(physaddr_t address);

/**
 * Initializes the physical memory manager
 * NOTE: Depends on the heap memory manager!
 */
void physmm_init(void);

#endif
