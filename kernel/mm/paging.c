/**
 * kernel/paging.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-03-2014 - Created
 */

#include <stddef.h>
#include <string.h>

#include "kernel/paging.h"
#include "kernel/physmm.h"
#include "kernel/heapmm.h"
#include "kernel/exception.h"
#define CON_SRC "paging"
#include "kernel/console.h"
#include "kernel/process.h"
page_dir_t *paging_active_dir;

size_t heapmm_request_core ( void *address, size_t size )
{
	size_t size_counter;
	physaddr_t frame;
	if (paging_active_dir == NULL)
		return 0;
	if ( ((uintptr_t)(address  + size)) > 0xF0000000 )
		printf(CON_WARN, "[0x%x] %i morecore", address, size);
	for (size_counter = 0; size_counter < size; size_counter += PHYSMM_PAGE_SIZE) {
		frame = physmm_alloc_frame();
		if (frame == PHYSMM_NO_FRAME){
			printf(CON_WARN, " KHEAP:: OUT OF MEM !!!!");
			break;
		}
		paging_map(address, frame, PAGING_PAGE_FLAG_RW);
		paging_tag(address, PAGING_PAGE_TAG_KERNEL_DATA);
		address = (void *) ( ((uintptr_t) address) + ((uintptr_t) PHYSMM_PAGE_SIZE) );
	}
	return size_counter;
}

/**
 * @brief Map a range of physical memory into virtual memory
 * This function maps an arbitrarily sized chunk of physical address space
 * into kernel heap address space. The address space taken from the heap
 * must be restored to be backed by normal RAM before freeing it. For this
 * reason, this function returns a structure describing the mapping, which
 * can be passed to paging_unmap_phys_range.
 * @param addr  The physical address to map.
 * @param size  The size of the region to map.
 * @param flags Page mapping flags to be used when mapping the range.
 * @return The mapping, if successful, NULL otherwise.
 */
physmap_t *paging_map_phys_range( physaddr_t addr, size_t size, int flags )
{
	size_t ptr, start;
	void *page_ptr;
	physmap_t *map;

	start = addr & PHYSMM_PAGE_ADDRESS_MASK;
	addr &= ~PHYSMM_PAGE_ADDRESS_MASK;

	/* Allocate a descriptor for this mapping */
	map = heapmm_alloc( sizeof( physmap_t ) );
	if ( !map )
		return NULL;

	/* Compute page rounded size */
	map->map_size  = (size + start + PHYSMM_PAGE_SIZE - 1) &
	          ~PHYSMM_PAGE_ADDRESS_MASK;

	/* Allocate a suitable bit of kernel heap address space */
	map->map_start = heapmm_alloc_alligned( map->map_size,
	                                        PHYSMM_PAGE_SIZE );
	if ( !map->map_start ) {
		heapmm_free( map, sizeof( physmap_t ) );
		return NULL;
	}

	/* Fill out the descriptor */
	map->virt      = start + map->map_start;
	map->phys      = addr;
	map->size      = size;
	map->flags     = flags;

	/* Iterate over all the pages in the range */
	for ( ptr = 0; ptr < map->map_size; ptr += PHYSMM_PAGE_SIZE ) {
		/* Get the virtual memory address for this page */
		page_ptr = map->map_start + ptr;

		/* Release the frame currently backing this bit of heap... */
		physmm_free_frame( paging_get_physical_address( page_ptr ) );

		/* ...and unmap it. */
		paging_unmap( page_ptr );

		/* Map the input physical page */
		paging_map( page_ptr , map->phys + ptr, flags );
	}

	return map;
}

/**
 * @brief Unmap a phyisical range mapped using paging_map_phys_range()
 * This function needs to rebind the virtual mapping to normal memory, and
 * as such temporarily requires exactly map->map_size bytes of physical memory.
 * If this is not available the function will return ENOMEM and the mapping
 * will not be affected.
 * @param map The mapping to destroy
 * @return Zero on success, an ERRNO code otherwise.
 */
