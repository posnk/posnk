/**
 * kernel/heapmm.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 28-03-2014 - Created
 */

#include "kernel/physmm.h"
#include "kernel/heapmm.h"
#include "kernel/earlycon.h"
#include "kernel/scheduler.h"
#include "kdbg/dbgapi.h"
#include <stdint.h>
#include <string.h>

#define FALSE (0)

#define TRUE (1)

/**
 * Heap memory manager free block list head pointer
 */
llist_t 	 heapmm_free_blocks_list;

/**
 * Pointer to the top of the heap
 */
void       	*heapmm_top_of_heap;

/**
 * Keep track of 
 */
size_t		 heapmm_space_left = 0;

void heapmm_free_internal(void *address, size_t size);

void *heapmm_alloc_internal(size_t size, int morecore);

/**
 * Iterator function that tests for empty blocks
 */
int heapmm_morecore_iterator (llist_t *node, __attribute__((__unused__)) void *param)
{
	heapmm_block_t *block = (heapmm_block_t *) node;
	return block->size == 0;
}

/**
 * Internal function
 * Requests more memory and adds it to the free block table,
 * then cleans up low space markers
 */
void heapmm_morecore ( size_t size )
{
	
	heapmm_block_t *found_block;

	//TODO: Get lock on heapmm_top_of_heap
	
	size = heapmm_request_core ( heapmm_top_of_heap, size );

	//TODO: Handle out of memory error
	
	/* Add new core to free block table  */
	heapmm_free_internal ( heapmm_top_of_heap, size );
 
	/* Update top of heap pointer */
	heapmm_top_of_heap = ( void * ) ( ((uintptr_t)heapmm_top_of_heap) + ((uintptr_t)size));

	//TODO: Release lock on heapmm_top_of_heap

	//TODO: Get lock on the free block table

	/* Free small chunks of heap for which no block info's could be made */
/*
	for (	found_block = (heapmm_block_t *)
			llist_iterate_select(&heapmm_free_blocks_list,
					&heapmm_morecore_iterator, NULL);
		found_block != NULL;
		found_block = (heapmm_block_t *)
			llist_iterate_select(&heapmm_free_blocks_list,
					&heapmm_morecore_iterator, NULL)	) {
		/* END OF MESSY FOR STATEMENT */

		/* Unlink tag block info */
		//llist_unlink((llist_t *) found_block);

		//TODO: Release lock on the free block table

		/* Free its memory * /
		//heapmm_free_internal ( (void *) found_block , sizeof (heapmm_block_t) );	

		//TODO: Get lock on the free block table

	}*/

	//TODO: Release lock on the free block table
	
}

/**
 * Iterator function that tests for [our block][FREE] mergeable blocks
 */
int heapmm_merge_start_iterator (llist_t *node, void *param)
{
	heapmm_block_t *block = (heapmm_block_t *) node;
	return (block->start == param) && !(block->size == 0);
}

/**
 * Iterator function that tests for [FREE][our block] mergeable blocks
 */
int heapmm_merge_end_iterator (llist_t *node, void *param)
{
	heapmm_block_t *block = (heapmm_block_t *) node;
	void 		   *end_address;
	end_address = (void *) ( ((uintptr_t)block->start) + ((uintptr_t) block->size) );
	return (end_address == param) && !(block->size == 0);
}

/**
 * Releases a block of memory so it can be reallocated
 */
