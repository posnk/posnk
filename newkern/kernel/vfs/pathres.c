/**
 * @file kernel/vfs/pathres.c
 *
 * Implements path resolution
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 12-04-2014 - Created
 * \li 11-07-2014 - Rewrite 1
 * \li 12-07-2014 - Commented
 * \li 04-02-2015 - Split off from vfs.c
 */

/* Includes */

#include <assert.h>

#include <string.h>

#include "util/llist.h"

#include "kernel/process.h"

#include "kernel/scheduler.h"

#include "kernel/vfs.h"

#include "kernel/heapmm.h"

/* Global Variables */

/* Internal type definitions */

/* Public Functions */




/** 
 * @brief Look up the parent directory of a file referred to by a path
 * 
 * This function will also succeed if path does not exist but its parent does
 * @param path The path to resolve
 * @return A dir_cache entry on the parent of the file referred to by path
 */

SFUNC(dir_cache_t *, vfs_find_dirc_parent, char * path)
{
	CHAINRET(vfs_find_dirc_parent_at, 
		scheduler_current_task->current_directory, path);
}

/** 
 * @brief Look up the parent directory of a file referred to by a path
 * 
 * This function will also succeed if path does not exist but its parent does
 * @param curdir The directory to start resolving in
 * @param path The path to resolve
 * @return A dir_cache entry on the parent of the file referred to by path
 */

SFUNC(dir_cache_t *, vfs_find_dirc_parent_at, dir_cache_t *curdir, char * path)
{
	errno_t status;
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
		status = vfs_dir_cache_release(dirc);
		if (status)
			THROW( status, NULL );

		/* Empty path does not exist! */
		THROW( ENOENT, NULL );
	}

	/* Allocate buffer for path element */
	path_element = (char *) heapmm_alloc(CONFIG_FILE_MAX_NAME_LENGTH);

	/* Check whether the allocation succeeded */
	if (!path_element) {
		/* Release current directory */
		status = vfs_dir_cache_release(dirc);
		if (status)
			THROW( status, NULL );
		
		THROW( ENOMEM, NULL);
	}

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
			status = vfs_dir_cache_release(dirc);
			if (status){
				
				/* Clean up */
				heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
			
				THROW( status, NULL );
			}

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
				RETURN(dirc);
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
				status = vfs_dir_cache_release(dirc);
				if (status){
					/* status = */ vfs_dir_cache_release(newc);
				
					/* Clean up */
					heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
			
					THROW( status, NULL );
				}

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
			status = vfs_find_dirent(parent, path_element, &dirent);

			/* Check if it exists */
			if (status) {

				/* If not, clean up and return */
				heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

				/* Check if this is the last path element */					
				if ((separator + 1) >= end_of_path) {
					/* We have already determined the 
					 * parent so even though the entry does
					 * not exist, return its parent */
				
					/* Return dirc */
					RETURN(dirc);

				} else {
					/* Release dirc */
					/* status = */vfs_dir_cache_release(dirc);

					/* Return NULL */
					THROW(ENOENT, NULL);
				}
			}
			
			/* Create new dirc entry for this element */
			status = vfs_dir_cache_new(dirc, dirent->inode_id, &newc);
			
			/* Check an error occurred */
			if (status) {

				/* If so, clean up and return */
				heapmm_free(path_element, 
						CONFIG_FILE_MAX_NAME_LENGTH);

				/* Release parent dirc */
				/* ?!?!? status = ?!?!?! */vfs_dir_cache_release(dirc);

				THROW(status, NULL);
			}

			/* Release old element */
			status= vfs_dir_cache_release(dirc);

			/* Check an error occurred */
			if (status) {

				/* If so, clean up and return */
				heapmm_free(path_element, 
						CONFIG_FILE_MAX_NAME_LENGTH);

				THROW(status, NULL);
			}

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
			status = vfs_dir_cache_release(dirc);

			if (status) {

				/* If so, clean up and return */
				heapmm_free(path_element, 
						CONFIG_FILE_MAX_NAME_LENGTH);

				THROW(status, NULL);
			}

			/* Return the previous element */
			RETURN(newc);
		}
		
		/* Check whether this element is a directory */
		if (!S_ISDIR(parent->mode)) {
			/* If not, return NULL because we can't search inside 
                         * other kinds of files */	

			/* Clean up */	
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

			/* Release dirc  */
			/* status = */vfs_dir_cache_release(dirc);

			THROW(ENOTDIR, NULL);
		}
	}
}


