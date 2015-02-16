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

#include "util/llist.h"

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

/**
 * @brief Release a reference to an inode
 * @param inode The reference to release
 */

void vfs_inode_release(inode_t *inode)
{
	/* Decrease the reference count for this inode */
	if (inode->usage_count)
		inode->usage_count--;

	/* If the inode has no more references, destroy it */
	if (inode->usage_count)
		return;

	/* If the inode has a mount on it, don't destroy it */
	if (inode->mount)
		return;

	llist_unlink((llist_t *) inode);
	llist_add_end(inode_cache, (llist_t *) inode);

	inode->device->ops->store_inode(inode);
}

/**
 * @brief Create a new reference to an inode
 * @param dirc The entry to refer to
 * @return The new reference
 */

inode_t *vfs_inode_ref(inode_t *inode)
{
	assert (inode != NULL);
	if ((!inode->mount) && (!inode->usage_count)) {
		llist_unlink((llist_t *) inode);
		llist_add_end(open_inodes, (llist_t *) inode);
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