void heapmm_free_internal(void *address, size_t size)
{
	heapmm_block_t  *found_block;
	heapmm_block_t  *merge_block;
	void 		*end_address;
	end_address = (void *) ( ((uintptr_t)address) + ((uintptr_t) size) );
	memset(address, 0x42, size);

	//TODO: Get a lock on the free block table
	if(address != heapmm_top_of_heap)
	dbgapi_unreg_memuse(address,size);
	found_block = (heapmm_block_t *)
		llist_iterate_select(&heapmm_free_blocks_list,
							&heapmm_merge_start_iterator,
							address);
	if (found_block) debugcon_printf("DOUBLEFREE!!!!!!!\n");

	/* Try to find a mergeable block */
	/* First, try to find block [FREE][our block] */
	found_block = (heapmm_block_t *)
		llist_iterate_select(&heapmm_free_blocks_list,
							&heapmm_merge_end_iterator,
							address);
	if (found_block != NULL) {
		/* Found it! */
		/* Proceed and merge! */
		found_block->size += size;
	} else {
		/* No fitting free block was found */
		/* Now, try to find block [our block][FREE] */
		found_block = (heapmm_block_t *)
			llist_iterate_select(&heapmm_free_blocks_list,
								 &heapmm_merge_start_iterator,
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

			found_block = (heapmm_block_t *)
				heapmm_alloc_internal(sizeof(heapmm_block_t), FALSE);
			if (found_block == NULL) {
				/* No free blocks large enough to contain block info */
				/* Time to get hacky */
				if (size >= sizeof(heapmm_block_t)) {
					/* A block info structure will fit in this free space */
					/* Make the new block info at the start of the free space */
					found_block = (heapmm_block_t *) address;
					found_block->size = size - sizeof(heapmm_block_t);
					found_block->start =
						(void *) ( ((uintptr_t) address) +
								((uintptr_t) sizeof(heapmm_block_t)) );
					/* If the block is just large enough for the info struct   */
					/* the block will still be created but with a size of 0,   */
					/* if more core is added later on it will be freed and if  */
					/* another block is freed next to it it will be used as    */
					/* the block info for that block 						   */
				
					//TODO: Get a lock on the free block table
					dbgapi_register_memuse(found_block, sizeof(heapmm_block_t));
					llist_add_end(&heapmm_free_blocks_list , (llist_t *) found_block);

				} else {
					/* so, its a small block and we can't store info about it? */
					/* just request more core already! */
					
					heapmm_morecore(sizeof(heapmm_block_t));

					/* Try to free the block */
					heapmm_free_internal(address, size);
					
					return;
				}
			} else {
				found_block->start = address;
				found_block->size  = size;

				//TODO: Get a lock on the free block table

				llist_add_end(&heapmm_free_blocks_list, (llist_t *) found_block);
			}
		}
	}

	end_address = (void *) ( ((uintptr_t) found_block->start ) + ((uintptr_t) found_block->size) );

	merge_block = (heapmm_block_t *)
		llist_iterate_select(&heapmm_free_blocks_list,
							&heapmm_merge_start_iterator,
							end_address);

	if ( merge_block != NULL ) {
		/* Next block can also be merged */
		found_block->size += merge_block->size;

		//llist_unlink ((llist_t *) merge_block);
		merge_block->size = 0;		

		//TODO: Release a lock on the free block table

		//heapmm_free_internal( (void *) merge_block, sizeof(heapmm_block_t) );

		//TODO: Get a lock on the free block table
	}

	merge_block = (heapmm_block_t *)
		llist_iterate_select(&heapmm_free_blocks_list,
							&heapmm_merge_end_iterator,
							found_block->start);

	if ( merge_block != NULL ) {

		/* Previous block can also be merged */
		merge_block->size += found_block->size;
		found_block->size = 0;
//		llist_unlink ((llist_t *) found_block);		

		//TODO: Release a lock on the free block table

		//heapmm_free_internal( (void *) found_block, sizeof(heapmm_block_t) );
	}
	heapmm_space_left+=size;

}

/**
 * Iterator function that tests for blocks equal the size in param
 */
int heapmm_alloc_match_iterator (llist_t *node, void *param)
{
	heapmm_block_t *block = (heapmm_block_t *) node;
	size_t 			size  = (size_t) param;
	if ((((uintptr_t)block)+sizeof(heapmm_block_t)) > (uintptr_t)heapmm_top_of_heap)
		for(;;);
	
	return block->size == size;
}

/**
 * Iterator function that tests for blocks able to contain the size in param
 */
int heapmm_alloc_fit_iterator (llist_t *node, void *param)
{
	heapmm_block_t *block = (heapmm_block_t *) node;
	size_t 			size  = (size_t) param;
	return block->size >= size;
}

