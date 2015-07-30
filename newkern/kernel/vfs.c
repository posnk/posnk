/**
 * @file kernel/vfs.c
 *
 * Implements the virtual filesystem layer
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 12-04-2014 - Created
 * \li 11-07-2014 - Rewrite 1
 * \li 12-07-2014 - Commented
 */

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/errno.h>

#include "kernel/heapmm.h"
#include "kernel/vfs.h"
#include "kernel/scheduler.h"
#include "kernel/permissions.h"
#include "kernel/synch.h"
#include "kernel/device.h"
#include "kernel/pipe.h"

#include "config.h"

/** @name VFS Internal
 *  Utility functions for use by VFS functions only
 */
///@{

/**
 * @brief Get an inode by it's ID
 * 
 * @param device   The device to get the inode from
 * @param inode_id The ID of the inode to look up
 * @return The inode with id inode_id from device.
 */

SFUNC(inode_t *, vfs_get_inode, uint32_t device_id, ino_t inode_id)
{
	errno_t	 	 status;
	inode_t 	*result;
	fs_mount_t  *mount;

	/* Try to get inode from cache */
	result = vfs_get_cached_inode(device_id, inode_id);

	if (result) {
		/* inode was in cache */
		RETURN(result);
	}
	
	/* Get the mount for the device_id */
	mount = vfs_get_mount_by_dev( device_id );
	
	if ( mount == NULL )
		THROW(ENXIO, NULL);
	
	/* Cache miss, fetch it from disk */
	//NOTE : Schedule may happen below!
	status = ifs_get_ino ( mount->device, inode_id, &result );
	if (status)
		THROW(status, NULL);

	//NOTE : Schedule may have happened

	RETURN(vfs_inode_ref(result));
}

///@}

/** 
 * @brief Find a directory entry
 *
 * @param inode The directory to search
 * @param name  The filename to match
 * @return The inode number pointed to by the directory entry
 *
 * @exception ENOTSUP This driver does not support this function.
 * @exception EACCES The current user does not have search permission 
 * 					 for the directory
 * @exception ENOENT The directory entry was not found
 */
 
SFUNC(ino_t, vfs_findent, inode_t * ino, const char * name)
{
	errno_t	status;
	
	/* This function is implemented by the FS driver */
	assert ( ino != NULL );
	assert ( name != NULL );

	/* Check permissions on parent dir */
	status = ifs_access( ino, MODE_EXEC );

	/* Check for errors */
	if ( status ) {

		/* If not: return the error "Permission denied" */
		THROW( status, NULL );

	}

	/* Call the driver */
	CHAINRET( ifs_findent, inode, name );
}

/** @name VFS API
 *  Public VFS functions
 */
///@{

/**
 * @brief Write data to a file
 * 
 * If the data to be written would extend past the end of the file, the file 
 * will grow. If file_offset lies past the end of the file, the file will 
 * grow and the resulting gap will be filled with zero bytes.
 * 
 * @param _inode      The inode for the file
 * @param buffer      The buffer containing the data to write
 * @param file_offset The offset in the file to start writing at
 * @param count       The number of bytes to write
 * @param write_size  A pointer to a variable in which the number of bytes 
 *			 written will be stored
 * @param non_block   Whether to block the call until room is available in 
 *			 the file, this is only supported on FIFO's and 
 *                       character devices
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT Atleast one parameter was a NULL pointer
 * @exception EBADF  Tried to write to a file for which we do not have 
 *                    permission
 * @exception EISDIR Direct writes to directories are not allowed
 * @exception EIO    An IO error was encountered trying to write to the file
 * @exception EPIPE  The FIFO has no room left and there are no (open) read 
 *                  endpoints
 * @exception ENOSPC The disk was full when trying to write to the file
 * @exception ENOMEM Could not allocate kernel heap for a temporary structure
 */

