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
 * \li 27-07-2015 - Adapted to OOVFS
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
 * A file system driver calls this function to register itself
 *
 * @param driver The driver to register
 * @return If successful, 0 is returned, otherwise a valid error code will be returned
 */
SVFUNC( vfs_register_fs, FSDriver *driver )
{
	
	/* Check for NULL pointers */
	assert(name != NULL);
	assert(driver != NULL);

	/* Add driver to list */
	llist_add_end(&vfs_fs_driver_list, (llist_t *) driver);

	RETURNV; 
}

int vfs_mount_iterator (llist_t *node, void *param) {
	FSDriver *driver = (FSDriver *) node;
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

SFUNC(FSDriver *, vfs_get_driver, char *fstype)
{
	FSDriver *driver;

	/* Look up the fs driver */	
	driver = (FSDriver *) llist_iterate_select(
					&vfs_fs_driver_list, 
					&vfs_mount_iterator, 
					fstype);

	/* Check if it exists */
	if (!driver) {

		THROW( ENODEV, NULL );

	}

	RETURN(driver);
}