/**
 * Allocates a new block of memory of given size to the caller
 */
void *heapmm_alloc_internal(size_t size, int morecore)
{
	heapmm_block_t *found_block = NULL;
	void 		   *address;
	if (((heapmm_space_left - size) < 1024) && morecore)
		heapmm_morecore(PHYSMM_PAGE_SIZE);
		
	//TODO: Get a lock on the free block table

	/* Try to find fitting free block */
	if (size == sizeof(heapmm_block_t)) {		
		found_block = (heapmm_block_t *)
			llist_iterate_select(&heapmm_free_blocks_list,
								 &heapmm_morecore_iterator,
								 NULL);
		if (found_block) {
			llist_unlink((llist_t *) found_block);
			return found_block;
		}
	}
	
	if (found_block == NULL)
	found_block = (heapmm_block_t *)
		llist_iterate_select(&heapmm_free_blocks_list,
							 &heapmm_alloc_match_iterator,
							 (void *) size);
	if (found_block == NULL) {
		/* No fitting free block was found */
		found_block = (heapmm_block_t *)
			llist_iterate_select(&heapmm_free_blocks_list,
								 &heapmm_alloc_fit_iterator,
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

			if (size != sizeof(heapmm_block_t)) {
				/* NOPE, RETURN */
				return NULL;
			}

			found_block = (heapmm_block_t *)
				llist_iterate_select(&heapmm_free_blocks_list,
									 &heapmm_morecore_iterator,
									 NULL);

			if (found_block != NULL) {
				llist_unlink( (llist_t *) found_block);
				return (void *) found_block;
			}

			return NULL;

		}

		/* Request more core */

		//TODO: Release a lock on the free block table
		
		heapmm_morecore(size);

		return heapmm_alloc_internal(size, morecore);
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
		//llist_unlink((llist_t *) found_block);
		found_block->size = 0;
		//TODO: Release a lock on the free block table
		//if (morecore){//HACKHACKHACK
		//	void *test = heapmm_alloc_internal(3*sizeof(heapmm_block_t), TRUE);
		//	heapmm_free_internal(test,3*sizeof(heapmm_block_t));
		//}
		//heapmm_free_internal( (void *) found_block, sizeof(heapmm_block_t) );
	}

	
	memset(address, 0x23, size);
	dbgapi_register_memuse(address,size);
	heapmm_space_left-=size;
	//if (heapmm_space_left < 512)
	//	heapmm_morecore(PHYSMM_PAGE_SIZE);
	return address;
}

typedef struct heapmm_alignreq {
	size_t		size;
	uintptr_t	align;
} heapmm_alignreq_t;

/**
 * Iterator function that tests for aligned, matching sized blocks
 */
int heapmm_alloc_page_match_iterator	 (llist_t *node, void *param)
{
	heapmm_block_t *block  = (heapmm_block_t *) node;
	heapmm_alignreq_t *req = (heapmm_alignreq_t *) param;
	uintptr_t		inpage = (uintptr_t) block->start;
	inpage &= req->align - 1;			
	return (inpage == 0) && (block->size == req->size);
}

/**
 * Iterator function that tests for blocks large enough to contain a
 * whole page.
 */
int heapmm_alloc_page_split_iterator (llist_t *node, void *param)
{
	heapmm_block_t *block  = (heapmm_block_t *) node;
	heapmm_alignreq_t *req = (heapmm_alignreq_t *) param;
	uintptr_t		inpage = (uintptr_t) block->start;
	size_t			reqsize;
	inpage &= req->align - 1;			
	reqsize = req->size + (size_t)(req->align - inpage);
	return block->size >= reqsize;
}

/**
 * Iterator function that tests for page aligned, larger than page sized blocks
 */
int heapmm_alloc_page_fit_iterator (llist_t *node, void *param)
{
	heapmm_block_t *block  = (heapmm_block_t *) node;
	heapmm_alignreq_t *req = (heapmm_alignreq_t *) param;
	uintptr_t		inpage = (uintptr_t) block->start;
	inpage &= req->align - 1;			
	return (inpage == 0) && (block->size >= req->size);
}

