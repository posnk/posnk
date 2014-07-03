/**
 * kernel/heapmm.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 28-03-2014 - Created
 */

#ifndef __KERNEL_HEAPMM_H__
#define __KERNEL_HEAPMM_H__

#include "util/llist.h"
#include <stddef.h>

/**
 * typedef for heapmm_block:
 * Heap memory manager free block descriptor
 */
typedef struct heapmm_block	   heapmm_block_t;

/**
 * Heap memory manager free block descriptor
 * typedef: heapmm_block_t
 */
struct heapmm_block {
	llist_t node;
	void   *start;
	size_t  size;
};

size_t heapmm_request_core ( void *address, size_t size );

/**
 * Initializes the heap memory manager
 * @param heap_start The start of the heap
 */
void  heapmm_init(void *heap_start, size_t size);

/**
 * Allocates a new page of heap space to the caller
 */
void *heapmm_alloc_page();

/**
 * Allocates a page alligned block of RAM,no call to morecore
 */
void *heapmm_alloc_table();

/**
 * Allocates a page alligned block of RAM
 */
void *heapmm_alloc_page_alligned(size_t size);

/**
 * Allocates an alligned block of RAM
 */
void *heapmm_alloc_alligned(size_t size, uintptr_t alignment);

/**
 * Allocates a new block of memory of given size to the caller
 */
void *heapmm_alloc(size_t size);

/**
 * Releases a block of memory so it can be reallocated
 */
void  heapmm_free(void *address, size_t size);

#endif
