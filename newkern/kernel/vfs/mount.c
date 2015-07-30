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

typedev struct {
	uint32_t	dev;
	ino_t		ino;
} gmnt_params_t;

/* Public Functions */

void vfs_mount_initialize ( void )
{
	llist_create( &vfs_mount_list );
}

int vfs_gmnt_dev_iterator (llist_t *node, void *param) {
	fs_mount_t 	*mnt = (fs_mount_t *)	node;
	uint32_t	*dev = (uint32_t *)	param;

	return mnt->device->id == *dev;
}

int vfs_gmnt_mpt_iterator (llist_t *node, void *param) {
	fs_mount_t 		*mnt = (fs_mount_t *)		node;
	gmnt_params_t	*par = (gmnt_params_t *)	param;

	return mnt->mt_dev == par->dev && mnt->mt_ino == par->ino;
}

int vfs_gmnt_root_iterator (llist_t *node, void *param) {
	fs_mount_t 		*mnt = (fs_mount_t *)		node;
	gmnt_params_t	*par = (gmnt_params_t *)	param;

	return mnt->rt_dev == par->dev && mnt->rt_ino == par->ino;
}

/**
 * @brief Find a mounted filesystem by its device
 * @param device The device to look up
 * @return The mount descriptor or NULL if no file system was mounted.
 */
fs_mount_t	*vfs_get_mount_by_dev( uint32_t device )
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
fs_mount_t	*vfs_get_mount_by_root( uint32_t dev, ino_t ino )
{
	gmnt_params_t par;
	
	par.dev = dev;
	par.ino = ino;
	
	return (fs_mount_t *)
		llist_iterate_select(	&vfs_mount_list, 
					&vfs_gmnt_root_iterator,
					 &par );
}


/**
 * @brief Find a mounted filesystem by its mountpoint
 * @param mountpoint The mountpoint to look up
 * @return The mount descriptor or NULL if no file system was mounted.
 */
fs_mount_t	*vfs_get_mount_by_mountpoint( uint32_t dev, ino_t ino )
{
	gmnt_params_t par;
	
	par.dev = dev;
	par.ino = ino;
	
	return (fs_mount_t *)
		llist_iterate_select(	&vfs_mount_list, 
					&vfs_gmnt_mpt_iterator,
					 &par );
					 
}


/**
 * @brief Registers a mounted filesystem 
 * @param device The filesystem that was mounted
 * @param mountpoint The inode it was mounted on.
 */
SVFUNC( vfs_reg_mount, fs_device_t *device, uint32_t mt_dev, 
											ino_t mt_ino,
											uint32_t pt_dev,
											ino_t pt_ino,
											uint32_t rt_dev,
											ino_t rt_ino)
{

	fs_mount_t *mount;
	
	assert ( device != NULL );
		
	mount = heapmm_alloc ( sizeof( fs_mount_t ) );

	if ( !mount )
		THROWV( ENOMEM );

	mount->device = device;
	mount->mt_dev = mt_dev;
	mount->mt_ino = mt_ino;
	mount->rt_dev = rt_dev;
	mount->rt_ino = rt_ino;
	mount->pt_dev = pt_dev;
	mount->pt_ino = pt_ino;

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
	inode_t			*mp_inode;
	ino_t			 pt_ino;
	ino_t			 rt_ino;

	/* Check for null pointers */
	assert ( driver != NULL );
	assert ( mountpoint != NULL );
	
	/* Check if the device has already been mounted */
	if ( vfs_get_mount_by_dev( (uint32_t) device ) ) {
		
		THROWV( EBUSY );

	}

	/* Reference the mountpoint */
	mp_inode = vfs_inode_ref ( mountpoint );

	/* Check if it is a directory */
	if (!S_ISDIR(mp_inode->type)) {

		/* Release the mountpoint inode */
		vfs_inode_release( mp_inode );

		THROWV( ENOTDIR );

	}
	
	/* Check if the target is already a mountpoint */
	if ( vfs_get_mount_by_mountpoint( mp_inode->device_id, 
									  mp_inode->id ) ) {

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
		
	/* Look up the parent of the mountpoint */
	status = vfs_findent( mp_inode, "..", &pt_ino );
	
	if ( status ) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		THROWV( status ); //TODO: Unmount
		
	}
		
	/* Attach the filesystems root inode to the mountpoint */
	status = vfs_reg_mount( fsdevice,  
							mp_inode->device_id, 	/* mountpoint dev */
							mp_inode->id,			/* mountpoint ino */
							mp_inode->device_id, 	/* mpt parent dev */
							pt_ino,					/* mpt parent ino */
							fsdevice->id,			/* root dir   dev */
							fsdevice->root_inode_id);/*root dir	  ino */	
	
	/* Check for errors */	
	if (status) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		THROWV( status ); //TODO: Unmount
	}

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