int vfs_write(inode_t * _inode , aoff_t file_offset, void * buffer, aoff_t count, aoff_t *write_size, int non_block)
{
	inode_t *inode;
	errno_t status;
	aoff_t pad_size = 0;
	void *zbuffer;

	/* Check for null pointers */
	assert (_inode != NULL);
	assert (buffer != NULL);
	assert (write_size != NULL);

	/* Resolve the effective inode */
	inode = vfs_effective_inode(_inode);

	/* Accuire a lock on the inode */
	semaphore_down(inode->lock);

	/* Verify write permission */
	if (!vfs_have_permissions(inode, MODE_WRITE)) {
		
		/* Release the lock on this inode */
		semaphore_up(inode->lock);

		/* Release the dereferenced inode */
		vfs_inode_release(inode);
		
		/* Return the error "Bad file descriptor" */
		/* because the stream API should already  */
		/* have checked permissions */		
		return EBADF;

	}

	/* Handle write for each file type */
	switch ((inode->mode) & S_IFMT) {
		case S_IFREG:
			/* The file is a regular file */

			/* Check whether the write extends past EOF */
			if ((file_offset + count) > inode->size) {
				/* Writing past EOF */

				/* Check whether the write starts past EOF */
				if (file_offset > inode->size) {
					/* Writing starts past EOF */

					/* Allocate a buffer for the zero pad */
					//TODO: Make this more efficient, implement sparse files
					zbuffer = heapmm_alloc(file_offset - inode->size);

					/* Fill the buffer with zeroes */
					memset(zbuffer, 0, file_offset - inode->size);

					/* Write the zeroes to the file */
					status = ifs_write(inode, zbuffer, inode->size, file_offset - inode->size, &pad_size);

					/* Free the zero pad buffer */
					heapmm_free(zbuffer,file_offset - inode->size);		

					/* Increase the file size to include the padding */
					inode->size += pad_size;
	
					/* If padding was actually written, update mtime */
					if (pad_size)
						inode->mtime = system_time;

					/* Check for errors writing the padding */
					if (status) {
						/* An error occurred */

						/* Set write_size to 0, we have not yet written anything */
						*write_size = 0;

						/* Release the lock on this inode */
						semaphore_up(inode->lock);

						/* Release the dereferenced inode */
						vfs_inode_release(inode);
			
						/* Pass the error to the caller */
						return status;	
					}
				}
			}		
			
			/* Call on the FS driver to write the data */
			status = ifs_write(inode, buffer, file_offset, count, write_size);

			/* If data was actually written, update mtime */
			if (*write_size)
				inode->mtime = system_time;

			/* If the file grew, update file size */
			if ((file_offset + *write_size) > inode->size)
				inode->size = file_offset + *write_size;

			/* Release the lock on this inode */
			semaphore_up(inode->lock);

			/* Release the dereferenced inode */
			vfs_inode_release(inode);


			/* Pass any errors on the the caller */
			return status;

		case S_IFDIR:
			/* File is a directory */

			/* Release the lock on this inode */
			semaphore_up(inode->lock);

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Return error "Is a directory" */
			return EISDIR;

		case S_IFIFO:
			/* File is a FIFO */

			/* Release the lock on this inode so write accesses */
			/* can take place now */
			semaphore_up(inode->lock);

			/* Call on the pipe driver to write the data */
			status = pipe_write(inode->fifo, buffer, count, write_size, non_block);		

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Pass any errors on the the caller */
			return status;

		case S_IFCHR:			
			/* File is a character special file */

			/* Release the lock on this inode */	
			semaphore_up(inode->lock);	
			
			/* Call on the driver to write the data */			
			status = device_char_write(inode->if_dev, file_offset, buffer, count, write_size, non_block);

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Pass any errors on the the caller */
			return status;	

		case S_IFBLK:	
			/* File is a block special file */
			
			/* Call on the driver to write the data */	
			status = device_block_write(inode->if_dev, file_offset, buffer, count, write_size);

			/* Release the lock on this inode */			
			semaphore_up(inode->lock);

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Pass any errors on the the caller */			
			return status;	

		default:		
			/* Unknown file type */

			/* Release the lock on this inode */		
			semaphore_up(inode->lock);	

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Return error "Invalid argument" */
			return EINVAL;		

	}
	
}

/**
 * @brief Read data from a file
 *
 * If more data is requested than the file contains this function will 
 * successfully complete but read_size will be smaller than count
 *  
 * @param _inode       The inode for the file
 * @param buffer       The buffer to store the data in
 * @param file_offset  The offset in the file to start reading at
 * @param count        The number of bytes to read
 * @param read_size    A pointer to a variable in which the number of bytes 
 *			 read will be stored
 * @param non_block    Whether to block the call until data is available in 
 *			 the file, this is only supported on FIFO's and 
 *                       character devices
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception  EFAULT Atleast one parameter was a NULL pointer
 * @exception  EBADF  Tried to read from a file for which we do not have
 *                    permission
 * @exception  EISDIR Direct reads from directories are not allowed
 * @exception  EIO    An IO error was encountered trying to read from the file
 * @exception  EPIPE  The FIFO has no data left and there are no (open) write
 *                    endpoints
 * @exception  ENOMEM Could not allocate kernel heap for a temporary structure
 */

int vfs_read(inode_t * _inode , aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size, int non_block)
{
	int status;
	inode_t *inode;

	/* Check for null pointers */
	assert (_inode != NULL);
	assert (buffer != NULL);
	assert (read_size != NULL);

	/* Resolve the effective inode */
	inode = vfs_effective_inode(_inode);

	/* Accuire a lock on the inode */
	semaphore_down(inode->lock);

	/* Verify read permission */
	if (!vfs_have_permissions(inode, MODE_READ)) {
		
		/* Release the lock on this inode */
		semaphore_up(inode->lock);	

		/* Release the dereferenced inode */
		vfs_inode_release(inode);
		
		/* Return the error "Bad file descriptor" */
		/* because the stream API should already  */
		/* have checked permissions */		
		return EBADF;

	}

	/* Handle read for each file type */
	switch ((inode->mode) & S_IFMT) {
		case S_IFREG:
			/* The file is a regular file */

			/* Check whether the read extends past EOF */
			if ((file_offset + count) > inode->size) {
				/* Reading past EOF */

				/* Check whether the read starts past EOF */
				if (file_offset >= inode->size) {
					/* Read starts past EOF */

					/* Release the lock on this inode */
					semaphore_up(inode->lock);

					/* Set read_size to 0, we have not yet read anything */
					(*read_size) = 0;
					
					/* Release the dereferenced inode */
					vfs_inode_release(inode);
			
					/* This is not an error condition, return 0 */
					return 0;					
				}

				/* Reduce count so we read until EOF */
				count = inode->size - file_offset;	
			}				

			/* Call on the FS driver to read the data */
			status = ifs_read(inode, buffer, file_offset, count, read_size);

			/* If data was actually read, update atime */
			if (*read_size)
				inode->atime = system_time;

			/* Release the lock on this inode */
			semaphore_up(inode->lock);	

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Pass any errors on the the caller */
			return status;
		case S_IFDIR:
			/* File is a directory */

			/* Release the lock on this inode */
			semaphore_up(inode->lock);

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Return error "Is a directory" */
			return EISDIR;

		case S_IFIFO:
			/* File is a FIFO */

			/* Release the lock on this inode so read accesses */
			/* can take place now */
			semaphore_up(inode->lock);

			status = pipe_read(inode->fifo, buffer, count, read_size, non_block);		

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			return status;

		case S_IFCHR:	
			/* File is a character special file */
			
			/* Call on the driver to read the data */	
			status = device_char_read(inode->if_dev, file_offset, buffer, count, read_size, non_block);	

			/* Release the lock on this inode */			
			semaphore_up(inode->lock);		

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Pass any errors on the the caller */			
			return status;	

		case S_IFBLK:	
			/* File is a block special file */
			
			/* Call on the driver to read the data */	
			status = device_block_read(inode->if_dev, file_offset, buffer, count, read_size);	

			/* Release the lock on this inode */			
			semaphore_up(inode->lock);	

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Pass any errors on the the caller */			
			return status;	

		default:	
			/* Unknown file type */

			/* Release the lock on this inode */		
			semaphore_up(inode->lock);		

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Return error "Invalid argument" */
			return EINVAL;			

	}
	
}

