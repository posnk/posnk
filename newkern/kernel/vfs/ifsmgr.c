/**
 * @file kernel/vfs/ifsmgr.c
 *
 * Manages IFS drivers.
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
 * @exception ENODEV Unknown driver type
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

		THROW( ENODEV, NULL );

	}

	RETURN(driver);
}
