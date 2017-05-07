/**
 * util/mrul.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 14-07-2016 - Original version
 */

#include "util/llist.h"
#include "util/mruc.h"

#include <stddef.h>
#include <string.h>
#include <assert.h>

mruc_e_t *mruc_get_lru( mruc_t *cache )
{

	assert( cache != NULL );

	return ( mruc_e_t * ) llist_get_first( &cache->mrul );	
	
}

mruc_e_t *mruc_get_mru( mruc_t *cache )
{

	assert( cache != NULL );

	return ( mruc_e_t * ) llist_get_last( &cache->mrul );	
	
}

void mruc_create(	mruc_t		 *cache, 
					int			  csize, 
					int			  tsize, 
					mruc_k_t	  evictor,
					mruc_e_t	**table )
{
	
	assert( cache != NULL );
	
	cache->csize	= csize;
	cache->tsize	= tsize;
	cache->evictor	= evictor;
	cache->table	= table;
	cache->count    = 0;
	
	memset( cache->table, 0, sizeof( mruc_e_t * ) * tsize );
	
	llist_create( &cache->mrul );
	
}

void mruc_bump( mruc_e_t *entry )
{

	mruc_t *cache;

	assert( entry != NULL );
	
	cache = entry->cache;
	
	assert( cache != NULL );

	//TODO: Does this need locking?
	llist_unlink ( ( llist_t *) entry );
	llist_add_end( &cache->mrul, ( llist_t *) entry );
	
}

void mruc_add( mruc_t *cache, mruc_e_t *entry, uint64_t hash )
{

	mruc_e_t **bkt;

	assert( cache != NULL );
	assert( entry != NULL );
	
	//TODO: Figure out what to do when the entry is not unique.
	
	entry->cache = cache;
	entry->hash  = hash;
	
	bkt = &cache->table[ hash & ( cache->tsize - 1 )  ];
	
	if ( *bkt ) {
	
		entry->bkt_next             = *bkt;
		entry->bkt_prev             = (*bkt)->bkt_prev;
		(*bkt)->bkt_prev->bkt_next  = entry;
		(*bkt)->bkt_prev            = entry;
		
	} else { 
	
		entry->bkt_next = entry;
		entry->bkt_prev = entry;
		
		(*bkt) = entry;
	
	}
	
	llist_add_end( &cache->mrul, ( llist_t * ) entry );	
	
	cache->count++;
	
	if ( cache->count > cache->csize ) {
		debugcon_printf("evictor:0x%08x(0x%08x)\n",cache->evictor,cache);
		cache->evictor( mruc_get_lru( cache ) );
	}
	
}

void mruc_remove( mruc_e_t *entry )
{

	uint64_t hash;
	mruc_t *cache;

	assert( entry != NULL );
	
	cache = entry->cache;
	
	assert( cache != NULL );
	
	hash = entry->hash;
	

	llist_unlink( ( llist_t * ) entry );
	
	if ( entry->bkt_next == entry ) {
		
		cache->table[ hash & ( cache->tsize - 1 )  ] = NULL;
		
	} else {
	
		entry->bkt_next->bkt_prev = entry->bkt_prev;
		entry->bkt_prev->bkt_next = entry->bkt_next;
		
		if ( cache->table[ hash & ( cache->tsize - 1 ) ] == entry )
			cache->table[ hash & ( cache->tsize - 1 ) ] = entry->bkt_next;
		
	}
	
	entry->cache->count--;
	entry->cache	= NULL;
	entry->bkt_next = NULL;
	entry->bkt_prev = NULL;

}

mruc_e_t *mruc_get( mruc_t *cache, uint64_t hash ) {

	mruc_e_t *entry;
	
	assert( cache != NULL );
	
	mruc_e_t *bkt = cache->table[ hash & ( cache->tsize - 1 )  ];

	entry = bkt;
	
	if ( entry == NULL )
		return NULL;
	
	while ( entry->hash != hash ) {
	
		entry = entry->bkt_next;
	
		if ( entry == bkt )
			return NULL;
	
	}

	return entry;

}

void mruc_flush ( mruc_t *cache ) {

	mruc_e_t *entry;
	
	while ( entry = mruc_get_lru( cache ) ) {
		
		debugcon_printf("evictor:0x%08x(0x%08x)\n",cache->evictor,cache);
		cache->evictor( entry );
		
	}

}
