/**
 * kdbg/kdbgmm.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 28-03-2014 - Created as kernel heap mm
 * 12-05-2014 - Adapted as debugger heap mm
 */

#include "kernel/physmm.h"
#include "kdbg/kdbgmm.h"
#include "kernel/earlycon.h"
#include "kernel/scheduler.h"
#include <stdint.h>
#include <string.h>

#define FALSE (0)

#define TRUE (1)

/**
 * Heap memory manager free block list head pointer
 */
llist_t 	 kdbgmm_free_blocks_list;

/**
 * Pointer to the top of the heap
 */
void       	*kdbgmm_top_of_heap;



void kdbgmm_free_internal(void *address, size_t size);

void *kdbgmm_alloc_internal(size_t size, int morecore);

size_t heapmm_request_core ( void *address, size_t size );



/**
 * Iterator function that tests for empty blocks
 */
int kdbgmm_morecore_iterator (llist_t *node, __attribute__((__unused__)) void *param)
{
	kdbgmm_block_t *block = (kdbgmm_block_t *) node;
	return block->size == 0;
}

/**
 * Internal function
 * Requests more memory and adds it to the free block table,
 * then cleans up low space markers
 */
void kdbgmm_morecore ( size_t size )
{
	
	kdbgmm_block_t *found_block;
	
	size = heapmm_request_core ( kdbgmm_top_of_heap, size );
	
	/* Add new core to free block table  */
	kdbgmm_free_internal ( kdbgmm_top_of_heap, size );
 
	/* Update top of heap pointer */
	kdbgmm_top_of_heap = ( void * ) ( ((uintptr_t)kdbgmm_top_of_heap) + ((uintptr_t)size));

	/* Free small chunks of heap for which no block info's could be made */

	for (	found_block = (kdbgmm_block_t *)
			llist_iterate_select(&kdbgmm_free_blocks_list,
					&kdbgmm_morecore_iterator, NULL);
		found_block != NULL;
		found_block = (kdbgmm_block_t *)
			llist_iterate_select(&kdbgmm_free_blocks_list,
					&kdbgmm_morecore_iterator, NULL)	) {
		/* END OF MESSY FOR STATEMENT */

		/* Unlink tag block info */
		llist_unlink((llist_t *) found_block);

		/* Free its memory */
		kdbgmm_free_internal ( (void *) found_block , sizeof (kdbgmm_block_t) );	

	}
	
}

/**
 * Iterator function that tests for [our block][FREE] mergeable blocks
 */
int kdbgmm_merge_start_iterator (llist_t *node, void *param)
{
	kdbgmm_block_t *block = (kdbgmm_block_t *) node;
	return (block->start == param) && !(block->size == 0);
}

/**
 * Iterator function that tests for [FREE][our block] mergeable blocks
 */
int kdbgmm_merge_end_iterator (llist_t *node, void *param)
{
	kdbgmm_block_t *block = (kdbgmm_block_t *) node;
	void 		   *end_address;
	end_address = (void *) ( ((uintptr_t)block->start) + ((uintptr_t) block->size) );
	return (end_address == param) && !(block->size == 0);
}
/**
 * Releases a block of memory so it can be reallocated
 */
