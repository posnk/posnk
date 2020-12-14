/**
 * kernel/blkcache.c
 *
 * Part of P-OS kernel.
 *
 * Implements a LRU cache for blocks
 *
 * TODO: Rewrite this to be more time-efficient (stop using llist)
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 06-06-2014 - Created
 * 01-07-2014 - Fully implemented, commented.
 */

#include "kernel/heapmm.h"
#include "kernel/blkcache.h"
#include "kernel/device.h"
#include <sys/types.h>
#include <sys/errno.h>
#include <string.h>
#include <assert.h>

/**
 * blkcache_create - Creates a new, empty block cache
 *
 * @param block_size The size of the blocks the cache will contain
 * @param max_entries The amount of blocks the cache should be limited to
 *
 * @return The new cache or NULL if there was no memory left
 */

blkcache_cache_t *blkcache_create( aoff_t block_size, int max_entries )
{
	blkcache_cache_t *cache = ( blkcache_cache_t * )
		heapmm_alloc( sizeof(blkcache_cache_t) );

	assert(cache != NULL);

	cache->max_entries = max_entries;
	cache->block_size = block_size;
	cache->entry_count = 0;
	semaphore_init(&cache->lock);

	/* Release lock */
	semaphore_up( &cache->lock );

	llist_create( ( llist_t * ) &( cache->block_list ) );

	return cache;
}

/**
 * blkcache_free - Destroys a cache and releases all memory associated with it
 *
 * @param cache The cache to destroy
 *
 * @return TRUE if the cache was successfully destroyed, FALSE if there are
 * 	dirty blocks left in the cache
 */

int blkcache_free( blkcache_cache_t *cache )
{
	blkcache_entry_t	*entry;
	llist_t			*_e;
	int			successful = 1;

	assert(cache != NULL);

	/* Acquire lock on cache */
	semaphore_down( &cache->lock );

	/* Iterate over the blocklist */
	for (_e = cache->block_list.next;
		_e != &( cache->block_list ) ;
		_e = cache->block_list.next ) {

		entry = ( blkcache_entry_t * ) _e;

		assert (entry != NULL);

		/* Check if the block is dirty */

		if (entry->flags & BLKCACHE_ENTRY_FLAG_DIRTY) {

			/* If so, skip this block and note that we have */
			/* dirty blocks left */
			successful = 0;
			continue;

		}

		/* Remove the block from the list */
		llist_unlink( _e );

		/* Release its data memory */
		heapmm_free( entry->data, cache->block_size );

		/* Release its descriptory */
		heapmm_free( entry, sizeof(blkcache_entry_t) );

	}

	/* Release lock */
	semaphore_up( &cache->lock );

	/* If there were no dirty blocks left, release the cache */
	if ( successful ) {
		heapmm_free( cache, sizeof( blkcache_cache_t ) );
	}

	/* Return whether we had any dirty blocks */
	return successful;
}

/**
 * blkcache_get_iterator - INTERNAL function that selects the block to be
 * returned by llist_iterate_select in blkcache_get_dirty
 *
 * @param node The block to be tested
 * @param param UNUSED
 *
 * @return TRUE when node starts at the specified offset, FALSE otherwise
 */

int blkcache_dirty_iterator ( llist_t *node, __attribute__(( unused ))
											void *param )
{
	blkcache_entry_t *block = (blkcache_entry_t *) node;

	/* If the block starts at the requested offset return TRUE. */
	return block->flags & BLKCACHE_ENTRY_FLAG_DIRTY;
}

/**
 * blkcache_find - Get the first dirty block from the cache
 *
 * @param cache The cache to get the block from
 *
 * @return The block that was requested, or NULL incase it is not in the cache
 */

blkcache_entry_t *blkcache_get_dirty( blkcache_cache_t *cache )
{
	/* Look up the block in the cache which satisfies the condition for */
	/* blkcache_dirty_iterator */

	assert (cache != NULL);

	return (blkcache_entry_t *)
		llist_iterate_select(	&(cache->block_list),
					&blkcache_dirty_iterator,
					NULL);

}

/**
 * blkcache_get_iterator - INTERNAL function that selects the block to be
 * returned by llist_iterate_select in blkcache_find
 *
 * @param node The block to be tested
 * @param param The offset we are testing the blocks for
 *
 * @return TRUE when node starts at the specified offset, FALSE otherwise
 */

int blkcache_get_iterator ( llist_t *node, void *param )
{
	blkcache_entry_t *block = (blkcache_entry_t *) node;

	/* If the block starts at the requested offset return TRUE. */
	return block->offset == (aoff_t) param;
}

/**
 * blkcache_find - Get a block from the cache
 *
 * @param cache The cache to get the block from
 * @param offset The offset of the block to get
 *
 * @return The block that was requested, or NULL incase it is not in the cache
 */

