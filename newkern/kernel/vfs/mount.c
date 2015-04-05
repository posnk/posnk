/**
 * @file kernel/vfs/mount.c
 *
 * Manages file system mounts.
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 12-04-2014 - Created
 * \li 11-07-2014 - Rewrite 1
 * \li 12-07-2014 - Commented
 * \li 05-04-2015 - Split off from ifsmgr.c
 */

/* Includes */

#include <assert.h>

#include <sys/errno.h>

#include "util/llist.h"

#include "kernel/vfs.h"

#include "kernel/heapmm.h"

#include <string.h>


/* Global Variables */

/** List of currently mounted filesystems */
llist_t vfs_mount_list;


/* Internal type definitions */


/* Public Functions */

void vfs_mount_initialize ( void )
{
	llist_create( &vfs_mount_list );
}

int vfs_gmnt_dev_iterator (llist_t *node, void *param) {
	fs_mount_t 	*mnt = (fs_mount_t *)	node;
	dev_t		*dev = (dev_t *)	param;

	return mnt->device->id == (uint32_t) *dev;
}

int vfs_gmnt_mpt_iterator (llist_t *node, void *param) {
	fs_mount_t 	*mnt = (fs_mount_t *)	node;
	inode_t		*ino = (inode_t *)	param;

	return mnt->mountpoint == ino;
}

/**
 * @brief Find a mounted filesystem by its device
 * @param device The device to look up
 * @return The mount descriptor or NULL if no file system was mounted.
 */
fs_mount_t	*vfs_get_mount_by_dev( dev_t device )
{
	return (fs_mount_t *)
		llist_iterate_select(	&vfs_mount_list, 
					&vfs_gmnt_dev_iterator,
					&device );
}

/**
 * @brief Find a mounted filesystem by its mountpoint
 * @param mountpoint The mountpoint to look up
 * @return The mount descriptor or NULL if no file system was mounted.
 */
fs_mount_t	*vfs_get_mount_by_mountpoint( inode_t *mountpoint )
{
	assert( mountpoint != NULL );

	return (fs_mount_t *)
		llist_iterate_select(	&vfs_mount_list, 
					&vfs_gmnt_mpt_iterator,
					 mountpoint );
}


/**
 * @brief Registers a mounted filesystem 
 * @param device The filesystem that was mounted
 * @param mountpoint The inode it was mounted on.
 */
SVFUNC( vfs_reg_mount, fs_device_t *device, inode_t *mountpoint)
{

	fs_mount_t *mount;
	
	assert ( device != NULL );
	assert ( mountpoint != NULL );
		
	mount = heapmm_alloc ( sizeof( fs_mount_t ) );

	if ( !mount )
		THROWV( ENOMEM );

	mount->device = device;
	mount->mountpoint = vfs_inode_ref( mountpoint );

	llist_add_end ( &vfs_mount_list, (llist_t *) mount );

	RETURNV;

}

/**
 * @brief Unregisters a mounted filesystem 
 * @param device The filesystem that was mounted
 * @param mountpoint The inode it was mounted on.
 */
void vfs_unreg_mount( fs_mount_t *mount )
{

	llist_unlink ( (llist_t *) mount );
		
	vfs_inode_release( mount->mountpoint );
	
	heapmm_free( mount, sizeof( fs_mount_t ) );

}

/**
 * @brief Mount a filesystem
 * @param driver The file system driver to use.
 * @param device The device to mount
 * @param mountpoint The directory to mount the fs on
 * @param flags  Options for mounting the fs that are passed to the fs driver
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EINVAL An error occurred mounting the filesystem
 * @exception ENOTDIR The mount point is not a directory
 */

SVFUNC(vfs_do_mount, fs_driver_t *driver, dev_t device, inode_t *mountpoint, uint32_t flags)
{
	fs_device_t 	*fsdevice;
	errno_t	     	 status;
	inode_t		*mp_inode;

	/* Check for null pointers */
	assert ( driver != NULL );
	assert ( mountpoint != NULL );
	
	/* Check if the device has already been mounted */
	if ( vfs_get_mount_by_dev( device ) ) {
		
		THROWV( EBUSY );

	}

	/* Reference the mountpoint */
	mp_inode = vfs_inode_ref ( mountpoint );

	/* Check if it is a directory */
	if (!S_ISDIR(mp_inode->mode)) {

		/* Release the mountpoint inode */
		vfs_inode_release( mp_inode );

		THROWV( ENOTDIR );

	}
	
	/* Check if the target is already a mountpoint */
	if ( vfs_get_mount_by_mountpoint( mp_inode ) ) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);
		
		THROWV( EBUSY );

	}

	/* Mount the filesystem */
	status = driver->mount( device, flags, &fsdevice );
	
	/* Check for errors */
	if (status) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		THROWV( status );
	}
		
	/* Attach the filesystems root inode to the mountpoint */
	status = ifs_load_inode( fsdevice, 
				 fsdevice->root_inode_id, 
				 &(mp_inode->mount) );	
	
	/* Check for errors */	
	if (status) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		THROWV( status ); //TODO: Unmount
	}

	/* Register the mount */
	vfs_reg_mount ( fsdevice, mp_inode );

	/* Release the mountpoint inode */
	vfs_inode_release(mp_inode);

	/* Return success */
	RETURNV;
}

void vfs_sync_filesystems( void )
{
	int 		n_fs, n_ok;
	llist_t 	*_mnt;
	fs_mount_t 	*mnt;
	errno_t		*status;

	n_fs = 0;
	n_ok = 0;

	for ( 	_mnt = vfs_mount_list.next; 
		_mnt != &vfs_mount_list; 
		_mnt = _mnt->next ) {
		
		mnt = ( fs_mount_t * ) _mnt;

		n_fs++;

		status = ifs_sync ( mnt->device );
	
		if ( status ) {
			debugcon_printf(
				"vfs: failed to sync a filesystem ( %i )\n",
				status );
		} else {
			n_ok++;
		}		

	}

	debugcon_printf("vfs: %i/%i filesystems synchronized successfully.\n", n_ok, n_fs);

}