void kdbgmm_free_internal(void *address, size_t size)
{
	kdbgmm_block_t  *found_block;
	kdbgmm_block_t  *merge_block;
	void 		*end_address;
	end_address = (void *) ( ((uintptr_t)address) + ((uintptr_t) size) );
	memset(address, 0x42, size);

	found_block = (kdbgmm_block_t *)
		llist_iterate_select(&kdbgmm_free_blocks_list,
							&kdbgmm_merge_start_iterator,
							address);

	/* Try to find a mergeable block */
	/* First, try to find block [FREE][our block] */
	found_block = (kdbgmm_block_t *)
		llist_iterate_select(&kdbgmm_free_blocks_list,
							&kdbgmm_merge_end_iterator,
							address);
	if (found_block != NULL) {
		/* Found it! */
		/* Proceed and merge! */
		found_block->size += size;
	} else {
		/* No fitting free block was found */
		/* Now, try to find block [our block][FREE] */
		found_block = (kdbgmm_block_t *)
			llist_iterate_select(&kdbgmm_free_blocks_list,
								 &kdbgmm_merge_start_iterator,
								 end_address);
		if (found_block != NULL) {
			/* Found it! */
			/* Proceed and merge! */
			found_block->size += size;
			found_block->start = address;
		} else {
			/* Well then, no mergeable blocks? */
			/* Time to add a new block! */

			//TODO: Release a lock on the free block table

			found_block = (kdbgmm_block_t *)
				kdbgmm_alloc_internal(sizeof(kdbgmm_block_t), FALSE);
			if (found_block == NULL) {
				/* No free blocks large enough to contain block info */
				/* Time to get hacky */
				if (size >= sizeof(kdbgmm_block_t)) {
					/* A block info structure will fit in this free space */
					/* Make the new block info at the start of the free space */
					found_block = (kdbgmm_block_t *) address;
					found_block->size = size - sizeof(kdbgmm_block_t);
					found_block->start =
						(void *) ( ((uintptr_t) address) +
								((uintptr_t) sizeof(kdbgmm_block_t)) );
					/* If the block is just large enough for the info struct   */
					/* the block will still be created but with a size of 0,   */
					/* if more core is added later on it will be freed and if  */
					/* another block is freed next to it it will be used as    */
					/* the block info for that block 						   */
				
					//TODO: Get a lock on the free block table

					llist_add_end(&kdbgmm_free_blocks_list , (llist_t *) found_block);

				} else {
					/* so, its a small block and we can't store info about it? */
					/* just request more core already! */
					
					kdbgmm_morecore(sizeof(kdbgmm_block_t));

					/* Try to free the block */
					kdbgmm_free_internal(address, size);
					
					return;
				}
			} else {
				found_block->start = address;
				found_block->size  = size;

				//TODO: Get a lock on the free block table

				llist_add_end(&kdbgmm_free_blocks_list, (llist_t *) found_block);
			}
		}
	}

	end_address = (void *) ( ((uintptr_t) found_block->start ) + ((uintptr_t) found_block->size) );

	merge_block = (kdbgmm_block_t *)
		llist_iterate_select(&kdbgmm_free_blocks_list,
							&kdbgmm_merge_start_iterator,
							end_address);

	if ( merge_block != NULL ) {
		/* Next block can also be merged */
		found_block->size += merge_block->size;

		llist_unlink ((llist_t *) merge_block);		

		//TODO: Release a lock on the free block table

		kdbgmm_free_internal( (void *) merge_block, sizeof(kdbgmm_block_t) );

		//TODO: Get a lock on the free block table
	}

	merge_block = (kdbgmm_block_t *)
		llist_iterate_select(&kdbgmm_free_blocks_list,
							&kdbgmm_merge_end_iterator,
							found_block->start);

	if ( merge_block != NULL ) {

		/* Previous block can also be merged */
		merge_block->size += found_block->size;

		llist_unlink ((llist_t *) found_block);		

		//TODO: Release a lock on the free block table

		kdbgmm_free_internal( (void *) found_block, sizeof(kdbgmm_block_t) );
	}

	//TODO: Release a lock on the free block table

}

/**
 * Iterator function that tests for blocks equal the size in param
 */
int kdbgmm_alloc_match_iterator (llist_t *node, void *param)
{
	kdbgmm_block_t *block = (kdbgmm_block_t *) node;
	size_t 			size  = (size_t) param;
	if ((((uintptr_t)block)+sizeof(kdbgmm_block_t)) > (uintptr_t)kdbgmm_top_of_heap)
		for(;;);
	
	return block->size == size;
}

/**
 * Iterator function that tests for blocks able to contain the size in param
 */
int kdbgmm_alloc_fit_iterator (llist_t *node, void *param)
{
	kdbgmm_block_t *block = (kdbgmm_block_t *) node;
	size_t 			size  = (size_t) param;
	return block->size >= size;
}

/**
 * Allocates a new block of memory of given size to the caller
 */
