/**
 * tests/test_physmm.h
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
#include "kernel/physmm.h"

int remaining_blocks = 30;

size_t heapmm_request_core ( size_t size )
{	
	remaining_blocks--;
	printf("heapmm requested %i bytes of extra heap, giving 4096. remaining pages:%i \n",size, remaining_blocks);
	return 4096;
}

int main ( void )
{
	physaddr_t alloc1;
	physaddr_t alloc2;
	physaddr_t alloc3;
	physaddr_t alloc4;

	
	size_t heap_size_initial = 65536;
	size_t ram_size = 4096 * remaining_blocks;
	void *heap_start = malloc (4096 * remaining_blocks);
	printf("P-OS Physical memory manager test\n");
	printf("Virtual RAM for testcase : %i byte @ 0x%x\n",ram_size,(uint32_t)heap_start);
	printf("Physical RAM for testcase : %i byte @ 0x%x\n",ram_size,(uint32_t)0x10000);
	printf("Initial heap size: %i\n", heap_size_initial);
	printf("Committing initial heap...\n");
	remaining_blocks-=16;
	heapmm_init(heap_start, heap_size_initial);
	printf("Initial heap committed! Remaining heap pages: %i\n", remaining_blocks);

	printf("Initializing physical memory manager...");
	physmm_init();
	printf(" OK\n");

	printf("Registering RAM... \n");
	for (alloc1 = 0x10000; alloc1 < ram_size; alloc1+=4096){
		printf("\t Register frame : 0x%x\n", alloc1);
		physmm_free_frame(alloc1);
	}
	printf(" OK\n");

	
	printf("Allocating page...");
	alloc2 = physmm_alloc_frame();
	printf(" OK: 0x%x\n", (uint32_t) alloc2);

	printf("Allocating page...");
	alloc3 = physmm_alloc_frame();
	printf(" OK: 0x%x\n", (uint32_t) alloc3);
	
	printf("Freeing page 1...");
	physmm_free_frame(alloc2);
	printf(" OK!\n");

	printf("Allocating page...");
	alloc2 = physmm_alloc_frame();
	printf(" OK: 0x%x\n", (uint32_t) alloc2);

	free(heap_start);
}
