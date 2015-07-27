/**
 * @file kernel/vfs/ndcache.c
 *
 * Implements the directory cache and GC
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 15-07-2015 - Created
 */

/* Includes */

#include <assert.h>

#include "util/llist.h"

#include "kernel/vfs.h"

#include "kernel/heapmm.h"

/* Global Variables */

mrucache_t			dcache_mru;

/* Internal type definitions */

/* Class implementation templates */

/* Public Functions */

int dcache_evict(mrunode_t *node)
{
	
	Directory *dir = (Directory *) node;
	llist_t *_link, *_next, *_list;
	FileLink *link;
		
	/* If there is a filesystem mounted here, do not evict it */
	if ( dir->mount != NULL )
		return 0;
		
	/* If this directory is a filesystem root, do not evict it */
	if ( dir->parent == NULL )
		return 0;
			
	/* If there are still open handles to this dir, do not evict it */
	if ( dir->hndcount != 0 )
		return 0;
		
	/* If there are still references to this dir, do not evict it */
	if ( dir->refcount != 0 )
		return 0;	
	
	/* If there are still cached subdirectories, do not evict it */
	if ( !llist_is_empty ( &dir->subdirectories ) )
		return 0;	
	
	/* If there are still cached links, evict them */
	_list = (llist_t *) &dir->files;
	_next = (llist_t *) _list->next;
	for (_link = _next; _link != _list; _link = _next ) {
		_next = (llist_t *) _link->next;
		link = (FileLink *) _link;

		/* Destroy the link */
		SOMCALL(link, destroy); //TODO: Handle errors
		
	}

	//TODO: Check dirty flag and if set, store any changes

	/* Remove the directory from it's parents subdirectory list */
	llist_unlink ( (llist_t *) dir );

	/* Remove the file from the dcache */
	mrucache_del_entry( node );

}

int dcache_overflow(mrucache_t *cache, mrunode_t *node)
{
	
	//TODO: Log directory cache overflow
	
	/* Superuser must never get ENFILE */
	return is_superuser();
	
}

SVFUNC( dcache_initialize, int max_entries )
{
	
	mrucache_create(	&dcache_mru, 
						max_entries, 
						dcache_evict, 
						dcache_overflow,
						NULL
					);
	
	RETURNV;
	
}

/**
 * @note 	This function must only be called inside a lock on directory and its
 * 			parent
 */
SVFUNC( dcache_add_dir, Directory *dir )
{
	assert ( dir != NULL );
	
	/* Don't cache filesystem roots */
	if ( dir->parent != NULL ) {
		
		/* Add it to the parent's subdirectory list */
		llist_add_end( &dir->parent->subdirectories, (llist_t) dir );
	
		/* Add it to the mru cache */
		if (!mrucache_add_entry( &dcache_mru, (mrunode_t *) dir )) {
			
			/* Unlink the file from the subdirectory list */
			llist_unlink( (llist_t *) dir );
			
			/* If we couldn't add it, return ENFILE */
			THROWV(ENFILE);
			
		}
		
	}
	
	/* Exit successfully */
	RETURNV;
	
} 

/**
 * @note This function must only be called inside a lock on dir
 */
SFUNC( Directory *, dcache_find_subdir, Directory *dir, hname_t *name )
{
	llist_t *_sdir, *_next, *_list;
	Directory *sdir;
	
	assert ( dir != NULL );
	
	/* Iterate over all links */
	_list = (llist_t *) &dir->files;
	_next = (llist_t *) _list->next;
	for (_sdir = _next; _sdir != _list; _sdir = _next ) {
		_next = (llist_t *) _sdir->next;
		sdir = (Directory *) _sdir;

		/* Compare names */
		if ( hname_compare( sdir->name, name ) )
			RETURN(sdir);
		
	}
	
	THROW(ENOENT, NULL);
	
}

/**
 * @note This function must never be called inside a lock on dir
 */
Directory * directory_ref( Directory *dir )
{
	
	assert ( dir != NULL );
	
	/* Acquire a lock on dir */
	semaphore_down( dir->lock );
	
	/* Bump directory reference count */
	dir->refcount++;
	
	/* Release lock on dir */	
	semaphore_up( dir->lock );
	
	return dir;
	
}

/**
 * @note This function must never be called inside a lock on dir
 */
void directory_release( Directory *dir )
{
	
	assert ( dir != NULL );
	
	/* Acquire a lock on dir */
	semaphore_down( dir->lock );
	
	/* Bump directory reference count */
	dir->refcount--;
	
	/* Release lock on dir */	
	semaphore_up( dir->lock );
	
}




