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
#include "util/mruc.h"

#include "kernel/vfs.h"

#include "kernel/heapmm.h"

#include "kernel/earlycon.h"

/* Global Variables */

/** The linked list serving as open inode list */
llist_t *open_inodes;

/** The linked list serving as inode cache */
//TODO: Implement a proper inode cache
mruc_t *inode_cache;

/* Internal type definitions */

typedef struct vfs_cache_params {
	uint32_t device_id;
	ino_t inode_id;
} vfs_cache_params_t;
#define INODE_HASHR( iD, DeV ) ((iD & 0xFFFFFFFFL) | \
							(((uint64_t)((DeV & 0xFFFFFFFFL)) \
							<< 32L)))
#define INODE_HASH( iNoDe ) INODE_HASHR( (iNoDe)->id, (iNoDe)->device_id )

/* Public Functions */

void vfs_icache_evict( mruc_e_t *entry )
{

	errno_t status;
	inode_t *inode = ( inode_t * ) entry;

	debugcon_printf("evict inode: 0x%08x \n", entry );
	
	mruc_remove( entry );

	status = ifs_store_inode(inode);
	
	if (status) {
	
		debugcon_printf("vfs: error while syncing inode: %i\n", status);
	
	} else {
	
		
		semaphore_free( inode->lock ); 
		
		heapmm_free( inode, inode->device->inode_size );
	
	}
	
	

}

void vfs_icache_initialize()
{

	int tsize, csize; 
	mruc_e_t **table;

	tsize = CONFIG_INODE_CACHE_TABLESIZE;
	csize = CONFIG_INODE_CACHE_SIZE;

	/* Allocate inode cache */
	inode_cache = heapmm_alloc(sizeof(mruc_t));

	/* Allocate open inode list */
	open_inodes = heapmm_alloc(sizeof(llist_t));

	/* Allocate table */
	table = heapmm_alloc( tsize * sizeof(llist_t) );

	/* Create the inode cache */
	mruc_create( inode_cache, csize, tsize, vfs_icache_evict, table );

	/* Create the open inode list */
	llist_create(open_inodes);

}

/**
 * @brief Release a reference to an inode
 * @param inode The reference to release
 */

void vfs_inode_release(inode_t *inode)
{
	//debugcon_printf("rel inode: 0x%x rc: %i ENTER\n",inode,inode->usage_count);

	/* Decrease the reference count for this inode */
	if (inode->usage_count){
		inode->usage_count--;
		//debugcon_printf("rel inode: 0x%x rc: %i EXIT\n",inode,inode->usage_count);
	}
	/* If the inode has no more references, destroy it */
	if (inode->usage_count)
		return;

	/* Before moving it to cache, sync it to disk */
	//status = ifs_store_inode( inode );

	//if (status) {
	//	debugcon_printf("vfs: failed to sync inode (%i)\n", status);
	//}

	llist_unlink((llist_t *) inode);
	
	mruc_add( inode_cache, ( mruc_e_t * ) inode, INODE_HASH(inode) );
	//debugcon_printf("rel inode: 0x%x rc: %i EXITM\n",inode,inode->usage_count);

}

/**
 * @brief Create a new reference to an inode
 * @param dirc The entry to refer to
 * @return The new reference
 */

inode_t *vfs_inode_ref(inode_t *inode)
{

	assert (inode != NULL);

	//debugcon_printf("ref inode: 0x%x rc: %i ENTER\n",inode,inode->usage_count);
	if (!(inode->usage_count)) {
		/* Move inode from cache to open inode list */
		mruc_remove( ( mruc_e_t * ) inode );
		llist_add_end( open_inodes, (llist_t *) inode );
	}

	inode->usage_count++;
	//debugcon_printf("ref inode: 0x%x rc: %i EXIT\n",inode,inode->usage_count);

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

	mruc_add( inode_cache, ( mruc_e_t * ) inode, INODE_HASH(inode) );
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

inode_t *vfs_get_cached_inode(fs_device_t *device, ino_t inode_id)
{
	inode_t *result;
	vfs_cache_params_t search_params;

	/* Check for NULL pointers */
	assert ( device != NULL );

	/* Setup search parameters */
	search_params.device_id = device->id;
	search_params.inode_id = inode_id;

	/* Search open inode list */
	result = (inode_t *) llist_iterate_select(open_inodes, &vfs_cache_find_iterator, (void *) &search_params);

	if (result) {
		/* Cache hit, return inode */
		return vfs_inode_ref(result);
	}

	/* Search inode cache */
	result = (inode_t *) mruc_get( inode_cache, 
									INODE_HASHR( inode_id, device->id ));

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

int vfs_cache_flush_iterator (llist_t *node, __attribute__((unused)) void *param)
{
	errno_t status;
	inode_t *inode = (inode_t *) node;
	
	//semaphore_down( inode->lock );

	status = ifs_store_inode(inode);
	
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
	mruc_flush( inode_cache );

}

/**
 * @brief Sync an inode from cache to disk
 *
 */
SVFUNC(vfs_sync_inode, inode_t *inode)
{
	assert ( inode != NULL );
	
	CHAINRETV(ifs_store_inode, inode);
	
}

