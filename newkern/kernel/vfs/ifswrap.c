/**
 * @file kernel/vfs/ifswrap.c
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
 * \li 04-03-2015 - Split off from vfs.c
 */

/* Includes */

#include <assert.h>

#include <sys/errno.h>

#include "util/llist.h"

#include "kernel/vfs.h"

/* Global Variables */

/* Internal type definitions */


/* Public Functions */

/** 
 * @brief Call on the fs driver to load an inode
 * @see fs_device_operations
 *
 * @param device The driver instance to call on
 * @param id The inode id to load
 * @return The loaded inode
 *
 * @exception ENOTSUP This driver does not support this function.
 */
 
SFUNC( inode_t *, ifs_get_ino, fs_device_t * device, ino_t id )
{
	/* This function is implemented by the FS driver */
	assert ( device != NULL );

	/* Check if the driver supports load_inode */
	if ( ! device->ops->get_ino ) {

		/* If not: return the error "Operation not supported" */
		THROW( ENOTSUP, NULL );

	}

	/* Call the driver */
	CHAINRET( device->ops->get_ino, device, id );
}

/** 
 * @brief Call on the fs driver to push an inode to backing storage
 * @see fs_device_operations
 *
 * @param inode The inode to store
 *
 * @exception ENOTSUP This driver does not support this function
 */
SVFUNC( ifs_sync_inode, inode_t * inode)
{
	/* This function is implemented by the FS driver */
	assert ( inode != NULL );

	/* Check if the driver supports this function */
	if ( ! inode->ops->sync ) {

		/* If not: return the error "Operation not supported" */
		THROWV( ENOTSUP );

	}

	/* Call the driver */
	CHAINRETV( inode->ops->sync, inode );
}

SFUNC(inode_t *, ifs_mknod, fs_device_t * fs, mode_t mode, dev_t dev )
{
	/* This function is implemented by the FS driver */
	assert ( fs != NULL );

	/* Check if the driver supports this function */
	if ( ! fs->ops->mknod ) {

		/* If not: return the error "Operation not supported" */
		THROWV( ENOTSUP );

	}

	/* Call the driver */
	CHAINRET( fs->ops->mknod, fs, mode, dev );
}


/** 
 * @brief Call on the fs driver to remove an inode from storage
 * @see fs_device_operations
 *
 * @param inode The inode to remove
 *
 * @exception ENOTSUP This driver does not support this function
 */
SVFUNC( ifs_rmnod, inode_t * inode)
{
	/* This function is implemented by the FS driver */
	assert ( inode != NULL );

	/* Check if the driver supports this function */
	if ( ! inode->ops->rmnod ) {

		/* If not: return the error "Operation not supported" */
		THROWV( ENOTSUP );

	}

	/* Call the driver */
	CHAINRETV( inode->ops->rmnod, inode );
}


/** 
 * @brief Call on the fs driver to create directory structures on backing strg
 * @see fs_device_operations
 *
 * @param inode The inode for the new directory
 *
 * @exception ENOTSUP This driver does not support this function
 */

SVFUNC( ifs_mkdir, inode_t * inode)
{
	/* This function is implemented by the FS driver */
	assert ( inode != NULL );

	/* Check if the driver supports this function */
	if ( ! inode->ops->mkdir ) {

		/* If not: return the error "Operation not supported" */
		THROWV( ENOTSUP );

	}

	/* Call the driver */
	CHAINRETV( inode->ops->mkdir, inode );
}


/** 
 * @brief Call on the fs driver to find a directory entry
 * @see fs_device_operations
 *
 * @param inode The directory to search
 * @param name  The filename to match
 * @return The directory entry matching name from the directory inode, 
 *          if none, NULL is returned
 *
 * @exception ENOTSUP This driver does not support this function.
 */
 
SFUNC(ino_t, ifs_findent, inode_t * inode, char * name)
{
	/* This function is implemented by the FS driver */
	assert ( inode != NULL );
	assert ( name != NULL );

	/* Check if the driver supports find_dirent */
	if ( ! inode->ops->findent ) {

		/* If not: return the error "Operation not supported" */
		THROW( ENOTSUP, 0 );

	}

	/* Call the driver */
	CHAINRET( inode->ops->findent, inode, name );
}

/**
 * @brief  Call on the fs driver to create a directory entry on backing storage
 * @see fs_device_operations
 * 
 * @param inode  The inode for the directory
 * @param name   The file name for the directory entry
 * @param nod_id The inode id that the directory entry will point to
 *
 * @exception ENOTSUP This driver does not support this function.
 */

SVFUNC( ifs_link, inode_t * inode , char * name , ino_t nod_id )
{
	/* This function is implemented by the FS driver */
	assert ( inode != NULL );
	assert ( name != NULL );

	/* Check if the driver supports link */
	if ( ! inode->ops->link ) {

		/* If not: return the error "Operation not supported" */
		THROWV( ENOTSUP );

	}

	/* Call the driver */
	CHAINRETV( inode->ops->link, inode, name, nod_id );
}

/**
 * @brief  Call on the fs driver to check access privileges for an inode
 * @see fs_device_operations
 * 
 * @param inode - The inode to operate on
 * @param mode  - The mode of access to attempt
 *
 * @exception ENOTSUP This driver does not support this function.
 */

SVFUNC( ifs_access, inode_t * inode , mode_t mode)
{	
	/* This function is implemented by the FS driver */
	assert ( inode != NULL );

	/* Check if the driver supports access */
	if ( ! inode->ops->access ) {

		/* If not: return the error "Operation not supported" */
		THROWV ( ENOTSUP );

	}

	/* Call the driver */
	CHAINRETV( inode->ops->access, inode, mode );
}

