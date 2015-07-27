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

/** List of currently mounted filesystems */
llist_t filesystem_list;


/* Internal type definitions */


/* Public Functions */

void vfs_mount_initialize ( void )
{
	llist_create( &filesystem_list );
}

int vfs_gmnt_mpt_iterator (llist_t *node, void *param) {
	Filesystem 	*mnt = (Filesystem *)	node;
	Directory	*dir = (Directory *)	param;

	return mnt->mountpoint == dir;
}

/**
 * @brief Find a mounted filesystem by its mountpoint
 * @param mountpoint The mountpoint to look up
 * @return The mount descriptor or NULL if no file system was mounted.
 */
Filesystem	*vfs_get_mount_by_mountpoint( Directory *mountpoint )
{
	assert( mountpoint != NULL );

	return (Filesystem *)
		llist_iterate_select(	&filesystem_list, 
								&vfs_gmnt_mpt_iterator,
								mountpoint );
}


/**
 * @brief Registers a mounted filesystem 
 * @param filesystem The filesystem that was mounted
 */
void vfs_reg_mount( Filesystem *filesystem )
{
	
	assert ( device != NULL );

	llist_add_end ( &filesystem_list, (llist_t *) filesystem );

}

/**
 * @brief Unregisters a mounted filesystem 
 * @param filesystem The filesystem that was mounted
 */
void vfs_unreg_mount( Filesystem *filesystem )
{

	llist_unlink ( (llist_t *) filesystem );

}

/**
 * @brief Mount a filesystem
 * @param driver The file system driver to use.
 * @param device The device to mount
 * @param mountpoint The directory to mount the fs on
 * @param flags  Options for mounting the fs that are passed to the fs driver
 *
 * @exception EINVAL An error occurred mounting the filesystem
 * @exception ENOTDIR The mount point is not a directory
 */

SVFUNC(vfs_do_mount_device,	
						FSDriver *driver, 
						dev_t device, 
						Directory *mountpoint, 
						mflag_t flags
		)
{
	Filesystem	*fsdevice;
	Directory	*mp_ref;
	errno_t	     status;
	errno_t	     nstatus;

	/* Check for null pointers */
	assert ( driver != NULL );
	assert ( mountpoint != NULL );
	
	/* Check if the device has already been mounted */
	//TODO: Implement this (Check if the device has already been mounted)
	//XXX: Regression...

	/* Reference the mountpoint */
	mp_ref = directory_ref ( mountpoint );
	
	/* Check if the target is already a mountpoint */
	if ( vfs_get_mount_by_mountpoint( mp_ref ) ) {

		/* Release the mountpoint */
		directory_release(mp_ref);
		
		THROWV( EBUSY );

	}

	/* Mount the filesystem */
	status = SMCALL(driver, &fsdevice, mount_device, device, flags );
	
	/* Check for errors */
	if (status) {

		/* Release the mountpoint */
		directory_release(mp_ref);

		THROWV( status );
	}
		
	/* Attach the filesystems root inode to the mountpoint */
	status = SVMCALL(fsdevice, attach, mp_ref);	
	
	/* Check for errors */	
	if (status) {

		/* Release the mountpoint */
		directory_release(mp_ref);
		
		/* Unmount the filesystem */
		nstatus = SVMCALL(fsdevice, unmount, 0);
		
		if ( nstatus ) {
			
			//TODO: Oops here. "Filesystem attachment failure followed by
			//					unmount failure"
			
		}

		THROWV( status ); 
	}

	/* Register the mount */
	vfs_reg_mount ( fsdevice );

	/* Release the mountpoint */
	directory_release(mp_ref);

	/* Return success */
	RETURNV;
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

SVFUNC(vfs_do_mount_named,	
						FSDriver *driver, 
						char * target, 
						Directory *mountpoint, 
						mflag_t flags
		)
{
	Filesystem	*fsdevice;
	Directory	*mp_ref;
	errno_t	     status;
	errno_t	     nstatus;

	/* Check for null pointers */
	assert ( target != NULL );
	assert ( driver != NULL );
	assert ( mountpoint != NULL );
	
	/* Check if the device has already been mounted */
	//TODO: Implement this (Check if the device has already been mounted)
	//XXX: Regression...

	/* Reference the mountpoint */
	mp_ref = directory_ref ( mountpoint );
	
	/* Check if the target is already a mountpoint */
	if ( vfs_get_mount_by_mountpoint( mp_ref ) ) {

		/* Release the mountpoint */
		directory_release(mp_ref);
		
		THROWV( EBUSY );

	}

	/* Mount the filesystem */
	status = SMCALL(driver, &fsdevice, mount_named, target, flags );
	
	/* Check for errors */
	if (status) {

		/* Release the mountpoint */
		directory_release(mp_ref);

		THROWV( status );
	}
		
	/* Attach the filesystems root inode to the mountpoint */
	status = SVMCALL(fsdevice, attach, mp_ref);	
	
	/* Check for errors */	
	if (status) {

		/* Release the mountpoint */
		directory_release(mp_ref);
		
		/* Unmount the filesystem */
		nstatus = SVMCALL(fsdevice, unmount, 0);
		
		if ( nstatus ) {
			
			//TODO: Oops here. "Filesystem attachment failure followed by
			//					unmount failure"
			
		}

		THROWV( status ); 
	}

	/* Register the mount */
	vfs_reg_mount ( fsdevice );

	/* Release the mountpoint */
	directory_release(mp_ref);

	/* Return success */
	RETURNV;
}

void vfs_sync_filesystems( void )
{
	int 		n_fs, n_ok;
	llist_t 	*_mnt;
	Filesystem 	*mnt;
	errno_t		*status;

	n_fs = 0;
	n_ok = 0;

	for ( 	_mnt = filesystem_list.next; 
		_mnt != &filesystem_list; 
		_mnt = _mnt->next ) {
		
		mnt = ( Filesystem * ) _mnt;

		n_fs++;

		status = SOMCALL( mnt, sync );
	
		if ( status ) {
			debugcon_printf(
				"vfs: failed to sync a filesystem ( %i )\n",
				status );
		} else {
			n_ok++;
		}		

	}

	debugcon_printf("vfs: %i/%i filesystems synchronized successfully.\n", 
						n_ok, 
						n_fs);

}