/**
 * Allocates a page alligned block of RAM
 */
void *heapmm_alloc_page_alligned(size_t size)
{
	return heapmm_alloc_alligned(size, PHYSMM_PAGE_SIZE);
}


/**
 * Allocates an alligned block of RAM
 */
void *heapmm_alloc_alligned(size_t size, uintptr_t alignment)
{
	heapmm_block_t *found_block;
	heapmm_alignreq_t alignreq;
	void 		   *address;
	void		   *pad_address;
	uintptr_t		in_page,alignmask;
	if (((heapmm_space_left - size) < 1024))
		heapmm_morecore(PHYSMM_PAGE_SIZE);
	alignmask = alignment - 1;
	alignreq.size = size;
	alignreq.align = alignment;

	//TODO: Get a lock on the free block table
	
	/* Try to find fitting, aligned free block */
	found_block = (heapmm_block_t *)
		llist_iterate_select(&heapmm_free_blocks_list,
							 &heapmm_alloc_page_fit_iterator,
							 &alignreq);
	if (found_block == NULL) {
		/* No fitting, aligned free block was found */
		found_block = (heapmm_block_t *)
			llist_iterate_select(&heapmm_free_blocks_list,
								 &heapmm_alloc_page_match_iterator,
								 &alignreq);
	}
	if (found_block == NULL) {
		/* No aligned free block was found */
		found_block = (heapmm_block_t *)
			llist_iterate_select(&heapmm_free_blocks_list,
								 &heapmm_alloc_page_split_iterator,
								 &alignreq);
	} 
	if (found_block == NULL) {
		/* No aligned or alignable free block was found */		

		/* Request more core */

		//TODO: Release a lock on the free block table
		
		heapmm_morecore(size);

		return heapmm_alloc_alligned(size, alignment);
	}

	pad_address = address = found_block->start;
	
	in_page = ((size_t) address) & alignmask;

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

			heapmm_free_internal( (void *) found_block, sizeof(heapmm_block_t) );
		}
	} else {
		/* Block is not aligned */

		/* Proceed to shrink block */
 		size_t padding = (size_t) (alignment - in_page);
		found_block->start = (void *) ( ((uintptr_t)found_block->start) + ((uintptr_t) padding) + ((uintptr_t) size));
		found_block->size -= padding + ((size_t) size);
		
		address = (void *) ( ((uintptr_t)address) + ((uintptr_t) padding));

		//TODO: Release a lock on the free block table

		/* Free padding */
		dbgapi_register_memuse( pad_address, padding );
		heapmm_free_internal ( pad_address, padding );

	}

	dbgapi_register_memuse(address,size);
	heapmm_space_left-=size;
	if (heapmm_space_left < 512)
		heapmm_morecore(PHYSMM_PAGE_SIZE);
	return address;
}



/**
 * Allocates a new block of memory of given size to the caller
 */
void *heapmm_alloc(size_t size) 
{
	void *addr =  heapmm_alloc_internal(size, TRUE);
	if (((uintptr_t)addr) < 0xD0000000)
		earlycon_printf("\nalloc(%i) = 0x%x\n",size,addr);
	if ((((uintptr_t)addr)+size) > (uintptr_t)heapmm_top_of_heap)
		for(;;);
	return addr;
}


/**
 * Allocates a new page of heap space to the caller
 */
void *heapmm_alloc_page() 
{
	return heapmm_alloc_page_alligned(PHYSMM_PAGE_SIZE);
}

/**
 * Releases a block of memory so it can be reallocated
 */
void  heapmm_free(void *address, size_t size) 
{
	heapmm_free_internal(address, size);
}

/**
 * Initializes the heap memory manager
 * @param heap_start The start of the heap
 * @param size       The initial size of the heap
 */
void  heapmm_init(void *heap_start, size_t size)
{
	llist_create(&heapmm_free_blocks_list);
	heapmm_top_of_heap = heap_start;
	heapmm_free_internal(heap_start, size);
	heapmm_top_of_heap = (void *) ( ((uintptr_t)heap_start) + ((uintptr_t) size) );
}

