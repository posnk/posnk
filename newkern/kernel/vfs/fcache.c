/**
 * @file kernel/vfs/fcache.c
 *
 * Implements the file cache and file GC
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 13-07-2015 - Created
 */

/* Includes */

#include <assert.h>

#include "util/llist.h"

#include "kernel/vfs.h"

#include "kernel/heapmm.h"

/* Global Variables */

mrucache_t			fcache_mru;

/* Internal type definitions */

/* Class implementation templates */

SOMIMPL( FileLink, flink_destroy );
class_impl( FileLink, flink_impl ) {
	method_impl( destroy, flink_destroy),
}

/* Public Functions */

int fcache_evict(mrunode_t *node)
{
	
	File *file = (File *) node;
	FileLink *link;
	flinkback_t *_link, *_next, *_list;
	
	/* If there are still references to this file, do not evict it */
	if ( file->refcount != 0 )
		return 0;
	
	/* If there are still open handles to this file, do not evict it */
	if ( file->hndcount != 0 )
		return 0;
		
	/* Iterate over the links to this file */
	_list = (flinkback_t *) &(file->links);
	_next = (flinkback_t *) _list->node.next;
	for (_link = _next; _link != _list; _link = _next ) {
		_next = (flinkback_t *) _link->node.next;
		link = _link->link;

		/* Destroy the link */
		SOMCALL(link, destroy); //TODO: Handle errors
		
	}

	/* Remove the file from the filesystem file list */
	llist_unlink ( (llist_t *) file );

	/* Remove the file from the fcache */
	mrucache_del_entry( node );

	/* Destroy the file object (free its memory, store any updates) */
	SOMCALL(file, destroy); //TODO: Handle errors

}

int fcache_overflow(mrucache_t *cache, mrunode_t *node)
{
	
	//TODO: Log file cache overflow
	
	/* Superuser must never get ENFILE */
	return is_superuser();
	
}

SVFUNC( fcache_initialize, int max_entries )
{
	
	mrucache_create(	&fcache_mru, 
						max_entries, 
						fcache_evict, 
						fcache_overflow,
						NULL
					);
	
	RETURNV;
	
}

SOMIMPL( FileLink, flink_destroy )
{
	
	/* Acquire lock on the file we point to so we can safely modify it's link
	 * list and link counter */
	semaphore_down( _this->file->lock );
	
	/* Remove the linkback from the file */
	llist_unlink( &(_this->linkback) );
	
	/* Release the lock on the file */
	semaphore_up ( _this->file->lock );
	
	/* Acquire lock on the parent directory so we can decrease it's reference 
	 * count
	 */
	semaphore_down( _this->parent->lock );
	
	/* Remove the link from the list of our parent directory */
	llist_unlink( (llist_t *) _this );
	
	/* Release the lock on the parent */
	semaphore_up ( _this->parent->lock );
	
	/* Deallocate link */
	heapmm_free ( _this, sizeof( FileLink ) );
	
	RETURNV;
	
}

/**
 * @note This function must only be called inside a lock on file and parent dir
 */
SVFUNC( fcache_add_link, Directory *parent, File *file, hname_t *name )
{
	
	FileLink *link;

	assert ( parent != NULL );
	assert ( file != NULL );
	
	/* Allocate and preinit link */
	link = heapmm_alloc( sizeof( FileLink ) );
	
	/* Check whether we ran out of memory */
	if ( link == NULL )
		THROWV( ENOMEM );
	
	class_init( link, flink_impl );
	
	/* Fill link fields */
	link->name	 = name;
	link->file	 = file;
	link->linkback.link = link;
	
	/* Attach link to file's link list */
	llist_add_end( &(file->links), (llist_t *) &(link->linkback) );
	
	/* Attach link to the parent's link list */
	llist_add_end (&(parent->files), (llist_t *) link);
	
} 

/**
 * @note This function must only be called inside a lock on file
 */
SVFUNC( fcache_add_file, File *file )
{
	assert ( file != NULL );
	assert ( file->fs != NULL );
	
	semaphore_down(file->fs->lock);
	
	/* Attach file to filesystem's file list */
	llist_add_end( &(file->fs->files), (llist_t *) file );
	
	/* Add it to the mru cache */
	if (!mrucache_add_entry( &fcache_mru, (mrunode_t *) file )) {
		
		/* Unlink the file from the file list of the fs */
		llist_unlink( (llist_t *) file );
	
		semaphore_up(file->fs->lock);
		
		/* If we couldn't add it, return ENFILE */
		THROWV(ENFILE);
		
	}
	
	semaphore_up(file->fs->lock);
	
	/* Exit successfully */
	RETURNV;
	
} 

/**
 * @note This function must only be called inside a lock on dir
 */
SFUNC( FileLink *, fcache_find_link, Directory *dir, hname_t *name )
{
	llist_t *_link, *_next, *_list;
	FileLink *link;
	
	assert ( dir != NULL );
	
	/* Iterate over all links */
	_list = (llist_t *) &dir->files;
	_next = (llist_t *) _list->next;
	for (_link = _next; _link != _list; _link = _next ) {
		_next = (llist_t *) _link->next;
		link = (FileLink *) _link;

		/* Compare names */
		if ( hname_compare( link->name, name ) )
			RETURN(link);
		
	}
	
	THROW(ENOENT, NULL);
	
}

/**
 * @note This function must never be called inside a lock on file
 */
File *file_ref( File *file )
{
	
	assert ( file != NULL );
	
	/* Acquire a lock on file */
	semaphore_down( file->lock );
	
	/* Bump file reference count */
	file->refcount++;
	
	/* Release lock on file */	
	semaphore_up( file->lock );
	
	return file;
	
}

/**
 * @note This function must never be called inside a lock on file
 */
void file_release( File *file )
{
	
	assert ( file != NULL );
	
	/* Acquire a lock on file */
	semaphore_down( file->lock );
	
	/* Decrease file reference count */
	file->refcount--;
	
	/* Release lock on file */	
	semaphore_up( file->lock );
	
}