void *kdbgmm_alloc_internal(size_t size, int morecore)
{
	kdbgmm_block_t *found_block;
	void 		   *address;
		
	//TODO: Get a lock on the free block table

	/* Try to find fitting free block */
	found_block = (kdbgmm_block_t *)
		llist_iterate_select(&kdbgmm_free_blocks_list,
							 &kdbgmm_alloc_match_iterator,
							 (void *) size);
	if (found_block == NULL) {
		/* No fitting free block was found */
		found_block = (kdbgmm_block_t *)
			llist_iterate_select(&kdbgmm_free_blocks_list,
								 &kdbgmm_alloc_fit_iterator,
								 (void *) size);
	}
	if (found_block == NULL) {
		/* No large enough free block was found */

		/* Should we allocate more heap ? */

		if (!morecore) {

			/* Last resort: find 0 sized blocks */
			/* and use their info struct memory */

			/* Our last resort must not be used     */
			/* for anything other than info structs */

			if (size != sizeof(kdbgmm_block_t)) {
				/* NOPE, RETURN */
				return NULL;
			}

			found_block = (kdbgmm_block_t *)
				llist_iterate_select(&kdbgmm_free_blocks_list,
									 &kdbgmm_morecore_iterator,
									 NULL);

			if (found_block != NULL) {
				llist_unlink( (llist_t *) found_block);
				return (void *) found_block;
			}

			return NULL;

		}

		/* Request more core */

		//TODO: Release a lock on the free block table
		
		kdbgmm_morecore(size);

		return kdbgmm_alloc_internal(size, morecore);
	}

	address = found_block->start;

	if (found_block->size > size) {
		/* Block is larger than requested size */
		/* Proceed to shrink block */
		found_block->size -= size;
		found_block->start = (void *) ( ((uintptr_t)found_block->start) + ((uintptr_t) size) );

		//TODO: Release a lock on the free block table

	} else {
		/* Block is a perfect fit */
		/* Proceed to unlink and free block */
		llist_unlink((llist_t *) found_block);

		//TODO: Release a lock on the free block table
		//if (morecore){//HACKHACKHACK
		//	void *test = kdbgmm_alloc_internal(3*sizeof(kdbgmm_block_t), TRUE);
		//	kdbgmm_free_internal(test,3*sizeof(kdbgmm_block_t));
		//}
		kdbgmm_free_internal( (void *) found_block, sizeof(kdbgmm_block_t) );
	}

	
	memset(address, 0x23, size);
	//if ((paging_get_physical_address(address)&~ 0xfff) == 0x599000)
	//	debugcon_printf("[0x%x] %i alloc R\n",address,size);//, address, size, p==palloc_a);
	//if ((paging_get_physical_address(address+0x1000)&~0xfff) == 0x599000)
	//	debugcon_printf("[0x%x] %i alloc E\n",address,size);//, address, size, p==palloc_a);
	return address;
}

/**
 * Iterator function that tests for page aligned, page sized blocks
 */
int kdbgmm_alloc_page_match_iterator	 (llist_t *node, void *param)
{
	kdbgmm_block_t *block  = (kdbgmm_block_t *) node;
	uintptr_t		inpage = (uintptr_t) block->start;
	size_t 			size  = (size_t) param;
	inpage &= PHYSMM_PAGE_ADDRESS_MASK;			
	return (inpage == 0) && (block->size == size);
}

/**
 * Iterator function that tests for blocks large enough to contain a
 * whole page.
 */
int kdbgmm_alloc_page_split_iterator (llist_t *node, void *param)
{
	kdbgmm_block_t *block  = (kdbgmm_block_t *) node;
	uintptr_t		inpage = (uintptr_t) block->start;
	size_t 			size  = (size_t) param;
	size_t			reqsize;
	inpage &= PHYSMM_PAGE_ADDRESS_MASK;			
	reqsize = size + (size_t)(PHYSMM_PAGE_SIZE - inpage);
	return block->size >= reqsize;
}

/**
 * Iterator function that tests for page aligned, larger than page sized blocks
 */