/**
 * @brief Read directory entries
 * 
 * This function will only read whole directory entries, if a directory entry
 * does not fit in count it will return only the amount of bytes read before 
 * that entry
 *
 * @param _inode      The inode for the directory
 * @param buffer      The buffer to store the entries in
 * @param file_offset The offset in the directory to start reading at
 * @param count       The number of bytes to read
 * @param read_size   A pointer to a variable in which the number of bytes 
 *			 read will be stored
 *
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT  Atleast one parameter was a NULL pointer
 * @exception EBADF   Tried to read from a directory for which we do not have
 *                  	permission
 * @exception ENOTDIR Can only read directory entries from a directory
 * @exception EIO     An IO error was encountered trying to read from the dir
 * @exception ENOMEM  Could not allocate kernel heap for a temporary structure
 */

int vfs_getdents(inode_t * _inode , aoff_t file_offset, dirent_t * buffer, aoff_t count, aoff_t *read_size)
{
	int status;
	inode_t *inode;

	/* Check for null pointers */
	assert (_inode != NULL);
	assert (buffer != NULL);
	assert (read_size != NULL);

	/* Resolve the effective inode */
	inode = vfs_effective_inode(_inode);

	/* Accuire a lock on the inode */
	semaphore_down(inode->lock);

	/* Verify read permission */
	if (!vfs_have_permissions(inode, MODE_READ)) {
		
		/* Release the lock on this inode */
		semaphore_up(inode->lock);

		/* Release the dereferenced inode */
		vfs_inode_release(inode);
		
		/* Return the error "Bad file descriptor" */
		/* because the stream API should already  */
		/* have checked permissions */		
		return EBADF;

	}

	/* Handle read for each file type */
	switch ((inode->mode) & S_IFMT) {
		case S_IFDIR:
			/* File is a directory */

			/* Check whether the read extends past EOF */
			if ((file_offset + count) > inode->size) {
				/* Reading past EOF */

				/* Check whether the read starts past EOF */
				if (file_offset >= inode->size) {
					/* Read starts past EOF */

					/* Release the lock on this inode */
					semaphore_up(inode->lock);

					/* Set read_size to 0, we have not yet read anything */
					(*read_size) = 0;

					/* Release the dereferenced inode */
					vfs_inode_release(inode);
					
					/* This is not an error condition, return 0 */
					return 0;					
				}

				/* Reduce count so we read until EOF */
				count = inode->size - file_offset;	
			}				

			/* Call on the FS driver to read the entries */			
			status = ifs_read_dir(inode, buffer, file_offset, count, read_size);

			/* If entries were actually read, update atime */
			if (*read_size)
				inode->atime = system_time;

			/* Release the lock on this inode */
			semaphore_up(inode->lock);

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Pass any errors on the the caller */
			return status;
		default:		
			/* Other file type */

			/* Release the lock on this inode */		
			semaphore_up(inode->lock);

			/* Release the dereferenced inode */
			vfs_inode_release(inode);	

			/* Return error "Not a directory" */
			return ENOTDIR;			

	}
	
}

/**
 * @brief Resize a file
 * 
 * If the new size is larger than the file currently is the new room will be
 * filled with zero bytes, if the new size is smaller the data in the truncated
 * part of the file is deleted
 * 
 * @param _inode      The inode for the file
 * @param length      The new size of the file
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT  Atleast one parameter was a NULL pointer
 * @exception EACCES Tried to resize a file for which we do not have permission
 * @exception EISDIR Directories can not be resized
 * @exception EIO    An IO error was encountered trying to resize the file
 * @exception ENOSPC The disk was full when trying to grow the file
 * @exception ENOMEM Could not allocate kernel heap for a temporary structure
 */

