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
/** Filesystem driver list */
llist_t vfs_fs_driver_list;

/** The linked list serving as open inode list */
llist_t *open_inodes;

/** The linked list serving as inode cache */
//TODO: Implement a proper inode cache
llist_t *inode_cache;

typedef struct vfs_cache_params {
	uint32_t device_id;
	ino_t inode_id;
} vfs_cache_params_t;


/**
 * @brief Extract the filename part of a path
 * @warning This allocates heap for the return value, free with
 *	    heapmm_free(r, strlen(r) + 1);
 * @param path The path to analyze
 * @return The file name, see warning!
 */
char *vfs_get_filename(const char *path)
{	
	char *separator;
	char *name;
	char *pathcopy;
	size_t pathlen;
	char *newname;

	/* Check for null pointers */
	assert(path != NULL);
	
	/* Make a working copy of path */
	pathlen = strlen(path);
	
	/* Check path length */
	if (pathlen > 1023)
		return NULL;

	/* Allocate room for copy */
	pathcopy = heapmm_alloc(pathlen + 1);

	/* Copy path */
	strcpy(pathcopy, path);

	/* Resolve the filename */

	/* Find the last / in the path */
	separator = strrchr(pathcopy, '/');
	
	/* Check for trailing / */
	if (separator == (pathcopy + strlen(pathcopy) - 1)){

		/* Trailing / found, remove it */		
		pathcopy[strlen(path) - 1] = '\0';

		/* Search for the real last / */
		separator = strrchr(pathcopy, '/');
	}

	/* Check if a / was found */
	if (separator) /* If so, the string following it is the name */
		name = separator + 1;
	else /* If not, the path IS the name */
		name = pathcopy;

	newname = heapmm_alloc(strlen(name) + 1);
	assert(newname != NULL);
	
	strcpy(newname, name);
	
	heapmm_free(pathcopy, pathlen + 1);

	return newname;
}


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

/** @name VFS Internal
 *  Utility functions for use by VFS functions only
 */
///@{

/**
 * @brief Returns the minimum privilege level required for access with mode
 * @param inode The file to check
 * @param req_mode The requested access mode
 * @return The required privilege class : other, group or owner
 */

perm_class_t vfs_get_min_permissions(inode_t *inode, mode_t req_mode)
{
	assert (inode != NULL);
	if (req_mode & ((inode->mode) & 7))
		return PERM_CLASS_OTHER;
	if (req_mode & ((inode->mode >> 3) & 7))
		return PERM_CLASS_GROUP;
	if (req_mode & ((inode->mode >> 6) & 7))
		return PERM_CLASS_OWNER;
	return PERM_CLASS_NONE;
}

/**
 * @brief Check permissions for access with mode
 * @param inode The file to check
 * @param req_mode The requested access mode
 * @return Whether the requested access is allowed
 */

int vfs_have_permissions(inode_t *inode, mode_t req_mode) {
	assert (inode != NULL);
	return get_perm_class(inode->uid, inode->gid) <= vfs_get_min_permissions(inode, req_mode);
}

/**
 * @brief Get an inode by it's ID
 * 
 * @param device   The device to get the inode from
 * @param inode_id The ID of the inode to look up
 * @return The inode with id inode_id from device.
 */

inode_t *vfs_get_inode(fs_device_t *device, ino_t inode_id)
{
	inode_t *result;
	vfs_cache_params_t search_params;
	assert (device != NULL);

	/* Search open inode list */
	search_params.device_id = device->id;
	search_params.inode_id = inode_id;
	result = (inode_t *) llist_iterate_select(open_inodes, &vfs_cache_find_iterator, (void *) &search_params);

	if (result) {
		/* Cache hit, return inode */
		return vfs_inode_ref(result);
	}

	/* Search inode cache */
	search_params.device_id = device->id;
	search_params.inode_id = inode_id;
	result = (inode_t *) llist_iterate_select(inode_cache, &vfs_cache_find_iterator, (void *) &search_params);

	if (result) {
		/* Cache hit, return inode */
		return vfs_inode_ref(result);
	}

	/* Cache miss, fetch it from disk */
	//NOTE : Schedule may happen below!
	result = device->ops->load_inode(device, inode_id);
	//NOTE : Schedule may have happened

	return vfs_inode_ref(result);
}

/*
 * Iterator function that flushes the inode caches
 */

int vfs_cache_flush_iterator (llist_t *node, void *param)
{
	inode_t *inode = (inode_t *) node;
	inode->device->ops->store_inode(inode);
	return 0;		
}

/**
 * @brief Get the effective inode for an inode,
 * in other words : Dereference possible symlinks and mounts
 *
 * @param inode The inode that is to be checked
 * @return The inode that the parameter points to.
 */

