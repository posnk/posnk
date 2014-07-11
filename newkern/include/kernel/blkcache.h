/**
 * kernel/blkcache.h
 *
 * Part of P-OS kernel.
 *
 * Implements a LRU cache for blocks
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 06-06-2014 - Created
 * 01-07-2014 - Fully implemented, commented.
 */

#ifndef __KERNEL_BLKCACHE_H__
#define __KERNEL_BLKCACHE_H__

#include <sys/types.h>
#include "util/llist.h"

#define BLKCACHE_ENTRY_FLAG_DIRTY	( 1<<1 )

#define BLKCACHE_ENOMEM			( (blkcache_entry_t *) 0xFFFFFFFF )

struct blkcache_cache {
	int	 entry_count;
	int	 max_entries;
	aoff_t	 block_size;
	llist_t	 block_list;
};

struct blkcache_entry {
	llist_t	 link;
	aoff_t	 offset;
	int	 flags;
	int	 access_count;
	void	*data;
};

typedef struct blkcache_cache blkcache_cache_t;
typedef struct blkcache_entry blkcache_entry_t;

/**
 * blkcache_create - Creates a new, empty block cache
 *
 * @param block_size The size of the blocks the cache will contain
 * @param max_entries The amount of blocks the cache should be limited to
 *
 * @return The new cache or NULL if there was no memory left
 */

blkcache_cache_t *blkcache_create( aoff_t block_size, int max_entries );

/**
 * blkcache_free - Destroys a cache and releases all memory associated with it
 *
 * @param cache The cache to destroy
 *
 * @return TRUE if the cache was successfully destroyed, FALSE if there are 
 * 	dirty blocks left in the cache
 */
 
int blkcache_free( blkcache_cache_t *cache );

/**
 * blkcache_find - Get a block from the cache
 *
 * @param cache The cache to get the block from
 * @param offset The offset of the block to get
 *
 * @return The block that was requested, or NULL incase it is not in the cache
 */

blkcache_entry_t *blkcache_find( blkcache_cache_t *cache, aoff_t offset );
/**
 * blkcache_bump - Notify the cache of the usage of a block
 *
 * @param cache The cache to operate on
 * @param block The block that was used
 */

void blkcache_bump( blkcache_cache_t *cache, blkcache_entry_t *entry );

/**
 * blkcache_get_discard_candidate - Returns the block to be discarded next
 *
 * @param cache The cache to operate on
 *
 * @return The block to be discarded next
 */

blkcache_entry_t *blkcache_get_discard_candidate( blkcache_cache_t *cache );

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

blkcache_entry_t *blkcache_get( blkcache_cache_t *cache, aoff_t offset );

#endif