int vfs_truncate(inode_t * _inode, aoff_t length)
{
	int status;
	inode_t *inode;

	/* Check for null pointers */
	assert (_inode != NULL);

	/* Resolve the effective inode */
	inode = vfs_effective_inode(_inode);

	/* Accuire a lock on the inode */
	semaphore_down(inode->lock);

	/* Verify write permission */
	if (!vfs_have_permissions(inode, MODE_WRITE)) {
		
		/* Release the lock on this inode */
		semaphore_up(inode->lock);

		/* Release the dereferenced inode */
		vfs_inode_release(inode);
		
		/* Return the error "Permission denied" */	
		return EACCES;

	}

	/* Handle truncate for each file type */
	switch ((inode->mode) & S_IFMT) {
		case S_IFREG:		
			/* The file is a regular file */

			/* Call on FS driver to resize file */	
			status = ifs_truncate(inode, length);

			/* If resize succeeded, update metadata */
			if (!status) {
				inode->mtime = system_time;
				inode->size = length;
			}

			/* Release the lock on this inode */
			semaphore_up(inode->lock);

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Pass any errors on the the caller */
			return status;

		case S_IFDIR:
			/* File is a directory */

			/* Release the lock on this inode */
			semaphore_up(inode->lock);

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Return error "Is a directory" */
			return EISDIR;

		case S_IFBLK:	
		case S_IFCHR:	
		case S_IFIFO:
		default:		
			/* Other file type */

			/* Release the lock on this inode */		
			semaphore_up(inode->lock);	

			/* Release the dereferenced inode */
			vfs_inode_release(inode);

			/* Return error "Invalid argument" */
			return EINVAL;		

	}
	
}

/** 
 * @brief Delete a directory
 * @param path The path of the directory to delete
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT Atleast one parameter was a NULL pointer
 * @exception ENOENT The directory does not exist
 * @exception EEXIST The directory still has files in it
 * @exception EBUSY  The directory is in use
 * @exception EIO    An IO error occurred while deleting the directory
 * @exception EACCES Tried to delete a directory from a directory for which we
 *                   do not have write permission
 * @exception ENAMETOOLONG The name of the file is longer than the maximum 
 *                   allowed number of characters for a file name
 */

int vfs_rmdir(char *path)
{

	inode_t *inode;
	errno_t status;

	/* Check for null pointers */
	assert (path != NULL);

	/* Lookup the inode for the directory*/
	status = vfs_find_inode(path, &inode);

	/*If the inode does not exist, return error "No such file or directory"*/
	if (status)
		return status;	

	/* Acquire a lock on the inode */
	semaphore_down(inode->lock);

	/* If directory still has entries, return error "File exists" */
	if (inode->size) {

		/* Release the lock */
		semaphore_up(inode->lock);

		return EEXIST;
	}

	/* If a file system is currently mounted on this directory or if it */
	/* is the root directory for the current process, return error : */
	/* "Resource busy" */
	if (inode->mount || (scheduler_current_task->root_directory->inode == inode))
		return EBUSY;

	/* Release the inode */
	vfs_inode_release(inode);	

	/* Release the lock */
	semaphore_up(inode->lock);	

	/* Remove directory */
	return vfs_unlink(path);
}

/**
 * @brief Create a directory
 * @param path The path of the directory to create
 * @param mode The mode of the directory to create, only access modes are used
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT Atleast one parameter was a NULL pointer
 * @exception ENOENT The parent directory does not exist
 * @exception EEXIST A file already exists at the given path
 * @exception EIO    An IO error occurred while creating the directory
 * @exception EACCES User does not have write permission for the parent dir
 * @exception ENOSPC Not enough disk space was available
 * @exception ENOMEM Could not allocate kernel heap for a temporary structure 
 * @exception ENAMETOOLONG The name of the file is longer than the maximum 
 *                   allowed number of characters for a file name
 */

