/* 
 * kdbg/heapdbg.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 12-05-2014 - Created
 */

#ifndef __KDBG_HEAPDBG_H__
#define __KDBG_HEAPDBG_H__

#include <stdint.h>
#include <stddef.h>
#include "util/llist.h"


typedef struct kdbg_heap_use {
	llist_t		 link;
	uintptr_t	 start;
	size_t		 size;
	llist_t		*calltrace;
} kdbg_heap_use_t;

void kdbg_init_memuse();
void kdbg_enable_memuse();
void kdbg_print_memuse_brdr(void *addr);

#endif
