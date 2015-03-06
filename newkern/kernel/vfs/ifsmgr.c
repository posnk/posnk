/**
 * @file kernel/vfs/ifsmgr.c
 *
 * Provides wrappers around the filesystem driver interface.
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 12-04-2014 - Created
 * \li 11-07-2014 - Rewrite 1
 * \li 12-07-2014 - Commented
 * \li 06-03-2015 - Split off from vfs.c
 */

/* Includes */

#include <assert.h>

#include <sys/errno.h>

#include "util/llist.h"

#include "kernel/vfs.h"

#include "kernel/heapmm.h"

#include <string.h>
/* Global Variables */

/** Filesystem driver list */
llist_t vfs_fs_driver_list;

/* Internal type definitions */


/* Public Functions */

void register_fs_drivers();

/**
 * @brief Initialize the installable filesystem driver manager
 *
 */

void vfs_ifsmgr_initialize( void )
{

	/* Create the file system device list */
	llist_create(&vfs_fs_driver_list);
	
	/* Register fs drivers */
	register_fs_drivers();

}

/**
 * @brief Register a file system driver
 * 
 * A file system driver calls this function to register its mount callback 
 *
 * @param name The name to register with (this is the identifier passed to mount)
 * @param mnt_cb A pointer to the filesystem's mount function
 * @return If successful, 0 is returned, otherwise a valid error code will be returned
 */
SVFUNC( vfs_register_fs, 
		const char *name, 
		SFUNCPTR(fs_device_t *, mnt_cb, dev_t, uint32_t) )
{
	fs_driver_t *driver;

	/* Check for NULL pointers */
	assert(name != NULL);
	assert(mnt_cb != NULL);

	/* Allocate memory for driver descriptor */
	driver = heapmm_alloc(sizeof(fs_driver_t));

	/* Check for errors */
	if (!driver)
		THROWV(ENOMEM);

	/* Allocate memory for driver name */
	driver->name = heapmm_alloc(strlen(name) + 1);

	/* Check for errors */
	if (!driver->name) {
		/* Clean up */
		heapmm_free(driver, sizeof(fs_driver_t));
		THROWV(ENOMEM);
	}

	/* Copy driver name */
	strcpy(driver->name, name);
	
	/* Set driver mount callback */
	driver->mount = mnt_cb;

	/* Add driver to list */
	llist_add_end(&vfs_fs_driver_list, (llist_t *) driver);

	RETURNV; 
}

int vfs_mount_iterator (llist_t *node, void *param) {
	fs_driver_t *driver = (fs_driver_t *) node;
	assert(driver->name != NULL);
	return 0 == strcmp(driver->name, (char *) param);
}

/**
 * @brief Get a filesystem driver
 * @param fstype The filesystem driver to use e.g.: ext2, ramfs
 * @return A filesystem driver descriptor
 *
 * @exception ENOENT A file does not exist
 */

SFUNC(fs_driver_t *, vfs_get_driver, char *fstype)
{
	fs_driver_t *driver;

	/* Look up the fs driver */	
	driver = (fs_driver_t *) llist_iterate_select(
					&vfs_fs_driver_list, 
					&vfs_mount_iterator, 
					fstype);

	/* Check if it exists */
	if (!driver) {

		THROW( ENOENT, NULL );
	}

	RETURN(driver);
}

/**
 * @brief Mount a filesystem
 * @param device The path of the block special file the fs resides on
 * @param mountpoint The directory to mount the fs on
 * @param fstype The filesystem driver to use e.g.: ext2, ramfs
 * @param flags  Options for mounting the fs that are passed to the fs driver
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT Atleast one parameter was a NULL pointer
 * @exception ENOENT A file does not exist
 * @exception EINVAL An error occurred mounting the filesystem
 * @exception ENOTDIR The mount point is not a directory
 * @exception ENOTBLK The special file is not a block special file
 */

SVFUNC(vfs_mount, char *device, char *mountpoint, char *fstype, uint32_t flags)
{
	fs_driver_t *driver;
	fs_device_t *fsdevice;
	inode_t	    *mp_inode;
	inode_t	    *dev_inode;
	errno_t	     status;

	/* Check for null pointers */
	assert (device != NULL);
	assert (mountpoint != NULL);

	/* Look up the inode for the mountpoint */
	status = vfs_find_inode(mountpoint, &mp_inode);

	/* Check if it exists */
	if (status) 
		THROWV( status );

	/* Check if it is a directory */
	if (!S_ISDIR(mp_inode->mode)) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		THROWV( status );
	}

	/* Look up the inode for the special file */
	status = vfs_find_inode(device, &dev_inode);
	
	/* Check if it exists */
	if (status) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		THROWV( status );
	}

	/* Check if it is a block special file */
	if (!S_ISBLK(dev_inode->mode)) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		/* Release the special file inode */
		vfs_inode_release(dev_inode);

		THROWV( ENOTBLK );
	}
	
	/* Look up the fs driver */	
	driver = (fs_driver_t *) llist_iterate_select(&vfs_fs_driver_list, &vfs_mount_iterator, fstype);

	/* Check if it exists */
	if (!driver) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		/* Release the special file inode */
		vfs_inode_release(dev_inode);

		THROWV( ENOENT );
	}

	/* Mount the filesystem */
	status = driver->mount(dev_inode->if_dev, flags, &fsdevice);
	
	/* Check for errors */
	if (status) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		/* Release the special file inode */
		vfs_inode_release(dev_inode);

		THROWV( status );
	}
		
	/* Attach the filesystems root inode to the mountpoint */
	status = ifs_load_inode(fsdevice, fsdevice->root_inode_id, &(mp_inode->mount));	
	
	/* Check for errors */	
	if (status) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		/* Release the special file inode */
		vfs_inode_release(dev_inode);

		THROWV(status ); //TODO: Unmount
	}

	/* Release the mountpoint inode */
	vfs_inode_release(mp_inode);

	/* Release the special file inode */
	vfs_inode_release(dev_inode);

	/* Return success */
	RETURNV;
}
