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
 * symlinks
 * @param path The path of the file to resolve
 * @return The file's inode
 */

SFUNC(inode_t *, vfs_find_symlink, char * path)
{ 
	inode_t *ino;
	dir_cache_t *dirc;
	errno_t status;

	/* Request the dir cache entry for this path */
	status = vfs_find_dirc_symlink(path, &dirc);

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
 * @brief Look up the parent directory of a file referred to by a path
 * 
 * This function will also succeed if path does not exist but its parent does
 * @param path The path to resolve
 * @return The parent directory of the file or directory that was found
 */

SFUNC(fslookup_t, vfs_find_parent, char * path)
{
	CHAINRET(vfs_find_parent_at, 
		scheduler_current_task->current_directory, path);
}


/** 
 * @brief Look up the parent directory of a file referred to by a path
 * 
 * This function will also succeed if path does not exist but its parent does
 * @param curdir The directory to start resolving in
 * @param path The path to resolve
 * @return The parent directory of the file or directory that was found
 */

SFUNC(fslookup_t, vfs_find_parent_at, Directory *curdir, char * path){
	CHAINRET(_vfs_find_at, curdir, path, PATHRES_FLAG_PARENT, 0);
}

/** 
 * @brief Look up a file referred to by a path without resolving it as a symlink
 * @param path The path to resolve
 * @return The file or directory that was found
 */

SFUNC(fslookup_t, vfs_find_symlink, char * path) {
	CHAINRET(vfs_find_symlink_at, 
				scheduler_current_task->current_directory, 
				path);
}

/** 
 * @brief Look up a file referred to by a path without resolving it as a symlink
 * @param path The path to resolve
 * @param curdir The directory to start resolving in
 * @return The file or directory that was found
 */

SFUNC(fslookup_t, vfs_find_symlink_at, 
						Directory *curdir, 
						char * path) {
	CHAINRET(_vfs_find_at, curdir, path, PATHRES_FLAG_NOSYMLINK, 0);
}

/** 
 * @brief Look up a file referred to by a path
 * @param path The path to resolve
 * @return The file or directory that was found
 */

SFUNC(fslookup_t, vfs_find, char * path) {
	CHAINRET(vfs_find_at, scheduler_current_task->current_directory, 
				path);
}

/** 
 * @brief Look up a file referred to by a path
 * @param path The path to resolve
 * @param curdir The directory to start resolving in
 * @return The file or directory that was found
 */

SFUNC(fslookup_t, vfs_find_at, Directory *curdir, char * path) {
	CHAINRET(_vfs_find_at, curdir, path, 0, 0);
}


///@}

/* Internal functions */

/** 
 * @brief Look up a file referred to by a path using a specified directory as
 * current.
 * @param curdir The directory to start resolving in
 * @param path The path to resolve
 * @return The file or directory that was found
 */

SFUNC(fslookup_t , _vfs_find_at, 
					Directory	*curdir, 
					char		*path, 
					int flags,
					int recurse_level )
{ 
	Directory *dirc = directory_ref(curdir);
	hname_t	temp_name;
	fslookup_t	lookup;
	char * separator;
	char * path_element;
	char * remaining_path = path;
	char * end_of_path;
	char * target;
	size_t element_size;
	dirent_t *dirent;
	int element_count = 0;	
	int	temp_rv;
	aoff_t	sl_size;
	errno_t status;
	aoff_t rlsize;

	/* Check for NULL  */
	assert (path != NULL);

	/* Check for loops */
	if ( recurse_level > CONFIG_MAX_PATH_RECURSION ){
		/* Release current directory */
		directory_release(dirc);

		/* Recursing too deep, return ELOOP */
		THROW( ELOOP, NULL );
	}

	/* Check for empty path */
	if ((*path) == '\0'){
		/* Release current directory */
		directory_release(dirc);

		/* Empty path does not exist! */
		THROW( ENOENT, NULL );
	}

	/* Allocate buffer for path element */
	path_element = (char *) heapmm_alloc(CONFIG_FILE_MAX_NAME_LENGTH);

	/* Check whether the allocation succeeded */
	if (!path_element) {

		/* Release current directory */
		directory_release(dirc);
		
		/* Throw out of memory error */
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
		if ( (element_count == 0) && (element_size == 0) ){ 
			/* Path is absolute */

			/* Release current dirc */
			directory_release(dirc);

			/* Update current element */
			lookup.file = NULL;
			lookup.directory
					= directory_ref(
						scheduler_current_task->root_directory);

		} else if ( element_size == 0 ) {
			/* Element size is 0 but not first element */

			/* Check whether this is the last element */
			if ((separator + 2) >= end_of_path) {
				/* If so, we have detected a trailing slash */
				/* and reached the end of the path */
				/* Because we arrived here after completing an*/
				/* iteration of the loop we have already done */
				/* the S_ISDIR check and made sure dirc refers*/
				/* to a directory */
	
				/* Clean up */
				heapmm_free(path_element,
					 CONFIG_FILE_MAX_NAME_LENGTH);

				/* Return dirc */
				RETURN(dirc);
			}

		} else if ( (element_size == 2) &&
					 !strncmp(remaining_path, "..", 2)) {
			/* Element is ".." */

			/* Check if the directory has a parent set for it and we are not */
			/* escaping our root */
			if ( dirc == scheduler_current_task->root_directory ) {
				/* Trying to escape root, ignore element */
				
				lookup.file = NULL;
				lookup.directory
					= dirc;	
					
			} else if ( dirc->parent != NULL ) {	
				/* It does */

				/* Set next current path element to the parent of
				 * the current element */
				lookup.file = NULL;
				lookup.directory
						= directory_ref(dirc->parent);

				/* Release old dirc */
				directory_release(dirc);

			} else {
				/* It does not, we have arrived at a filesystem root */
				
				/* Lookup the filesystem mountpoint */
				lookup.file = NULL;
				lookup.directory
						= dirc->fs->mountpoint;
						
				/* Make sure the mountpoint exists (e.g. , is not the rootfs )*/
				if ( lookup.directory == NULL ) {
					/* It does not, ignore this element */
					lookup.directory
						= dirc;					
				} else {
					/* It does, reference it */
					lookup.directory
						= directory_ref(lookup.directory);	

					/* Release old dirc */
					directory_release(dirc);
				}
				
			}

		} else if ((element_size == 1) && 
					!strncmp(remaining_path, ".", 1)) {				
			/* Element is . */
			/* and reached the end of the path */
			/* Because we arrived here after completing an*/
			/* iteration of the loop we have already done */
			/* the S_ISDIR check and made sure dirc refers*/
			/* to a directory */

			/* Ignore this */
			lookup.file = NULL;
			lookup.directory
				= dirc;

		} else {
			/* Element is a normal path element */
	
			/* Check whether it is the final element */
			if ( (flags & PATHRES_FLAG_PARENT) && 
				( (separator + 1) >= end_of_path ) ) {
				/* We have already determined the 
				 * parent, return that instead */
				
				/* Return dirc */
				lookup.file = NULL;
				lookup.directory
					= dirc;
				
				RETURN(lookup);

			}
	
			/* Isolate it */
			strncpy(path_element, remaining_path, element_size);
			path_element[element_size] = '\0';

			/* Create a hname for it */
			hname_createtmp( &tmp_name, path_element );

			/* Find the directory entry for it */
			status = SMCALL(dirc, &lookup, get_file, &tmp_name, 0);

			/* Check an error occurred */
			if (status) {

				/* If so, clean up and return */
				heapmm_free(path_element, 
						CONFIG_FILE_MAX_NAME_LENGTH);

				/* Release parent dirc */
				directory_release(dirc);

				/* Pass the error to the caller */
				THROW(status, NULL);
			}
			
			/* Check if the element is a file */
			if ( lookup.file != NULL ) {
				
				/* Check if it is a symbolic link */
				status = SNMCALL(lookup.file, &temp_rv, is_symlink);
				
				/* Check for errors */
				if ( status ) {

					/* If so, clean up and return */
					heapmm_free(path_element, 
							CONFIG_FILE_MAX_NAME_LENGTH);

					/* Release parent dirc */
					directory_release( dirc );
					
					/* Release element */
					file_release( lookup.file );

					/* Pass the error to the caller */
					THROW(status, NULL);
					
				}
				
				/* If it was a symlink, handle it */				
				if ( temp_rv ) {
					/* It is a symlink */

					/* Check if we should dereference it */
					if ( !( ( (separator + 1) >= end_of_path ) &&
						( flags & PATHRES_FLAG_NOSYMLINK ) ) ) {
						/* We are either allowed to deref the */
						/* last element of the path or this is*/
						/* not the last element */
		
						/* Get the symlink size */
						status = SNMCALL( lookup.file, &sl_size, get_size );
						
						/* Check for errors */
						if ( status ) {
							
							/* If so, clean up and return */
							heapmm_free(path_element, 
									CONFIG_FILE_MAX_NAME_LENGTH);

							/* Release parent dirc */
							directory_release( dirc );
							
							/* Release element */
							file_release( lookup.file );

							/* Pass the error to the caller */
							THROW(status, NULL);
					
						}
						
						/* Check whether the symlink size is */
						/* within bounds */
						if ( sl_size >= 
							CONFIG_FILE_MAX_NAME_LENGTH ) {
							/* If so, clean up and return */
							heapmm_free(path_element, 
							   CONFIG_FILE_MAX_NAME_LENGTH);
					
							/* Release element */
							file_release( lookup.file );

							/* Release dirc */
							directory_release(dirc);

							/* Pass the error to the callr*/
							THROW(ENAMETOOLONG, NULL);
						}
						
						/* Allocate symlink target buffer */
						target = heapmm_alloc(sl_size + 1);

						/* Check whether the allocation failed*/
						if ( !target ) {
							/* If so, clean up and return */
							heapmm_free(path_element, 
							   CONFIG_FILE_MAX_NAME_LENGTH);
					
							/* Release element */
							file_release( lookup.file );

							/* Release dirc */
							directory_release(dirc);

							/* Pass the error to the callr*/
							THROW(ENOMEM, NULL);
						}

						/* Read link target */
						status = SMCALL(lookup.file, &rlsize, readlink,
													target,
													sl_size,
													0
										);

						/* Check if an error occurred */
						if (status) {
							/* If so, clean up and return */
							heapmm_free(path_element, 
							   CONFIG_FILE_MAX_NAME_LENGTH);
							heapmm_free(target, 
							   sl_size + 1);
					
							/* Release element */
							file_release( lookup.file );

							/* Release dirc */
							directory_release(dirc);

							/* Pass the error to the callr*/
							THROW(status, NULL);
						}
				
						/* Add terminator to target path */
						target[sl_size] = 0;
					
						/* Release element */
						file_release( lookup.file );

						/* Resolve target path */
						status = _vfs_find_dirc_at( 
								dirc, 
								target, 
								0,
								recurse_level + 1,
								&lookup );

						/* Check if an error occurred */
						if (status) {
							/* If so, clean up and return */
							heapmm_free(path_element, 
							   CONFIG_FILE_MAX_NAME_LENGTH);
							heapmm_free(target, 
							   sl_size + 1);

							/* Release dirc */
							directory_release(dirc);

							/* Pass the error to the callr*/
							THROW(status, NULL);
						}	
						
						/* Release the symlink target buffer */
						heapmm_free(target, sl_size + 1);	

					}
				}
				
			}

			/* Release old element */
			directory_release(dirc);
			
		}

		/* Update remaining path to point to the start of the next
		 * element */
		remaining_path = separator + 1;

		/* Check whether we have reached the end of the path */

		if (remaining_path >= end_of_path) {
			/* If so, return the current element */
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);
				
			/* Check if we were resolving the parent */
			if ( flags & PATHRES_FLAG_PARENT ) {
				/* We have apparently reached a final element */
				/* that was not a filename ( ., .., empty or  */
				/* the filesystem root ), this is not a valid */
				/* operation */

				/* Throw EINVAL: Invalid operation */
				THROW(EINVAL, NULL);

			}

			/* Return dirc */
			RETURN(lookup);
		}

		/* There are still elements after this one */

		/* Check if the current element is a directory */
		if ( lookup.directory == NULL ) {
			/* If not, return NULL because we can't search inside 
                         * other kinds of files */	

			/* Clean up */	
			heapmm_free(path_element, CONFIG_FILE_MAX_NAME_LENGTH);

			/* Release old element */
			file_release(lookup.file);

			THROW(ENOTDIR, NULL);
		} else {
			
			/* Update current element */
			dirc = lookup.directory;
			
		}

	}
} 

/**
 * @brief Extract the filename part of a path
 * @warning This allocates heap for the return value, free with
 *	    heapmm_free(r, strlen(r) + 1);
 * @param path The path to analyze
 * @return The file name, see warning!
 */
SFUNC(char *, vfs_get_filename, const char *path)
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
		THROW(ENAMETOOLONG, NULL);

	/* Allocate room for copy */
	pathcopy = heapmm_alloc(pathlen + 1);

	/* Check if allocation succeeded */
	if (!pathcopy)
		THROW(ENOMEM, NULL);

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

	/* Check if allocation succeeded */
	if (!newname) {
		heapmm_free(pathcopy, pathlen + 1);
		THROW(ENOMEM, NULL);
	}
	
	strcpy(newname, name);
	
	heapmm_free(pathcopy, pathlen + 1);

	RETURN(newname);
}
