#include "kernel/vfs.h"
#include "kernel/heapmm.h"
#include "util/llist.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


/** Filesystem driver list */
llist_t vfs_fs_driver_list;

void vfs_test_init()
{
	llist_create(&vfs_fs_driver_list);
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
	if (!driver) {

		fprintf(stderr, "error: could not allocate memory for driver name\n");
		exit(EXIT_FAILURE);

	}

	/* Allocate memory for driver name */
	driver->name = heapmm_alloc(strlen(name) + 1);

	/* Check for errors */
	if (!driver->name) {
		/* Clean up */
		heapmm_free(driver, sizeof(fs_driver_t));

		fprintf(stderr, "error: could not allocate memory for driver name\n");
		exit(EXIT_FAILURE);

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

fs_device_t *vfs_test_mount(char *fstype, dev_t device, uint32_t flags)
{
	fs_driver_t *driver;
	fs_device_t *fsdevice;
	
	/* Look up the fs driver */	
	driver = (fs_driver_t *) llist_iterate_select(&vfs_fs_driver_list, &vfs_mount_iterator, fstype);

	/* Check if it exists */
	if (!driver) {
		fprintf(stderr, "error: no such filesystem loaded\n");
		exit(EXIT_FAILURE);
	}

	/* Mount the filesystem */
	fsdevice = driver->mount(device, flags);
	
	/* Check for errors */
	if (!fsdevice) {

		fprintf(stderr, "error: could not mount filesystem\n");
		exit(EXIT_FAILURE);

	}

	/* Return success */
	return fsdevice;
}

inode_t *vfs_test_get_root(fs_device_t *fsdevice)
{
	inode_t *root;
	/* Attach the filesystems root inode to the mountpoint */
	root = fsdevice->ops->load_inode(fsdevice, fsdevice->root_inode_id);	
	
	/* Check for errors */	
	if (!root) {

		fprintf(stderr, "error: could not load root inode\n");
		exit(EXIT_FAILURE);

	}

	return root;
}

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

int vfs_test_mknod(inode_t *parent, char *name, mode_t mode, dev_t dev)
{
	int status;
	
	inode_t *inode;
	inode_t *_inode;

	/* Check for null pointers */
	assert (name != NULL);

	/* Check if it exists, if not, return error */
	if (!parent) 
		return ENOENT;

	/* Allocate memory for new inode */
	inode = heapmm_alloc(parent->device->inode_size);

	/* Check if the allocation succeded, if not, return error */
	if (!inode) {

		fprintf(stderr, "error: ran out of memory in mknod\n");
		exit(EXIT_FAILURE);

	}

	/* Acquire a lock on the parent inode */
	semaphore_down(parent->lock);

	/* Clear the newly allocated inode */
	memset(inode, 0, parent->device->inode_size);

	/* Fill it's device fields */
	inode->device_id = parent->device_id;
	inode->device = parent->device;

	/* INODE ID is filled by fs driver */

	/* Check the filename length */
	if (strlen(name) >= CONFIG_FILE_MAX_NAME_LENGTH) {
		/* Length is too long, clean up and return error */
		heapmm_free(inode, parent->device->inode_size);	

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Free filename */
		heapmm_free(name, strlen(name) + 1);	

		fprintf(stderr, "error: filename too long in mknod\n");
		exit(EXIT_FAILURE);
	}

	/* Fill other inode fields */
	inode->mode = mode;
	inode->if_dev = dev;
	inode->uid = 0;
	inode->gid = 0;

	/* Allocate inode lock */
	inode->lock = semaphore_alloc();

	/* Set atime, mtime, ctime */
	inode->atime = inode->mtime = inode->ctime = 123456;
	
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

		/* Pass the error to the caller */
		return status;
	}	

	/* Inode is now on storage and in the cache */
	/* Proceed to make initial link */
	status = vfs_int_link(parent, name, inode->id);

	/* Check for errors */
	if (status) {
		/* If an error occurred, clean up */	
		llist_unlink((llist_t *) inode);
		vfs_int_rmnod(inode);
		heapmm_free(inode, parent->device->inode_size);

		/* Release the lock on the parent inode*/
		semaphore_up(parent->lock);

		/* Pass the error to the caller */
		return status;
	}

	/* Set link count to 1 */
	inode->hard_link_count++;

	/* Release the lock on the inode */
	semaphore_up(inode->lock);

	/* Release the lock on the parent inode*/
	semaphore_up(parent->lock);

	/* Push inode to storage */
	return inode->device->ops->store_inode(inode);	
}
