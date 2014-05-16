/**
 * tests/test_heapmm.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 29-03-2014 - Created
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "kernel/heapmm.h"

int remaining_blocks = 30;

extern llist_t heapmm_free_blocks_list;

size_t heapmm_request_core ( void *address, size_t size )
{	
	remaining_blocks--;
	printf("heapmm requested %i bytes of extra heap, giving 4096. remaining pages:%i \n",size, remaining_blocks);
	return 4096;
}

int heapmm_debug_iterator	 (llist_t *node, void *param)
{
	heapmm_block_t *block  = (heapmm_block_t *) node;
	printf("\t FREE [start: 0x%x\t size:%i]\n", (uint32_t) block->start, block->size);		
	return 0;
}

void heapmm_debug() {
	printf("\n\t heapmm debug [blocklist size: %i]\n",llist_size(&heapmm_free_blocks_list));
	llist_iterate_select(&heapmm_free_blocks_list, &heapmm_debug_iterator, NULL);
	printf("\n");
}



int main ( void )
{
	void *alloc1;
	void *alloc2;
	void *alloc3;
	void *alloc4;
	void *alloc5;
	
	void **alloc6;
	int l;
	
	size_t heap_size_initial = 4096;
	void *heap_start = malloc (4096 * remaining_blocks);
	printf("P-OS Heap memory manager test\n");
	printf("Virtual RAM for testcase : %i byte @ 0x%x\n",4096 * remaining_blocks,(uint32_t)heap_start);
	printf("Initial heap size: %i\n", heap_size_initial);
	printf("Committing initial heap...\n");
	remaining_blocks--;
	heapmm_init(heap_start, heap_size_initial);
	printf("Initial heap committed! Remaining heap pages: %i\n", remaining_blocks);

	heapmm_debug();

	printf("Allocating 10 bytes...");
	alloc1 = heapmm_alloc(10);
	printf(" OK: 0x%x\n", (uint32_t) alloc1);

	heapmm_debug();

	printf("Allocating 30 bytes...");
	alloc2 = heapmm_alloc(30);
	printf(" OK: 0x%x\n", (uint32_t) alloc2);

	heapmm_debug();

	printf("Allocating 2 bytes...");
	alloc3 = heapmm_alloc(2);
	printf(" OK: 0x%x\n", (uint32_t) alloc3);

	heapmm_debug();
	
	printf("Freeing 1st alloc (10 bytes)...");
	heapmm_free(alloc1, 10);
	printf(" OK!\n");

	heapmm_debug();
	
	printf("Freeing 3rd alloc (2 bytes)...");
	heapmm_free(alloc3, 2);
	printf(" OK!\n");

	heapmm_debug();
	
	printf("Freeing 2nd alloc (30 bytes)...");
	heapmm_free(alloc2, 30);
	printf(" OK!\n");

	heapmm_debug();

	printf("Allocating 42 bytes...");
	alloc1 = heapmm_alloc(42);
	printf(" OK: 0x%x\n", (uint32_t) alloc1);

	heapmm_debug();

	printf("Allocating page...");
	alloc2 = heapmm_alloc_page();
	printf(" OK: 0x%x\n", (uint32_t) alloc2);

	heapmm_debug();

	printf("Allocating page...");
	alloc3 = heapmm_alloc_page();
	printf(" OK: 0x%x\n", (uint32_t) alloc3);

	heapmm_debug();
	
	printf("Freeing page 1...");
	heapmm_free(alloc2, 4096);
	printf(" OK!\n");

	heapmm_debug();

	printf("Allocating page...");
	alloc2 = heapmm_alloc_page();
	printf(" OK: 0x%x\n", (uint32_t) alloc2);

	heapmm_debug();

	printf("Allocating array...");

	alloc6 = (void **) heapmm_alloc(100*sizeof(void*));

	printf(" OK: 0x%x\n", (uint32_t) alloc6);

	heapmm_debug();

	printf("Allocating 100 x 80 bytes...\n");

	for (l = 0; l < 100; l++){
		alloc6[l] = heapmm_alloc(80);
	}

	printf(" OK.");

	heapmm_debug();

	printf("Freeing 100 x 80 bytes...\n");

	for (l = 0; l < 100; l++){
		heapmm_free(alloc6[l],80);
	}

	printf(" OK.");

	heapmm_debug();
	
	printf("Freeing array...");
	heapmm_free((void *)alloc6, 100*sizeof(void*));
	printf(" OK!\n");

	heapmm_debug();
	
	printf("Freeing page 1...");
	heapmm_free(alloc2, 4096);
	printf(" OK!\n");
	
	printf("Freeing page 2...");
	heapmm_free(alloc3, 4096);
	printf(" OK!\n");

	heapmm_debug();
	

	free(heap_start);
}
