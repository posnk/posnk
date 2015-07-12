/**
 * util/mrucache.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 12-07-2015 - Created
 */

#ifndef __util_mrucache_h__
#define __util_mrucache_h__

/**
 * typedef for mrunode:
 * MRU Cache entry node prototype
 */
typedef struct mrunode	   mrunode_t;

/**
 * typedef for mrucache:
 * MRU Cache prototype
 */
typedef struct mrucache	   mrucache_t;

/**
 * Eviction test function
 * Usage: int function_evicttest(mrunode_t *node);
 * node is the node which is to be evicted
 * returns nonzero if the node can be evicted
 */
typedef int (*mru_evicttest_t)(mrunode_t *);

/**
 * Eviction function
 * Usage: int function_evict(mrunode_t *node);
 * node is the node which is to be evicted
 * returns nonzero if entry was successfully evicted
 */
typedef int (*mru_evict_t)(mrunode_t *);

/**
 * Overflow function
 * Usage: int function_overflow(mrucache_t *cache, mrunode_t *node);
 * cache is the cache the node was to be added to
 * node is the node which was added
 * returns nonzero if the node must still be added
 */
typedef int (*mru_overflow_t)(mrucache_t *, mrunode_t *);

/**
 * MRU cache descriptor structure
 */
struct mrucache {
	mrunode_t		 head;
	int				 max_entries;
	int				 num_entries;
	mru_evicttest_t	*evict_test;
	mru_evict_t		*do_evict;
	mru_overflow_t	*overflow;
	void			*param;
};

/**
 * MRU cache header structure prototype
 * To use mru caches, create a structure
 * containing a mrunode_t as its first member.
 */
struct mrunode {
	/** Node for linking the tracked object into a list */
	llist_t			 node;
	mrunode_t		*mnext;
	mrunode_t		*mprev;
	mrucache_t		*mcache;
};

/**
 * Initializes a new mrucache
 * @param cache			A pointer to the uninitialized cache structure
 * @param max_entries	The maximal number of entries that the cache can hold
 * @param evict_test	The function that determines whether an entry may be
 * 						evicted
 * @param evict			The function that will evict a member from the cache
 * @param overflow		The function that will be called if the cache is full 
 * 						and no entries could be evicted
 * @param param			A parameter that may be used by the delegates operating
 * 						on this cache
 */
void mrucache_create( 	mrucache_t		*cache, 
						int				 max_entries, 
						mru_evicttest_t *evict_test,
						mru_evict_t		*evict,
						mru_overflow_t	*overflow,
						void			*param
					);

/**
 * Adds an entry to the cache
 * @param cache			The mru cache to add the entry to
 * @param entry			The entry to add
 * @return				Whether the entry could be added: nonzero if successful
 */
int mrucache_add_entry( mrucache_t *cache, mrunode_t *entry );

/**
 * Removes an entry from the cache and list
 * @param entry 		The entry to remove
 * @return				Whether the item was successfully deleted, zero if faild
 */
int mrucache_del_entry( mrunode_t *entry );

/**
 * Signals the use of an entry and pushes it to front on the MRU list
 * @param entry		The entry that was used
 */
void mrucache_bump(mrunode_t *entry);

/**
 * Gets the most recently used item from the cache
 * @param cache		The cache to operate on
 * @return			The most recently used entry in the cache
 */
mrunode_t *mrucache_get_mru ( mrucache_t *cache );
/**
 * Gets the least recently used item from the cache
 * @param cache		The cache to operate on
 * @return			The least recently used entry in the cache
 */
mrunode_t *mrucache_get_lru ( mrucache_t *cache );

#endif