inode_t *vfs_effective_inode(inode_t * inode)
{
	/* Check for null pointers */
	if (!inode)
		return NULL;

	/* Is a filesystem mounted on this inode? */
	if (inode->mount) {
		/* If so: return it's root inode */
		return vfs_inode_ref(inode->mount);
	}

	/*
//TODO: NEW SYMLINK IMPLEMENTATION
	/* Is this inode a symlink? * /
	if (inode->link_path[0]) {
		/* If so: look up the inode it points to * /
		return vfs_find_inode(inode->link_path);
	}

	*/

	/* This is a regular inode, return it. */
	return vfs_inode_ref(inode);
}

///@}


/** @name VFS API
 *  Public VFS functions
 */
///@{

/** 
 * @brief Find a directory entry
 * 
 * @param inode The directory to search
 * @param name  The filename to match
 * @return The directory entry matching name from the directory inode, 
 *          if none, NULL is returned
 */
 
dirent_t *vfs_find_dirent(inode_t * inode, char * name)
{
	/* This function is implemented by the FS driver */
	assert (inode != NULL);
	assert (name != NULL);

	/* Check permissions on parent dir */
	if (!vfs_have_permissions(inode, MODE_EXEC)) {
		return NULL; //TODO: Find a way to pass an error from here
	}

	/* Call the driver */
	return inode->device->ops->find_dirent(inode, name);
}
///@}

/** @name VFS Backend
 *  FS driver call wrappers
 */
///@{
/**
 * @brief Push a new inode to backing storage
 * @warning INTERNAL FUNCTION
 * 
 * @param inode The inode that was created
 * @return In case of error: a valid error code, Otherwise 0
 */

int vfs_int_mknod(inode_t * inode)
{
	/* This function is implemented by the FS driver */
	assert (inode != NULL);

	/* Check if the driver supports mknod */
	if (!inode->device->ops->mknod) {
		/* If not: return the error "Operation not supported" */
		return ENOTSUP;
	}

	/* Call the driver */
	return inode->device->ops->mknod(inode);
}

/**
 * @brief Remove an inode from backing storage
 * @warning INTERNAL FUNCTION
 * 
 * @param inode The inode that was deleted
 * @return In case of error: a valid error code, Otherwise 0
 */

int vfs_int_rmnod(inode_t * inode)
{
	/* This function is implemented by the FS driver */
	assert (inode != NULL);

	/* Check if the driver supports rmnod */
	if (!inode->device->ops->rmnod) {
		/* If not: return the error "Operation not supported" */
		return ENOTSUP;
	}

	/* Call the driver */
	return inode->device->ops->rmnod(inode);
}

/**
 * @brief Create directory structures on backing storage
 * @warning INTERNAL FUNCTION
 * 
 * @param inode The inode for the new directory
 * @return In case of error: a valid error code, Otherwise 0
 */

int vfs_int_mkdir(inode_t * inode)
{
	/* This function is implemented by the FS driver */
	assert (inode != NULL);

	/* Check if the driver supports mkdir */
	if (!inode->device->ops->mkdir) {
		/* If not: return the error "Operation not supported" */
		return ENOTSUP;
	}

	/* Call the driver */
	return inode->device->ops->mkdir(inode);
}

/**
 * @brief Create a directory entry on backing storage
 * @warning INTERNAL FUNCTION
 * 
 * @param inode  The inode for the directory
 * @param name   The file name for the directory entry
 * @param nod_id The inode id that the directory entry will point to
 * @return In case of error: a valid error code, Otherwise 0
 */

int vfs_int_link(inode_t * inode , char * name , ino_t nod_id)
{
	/* This function is implemented by the FS driver */
	assert (inode != NULL);
	assert (name != NULL);

	/* Check if the driver supports link */
	if (!inode->device->ops->link) {
		/* If not: return the error "Operation not supported" */
		return ENOTSUP;
	}

	/* Call the driver */
	return inode->device->ops->link(inode, name, nod_id);
}

/**
 * @brief Delete a directory entry from backing storage
 * @warning INTERNAL FUNCTION
 * 
 * @param inode - The inode for the directory
 * @param name  - The file name of the directory entry to delete
 * @return In case of error: a valid error code, Otherwise 0
 */

int vfs_int_unlink(inode_t * inode , char * name)
{	
	/* This function is implemented by the FS driver */
	assert (inode != NULL);
	assert (name != NULL);

	/* Check if the driver supports unlink */
	if (!inode->device->ops->unlink) {
		/* If not: return the error "Operation not supported" */
		return ENOTSUP;
	}

	/* Call the driver */
	return inode->device->ops->unlink(inode, name);
}

/**
 * @brief Read directory entries from backing storage
 * @warning INTERNAL FUNCTION
 * 
 * @param inode       The inode for the directory
 * @param buffer      The buffer to store the entries in
 * @param file_offset The offset in the directory to start reading at
 * @param count       The number of bytes to read
 * @param read_size   A pointer to a variable in which the number of bytes 
 *			 read will be stored
 * @return In case of error: a valid error code, Otherwise 0
 */

