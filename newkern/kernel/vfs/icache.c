/**
 * @file kernel/vfs/icache.c
 *
 * Implements the inode cache and inode GC
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 12-04-2014 - Created
 * \li 11-07-2014 - Rewrite 1
 * \li 12-07-2014 - Commented
 * \li 16-02-2015 - Split off from vfs.c
 */

/* Includes */

#include <assert.h>

#include "util/llist.h"

#include "kernel/vfs.h"

#include "kernel/heapmm.h"

/* Global Variables */

/** The linked list serving as open inode list */
llist_t *open_inodes;

/** The linked list serving as inode cache */
//TODO: Implement a proper inode cache
llist_t *inode_cache;

/* Internal type definitions */

typedef struct vfs_cache_params {
	uint32_t device_id;
	ino_t inode_id;
} vfs_cache_params_t;

/* Public Functions */

void vfs_icache_initialize()
{

	/* Allocate inode cache */
	inode_cache = heapmm_alloc(sizeof(llist_t));

	/* Allocate open inode list */
	open_inodes = heapmm_alloc(sizeof(llist_t));

	/* Create the inode cache */
	llist_create(inode_cache);

	/* Create the open inode list */
	llist_create(open_inodes);

}

/**
 * @brief Release a reference to an inode
 * @param inode The reference to release
 */

void vfs_inode_release(inode_t *inode)
{
	errno_t status;

	/* Decrease the reference count for this inode */
	if (inode->usage_count)
		inode->usage_count--;

	/* If the inode has no more references, destroy it */
	if (inode->usage_count)
		return;

	/* Before moving it to cache, sync it to disk */
	//status = ifs_store_inode( inode );

	//if (status) {
	//	debugcon_printf("vfs: failed to sync inode (%i)\n", status);
	//}

	llist_unlink((llist_t *) inode);
	llist_add_end(inode_cache, (llist_t *) inode);

}

/**
 * @brief Create a new reference to an inode
 * @param dirc The entry to refer to
 * @return The new reference
 */

inode_t *vfs_inode_ref(inode_t *inode)
{

	assert (inode != NULL);

	if (!(inode->usage_count)) {
		/* Move inode from cache to open inode list */
		llist_unlink( (llist_t *) inode );
		llist_add_end( open_inodes, (llist_t *) inode );
	}

	inode->usage_count++;

	return inode;
}

/**
 * @brief Add an inode to the cache
 * @param dirc The entry to refer to
 * @return The new reference
 */

void vfs_inode_cache(inode_t *inode)
{

	assert (inode != NULL);

	llist_add_end(inode_cache, (llist_t *) inode);
}

/*
 * Iterator function that looks up the requested inode
 */

int vfs_cache_find_iterator (llist_t *node, void *param)
{
	inode_t *inode = (inode_t *) node;
	vfs_cache_params_t *p = (vfs_cache_params_t *) param;
	return (inode->id == p->inode_id) && (inode->device_id == p->device_id);		
}

/**
 * @brief Get an inode from cache by it's ID
 * 
 * @param device   The device to get the inode from
 * @param inode_id The ID of the inode to look up
 * @return The inode with id inode_id from device or NULL if the inode was not
 *	   cached.
 */

inode_t *vfs_get_cached_inode(uint32_t device_id, ino_t inode_id)
{
	inode_t *result;
	vfs_cache_params_t search_params;

	/* Setup search parameters */
	search_params.device_id = device_id;
	search_params.inode_id = inode_id;

	/* Search open inode list */
	result = (inode_t *) llist_iterate_select(open_inodes, &vfs_cache_find_iterator, (void *) &search_params);

	if (result) {
		/* Cache hit, return inode */
		return vfs_inode_ref(result);
	}

	/* Search inode cache */
	result = (inode_t *) llist_iterate_select(inode_cache, &vfs_cache_find_iterator, (void *) &search_params);

	if (result) {
		/* Cache hit, return inode */
		return vfs_inode_ref(result);
	}

	/* Cache miss */
	return NULL;
}

/*
 * Iterator function that flushes the inode caches
 */

int vfs_cache_flush_iterator (llist_t *node, void *param)
{
	errno_t status;
	inode_t *inode = (inode_t *) node;
	
	//semaphore_down( inode->lock );

	status = ifs_sync_inode(inode);
	
	if (status) {
		debugcon_printf("vfs: error while syncing inode: %i\n", status);
	}

	//semaphore_up( inode->lock );

	return 0;		
}

/**
 * @brief Flush the inode cache
 * 
 */

void vfs_cache_flush()
{

	/* Flush open inode list */
	llist_iterate_select(open_inodes, &vfs_cache_flush_iterator, NULL);

	/* Flush inode cache */
	llist_iterate_select(inode_cache, &vfs_cache_flush_iterator, NULL);

}

/**
 * @brief Sync an inode from cache to disk
 *
 */
SVFUNC(vfs_sync_inode, inode_t *inode)
{
	assert ( inode != NULL );
	
	CHAINRETV(ifs_sync_inode, inode);
	
}

