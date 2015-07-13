/**
 * @file util/mruc.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * @brief Implements a MRU cache management system
 * This library provides a MRU cache management system that is compatible with
 * the linked list library by reserving a linked list node head structure at the
 * start of the MRU entry head structure.
 * 
 * Changelog:
 * 12-07-2015 - Created
 */

#include "util/llist.h"
#include "util/mrucache.h"

#include <stddef.h>
#include <assert.h>

/**
 * Gets the first entry in a MRU list
 * @param list Pointer to the linked list head node
 * @return The first node in the linked list
 */
mrunode_t *mrucache_get_first(mrunode_t *list)
{
	assert(list != NULL);
	return list->mnext;
}

/**
 * Gets the last entry in a MRU list
 * @param list Pointer top the linked list head node
 * @return The last node in the linked list
 */
mrunode_t *mrucache_get_last(mrunode_t *list)
{
	assert(list != NULL);
	return list->mprev;
}

/**
 * Adds a node to the end of the MRU list
 * @param list  Pointer to the linked list head node
 * @param entry The node to add to the list
 */
void mrucache_add_end(mrunode_t *list, mrunode_t *entry)
{
	assert(list != NULL);
	assert(entry != NULL);
	entry->mprev		= list->mprev;
	entry->mnext 		= list;
	list->mprev->mnext	= entry;
	list->mprev			= entry;
}

/**
 * Unlinks a node from the MRU list
 * @param entry The node to unlink
 */
void mrucache_unlink(mrunode_t *entry)
{
	assert(entry != NULL);
	assert(	  (entry->mprev && entry->mnext) ||
			((!entry->mprev) && (!entry->mnext)));
	if (entry->mprev)
		entry->mprev->mnext = entry->mnext;
	if (entry->mnext)
		entry->mnext->mprev = entry->mprev;
	entry->mnext = 0;
	entry->mprev = 0;
}

/**
 * Signals the use of an entry and pushes it to front on the MRU list
 * @param entry		The entry that was used
 */
void mrucache_bump(mrunode_t *entry)
{
	assert ( entry != NULL );
	assert ( entry->mcache != NULL );
	mrucache_unlink( entry );
	mrucache_add_end( &(entry->mcache->head), entry );
}

/**
 * Gets the most recently used item from the cache
 * @param cache		The cache to operate on
 * @return			The most recently used entry in the cache
 */
mrunode_t *mrucache_get_mru ( mrucache_t *cache )
{
	assert ( cache != NULL );
	return mrucache_get_last( &(cache->head) );
}

/**
 * Gets the least recently used item from the cache
 * @param cache		The cache to operate on
 * @return			The least recently used entry in the cache
 */
mrunode_t *mrucache_get_lru ( mrucache_t *cache )
{
	assert ( cache != NULL );
	return mrucache_get_first( &(cache->head) );
}

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
void mrucache_create( 	mrucache_t *cache, 
						int max_entries, 
						mru_evict_t		*evict,
						mru_overflow_t	*overflow,
						void			*param
					)
{
	assert ( cache != NULL );
	assert ( evict != NULL );
	assert ( overflow != NULL );
	
	cache->evict			= evict;
	cache->overflow			= overflow;
	cache->param			= param;
	cache->max_entries		= max_entries;
	cache->num_entries		= 0;
	
	cache->head->mcache 	= cache;
	cache->head->mnext		= &(cache->head);
	cache->head->mprev		= &(cache->head);
	cache->head->node->next	= NULL;
	cache->head->node->prev	= NULL;	
	
}

/**
 * Adds an entry to the cache
 * @param cache			The mru cache to add the entry to
 * @param entry			The entry to add
 * @return				Whether the entry could be added: nonzero if successful
 */
int mrucache_add_entry( mrucache_t *cache, mrunode_t *entry )
{
	
	mrunode_t	*lru, *next_lru, *head_lru;
	
	assert ( cache != NULL );
	assert ( entry != NULL );
	
	/* Check if there is room in the cache */
	if ( cache->num_entries >= cache->max_entries ) {
		/* If not: try to evict the least recently used entry that can be */
		/* evicted */
		
		head_lru = &(cache->head);
		
		for ( lru = mrucache_get_lru( cache ); lru != head_lru; lru = next_lru){
			next_lru = lru->mnext;
			
			/* We can, evict the entry */
			if (!mrucache_del_entry(lru))
				/* Eviction failed, try the next entry */
				continue;
			
			/* Signal that we were successful */
			head_lru = NULL;
			
			break;
			
		}
		
		/* Check if we were able to evict at least one entry */
		if ( head_lru != NULL ) {
			
			/* If not, check if we may proceed on overflow */
			if ( !cache->overflow(cache, entry) )
				/* If we may not, return 0 to indicate failure */
				return 0;
			
		}
		
	}
	
	/* Bump the entry counter */
	cache->num_entries++;
	
	/* Set the cache pointer on the entry */
	entry->mcache = cache;
	
	/* Add the entry to the MRU list */
	mrucache_add_end( &(cache->head), entry );
	
	/* Return nonzero to indicate success */
	return 1;
	
}

/**
 * Removes an entry from the cache and list
 * @param entry 		The entry to remove
 * @return				Whether the item was successfully deleted, zero if faild
 */
int mrucache_del_entry( mrunode_t *entry )
{
	
	assert ( entry != NULL );
	assert ( entry->mcache != NULL );
	
	/* Remove the entry from the MRU list */
	mrucache_unlink( entry );
	
	/* Call the eviction routine */
	if (!entry->mcache->do_evict( entry )) {
		
		/* If eviction failed, restore the MRU list */
		mrucache_add_end( &(entry->mcache->head), entry );
		
		/* Return zero to indicate failure */
		return 0;
	}
	
	/* Return nonzero to indicate success */
	return 1;
}