int vfs_mkdir(char *path, mode_t mode)
{
	inode_t *dir;
	inode_t *parent;
	int err;
	
	/* Check for null pointers */
	assert (path != NULL);

	/* Create the directory inode */
	err = vfs_mknod(path, S_IFDIR | (mode & 0777), 0);

	/* If there was an error, pass it to the caller */
	if (err)
		return err;

	/* Look up the inode for the parent directory */
	err = vfs_find_parent(path, &parent);

	if (err)
		return err;

	/* Look up the inode for the new directory */
	err = vfs_find_inode(path, &dir);

	if (err)
		return err;

	/* Accuire a lock on the inode */
	semaphore_down(dir->lock);

	/* Create directory internal structure */
	err = ifs_mkdir(dir);

	/* If there was an error, remove the directory */
	if (err) {

		/* Release the lock on this inode */
		semaphore_up(dir->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the inode */
		vfs_inode_release(dir);

		/* Delete the directory */
		vfs_unlink(path);

		/* Pass the error to the caller */
		return err;
	}
	
	/* Update mtime of the parent dir */
	parent->mtime = system_time;

	/* Release the lock on this inode */
	semaphore_up(dir->lock);

	/* Make self (".") link */
	err = ifs_link(dir, ".", dir->id);

	/* If there was an error, remove the directory */
	if (err) {

		/* Release the lock on this inode */
		semaphore_up(dir->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the inode */
		vfs_inode_release(dir);

		/* Delete the directory */
		vfs_unlink(path);

		/* Pass the error to the caller */
		return err;
	}

	/* Make parent ("..") link */
	err = ifs_link(dir, "..", parent->id);

	/* If there was an error, remove the directory */
	if (err) {

		/* Release the lock on this inode */
		semaphore_up(dir->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the inode */
		vfs_inode_release(dir);

		/* Delete the directory */
		vfs_unlink(path);

		/* Pass the error to the caller */
		return err;
	}

	/* Release the parent inode */
	vfs_inode_release(parent);

	/* Release the inode */
	vfs_inode_release(dir);

	/* Return success */
	return 0;
}

/**
 * @brief Create a file
 * @param path The path of the file to create
 * @param mode The mode of the file to create
 * @param dev  The device id to use in case of a special file
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT Atleast one parameter was a NULL pointer
 * @exception ENOENT The parent directory does not exist
 * @exception EEXIST A file already exists at the given path
 * @exception EIO    An IO error occurred while creating the file
 * @exception EACCES User does not have write permission for the parent dir
 * @exception ENOSPC Not enough disk space was available
 * @exception ENOMEM Could not allocate kernel heap for a temporary structure 
 * @exception ENAMETOOLONG The name of the file is longer than the maximum 
 *                   allowed number of characters for a file name
 */

int vfs_mknod(char *path, mode_t mode, dev_t dev)
{
	int status;
	char * name;
	inode_t *parent;
	inode_t *inode;
	inode_t *_inode;

	/* Check for null pointers */
	assert (path != NULL);
	
	/* Look up the inode for the parent directory */
	status = vfs_find_parent(path, &parent);

	/* Check if it exists, if not, return error */
	if (status) 
		return status;

	/* Allocate memory for new inode */
	inode = heapmm_alloc(parent->device->inode_size);

	/* Check if the allocation succeded, if not, return error */
	if (!inode) {

		/* Release the parent inode */
		vfs_inode_release(parent);

		return ENOMEM;
	}

	/* Acquire a lock on the parent inode */
	semaphore_down(parent->lock);

	/* Check whether the file already exists */
	status = vfs_find_inode(path, &_inode);
	if (status != ENOENT) {
		/* If so, clean up and return an error */
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the inode */
		vfs_inode_release(_inode);
		if (!status)
			return EEXIST;
		else 
			return status;
	}

	/* Check permissions on parent dir */
	if (!vfs_have_permissions(parent, MODE_WRITE)) {
		/* If not, clean up and return an error */
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		return EACCES;
	}

	/* Clear the newly allocated inode */
	memset(inode, 0, parent->device->inode_size);

	/* Fill it's device fields */
	inode->device_id = parent->device_id;
	inode->device = parent->device;

	/* INODE ID is filled by fs driver */

	/* Resolve the filename */
	status = vfs_get_filename(path, &name);
	if (status) {
		/* Length is too long, clean up and return error */
		heapmm_free(inode, parent->device->inode_size);	

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);	
	
		/* Release the parent inode */
		vfs_inode_release(parent);
	
		return status;
	}

	/* Check the filename length */
	if (strlen(name) >= CONFIG_FILE_MAX_NAME_LENGTH) {
		/* Length is too long, clean up and return error */
		heapmm_free(inode, parent->device->inode_size);	

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Free filename */
		heapmm_free(name, strlen(name) + 1);		
	
		/* Release the parent inode */
		vfs_inode_release(parent);
	
		return ENAMETOOLONG;
	}

	/* Fill other inode fields */
	inode->mode = mode & ~(scheduler_current_task->umask);
	inode->if_dev = dev;
	inode->uid = scheduler_current_task->uid;
	inode->gid = scheduler_current_task->gid;

	/* Allocate inode lock */
	inode->lock = semaphore_alloc();

	/* Set atime, mtime, ctime */
	inode->atime = inode->mtime = inode->ctime = system_time;

	/* If needed, allocate pipe for FIFO */
	if (S_ISFIFO(inode->mode)) {
		inode->fifo = pipe_create();
		assert(inode->fifo != NULL);
	}
	
	/* Push inode to storage */
	status = ifs_mknod(inode);

	/* Check for errors */
	if (status) { 
		/* If an error occurred, clean up */
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Free filename */
		heapmm_free(name, strlen(name) + 1);			

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Pass the error to the caller */
		return status;
	}	

	/* Inode is now on storage and in the cache */
	/* Proceed to make initial link */
	status = ifs_link(parent, name, inode->id);

	/* Free filename */
	heapmm_free(name, strlen(name) + 1);

	/* Check for errors */
	if (status) {
		/* If an error occurred, clean up */	
		llist_unlink((llist_t *) inode);
		ifs_rmnod(inode);
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Pass the error to the caller */
		return status;
	}

	/* Set link count to 1 */
	inode->hard_link_count++;

	/* Release the lock on the inode */
	semaphore_up(inode->lock);

	/* Release the lock on the parent inode*/
	semaphore_up(parent->lock);

	/* Release the parent inode */
	vfs_inode_release(parent);

	/* Add inode to cache */
	vfs_inode_cache(inode);

	/* Return success */
	return 0;	
}

/**
 * @brief Reads a symbolic link path
 * @param inode The symbolic link inode to read
 * @param buffer The buffer to read the path to
 * @param size The size of the read buffer
 * @param read_size The number of characters actually read
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT Atleast one parameter was a NULL pointer
 * @exception EINVAL The file is not a symbolic link
 * @exception EIO    An IO error occurred while reading the link
 * @exception EACCES User does not have read permission for the symbolic link
 * @exception ENOMEM Could not allocate kernel heap for a temporary structure 
 */

int vfs_readlink(inode_t *_inode, char * buffer, size_t size, size_t *read_size)
{

	inode_t *inode;
	int status;
	aoff_t _read_size;

	/* Check for null pointers */
	assert (_inode != NULL);
	assert (buffer != NULL);
	assert (read_size != NULL);

	/* Get a stable reference to the inode */
	inode = vfs_inode_ref(_inode);

	/* Accuire a lock on the inode */
	semaphore_down(inode->lock);

	/* Verify read permission */
	if (!vfs_have_permissions(inode, MODE_READ)) {
		
		/* Release the lock on this inode */
		semaphore_up(inode->lock);	

		/* Release the dereferenced inode */
		vfs_inode_release(inode);
		
		/* Return the error "Access Denied"  */
		/* because this call might have been */
		/* made without checking permissions */		
		return EACCES;

	}

	/* Make sure the file is a symbolic link */
	if (!S_ISLNK(inode->mode)) {
		
		/* Release the lock on this inode */
		semaphore_up(inode->lock);	

		/* Release the dereferenced inode */
		vfs_inode_release(inode);
		
		/* Return the error "Invalid Operation", */
		/* we can not readlink() something that  */
		/* is not a symbolic link */		
		return EINVAL;

	}

	if (size > inode->size)
		size = inode->size;	

	/* Call on the FS driver to read the data */
	status = ifs_read(inode, (void *) buffer, 0, (aoff_t) size, &_read_size);

	*read_size = (size_t) _read_size;

	/* If data was actually read, update atime */
	if (*read_size)
		inode->atime = system_time;

	/* Release the lock on this inode */
	semaphore_up(inode->lock);	

	/* Release the dereferenced inode */
	vfs_inode_release(inode);

	/* Pass any errors on the the caller */
	return status;

}


/**
 * @brief Create a symbolic link
 * @param oldpath The target of the link
 * @param path The path of the link
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT Atleast one parameter was a NULL pointer
 * @exception ENOENT The parent directory does not exist
 * @exception EEXIST A file already exists at the given path
 * @exception EIO    An IO error occurred while creating the file
 * @exception EACCES User does not have write permission for the parent dir
 * @exception ENOSPC Not enough disk space was available
 * @exception ENOMEM Could not allocate kernel heap for a temporary structure 
 * @exception ENAMETOOLONG The name of the file is longer than the maximum 
 *                   allowed number of characters for a file name
 */

int vfs_symlink(char *oldpath, char *path)
{
	aoff_t write_size_rv, write_size;
	int status;
	char * name;
	inode_t *parent;
	inode_t *inode;
	inode_t *_inode;

	/* Check for null pointers */
	assert (path != NULL);
	assert (oldpath != NULL);
	
	/* Look up the inode for the parent directory */
	status = vfs_find_parent(path, &parent);

	/* Check if it exists, if not, return error */
	if (status) 
		return status;

	/* Allocate memory for new inode */
	inode = heapmm_alloc(parent->device->inode_size);

	/* Acquire a lock on the parent inode*/
	semaphore_down(parent->lock);

	/* Check if the allocation succeded, if not, return error */
	if (!inode) {

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		return ENOMEM;
	}

	status = vfs_find_inode(path, &_inode);

	/* Check whether the file already exists */
	if (status != ENOENT) {
		/* If so, clean up and return an error */
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		if (!status) {
			return EEXIST;

			/* Release the parent inode */
			vfs_inode_release(_inode);
		} else 
			return status;
	}

	/* Check permissions on parent dir */
	if (!vfs_have_permissions(parent, MODE_WRITE)) {
		/* If not, clean up and return an error */
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		return EACCES;
	}

	/* Clear the newly allocated inode */
	memset(inode, 0, parent->device->inode_size);

	/* Fill it's device fields */
	inode->device_id = parent->device_id;
	inode->device = parent->device;

	/* INODE ID is filled by fs driver */

	/* Resolve the filename */
	
	status = vfs_get_filename(path, &name);

	/* Check the filename length */
	if (status) {
		/* Length is too long, clean up and return error */
		heapmm_free(inode, parent->device->inode_size);	

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Free filename */
		heapmm_free(name, strlen(name) + 1);
	
		return status;
	}

	/* Check the filename length */
	if (strlen(name) >= CONFIG_FILE_MAX_NAME_LENGTH) {
		/* Length is too long, clean up and return error */
		heapmm_free(inode, parent->device->inode_size);	

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Free filename */
		heapmm_free(name, strlen(name) + 1);
	
		return ENAMETOOLONG;
	}
	/* Fill other inode fields */
	inode->mode = S_IFLNK | 0777;
	inode->if_dev = 0;
	inode->uid = scheduler_current_task->uid; 
	inode->gid = scheduler_current_task->gid; 

	/* Allocate inode lock */
	inode->lock = semaphore_alloc();

	/* Set atime, mtime, ctime */
	inode->atime = inode->mtime = inode->ctime = system_time;

	/* Push inode to storage */
	status = ifs_mknod(inode);

	/* Check for errors */
	if (status) { 
		/* If an error occurred, clean up */
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Pass the error to the caller */
		return status;
	}	

	/* Set link target */
	write_size = strlen(oldpath) + 1;

	status = ifs_write(inode, oldpath, 0, write_size, &write_size_rv);

	inode->size = write_size;

	/* Check for errors */
	if (status || (write_size_rv != write_size)) {
		/* If an error occurred, clean up */
		ifs_rmnod(inode);
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Pass the error to the caller */
		return status;
	}

	/* Add inode to cache */
	vfs_inode_cache(inode);

	/* Inode is now on storage and in the cache */
	/* Proceed to make initial link */
	status = ifs_link(parent, name, inode->id);

	/* Free filename */
	heapmm_free(name, strlen(name) + 1);

	/* Check for errors */
	if (status) {
		/* If an error occurred, clean up */	
		llist_unlink((llist_t *) inode);
		ifs_rmnod(inode);
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Pass the error to the caller */
		return status;
	}

	/* Set link count to 1 */
	inode->hard_link_count++;

	/* Release the lock on the inode */
	semaphore_up(inode->lock);

	/* Add inode to cache */
	vfs_inode_cache(inode);

	/* Release the lock on the parent inode*/
	semaphore_up(parent->lock);

	/* Release the parent inode */
	vfs_inode_release(parent);

	/* Return success */
	return 0;	
}

/**
 * @brief Delete a file
 * @param path The path of the file to delete
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT Atleast one parameter was a NULL pointer
 * @exception ENOENT The file does not exist
 * @exception EEXIST A file already exists at the given path
 * @exception EBUSY  The file is in use
 * @exception EIO    An IO error occurred while deleting the file
 * @exception EACCES User does not have write permission for the file
 * @exception ENOMEM Could not allocate kernel heap for a temporary structure 
 * @exception ENAMETOOLONG The name of the file is longer than the maximum 
 *                   allowed number of characters for a file name
 */


int vfs_unlink(char *path)
{
	int status;
	inode_t *inode;
	inode_t *parent;
	char * name;

	/* Check for null pointers */
	assert (path != NULL);
	
	/* Look up the inode for the file */

	status = vfs_find_symlink(path, &inode);

	/* Check if it exists, if not, return error */
	if (status)
		return status;
	
	/* Look up the inode for the parent directory */
	status = vfs_find_parent(path, &parent);

	/* Check if it exists, if not, return error */
	if (status) {

		/* Release the inode */
		vfs_inode_release(inode);

		return status;
	}

	/* Check permissions on parent dir */
	if (!vfs_have_permissions(parent, MODE_WRITE)) {

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the inode */
		vfs_inode_release(inode);

		return EACCES;
	}

	/* Aqquire lock on inode */
	semaphore_down(inode->lock);

	/* Resolve the filename */
	
	status = vfs_get_filename(path, &name);

	/* Check the filename length */
	if (status) {
		/* Length is too long, clean up and return error */	
		semaphore_up(inode->lock);


		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the inode */
		vfs_inode_release(inode);

		return status;
	}

	/* Check the filename length */
	if (strlen(name) >= CONFIG_FILE_MAX_NAME_LENGTH) {
		/* Length is too long, clean up and return error */		
		semaphore_up(inode->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the inode */
		vfs_inode_release(inode);

		/* Free filename */
		heapmm_free(name, strlen(name) + 1);

		return ENAMETOOLONG;
	}

	/* Aqquire lock on the parent dir */
	semaphore_down(parent->lock);

	/* Delete directory entry */
	status = ifs_unlink(parent, name);

	/* Free filename */
	heapmm_free(name, strlen(name) + 1);
	
	/* Check for errors */
	if (status) {
		/* Release the lock on the inode */
		semaphore_up(inode->lock);
		/* Release the lock on the parent dir */
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the inode */
		vfs_inode_release(inode);

		/* Pass error to the caller */
		return status;
	}

	/* Release the lock on the parent dir */
	semaphore_up(parent->lock);

	/* Decrease hardlink count */
	inode->hard_link_count--;
	
	/* Update ctime */
	inode->ctime = system_time;

	/* Check for orphaned files */
	if (inode->hard_link_count == 0) {
		/* File is orphaned */

		/* Check if file is in use */
		if (inode->open_count != 0) {
			semaphore_up(inode->lock);

			/* Release the parent inode */
			vfs_inode_release(parent);

			/* Release the inode */
			vfs_inode_release(inode);

			return EBUSY;
		}

		/* Delete the file itself */
		ifs_rmnod(inode); //This might error but there is little we can do about that anyway
		/* Remove its inode from the cache */
		llist_unlink((llist_t *) inode);
		/* Free its inode */
		heapmm_free(inode, inode->device->inode_size);	

		/* Release the parent inode */
		vfs_inode_release(parent);
	
	} else {
		/* Release the lock on the inode */
		semaphore_up(inode->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the inode */
		vfs_inode_release(inode);
	}

	/* Return success */
	return 0;
}

/**
 * @brief Create a hard link
 * @param oldpath The target of the link
 * @param newpath The path of the link
 * @return In case of error: a valid error code, Otherwise 0
 *
 * @exception EFAULT Atleast one parameter was a NULL pointer
 * @exception ENOENT The parent directory or target does not exist
 * @exception EEXIST A file already exists at the given path
 * @exception EIO    An IO error occurred while creating the link
 * @exception EACCES User does not have write permission for the parent dir
 * @exception ENOSPC Not enough disk space was available
 * @exception ENOMEM Could not allocate kernel heap for a temporary structure 
 * @exception EXDEV  Tried to create a cross-device link 
 * @exception ENAMETOOLONG The name of the file is longer than the maximum 
 *                   allowed number of characters for a file name
 */

int vfs_link(char *oldpath, char *newpath)
{
	int status;
	char * name;
	inode_t *inode;
	inode_t *_inode;
	inode_t *parent;

	/* Check for null pointers */
	assert (newpath != NULL);
	assert (oldpath != NULL);
	
	/* Look up the inode for the parent directory */
	status = vfs_find_parent(newpath, &parent);

	/* Check if it exists, if not, return error */
	if (status) 
		return status;
	
	/* Look up the inode for the target */
	status = vfs_find_inode(oldpath, &inode);

	/* Check if it exists, if not, return error */
	if (!inode) {

		/* Release the parent inode */
		vfs_inode_release(parent);

		return status;

	}

	/* Aqquire a lock on the target */
	semaphore_down(inode->lock);
	
	/* Aqquire a lock on the parent directory */
	semaphore_down(parent->lock);

	/* Check whether the file already exists */
	status = vfs_find_inode(newpath, &_inode);

	if (status != ENOENT) {
		/* If so, clean up and return an error */
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the target inode */
		vfs_inode_release(inode);

		if (!status) {
			vfs_inode_release(_inode);
			return EEXIST;
		} else 
			return status;
	}

	/* Check permissions on parent dir */
	if (!vfs_have_permissions(parent, MODE_WRITE)) {
		/* If not, clean up and return an error */
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the target inode */
		vfs_inode_release(inode);

		return EACCES;
	}

	/* Check for cross-device links */
	if (parent->device_id != inode->device_id) {
		/* If so, clean up and return an error */
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);	

		/* Release the parent inode */
		vfs_inode_release(parent);	

		/* Release the target inode */
		vfs_inode_release(inode);

		return EXDEV;
	}

	/* Resolve the filename */
	
	status = vfs_get_filename(newpath, &name);

	if (status) {
		/* Length is too long, clean up and return error */
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);	

		/* Release the target inode */
		vfs_inode_release(inode);
		
		return status;
	}

	/* Check the filename length */
	if (strlen(name) >= CONFIG_FILE_MAX_NAME_LENGTH) {
		/* Length is too long, clean up and return error */
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);	

		/* Release the target inode */
		vfs_inode_release(inode);

		/* Free filename */
		heapmm_free(name, strlen(name) + 1);
		
		return ENAMETOOLONG;
	}

	/* Call on FS driver to create directory entry */
	status = ifs_link(parent, name, inode->id);

	/* Free filename */
	heapmm_free(name, strlen(name) + 1);
	
	/* Check for errors */
	if (status) {
		/* An error occurred */

		/* Clean up */
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);	

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the target inode */
		vfs_inode_release(inode);

		/* Pass error to caller */
		return status;
	}

	/* Update mtime on parent */
	parent->mtime = system_time;

	/* Release lock on parent inode */
	semaphore_up(parent->lock);
	
	/* Update hardlink count */
	inode->hard_link_count++;

	/* Update ctime */
	inode->ctime = system_time;

	/* Release inode lock */
	semaphore_up(inode->lock);

	/* Release the parent inode */
	vfs_inode_release(parent);

	/* Release the target inode */
	vfs_inode_release(inode);

	/* Return success */
	return 0;	
}

