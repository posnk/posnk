/**
 * kernel/dlheapmm.c
 *
 * Allows dlmalloc to be used as kernel allocator
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 14-07-2014 - Created
 */

#include "kernel/physmm.h"
#include "kernel/heapmm.h"
#include "kernel/earlycon.h"
#include "kernel/scheduler.h"
#include "kdbg/dbgapi.h"
#include <stdint.h>
#include <string.h>
#include "util/debug.h"
#include "kernel/system.h"

void* dlmemalign(size_t,size_t);
void* dlmalloc(size_t);
void  dlfree(void*);

/**
 * Pointer to the top of the heap
 */
void       	*heapmm_top_of_heap;

/**
 * Initializes the heap memory manager
 * @param heap_start The start of the heap
 */
void  heapmm_init(void *heap_start, size_t size)
{
	//TODO : Do not throw away initial heap
	heapmm_top_of_heap = (void *) ( ((uintptr_t)heap_start) + ((uintptr_t) size) );
}

/**
 * 
 */
void dlheapmm_abort()
{	
	earlycon_printf("\n\nPANIC! dlmalloc abort \n");
	earlycon_printf("Halting processor.\n");
	halt();
}

/**
 * Allocates an alligned block of RAM
 */
void *heapmm_alloc_alligned(size_t size, uintptr_t alignment)
{
	return dlmemalign((size_t) alignment, size);
}
 void * __builtin_return_address (unsigned int level);
int curpid()
{
	if (scheduler_current_task)
		return scheduler_current_task->pid;
	return -1;
}

/**
 * Allocates a new block of memory of given size to the caller
 */
void *heapmm_alloc(size_t size) 
{
	return dlmalloc(size);
}


/**
 * Allocates a new page of heap space to the caller
 */
void *heapmm_alloc_page() 
{
	return heapmm_alloc_alligned(PHYSMM_PAGE_SIZE,PHYSMM_PAGE_SIZE);
}

/**
 * Releases a block of meory so it can be reallocated
 */
void  heapmm_free(void *address, size_t size) 
{
	dlfree(address);
}

/**
 * Internal function
 * Requests more memory and adds it to the free block table,
 * then cleans up low space markers
 */
void * dlheapmm_sbrk ( size_t size )
{
	void *old = heapmm_top_of_heap;
	if (size == 0) {
		return heapmm_top_of_heap;
	} else if (size > 0) {
		size = heapmm_request_core ( heapmm_top_of_heap, size );

		/* Update top of heap pointer */
		heapmm_top_of_heap = ( void * ) ( ((uintptr_t)heapmm_top_of_heap) + ((uintptr_t)size));

		return old;
	} else {
		//TODO: WARN
		return -1;
	}
}