int kdbgmm_alloc_page_fit_iterator (llist_t *node, void *param)
{
	kdbgmm_block_t *block  = (kdbgmm_block_t *) node;
	uintptr_t		inpage = (uintptr_t) block->start;
	size_t 			size  = (size_t) param;
	inpage &= PHYSMM_PAGE_ADDRESS_MASK;			
	return (inpage == 0) && (block->size >= size);
}
/**
 * Allocates a page alligned block of RAM
 */
void *kdbgmm_alloc_page_alligned(size_t size)
{
	kdbgmm_block_t *found_block;
	void 		   *address;
	void		   *pad_address;
	uintptr_t		in_page;

	//TODO: Get a lock on the free block table
	
	/* Try to find fitting, aligned free block */
	found_block = (kdbgmm_block_t *)
		llist_iterate_select(&kdbgmm_free_blocks_list,
							 &kdbgmm_alloc_page_fit_iterator,
							 (void *) size);
	if (found_block == NULL) {
		/* No fitting, aligned free block was found */
		found_block = (kdbgmm_block_t *)
			llist_iterate_select(&kdbgmm_free_blocks_list,
								 &kdbgmm_alloc_page_match_iterator,
								 (void *) size);
	}
	if (found_block == NULL) {
		/* No aligned free block was found */
		found_block = (kdbgmm_block_t *)
			llist_iterate_select(&kdbgmm_free_blocks_list,
								 &kdbgmm_alloc_page_split_iterator,
								 (void *) size);
	} 
	if (found_block == NULL) {
		/* No aligned or alignable free block was found */		

		/* Request more core */

		//TODO: Release a lock on the free block table
		
		kdbgmm_morecore(size);

		return kdbgmm_alloc_page();
	}

	pad_address = address = found_block->start;
	
	in_page = ((size_t) address) & PHYSMM_PAGE_ADDRESS_MASK;

	if (in_page == 0) {
		/* Block is aligned */
		if (found_block->size > size) {
			/* Block is larger than requested size */
			/* Proceed to shrink block */
			found_block->size -= size;
			found_block->start = (void *) ( ((uintptr_t)found_block->start) + ((uintptr_t) size) );

			//TODO: Release a lock on the free block table

		} else {
			/* Block is a perfect fit */
			/* Proceed to unlink and free block */
			llist_unlink((llist_t *) found_block);

			//TODO: Release a lock on the free block table

			kdbgmm_free_internal( (void *) found_block, sizeof(kdbgmm_block_t) );
		}
	} else {
		/* Block is not aligned */

		/* Proceed to shrink block */
 		size_t padding = (size_t) (PHYSMM_PAGE_SIZE - in_page);
		found_block->start = (void *) ( ((uintptr_t)found_block->start) + ((uintptr_t) padding) + ((uintptr_t) size));
		found_block->size -= padding + ((size_t) size);
		
		address = (void *) ( ((uintptr_t)address) + ((uintptr_t) padding));

		//TODO: Release a lock on the free block table

		/* Free padding */
		kdbgmm_free_internal ( pad_address, padding );

	}
	return address;
}



/**
 * Allocates a new block of memory of given size to the caller
 */
void *kdbgmm_alloc(size_t size) 
{
	void *addr =  kdbgmm_alloc_internal(size, TRUE);
	if ((((uintptr_t)addr)+size) > (uintptr_t)kdbgmm_top_of_heap)
		for(;;);
	return addr;
}


/**
 * Allocates a new page of heap space to the caller
 */
void *kdbgmm_alloc_page() 
{
	return kdbgmm_alloc_page_alligned(PHYSMM_PAGE_SIZE);
}

/**
 * Releases a block of memory so it can be reallocated
 */
void  kdbgmm_free(void *address, size_t size) 
{
	kdbgmm_free_internal(address, size);
}

/**
 * Initializes the heap memory manager
 * @param heap_start The start of the heap
 */
void  kdbgmm_init(void *heap_start, size_t size)
{
	llist_create(&kdbgmm_free_blocks_list);
	kdbgmm_top_of_heap = (void *) ( ((uintptr_t)heap_start) + ((uintptr_t) size) );
	kdbgmm_free_internal(heap_start, size);
}