///@}

/** 
 * @brief Initialize the VFS layer 
 * @param root_device The filesystem device to use as root filesystem
 * @param root_fs_type The type of filesystem used for the root device
 * @return If successful 1, otherwise 0
 */

int vfs_initialize(dev_t root_device, char *root_fs_type)
{
	fs_driver_t *driver;
	fs_device_t *fsdevice;
	inode_t *root_inode;
	int status;
	
	vfs_mount_initialize();

	vfs_icache_initialize();
	
	vfs_ifsmgr_initialize();

	/* Look up the rootfs driver */	
	status = vfs_get_driver(root_fs_type, &driver);

	/* Check if it exists */
	if (status) {

		//TODO: Panic here  : "Unknown rootfs type"

		return 0;
	}

	/* Mount the filesystem */
	status = driver->mount(root_device,  0, &fsdevice);

	if (status) {

		//TODO: Panic here  : "Could not mount rootfs"

		return 0;
	}

	/* Look up root inode */
	status = ifs_load_inode(fsdevice, fsdevice->root_inode_id, &root_inode);

	/* Check for errors */
	if (status)
		return 0;
	
	//TODO: Register rootfs mountpoint

	/* Add the root inode to it */
	vfs_inode_cache(root_inode);

	/* Fill VFS fields in the proces info */
	scheduler_current_task->root_directory = vfs_dir_cache_mkroot(root_inode);

	scheduler_current_task->current_directory = vfs_dir_cache_ref(scheduler_current_task->root_directory);

	/* Return success */
	return 1;
}