int paging_unmap_phys_range( physmap_t *map )
{
	physaddr_t addr;
	void *page_ptr;
	size_t ptr, eptr;

	/* Iterate over all the pages in the range */
	for ( ptr = 0; ptr < map->map_size; ptr += PHYSMM_PAGE_SIZE ) {
		/* Get the virtual memory address for this page */
		page_ptr = map->map_start + ptr;

		/* ...and unmap it. */
		paging_unmap( page_ptr );

		/* Allocate the phys page */
		addr = physmm_alloc_frame();
		if ( addr == PHYSMM_NO_FRAME )
			goto fail_map;

		/* Map a normal physical page */
		paging_map( page_ptr, addr, PAGING_PAGE_FLAG_RW );
	}

	heapmm_free( map->map_start, map->map_size );
	heapmm_free( map, sizeof( physmap_t ) );
	return 0;

fail_map:
	eptr = ptr;
	/* Iterate over all the pages in the range */
	for ( ptr = 0; ptr <= eptr; ptr += PHYSMM_PAGE_SIZE ) {
		/* Get the virtual memory address for this page */
		page_ptr = map->map_start + ptr;

		/* Release the frame currently backing this bit of heap... */
		physmm_free_frame( paging_get_physical_address( page_ptr ) );

		/* ...and unmap it. */
		paging_unmap( page_ptr );

		/* Map the input physical page */
		paging_map( page_ptr , map->phys + ptr, map->flags );
	}
	return ENOMEM;
}

void paging_handle_out_of_memory()
{
	printf(CON_ERROR, "Out of memory! No handling for this yet");
}

void paging_handle_fault(void *virt_addr, void * instr_ptr, int present, int write, int user)
{
	struct siginfo info;
	uintptr_t addr = (uintptr_t) virt_addr;
	physaddr_t phys_addr;

	memset( &info, 0, sizeof( struct siginfo ) );
	//earlycon_printf("\n PF: 0x%x at IP:0x%x\n", virt_addr, instr_ptr);
//	debugcon_printf("[0x%x] page fault in %i @ 0x%x (P:%i, U:%i, W:%i) \n", virt_addr, curpid(), instr_ptr, present, user, write);
	/* Handle page fault here */
	if (user && (addr > 0xC0000000)) {
		/* Bad user access: exception */
	} else if (!present) {
		if ((addr >= 0xBFFF0000) && (addr < 0xBFFFB000)) {
			/* Task stack area, add more stack */
			virt_addr = (void *)(addr & ~PHYSMM_PAGE_ADDRESS_MASK);
			phys_addr = physmm_alloc_frame();
			if (phys_addr == PHYSMM_NO_FRAME) {
				paging_handle_out_of_memory();
				phys_addr = physmm_alloc_frame();
			}
			//TODO: Assess potential for race conditions
			paging_map(virt_addr, phys_addr, PAGING_PAGE_FLAG_RW | PAGING_PAGE_FLAG_USER);
			paging_tag(virt_addr, PAGING_PAGE_TAG_USER_DATA);
			return;
		} else if ((addr >= 0xE0000000) && (addr < 0xFFC00000)) {
			/* Kernel heap, check whether we have previously swapped out this page */
			//TODO: Swapping, for now: PANIC!!!
		} else if (addr < 0xC0000000){
			/* Application space page fault */
			if (procvmm_handle_fault(virt_addr))
				return;
		}
	} else if (write) {
		//TODO: Copy on write,
	}
	printf(CON_ERROR, "[0x%x] page fault in %i @ 0x%x (P:%i, U:%i, W:%i)", virt_addr, curpid(), instr_ptr, present, user, write);
	if ( !present )
		info.si_code = SEGV_MAPERR;
	else
		info.si_code = SEGV_ACCERR;
	info.si_addr = virt_addr;
	exception_handle( SIGSEGV, info, instr_ptr, user , !user );

}