/**
 * @brief  Call on the fs driver to inquire about inode metadata
 * @see fs_device_operations
 * 
 * @param inode - The inode to operate on
 *
 * @exception ENOTSUP This driver does not support this function.
 */

SFUNC( struct stat , ifs_stat, inode_t * inode )
{	
	/* This function is implemented by the FS driver */
	assert ( inode != NULL );

	/* Check if the driver supports stat */
	if ( ! inode->ops->stat ) {

		/* If not: return the error "Operation not supported" */
		THROWV ( ENOTSUP );

	}

	/* Call the driver */
	CHAINRET( inode->ops->stat, inode );
}

/**
 * @brief  Call on the fs driver to delete a directory entry from backing strg
 * @see fs_device_operations
 * 
 * @param inode - The inode for the directory
 * @param name  - The file name of the directory entry to delete
 *
 * @exception ENOTSUP This driver does not support this function.
 */

SVFUNC( ifs_unlink, inode_t * inode , char * name )
{	
	/* This function is implemented by the FS driver */
	assert ( inode != NULL );
	assert ( name != NULL );

	/* Check if the driver supports unlink */
	if ( ! inode->ops->unlink ) {

		/* If not: return the error "Operation not supported" */
		THROWV ( ENOTSUP );

	}

	/* Call the driver */
	CHAINRETV( inode->ops->unlink, inode, name );
}

/**
 * @brief  Call on the fs driver to read directory entries from backing storage
 * @see fs_device_operations
 * 
 * @param inode       The inode for the directory
 * @param buffer      The buffer to store the entries in
 * @param file_offset The offset in the directory to start reading at
 * @param count       The number of bytes to read
 * @return            The number of bytes that have been read
 *
 * @exception ENOTSUP This driver does not support this function.
 */

SFUNC( aoff_t, ifs_readdir, inode_t * inode, void * buffer, aoff_t file_offset, aoff_t count )
{
	/* This function is implemented by the FS driver */
	assert ( inode != NULL );
	assert ( buffer != NULL );

	/* Check if the driver supports this function */
	if ( ! inode->ops->readdir ) {

		/* If not: return the error "Operation not supported" */
		THROW( ENOTSUP, 0 );

	}

	/* Call the driver */
	CHAINRET( inode->ops->readdir, inode, buffer, file_offset, count );
}

/**
 * @brief  Call on the fs driver to read data from a file from backing storage
 * @see fs_device_operations
 * 
 * @param inode       The inode for the file
 * @param buffer      The buffer to store the data in
 * @param file_offset The offset in the file to start reading at
 * @param count       The number of bytes to read
 * @return            The number of bytes that have been read
 *
 * @exception ENOTSUP This driver does not support this function.
 */

SFUNC( aoff_t, ifs_read, inode_t * ino, void * buffer, aoff_t file_offset, aoff_t count )
{
	/* This function is implemented by the FS driver */
	assert ( ino != NULL );
	assert ( buffer != NULL );

	/* Check if the driver supports this function */
	if ( ! ino->ops->read ) {

		/* If not: return the error "Operation not supported" */
		THROW( ENOTSUP, 0 );

	}

	/* Call the driver */
	CHAINRET( ino->ops->read, ino, buffer, file_offset, count );
}

/**
 * @brief  Call on the fs driver to write data to a file from backing storage
 * @see fs_device_operations
 * 
 * @param inode       The inode for the file
 * @param buffer      The buffer containing the data to write
 * @param file_offset The offset in the file to start writing at
 * @param count       The number of bytes to write
 * @return            The number of bytes that have been written
 *
 * @exception ENOTSUP This driver does not support this function.
 */

SFUNC( aoff_t, ifs_write, inode_t * ino, void * buffer, aoff_t file_offset, aoff_t count )
{
	/* This function is implemented by the FS driver */
	assert ( ino != NULL );
	assert ( buffer != NULL );

	/* Check if the driver supports write */
	if ( ! ino->ops->write ) {

		/* If not: return the error "Operation not supported" */
		THROW( ENOTSUP, 0 );

	}

	/* Call the driver */
	CHAINRET( ino->ops->write, ino, buffer, file_offset, count );
}

/**
 * @brief  Call on the fs driver to resize a file on backing storage
 * @see fs_device_operations
 * 
 * @param inode       The inode for the file
 * @param size	      The new size of the file
 *
 * @exception ENOTSUP This driver does not support this function.
 */

SVFUNC( ifs_truncate, inode_t * inode, aoff_t size )
{
	/* This function is implemented by the FS driver */
	assert (inode != NULL);

	/* Check if the driver supports truncate */
	if (!inode->ops->truncate) {
		/* If not: return the error "Operation not supported" */
		THROWV(ENOTSUP);
	}

	/* Call the driver */
	CHAINRETV( inode->ops->truncate, inode, size);
}

/**
 * @brief  Call on the fs driver to synchronize its metadata
 * @see fs_device_operations
 * 
 * @param device	The driver instance to operate on
 *
 * @exception ENOTSUP This driver does not support this function.
 */

SVFUNC( ifs_sync, fs_device_t *device )
{
	/* This function is implemented by the FS driver */
	assert (device != NULL);

	/* Check if the driver supports truncate */
	if (!device->ops->sync) {
		/* If not: return the error "Operation not supported" */
		THROWV(ENOTSUP);
	}

	/* Call the driver */
	CHAINRETV( device->ops->sync, device );
}