blkcache_entry_t *blkcache_find( blkcache_cache_t *cache, aoff_t offset )
{
	blkcache_entry_t *entry;

	/* Look up the block in the cache which satisfies the condition for */
	/* blkcache_get_iterator */

	assert (cache != NULL);

	/* Acquire lock on cache */
	semaphore_down( &cache->lock );

	entry = (blkcache_entry_t *)
		llist_iterate_select(	&(cache->block_list),
					&blkcache_get_iterator,
					(void *) offset);

	/* Release lock */
	semaphore_up( &cache->lock );

	return entry;

}

/**
 * blkcache_bump - Notify the cache of the usage of a block
 *
 * @param cache The cache to operate on
 * @param block The block that was used
 */

void blkcache_bump( blkcache_cache_t *cache, blkcache_entry_t *entry )
{
	/* The current implementation of the cache removes the first block  */
	/* in the list when full so to implement a LRU cache we simply move */
	/* the block to the end of the list */

	assert (cache != NULL);
	assert (entry != NULL);

	/* Acquire lock on cache */
	semaphore_down( &cache->lock );

	/* Remove the block from the list */
	llist_unlink((llist_t *) &entry);

	/* Re-add it at the end of the list */
	llist_add_end(&(cache->block_list), (llist_t *) entry);

	/* Release lock */
	semaphore_up( &cache->lock );
}

/**
 * blkcache_get_discard_candidate - Returns the block to be discarded next
 *
 * @param cache The cache to operate on
 *
 * @return The block to be discarded next
 */

blkcache_entry_t *blkcache_get_discard_candidate( blkcache_cache_t *cache )
{
	/* The current implementation of the cache removes the first block  */
	/* in the list when full so simply return that block */

	assert (cache != NULL);

	return (blkcache_entry_t *) cache->block_list.next;
}

/**
 * blkcache_get - Get a block from the cache or create it if
 * it does not exist yet.
 *
 * @param cache The cache to get the block from
 * @param offset The offset of the block to get/add
 *
 * @return The block that was requested, or NULL incase the cache was full and
 *	the block that was to be removed was dirty.
 * @error 2^32-1 is returned if there was not enough memory to add the block
 */

blkcache_entry_t *blkcache_get( blkcache_cache_t *cache, aoff_t offset )
{
	blkcache_entry_t	*entry;

	assert (cache != NULL);

	/* Attempt to get cached block */
	entry = blkcache_find( cache, offset );
	if ( entry )
		return entry;

	/* Block not cached, add new block */

	/* Acquire lock on cache */
	semaphore_down( &cache->lock );

	/* Enforce cache size limit */
	if ( cache->entry_count == cache->max_entries ) {
		/* Cache full, proceed to remove item */

		/* Get the block to be removed */
		entry = blkcache_get_discard_candidate( cache );

		assert (entry != NULL);

		/* Check whether block is dirty */
		if (entry->flags & BLKCACHE_ENTRY_FLAG_DIRTY) {
			/* Block was dirty, return NULL to indicate a flush */
			/* is required */

			/* Release lock */
			semaphore_up( &cache->lock );

			return NULL;
		}

		/* Actually remove block from the list */
		llist_unlink((llist_t *) entry);

		/* Proceed to reuse the block memory, reset descriptor */
		entry->offset = offset;
		entry->flags = 0;
		entry->access_count = 0;
		/* ... clear its data memory */
		memset(entry->data, 0, cache->block_size);

		/* And add it as the new block */
		llist_add_end(&(cache->block_list), (llist_t *) entry);

	} else {
		/* There is still room left in the cache */

		/* Allocate new descriptor */
		entry = (blkcache_entry_t *)
			heapmm_alloc( sizeof (blkcache_entry_t) );

		/* Handle out of memory errors */
		if ( !entry ) {
			/* TODO: Decide whether this should cause a block removal */
			/* instead of an error */

			/* Release lock */
			semaphore_up( &cache->lock );

			return BLKCACHE_ENOMEM;
		}

		/* ... fill its fields */
		entry->offset = offset;
		entry->flags = 0;
		entry->access_count = 0;

		/* Allocate its data memory and */
		entry->data = heapmm_alloc_alligned( cache->block_size, cache->block_size );

		/* Handle out of memory errors */
		if ( !entry->data ) {
			heapmm_free( entry, sizeof(blkcache_entry_t) );

			/* Release lock */
			semaphore_up( &cache->lock );

			return BLKCACHE_ENOMEM;
		}

		/* ... clear it */
		memset(entry->data, 0, cache->block_size);

		/* Add the new block */
		cache->entry_count++;
		llist_add_end(&(cache->block_list), (llist_t *) entry);

	}

	/* Release lock */
	semaphore_up( &cache->lock );

	return entry;
}
