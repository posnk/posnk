/**
 * util/mruc.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 14-08-2016 - Original version
 */

#ifndef __UTIL_MRUC_H__
#define __UTIL_MRUC_H__

#include <stdint.h>
#include "util/llist.h"

/**
 * typedef for mruc_e
 */
typedef struct mruc_e	   mruc_e_t;
typedef struct mruc		   mruc_t;

/**
 * Evictor function prototype
 * this function will be called when the cache is flushed or when
 * the cache is too full to add a new one.
 */
typedef void (*mruc_k_t)( mruc_e_t * );

/**
 * MRU Cache structure
 */
struct mruc {

	/** The actual hash table */
	mruc_e_t	**table; 
	
	/** The size of the table */
	int			  tsize;
	
	/** The number of entries currently cached */
	int			  count;
	
	/** The number of entries to keep cached */
	int			  csize;
	
	/** The MRU linked list */
	llist_t		  mrul;
	
	/** The eviction callback */
	mruc_k_t	  evictor;
	
};

/**
 * MRU Cache entry, include this as first member of a struct that
 * will be added to a cache
 */ 
struct mruc_e {

	/** Linked list link entry */
	llist_t		  mru_link;
	
	/** Link to cache */
	mruc_t 		 *cache;
	
	/** Bucket links */
	mruc_e_t	 *bkt_next;
	mruc_e_t	 *bkt_prev;

	/** Hash */
	uint64_t	  hash;

};

/**
 * Gets the least recently used entry in the cache
 */
mruc_e_t *mruc_get_lru( mruc_t *list );

/**
 * Gets the most recently used entry in the cache
 */
mruc_e_t *mruc_get_mru( mruc_t *list );

/**
 * Create a new mru cache
 * @param cache		The block of memory to create a cache in
 * @param csize		The size of the cache.
 * @param tsize		The size of the hashtable
 * @param evictor	The function to call on eviction.
 * @param table		The memory to use for the table.
 */
void mruc_create(	mruc_t		 *cache, 
					int			  csize, 
					int			  tsize, 
					mruc_k_t	  evictor,
					mruc_e_t	**table );
					
/**
 * Mark an entry as used.
 */
void mruc_bump( mruc_e_t *entry );

/**
 * Add an entry to the cache
 */
void mruc_add( mruc_t *cache, mruc_e_t *entry, uint64_t hash );

/**
 * Remove an entry from the cache
 */
void mruc_remove( mruc_e_t *entry );

/**
 * Gets an entry from the cache 
 */
mruc_e_t *mruc_get( mruc_t *cache, uint64_t hash );

/**
 * Evict all entries from the cache
 */
void mruc_flush ( mruc_t *cache );

#endif