int vfs_int_read_dir(inode_t * inode, void * buffer, aoff_t file_offset, aoff_t count, aoff_t *read_size)
{
	/* This function is implemented by the FS driver */
	assert (inode != NULL);
	assert (buffer != NULL);
	assert (read_size != NULL);

	/* Call the driver */
	return inode->device->ops->read_dir(inode, buffer, file_offset, count, read_size);
}

/**
 * @brief Read data from a file from backing storage
 * @warning INTERNAL FUNCTION
 * 
 * @param inode       The inode for the file
 * @param buffer      The buffer to store the data in
 * @param file_offset The offset in the file to start reading at
 * @param count       The number of bytes to read
 * @param read_size   A pointer to a variable in which the number of bytes 
 *			 read will be stored
 * @return In case of error: a valid error code, Otherwise 0
 */

int vfs_int_read(inode_t * inode, void * buffer, aoff_t file_offset, aoff_t count, aoff_t *read_size)
{
	/* This function is implemented by the FS driver */
	assert (inode != NULL);
	assert (buffer != NULL);
	assert (read_size != NULL);

	/* Call the driver */
	return inode->device->ops->read_inode(inode, buffer, file_offset, count, read_size);
}

/**
 * @brief Write data to a file from backing storage
 * @warning INTERNAL FUNCTION
 * 
 * @param inode       The inode for the file
 * @param buffer      The buffer containing the data to write
 * @param file_offset The offset in the file to start writing at
 * @param count       The number of bytes to write
 * @param write_size  A pointer to a variable in which the number of bytes 
 *			 written will be stored
 * @return In case of error: a valid error code, Otherwise 0
 */

int vfs_int_write(inode_t * inode, void * buffer, aoff_t file_offset, aoff_t count, aoff_t *write_size)
{
	/* This function is implemented by the FS driver */
	assert (inode != NULL);
	assert (buffer != NULL);
	assert (write_size != NULL);

	/* Check if the driver supports write */
	if (!inode->device->ops->write_inode) {
		/* If not: return the error "Operation not supported" */
		return ENOTSUP;
	}

	/* Call the driver */
	return inode->device->ops->write_inode(inode, buffer, file_offset, count, write_size);
}

/**
 * @brief Resize a file on backing storage
 * @warning INTERNAL FUNCTION
 * 
 * @param inode       The inode for the file
 * @param size	      The new size of the file
 * @return In case of error: a valid error code, Otherwise 0
 */

int vfs_int_truncate(inode_t * inode, aoff_t size)
{
	/* This function is implemented by the FS driver */
	assert (inode != NULL);

	/* Check if the driver supports truncate */
	if (!inode->device->ops->trunc_inode) {
		/* If not: return the error "Operation not supported" */
		return ENOTSUP;
	}

	/* Call the driver */
	return inode->device->ops->trunc_inode(inode, size);
}
///@}

/**
 * @brief Flush the inode cache
 * 
 */