/** 
 * @brief Look up a file referred to by a path
 * @param path The path to resolve
 * @return A dir_cache entry on the file referred to by path
 */

SFUNC(dir_cache_t *, vfs_find_dirc, char * path) {
	CHAINRET(vfs_find_dirc_at, scheduler_current_task->current_directory, 
				path);
}

/** 
 * @brief Look up a file referred to by a path using a specified directory as
 * current.
 * @param curdir The directory to start resolving in
 * @param path The path to resolve
 * @return A dir_cache entry on the file referred to by path
 */

SFUNC(dir_cache_t *, vfs_find_dirc_at, dir_cache_t *curdir, char * path)
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
	errno_t status;

	/* Check for NULL  */
	assert (path != NULL);

	/* Check for empty path */
	if ((*path) == '\0'){
		/* Release current directory */
		status = vfs_dir_cache_release(dirc);
		if (status)
			THROW( status, NULL );

		/* Empty path does not exist! */
		THROW( ENOENT, NULL );
	}

	/* Allocate buffer for path element */
	path_element = (char *) heapmm_alloc(CONFIG_FILE_MAX_NAME_LENGTH);

	/* Check whether the allocation succeeded */
	if (!path_element) {
		/* Release current directory */
		status = vfs_dir_cache_release(dirc);
		if (status)
			THROW( status, NULL );
		
		THROW( ENOMEM, NULL);
	}

	/* Aqquire a pointer to the terminator */
	end_of_path = strchr(remaining_path, 0);

	/* Iterate over the path */
	for (;;) {
		/* Find the next / in the path */
		separator = strchrnul(remaining_path, (int) '/');

		/* Calculate the size of this path element */
		element_size = ((uintptr_t) separator) - 
				((uintptr_t) remaining_path);
	
		/* If the first element has size 0, this path has a leading */
		/* / so it is to be considered an absolute path */
		if ((element_count == 0) && (element_size == 0)){ 
			/* Path is absolute */

			/* Release current dirc */
			status = vfs_dir_cache_release(dirc);
			if (status) {
				
				/* Clean up */
				heapmm_free(path_element, 
						CONFIG_FILE_MAX_NAME_LENGTH);

				/* Pass on error */
				THROW( status, NULL );

			}

			/* Update current element */
			dirc = vfs_dir_cache_ref(
					scheduler_current_task->root_directory);
			parent = dirc->inode;

		} else if (element_size == 0) {
			/* Element size is 0 but not first element */

			/* Check whether this is the last element */
			if ((separator + 2) >= end_of_path) {
				/* If so, we have detected a trailing slash */
				/* and reached the end of the path */
				
				/* Clean up */
				heapmm_free(path_element,
					 CONFIG_FILE_MAX_NAME_LENGTH);

				/* Return dirc */
				RETURN(dirc);
			}

		} else if ((element_size == 2) &&
					 !strncmp(remaining_path, "..", 2)) {
			/* Element is ".." */

			/* Set next current path element to the parent of
			 * the current element */
			newc = vfs_dir_cache_ref(dirc->parent);

			/* If the new current element differs from the current 
			 * element, free the old current path element */
			if (newc != dirc) {	

				/* Release old dirc */
				status = vfs_dir_cache_release(dirc);
				if (status) {
					/* status = */ vfs_dir_cache_release(newc);
				
					/* Clean up */
					heapmm_free(path_element, 
						CONFIG_FILE_MAX_NAME_LENGTH);

					/* Pass on error */
					THROW( status, NULL );

				}
				/* Update current element */
				parent = newc->inode;
				dirc = newc;

			} else {
				status = vfs_dir_cache_release(newc);	
				if (status) {
				
					/* Clean up */
					heapmm_free(path_element, 
						CONFIG_FILE_MAX_NAME_LENGTH);

					/* Pass on error */
					THROW( status, NULL );

				}	
			}
		} else if ((element_size == 1) && !strncmp(remaining_path, ".", 1)) {				
			/* Element is . */
			/* Ignore this */
		} else {
			/* Element is a normal path element */
	
			/* Isolate it */
			strncpy(path_element, remaining_path, element_size);
			path_element[element_size] = '\0';

			/* Find the directory entry for it */
			status = vfs_find_dirent(parent, path_element, &dirent);

			/* Check an error occurred */
			if (status) {

				/* If so, clean up and return */
				heapmm_free(path_element, 
						CONFIG_FILE_MAX_NAME_LENGTH);

				/* Release parent dirc */
				/* ?!?!? status = ?!?!?! */vfs_dir_cache_release(dirc);

				THROW(status, NULL);
			}
			
			/* Create new dirc entry for this element */
			status = vfs_dir_cache_new(dirc, dirent->inode_id, &newc);

			/* Check an error occurred */
			if (status) {

				/* If so, clean up and return */
				heapmm_free(path_element, 
						CONFIG_FILE_MAX_NAME_LENGTH);

				/* Release parent dirc */
				/* ?!?!? status = ?!?!?! */vfs_dir_cache_release(dirc);

				THROW(status, NULL);
			}

			/* Release old element */
			status = vfs_dir_cache_release(dirc);

			/* Check an error occurred */
			if (status) {

				/* If so, clean up and return */
				heapmm_free(path_element, 
						CONFIG_FILE_MAX_NAME_LENGTH);

				THROW(status, NULL);
			}

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
			RETURN(dirc);
		}

		/* There are still elements after this one */

		/* Check if the current element is a directory */
		if (!S_ISDIR(parent->mode)) {
			/* If not, return NULL because we can't search inside 
                         * other kinds of files */	

			/* Clean up */	
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

		/* Release old element */
			/* ?!?!? status = ?!?!?! */vfs_dir_cache_release(dirc);

			THROW(ENOTDIR, NULL);
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

SFUNC(inode_t *, vfs_find_parent, char * path)
{ 
	inode_t *ino;
	dir_cache_t *dirc;
	errno_t status;

	/* Request the parent dir cache entry for this path */
	status = vfs_find_dirc_parent(path, &dirc);

	/* If it was not found, return NULL */
	if (status)
		THROW(status, NULL);

	/* Store it's inode */
	ino = vfs_inode_ref(dirc->inode);

	/* Release the dirc entry from the cache */
	vfs_dir_cache_release(dirc);

	/* Return the inode */
	RETURN(ino);
} 

/** 
 * @brief Look up the inode for the file referenced by path
 * @param path The path of the file to resolve
 * @return The file's inode
 */

SFUNC(inode_t *, vfs_find_inode, char * path)
{ 
	inode_t *ino;
	dir_cache_t *dirc;
	errno_t status;

	/* Request the dir cache entry for this path */
	status = vfs_find_dirc(path, &dirc);

	/* If it was not found, return NULL */
	if (status)
		THROW(status, NULL);

	/* Store it's inode */
	ino = vfs_inode_ref(dirc->inode);

	/* Release the dirc entry from the cache */
	vfs_dir_cache_release(dirc);

	/* Return the inode */
	RETURN(ino);
} 

/** 
 * @brief Look up the inode for the file referenced by path, not dereferencing
 * symlinks or mounts
 * @param path The path of the file to resolve
 * @return The file's inode
 */

SFUNC(inode_t *, vfs_find_symlink, char * path)
{ 
	dirent_t *dirent;
	char * name;
	inode_t *ino;
	dir_cache_t *dirc;
	errno_t status;

	/* Request the dir cache entry for this path */
	status = vfs_find_dirc(path, &dirc);

	/* If it was not found, return NULL */
	if (status)
		THROW(status, NULL);

	/* If the entry is the current root directory, return the */
	/* dereferenced inode, otherwise we would be able to escape a chroot */
	if ((dirc == scheduler_current_task->root_directory) || (dirc == scheduler_current_task->current_directory)) {

		/* Store the inode */
		ino = vfs_inode_ref(dirc->inode);

		/* Release the dirc entry from the cache */
		vfs_dir_cache_release(dirc);

		/* Return the inode */
		RETURN(ino);		
	}
	
	/* Resolve the filename */
	
	status = vfs_get_filename(path, &name);
	if (status)
		THROW(status, NULL);	

	/* Look up the directory entry for this file */
	status = vfs_find_dirent(dirc->parent->inode, name, &dirent);

	/* Free filename */
	heapmm_free(name, strlen(name) + 1);

	/* If it is not found, return NULL. Should never happen... */
	if (status)
		THROW(status, NULL);

	/* Get the inode pointed to by the dirent */
	status = vfs_get_inode(dirc->parent->inode->device, dirent->inode_id, &ino);

	/* Release the dirc entry from the cache */	
	vfs_dir_cache_release(dirc);
	if (status)
		THROW(status, NULL);

	/* Return the inode */
	RETURN(ino);
} 