/**
 * @brief Changes the working directory for this process
 * @param dirc The new working directory
 * @return An error code or 0 for success
 */
int vfs_chdir(dir_cache_t *dirc)
{
	/* Check for null pointers */
	assert (dirc != NULL);

	/* Check if it really is a directory */
	if (!S_ISDIR(dirc->inode->mode)) 
		return ENOTDIR;

	/* Release the old directory */
	vfs_dir_cache_release(scheduler_current_task->current_directory);

	/* Update current directory */
	scheduler_current_task->current_directory = vfs_dir_cache_ref(dirc);

	return 0;
}

/**
 * @brief Changes the root directory for this process
 * @param dirc The new root directory
 * @return An error code or 0 for success
 */
int vfs_chroot(dir_cache_t *dirc)
{
	/* Check for null pointers */
	assert (dirc != NULL);

	/* Check if it really is a directory */
	if (!S_ISDIR(dirc->inode->mode)) 
		return ENOTDIR;

	/* Release the old directory */
	vfs_dir_cache_release(scheduler_current_task->root_directory);

	/* Update current directory */
	scheduler_current_task->root_directory = vfs_dir_cache_ref(dirc);

	return 0;
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
	status = vfs_get_driver ( fstype, &driver ) ;

	/* Check if it exists */
	if ( status ) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		/* Release the special file inode */
		vfs_inode_release(dev_inode);

		THROWV( status );
	}

	/* Mount the filesystem */
	status = vfs_do_mount( driver, dev_inode->if_dev, mp_inode, flags );
	
	/* Check for errors */	
	if (status) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		/* Release the special file inode */
		vfs_inode_release(dev_inode);

		THROWV( status ); //TODO: Unmount
	}

	/* Release the mountpoint inode */
	vfs_inode_release(mp_inode);

	/* Release the special file inode */
	vfs_inode_release(dev_inode);

	/* Return success */
	RETURNV;
}


///@}