void vfs_cache_flush()
{

	/* Flush open inode list */
	llist_iterate_select(open_inodes, &vfs_cache_flush_iterator, NULL);

	/* Flush inode cache */
	llist_iterate_select(inode_cache, &vfs_cache_flush_iterator, NULL);

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
	int status;
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
					status = vfs_int_write(inode, zbuffer, inode->size, file_offset - inode->size, &pad_size);

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
			status = vfs_int_write(inode, buffer, file_offset, count, write_size);

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
			status = vfs_int_read(inode, buffer, file_offset, count, read_size);

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
			status = vfs_int_read_dir(inode, buffer, file_offset, count, read_size);

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
			status = vfs_int_truncate(inode, length);

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

	/* Check for null pointers */
	assert (path != NULL);

	/* Lookup the inode for the directory*/
	inode_t *inode = vfs_find_inode(path);

	/*If the inode does not exist, return error "No such file or directory"*/
	if (!inode)
		return ENOENT;	

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
	parent = vfs_find_parent(path);

	/* Check if it really exists */
	assert(parent != NULL);

	/* Look up the inode for the new directory */
	dir = vfs_find_inode(path);

	/* Check if it really exists */
	assert(dir != NULL);

	/* Accuire a lock on the inode */
	semaphore_down(dir->lock);

	/* Create directory internal structure */
	err = vfs_int_mkdir(dir);

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
	parent = vfs_find_parent(path);

	/* Check if it exists, if not, return error */
	if (!parent) 
		return ENOENT;

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
	if ((_inode = vfs_find_inode(path))) {
		/* If so, clean up and return an error */
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the inode */
		vfs_inode_release(_inode);

		return EEXIST;
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
	name = vfs_get_filename(path);

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
	status = vfs_int_mknod(inode);

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
	status = vfs_int_link(parent, name, inode->id);

	/* Free filename */
	heapmm_free(name, strlen(name) + 1);

	/* Check for errors */
	if (status) {
		/* If an error occurred, clean up */	
		llist_unlink((llist_t *) inode);
		vfs_int_rmnod(inode);
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
 * @brief Register a file system driver
 * 
 * A file system driver calls this function to register its mount callback 
 *
 * @param name The name to register with (this is the identifier passed to mount)
 * @param mnt_cb A pointer to the filesystem's mount function
 * @return If successful, 0 is returned, otherwise a valid error code will be returned
 */
int vfs_register_fs(const char *name, fs_device_t *(*mnt_cb)(dev_t, uint32_t))
{
	fs_driver_t *driver;

	/* Check for NULL pointers */
	assert(name != NULL);
	assert(mnt_cb != NULL);

	/* Allocate memory for driver descriptor */
	driver = heapmm_alloc(sizeof(fs_driver_t));

	/* Check for errors */
	if (!driver)
		return ENOMEM;

	/* Allocate memory for driver name */
	driver->name = heapmm_alloc(strlen(name) + 1);

	/* Check for errors */
	if (!driver->name) {
		/* Clean up */
		heapmm_free(driver, sizeof(fs_driver_t));
		return ENOMEM;
	}

	/* Copy driver name */
	strcpy(driver->name, name);
	
	/* Set driver mount callback */
	driver->mount = mnt_cb;

	/* Add driver to list */
	llist_add_end(&vfs_fs_driver_list, (llist_t *) driver);

	return 0; 
}

int vfs_mount_iterator (llist_t *node, void *param) {
	fs_driver_t *driver = (fs_driver_t *) node;
	assert(driver->name != NULL);
	return 0 == strcmp(driver->name, (char *) param);
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

int vfs_mount(char *device, char *mountpoint, char *fstype, uint32_t flags)
{
	fs_driver_t *driver;
	fs_device_t *fsdevice;
	inode_t	    *mp_inode;
	inode_t	    *dev_inode;

	/* Check for null pointers */
	assert (device != NULL);
	assert (mountpoint != NULL);

	/* Look up the inode for the mountpoint */
	mp_inode = vfs_find_inode(mountpoint);

	/* Check if it exists */
	if (!mp_inode) 
		return ENOENT;

	/* Check if it is a directory */
	if (!S_ISDIR(mp_inode->mode)) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		return ENOTDIR;
	}

	/* Look up the inode for the special file */
	dev_inode = vfs_find_inode(device);
	
	/* Check if it exists */
	if (!dev_inode) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		return ENOENT;
	}

	/* Check if it is a block special file */
	if (!S_ISBLK(dev_inode->mode)) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		/* Release the special file inode */
		vfs_inode_release(dev_inode);

		return ENOTBLK;
	}
	
	/* Look up the fs driver */	
	driver = (fs_driver_t *) llist_iterate_select(&vfs_fs_driver_list, &vfs_mount_iterator, fstype);

	/* Check if it exists */
	if (!driver) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		/* Release the special file inode */
		vfs_inode_release(dev_inode);

		return ENOENT;
	}

	/* Mount the filesystem */
	fsdevice = driver->mount(dev_inode->if_dev, flags);
	
	/* Check for errors */
	if (!fsdevice) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		/* Release the special file inode */
		vfs_inode_release(dev_inode);

		return EINVAL;
	}
		
	/* Attach the filesystems root inode to the mountpoint */
	mp_inode->mount = fsdevice->ops->load_inode(fsdevice, fsdevice->root_inode_id);	
	
	/* Check for errors */	
	if (!mp_inode->mount) {

		/* Release the mountpoint inode */
		vfs_inode_release(mp_inode);

		/* Release the special file inode */
		vfs_inode_release(dev_inode);

		return EINVAL; //TODO: Unmount
	}

	/* Release the mountpoint inode */
	vfs_inode_release(mp_inode);

	/* Release the special file inode */
	vfs_inode_release(dev_inode);

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
	status = vfs_int_read(inode, (void *) buffer, 0, (aoff_t) size, &_read_size);

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

	/* Check for null pointers */
	assert (path != NULL);
	assert (oldpath != NULL);
	
	/* Look up the inode for the parent directory */
	parent = vfs_find_parent(path);

	/* Check if it exists, if not, return error */
	if (!parent) 
		return ENOENT;

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

	/* Check whether the file already exists */
	if (vfs_find_inode(path)) {
		/* If so, clean up and return an error */
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		return EEXIST;
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
	
	name = vfs_get_filename(path);

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
	status = vfs_int_mknod(inode);

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

	status = vfs_int_write(inode, oldpath, 0, write_size, &write_size_rv);

	inode->size = write_size;

	/* Check for errors */
	if (status || (write_size_rv != write_size)) {
		/* If an error occurred, clean up */
		vfs_int_rmnod(inode);
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Pass the error to the caller */
		return status;
	}

	/* Add inode to cache */
	llist_add_end(inode_cache, (llist_t *) inode);

	/* Inode is now on storage and in the cache */
	/* Proceed to make initial link */
	status = vfs_int_link(parent, name, inode->id);

	/* Free filename */
	heapmm_free(name, strlen(name) + 1);

	/* Check for errors */
	if (status) {
		/* If an error occurred, clean up */	
		llist_unlink((llist_t *) inode);
		vfs_int_rmnod(inode);
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

	inode = vfs_find_symlink(path);

	/* Check if it exists, if not, return error */
	if (!inode)
		return ENOENT;
	
	/* Look up the inode for the parent directory */
	parent = vfs_find_parent(path);

	/* Check if it exists, if not, return error */
	if (!parent) {

		/* Release the inode */
		vfs_inode_release(inode);

		return ENOENT;
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
	
	name = vfs_get_filename(path);

	/* Check the filename length */
	if (strlen(name) >= CONFIG_FILE_MAX_NAME_LENGTH) {
		/* Length is too long, clean up and return error */
		heapmm_free(inode, parent->device->inode_size);		

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
	status = vfs_int_unlink(parent, name);

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
		vfs_int_rmnod(inode); //This might error but there is little we can do about that anyway
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
	inode_t *parent;

	/* Check for null pointers */
	assert (newpath != NULL);
	assert (oldpath != NULL);
	
	/* Look up the inode for the parent directory */
	parent = vfs_find_parent(newpath);

	/* Check if it exists, if not, return error */
	if (!parent) 
		return ENOENT;
	
	/* Look up the inode for the target */
	inode = vfs_find_inode(oldpath);

	/* Check if it exists, if not, return error */
	if (!inode) {
		return ENOENT;

		/* Release the parent inode */
		vfs_inode_release(parent);

	}

	/* Aqquire a lock on the target */
	semaphore_down(inode->lock);
	
	/* Aqquire a lock on the parent directory */
	semaphore_down(parent->lock);

	/* Check whether the file already exists */
	if (vfs_find_inode(newpath)) {
		/* If so, clean up and return an error */
		semaphore_up(inode->lock);
		semaphore_up(parent->lock);

		/* Release the parent inode */
		vfs_inode_release(parent);

		/* Release the target inode */
		vfs_inode_release(inode);

		return EEXIST;
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
	
	name = vfs_get_filename(newpath);

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
	status = vfs_int_link(parent, name, inode->id);

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

void register_fs_drivers();

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

	//TODO: Track mounted devices
	/* Allocate inode cache */
	inode_cache = heapmm_alloc(sizeof(llist_t));

	/* Allocate open inode list */
	open_inodes = heapmm_alloc(sizeof(llist_t));

	/* Create the inode cache */
	llist_create(inode_cache);

	/* Create the open inode list */
	llist_create(open_inodes);

	/* Create the file system device list */
	llist_create(&vfs_fs_driver_list);
	
	/* Register fs drivers */
	register_fs_drivers();
	
	/* Look up the rootfs driver */	
	driver = (fs_driver_t *) llist_iterate_select(&vfs_fs_driver_list, &vfs_mount_iterator, root_fs_type);

	/* Check if it exists */
	if (!driver) {

		//TODO: Panic here  : "Unknown rootfs type"

		return 0;
	}

	/* Mount the filesystem */
	fsdevice = driver->mount(root_device,  0);

	if (!fsdevice) {
		//TODO: Panic here  : "Could not mount rootfs"

		return 0;
	}

	/* Look up root inode */
	root_inode = fsdevice->ops->load_inode(fsdevice, fsdevice->root_inode_id);

	/* Check for errors */
	if (!root_inode)
		return 0;
	
	/* Add the root inode to it */
	llist_add_end(inode_cache, (llist_t *) root_inode);

	/* Fill VFS fields in the proces info */
	scheduler_current_task->root_directory = vfs_dir_cache_mkroot(root_inode);

	scheduler_current_task->current_directory = vfs_dir_cache_ref(scheduler_current_task->root_directory);

	/* Return success */
	return 1;
}

/**
 * @brief Create the initial dir cache entry
 * @param root_inode The root inode of the initial root filesystem
 * @return The dir cache entry for the root inode
 */

dir_cache_t *vfs_dir_cache_mkroot(inode_t *root_inode)
{
	/* Allocate memory for the cache entry */
	dir_cache_t *dirc = heapmm_alloc(sizeof(dir_cache_t));
	assert (dirc != NULL);
	
	/* Set its parent to itself because it is the root of the graph */
	dirc->parent = dirc;

	/* Set the inode */
	dirc->inode = vfs_inode_ref(root_inode);

	/* Initialize the reference count to 1 */
	dirc->usage_count = 1;
	return dirc;	
}

/**
 * @brief Release a reference to a directory cache entry
 * @param dirc The reference to release
 */

void vfs_dir_cache_release(dir_cache_t *dirc)
{
	assert (dirc != NULL);

	/* Decrease the reference count for this dir cache entry */
	if (dirc->usage_count)
		dirc->usage_count--;

	/* If the entry has no more references, destroy it */
	if (dirc->usage_count)
		return;

	/* If this is not the graph root, release the parent ref */
	if (dirc->parent != dirc)
		vfs_dir_cache_release(dirc->parent);

	/* Decrease the inode reference count */
	vfs_inode_release(dirc->inode);

	/* Release it's memory */
	heapmm_free(dirc, sizeof(dir_cache_t));	
}

/**
 * @brief Create a new reference to a directory cache entry
 * @param dirc The entry to refer to
 * @return The new reference
 */

dir_cache_t *vfs_dir_cache_ref(dir_cache_t *dirc)
{
	assert (dirc != NULL);
	dirc->usage_count++;
	return dirc;
}

/**
 * @brief Create a new directory cache entry
 * @param parent Directory cache entry to use as parent
 * @return The new entry
 */

dir_cache_t *vfs_dir_cache_new(dir_cache_t *par, ino_t inode_id)
{
	inode_t *oi;

	/* Check for null pointers */
	assert (par != NULL);

	/* Allocate memory for the cache entry */
	dir_cache_t *dirc = heapmm_alloc(sizeof(dir_cache_t));
	
	/* Check if the allocation succeeded */
	if (!dirc)
		return NULL;

	/* Set its parent to itself because it is the root of the graph */
	dirc->parent = vfs_dir_cache_ref(par);

	oi = vfs_get_inode(par->inode->device, inode_id);
	
	/* Set the inode */
	dirc->inode = vfs_effective_inode( oi );

	assert(dirc->inode != NULL);

	/* Release the outer inode */
	if (oi)
		vfs_inode_release( oi );

	if (!dirc->inode) {
		heapmm_free(dirc, sizeof(dir_cache_t));
		vfs_dir_cache_release(par);
		return NULL;
	}

	/* Initialize the reference count to 1 */
	dirc->usage_count = 1;

	return dirc;	
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
 * @brief Look up a file referred to by a path
 * @param path The path to resolve
 * @return A dir_cache entry on the file referred to by path
 */

dir_cache_t *vfs_find_dirc(char * path) {
	return vfs_find_dirc_at(scheduler_current_task->current_directory, path);
}


/** 
 * @brief Look up the parent directory of a file referred to by a path
 * 
 * This function will also succeed if path does not exist but its parent does
 * @param path The path to resolve
 * @return A dir_cache entry on the parent of the file referred to by path
 */

dir_cache_t *vfs_find_dirc_parent(char * path)
{
	return vfs_find_dirc_parent_at(scheduler_current_task->current_directory, path);
}

/** 
 * @brief Look up the parent directory of a file referred to by a path
 * 
 * This function will also succeed if path does not exist but its parent does
 * @param curdir The directory to start resolving in
 * @param path The path to resolve
 * @return A dir_cache entry on the parent of the file referred to by path
 */

dir_cache_t *vfs_find_dirc_parent_at(dir_cache_t *curdir, char * path)
{
	dir_cache_t *dirc = vfs_dir_cache_ref(curdir);
	dir_cache_t *newc;
	inode_t * parent = dirc->inode;
	char * separator;
	char * path_element;
	char * remaining_path = path;
	char * end_of_path;
	size_t element_size;
	dirent_t *dirent;
	int element_count = 0;	

	/* Check for NULL  */
	assert (path != NULL);

	/* Check for empty path */
	if ((*path) == '\0'){
		/* Release current directory */
		vfs_dir_cache_release(dirc);

		/* Empty path does not exist! */
		return NULL;
	}

	/* Allocate buffer for path element */
	path_element = (char *) heapmm_alloc(CONFIG_FILE_MAX_NAME_LENGTH);

	/* Aqquire a pointer to the terminator */
	end_of_path = strchr(remaining_path, 0);

	/* Iterate over the path */
	for (;;) {
		/* Find the next / in the path */
		separator = strchrnul(remaining_path, (int) '/');

		/* Calculate the size of this path element */
		element_size = ((uintptr_t) separator) - ((uintptr_t) remaining_path);
	
		/* If the first element has size 0, this path has a leading */
		/* / so it is to be considered an absolute path */
		if ((element_count == 0) && (element_size == 0)){ // First character is / 
			/* Path is absolute */

			/* Release current dirc */
			vfs_dir_cache_release(dirc);

			/* Update current element */
			dirc = vfs_dir_cache_ref(scheduler_current_task->root_directory);
			parent = dirc->inode;

		} else if (element_size == 0) {
			/* Element size is 0 but not first element */

			/* Check whether this is the last element */
			if ((separator + 2) >= end_of_path) {
				/* If so, we have detected a trailing slash */
				/* and reached the end of the path */
				
				/* Clean up */
				heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

				/* Return dirc */
				return dirc;
			}

		} else if ((element_size == 2) && !strncmp(remaining_path, "..", 2)) {
			/* Element is ".." */

			/* Set next current path element to the parent of
			 * the current element */
			newc = vfs_dir_cache_ref(dirc->parent);

			/* If the new current element differs from the current 
			 * element, free the old current path element */
			if (newc != dirc) {		

				/* Release old element */	
				vfs_dir_cache_release(dirc);

				/* Update current element */
				parent = newc->inode;
				dirc = newc;
			} else 
				vfs_dir_cache_release(newc);	
					
		} else if ((element_size == 1) && !strncmp(remaining_path, ".", 1)) {			
			/* Element is . */
			/* Ignore this */			
		} else {
			/* Element is a normal path element */
	
			/* Isolate it */
			strncpy(path_element, remaining_path, element_size);
			path_element[element_size] = '\0';

			/* Find the directory entry for it */
			dirent = vfs_find_dirent(parent, path_element);

			/* Check if it exists */
			if (dirent == NULL) {

				/* If not, clean up and return */
				heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

				/* Check if this is the last path element */					
				if ((separator + 1) >= end_of_path) {
					/* We have already determined the 
					 * parent so even though the entry does
					 * not exist, return its parent */
				
					/* Return dirc */
					return dirc;

				} else {
					/* Release dirc */
					vfs_dir_cache_release(dirc);

					/* Return NULL */
					return NULL;
				}
			}
			
			/* Create new dirc entry for this element */
			newc = vfs_dir_cache_new(dirc, dirent->inode_id);

			assert(newc != NULL);

			/* Release old element */
			vfs_dir_cache_release(dirc);

			/* Update current element */
			dirc = newc;		
			parent = dirc->inode;

		}

		/* Update remaining path to point to the start of the next
		 * element */
		remaining_path = separator + 1;

		/* Check whether we have reached the end of the path */
		if (remaining_path >= end_of_path) {
			/* If so, clean up and return the previous element */

			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

			/* Reuse the newc var to temporarily store parent */
			newc = vfs_dir_cache_ref(dirc->parent);
			
			/* Release the current dir cache entry */
			vfs_dir_cache_release(dirc);

			/* Return the previous element */
			return newc;
		}
		
		/* Check whether this element is a directory */
		if (!S_ISDIR(parent->mode)) {
			/* If not, return NULL because we can't search inside 
                         * other kinds of files */	

			/* Clean up */	
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

			/* Release dirc  */
			vfs_dir_cache_release(dirc);

			return NULL;
		}
	}
}

/** 
 * @brief Look up a file referred to by a path using a specified directory as
 * current.
 * @param curdir The directory to start resolving in
 * @param path The path to resolve
 * @return A dir_cache entry on the file referred to by path
 */

dir_cache_t *vfs_find_dirc_at(dir_cache_t *curdir, char * path)
{ 
	dir_cache_t *dirc = vfs_dir_cache_ref(curdir);
	dir_cache_t *newc;
	inode_t * parent = dirc->inode;
	char * separator;
	char * path_element;
	char * remaining_path = path;
	char * end_of_path;
	size_t element_size;
	dirent_t *dirent;
	int element_count = 0;	

	/* Check for NULL  */
	assert (path != NULL);

	/* Check for empty path */
	if ((*path) == '\0'){
		/* Release current directory */
		vfs_dir_cache_release(dirc);

		/* Empty path does not exist! */
		return NULL;
	}

	/* Allocate buffer for path element */
	path_element = (char *) heapmm_alloc(CONFIG_FILE_MAX_NAME_LENGTH);

	/* Aqquire a pointer to the terminator */
	end_of_path = strchr(remaining_path, 0);

	/* Iterate over the path */
	for (;;) {
		/* Find the next / in the path */
		separator = strchrnul(remaining_path, (int) '/');

		/* Calculate the size of this path element */
		element_size = ((uintptr_t) separator) - ((uintptr_t) remaining_path);
	
		/* If the first element has size 0, this path has a leading */
		/* / so it is to be considered an absolute path */
		if ((element_count == 0) && (element_size == 0)){ 
			/* Path is absolute */

			/* Release current dirc */
			vfs_dir_cache_release(dirc);

			/* Update current element */
			dirc = vfs_dir_cache_ref(scheduler_current_task->root_directory);
			parent = dirc->inode;

		} else if (element_size == 0) {
			/* Element size is 0 but not first element */

			/* Check whether this is the last element */
			if ((separator + 2) >= end_of_path) {
				/* If so, we have detected a trailing slash */
				/* and reached the end of the path */
				
				/* Clean up */
				heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

				/* Return dirc */
				return dirc;
			}

		} else if ((element_size == 2) && !strncmp(remaining_path, "..", 2)) {
			/* Element is ".." */

			/* Set next current path element to the parent of
			 * the current element */
			newc = vfs_dir_cache_ref(dirc->parent);

			/* If the new current element differs from the current 
			 * element, free the old current path element */
			if (newc != dirc) {	

				/* Release old dirc */
				vfs_dir_cache_release(dirc);

				/* Update current element */
				parent = newc->inode;
				dirc = newc;

			} else 
				vfs_dir_cache_release(newc);		
		} else if ((element_size == 1) && !strncmp(remaining_path, ".", 1)) {				
			/* Element is . */
			/* Ignore this */
		} else {
			/* Element is a normal path element */
	
			/* Isolate it */
			strncpy(path_element, remaining_path, element_size);
			path_element[element_size] = '\0';

			/* Find the directory entry for it */
			dirent = vfs_find_dirent(parent, path_element);

			/* Check if it exists */
			if (dirent == NULL) {

				/* If not, clean up and return */
				heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

				/* Release parent dirc */
				vfs_dir_cache_release(dirc);

				return NULL;
			}
			
			/* Create new dirc entry for this element */
			newc = vfs_dir_cache_new(dirc, dirent->inode_id);

			assert(newc != NULL);

			/* Release old element */
			vfs_dir_cache_release(dirc);

			/* Update current element */
			dirc = newc;		
			parent = dirc->inode;
		}

		/* Update remaining path to point to the start of the next
		 * element */
		remaining_path = separator + 1;

		/* Check whether we have reached the end of the path */
		if (remaining_path >= end_of_path) {
			/* If so, return the current element */
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
				
			/* Return dirc */
			return dirc;
		}

		/* There are still elements after this one */

		/* Check if the current element is a directory */
		if (!S_ISDIR(parent->mode)) {
			/* If not, return NULL because we can't search inside 
                         * other kinds of files */	

			/* Clean up */	
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

			/* Release old element */
			vfs_dir_cache_release(dirc);

			return NULL;
		}

	}
} 

/** @name VFS API
 *  Public VFS functions
 */
///@{

/** 
 * @brief Look up the inode for the directory containing the file
 * @param path The path of the file to resolve
 * @return The file's parent directory 
 */

inode_t *vfs_find_parent(char * path)
{ 
	inode_t *ino;

	/* Request the parent dir cache entry for this path */
	dir_cache_t *dirc = vfs_find_dirc_parent(path);

	/* If it was not found, return NULL */
	if (!dirc)
		return NULL;

	/* Store it's inode */
	ino = vfs_inode_ref(dirc->inode);

	/* Release the dirc entry from the cache */
	vfs_dir_cache_release(dirc);

	/* Return the inode */
	return ino;
} 

/** 
 * @brief Look up the inode for the file referenced by path
 * @param path The path of the file to resolve
 * @return The file's inode
 */

inode_t *vfs_find_inode(char * path)
{ 
	inode_t *ino;

	/* Request the dir cache entry for this path */
	dir_cache_t *dirc = vfs_find_dirc(path);

	/* If it was not found, return NULL */
	if (!dirc)
		return NULL;

	/* Store it's inode */
	ino = vfs_inode_ref(dirc->inode);

	/* Release the dirc entry from the cache */
	vfs_dir_cache_release(dirc);

	/* Return the inode */
	return ino;
} 

/** 
 * @brief Look up the inode for the file referenced by path, not dereferencing
 * symlinks or mounts
 * @param path The path of the file to resolve
 * @return The file's inode
 */

inode_t *vfs_find_symlink(char * path)
{ 
	dirent_t *dirent;
	char * name;
	inode_t *ino;

	/* Request the dir cache entry for this path */
	dir_cache_t *dirc = vfs_find_dirc(path);

	/* If it was not found, return NULL */
	if (!dirc)
		return NULL;

	/* If the entry is the current root directory, return the */
	/* dereferenced inode, otherwise we would be able to escape a chroot */
	if ((dirc == scheduler_current_task->root_directory) || (dirc == scheduler_current_task->current_directory)) {

		/* Store the inode */
		ino = vfs_inode_ref(dirc->inode);

		/* Release the dirc entry from the cache */
		vfs_dir_cache_release(dirc);

		/* Return the inode */
		return ino;		
	}
	
	/* Resolve the filename */
	
	name = vfs_get_filename(path);

	/* Look up the directory entry for this file */
	dirent = vfs_find_dirent(dirc->parent->inode, name);

	/* Free filename */
	heapmm_free(name, strlen(name) + 1);

	/* If it is not found, return NULL. Should never happen... */
	assert(dirent != NULL);

	/* Get the inode pointed to by the dirent */
	ino = vfs_get_inode(dirc->parent->inode->device, dirent->inode_id);

	/* Release the dirc entry from the cache */	
	vfs_dir_cache_release(dirc);

	/* Return the inode */
	return ino;
} 

///@}

