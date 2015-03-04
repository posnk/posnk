/**
 * @file kernel/vfs/dcache.c
 *
 * Implements the directory cache and GC
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 12-04-2014 - Created
 * \li 11-07-2014 - Rewrite 1
 * \li 12-07-2014 - Commented
 * \li 04-02-2015 - Split off from vfs.c
 */

/* Includes */

#include <assert.h>

#include "util/llist.h"

#include "kernel/vfs.h"

#include "kernel/heapmm.h"

/* Global Variables */

/* Internal type definitions */

/* Public Functions */

/**
 * @brief Create the initial dir cache entry
 * @param root_inode The root inode of the initial root filesystem
 * @return The dir cache entry for the root inode
 */

dir_cache_t *vfs_dir_cache_mkroot(inode_t *root_inode)
{
	/* Allocate memory for the cache entry */
	dir_cache_t *dirc = heapmm_alloc( sizeof( dir_cache_t ) );
	assert (dirc != NULL);
	
	/* Set its parent to itself because it is the root of the graph */
	dirc->parent = dirc;

	/* Set the inode */
	dirc->inode = vfs_inode_ref(root_inode);

	/* Initialize the reference count to 1 */
	dirc->usage_count = 1;

	return dirc;	
}

/**
 * @brief Release a reference to a directory cache entry
 * @param dirc The reference to release
 */

SVFUNC(vfs_dir_cache_release, dir_cache_t *dirc)
{
	errno_t status;

	assert (dirc != NULL);

	/* Decrease the reference count for this dir cache entry */
	if (dirc->usage_count)
		dirc->usage_count--;

	/* If the entry has no more references, destroy it */
	if (dirc->usage_count)
		RETURNV;

	/* If this is not the graph root, release the parent ref */
	if (dirc->parent != dirc) {
		status = vfs_dir_cache_release(dirc->parent);
		if ( status )
			THROWV( status );
	}

	/* Decrease the inode reference count */
	status = vfs_inode_release(dirc->inode);
	if ( status )
		THROWV( status );

	/* Release it's memory */
	heapmm_free(dirc, sizeof(dir_cache_t));	

	RETURNV;
}

/**
 * @brief Create a new reference to a directory cache entry
 * @param dirc The entry to refer to
 * @return The new reference
 */

dir_cache_t *vfs_dir_cache_ref(dir_cache_t *dirc)
{
	assert (dirc != NULL);
	dirc->usage_count++;
	return dirc;
}

/**
 * @brief Create a new directory cache entry
 * @param parent Directory cache entry to use as parent
 * @return The new entry
 */

SFUNC( dir_cache_t *, vfs_dir_cache_new, dir_cache_t *par, ino_t inode_id )
{
	inode_t *oi;
	errno_t status;

	/* Check for null pointers */
	assert (par != NULL);

	/* Allocate memory for the cache entry */
	dir_cache_t *dirc = heapmm_alloc(sizeof(dir_cache_t));
	
	/* Check if the allocation succeeded */
	if (!dirc)
		THROW(ENOMEM, NULL);

	/* Set its parent to itself because it is the root of the graph */
	dirc->parent = vfs_dir_cache_ref(par);

	status = vfs_get_inode(par->inode->device, inode_id, &oi);
	if ( status )
		THROW( status, NULL );
	
	/* Set the inode */
	dirc->inode = vfs_effective_inode( oi );

	assert(dirc->inode != NULL);

	/* Release the outer inode */
	if (oi) {
		status = vfs_inode_release( oi );
		if ( status )
			THROW( status, NULL );
	}
	
	/*if (!dirc->inode) {
		heapmm_free(dirc, sizeof(dir_cache_t));
		vfs_dir_cache_release(par);
		return NULL;
	}*/

	/* Initialize the reference count to 1 */
	dirc->usage_count = 1;

	RETURN(dirc);	
}
