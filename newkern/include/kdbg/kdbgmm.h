/**
 * kdbg/kdbgmm.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 28-03-2014 - Created as kernel heap mm
 * 12-05-2014 - Adapted as debugger heap mm
 */

#ifndef __KDBG_KDBGMM_H__
#define __KDBG_KDBGMM_H__

#include "util/llist.h"
#include <stddef.h>

/**
 * typedef for kdbgmm_block:
 * Heap memory manager free block descriptor
 */
typedef struct kdbgmm_block	   kdbgmm_block_t;

/**
 * Heap memory manager free block descriptor
 * typedef: kdbgmm_block_t
 */
struct kdbgmm_block {
	llist_t node;
	void   *start;
	size_t  size;
};

/**
 * Initializes the heap memory manager
 * @param heap_start The start of the heap
 */
void  kdbgmm_init(void *heap_start, size_t size);

/**
 * Allocates a new page of heap space to the caller
 */
void *kdbgmm_alloc_page();

/**
 * Allocates a page alligned block of RAM,no call to morecore
 */
void *kdbgmm_alloc_table();

/**
 * Allocates a page alligned block of RAM
 */
void *kdbgmm_alloc_page_alligned(size_t size);

/**
 * Allocates a new block of memory of given size to the caller
 */
void *kdbgmm_alloc(size_t size);

/**
 * Releases a block of memory so it can be reallocated
 */
void  kdbgmm_free(void *address, size_t size);

#endif
