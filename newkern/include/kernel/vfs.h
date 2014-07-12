/**
 * kernel/vfs.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-04-2014 - Created
 */

#include "kernel/process.h"

#ifndef __KERNEL_VFS_H__
#define __KERNEL_VFS_H__

#include "kernel/synch.h"
#include "kernel/permissions.h"
#include "kernel/pipe.h"
#include "kernel/time.h"
#include "util/llist.h"
#include <sys/types.h>
#include <sys/stat.h>

typedef struct inode inode_t;

typedef struct dirent dirent_t;

typedef struct dir_cache dir_cache_t;

typedef struct fs_device fs_device_t;

typedef struct fs_device_operations fs_device_operations_t;

/**
 * Describes a file currently cached by the kernel
 */
struct inode {	
	/** Linked list node */
	llist_t		 node;
	/* Inode */
	/** Inode ID */
	ino_t		 id;
	/** Block device this inode resides on */
	uint32_t	 device_id;
	/** Filesystem device this inode resides on */
	fs_device_t	*device;
	/** Filename @deprecated */
	char	 	 name[CONFIG_FILE_MAX_NAME_LENGTH];
	/** Number of hard links to this file */
	nlink_t 	 hard_link_count;
	/* Permissions */
	/** Owner UID */
	uid_t	 	 uid;
	/** Owner GID */
	gid_t	 	 gid;
	/** File mode, see stat.h */
	umode_t	 	 mode;
	/* Special file */
	/** Device id for special files */
	dev_t	 	 if_dev;
	/** Symbolic link target */
	char 	 	 link_path[CONFIG_FILE_MAX_NAME_LENGTH];
	/** FIFO pipe back end */
	pipe_info_t 	*fifo;
	/** Mounted filesystem root inode */
	inode_t	 	*mount;
	/** Lock */
	semaphore_t 	*lock;
	/** Usage count */
	uint32_t 	 usage_count;
	/** File size */
	aoff_t	 	 size;	
	/** Access time */
	ktime_t		 atime; 
	/** Modification time */
	ktime_t		 mtime;
	/** Metadata modification time */
	ktime_t		 ctime;
};

struct dirent {
	ino_t	 inode_id;
	dev_t	device_id;
	unsigned short int d_reclen;//2 + 2 + 4 = 8 -> this struct is long alligned
	char	 name[257];
}  __attribute__((packed));

struct dir_cache {
	dir_cache_t	*parent;
	inode_t		*inode;
	uint32_t	 usage_count;
};

/** 
 * @brief Contains callbacks for all filesystem driver functions 
 *
 * This is the structure used to pass the callbacks for a filesystem driver to
 * the kernel, it is referenced by fs_device, which describes an instance of a
 * filesystem driver, implementation hints and requirements are given in the 
 * description of each callback, for more info on FS driver implementation see
 * fs_device
 */
struct fs_device_operations {

	/**
	 * @brief Load an inode from storage
	 * 
         * REQUIRED
         *
	 * Implementations of this function must fill ALL inode fields relevant
         * to the file type, this includes instance fields such as lock
	 * @param inode The inode id to load
         * @return The loaded inode
	 */
	inode_t *	(*load_inode)	(fs_device_t *, ino_t);//inode id -> inode

	/**
	 * @brief Write an inode to storage
         *
	 * @warning Implementations must not modify the inode metadata from
	 * this function
         *
	 * @param inode The inode to write 
         * @return In case of error: a valid error code, Otherwise 0
	 */
	int		(*store_inode)	(inode_t *);//inode -> status

	/**
	 * @brief Create an inode
         *
         * @warning Implementations must not depend on any fields in inode
         * being valid
         *
	 * Implementations must fill inode->id
	 * @param inode The inode to create
         * @return In case of error: a valid error code, Otherwise 0
	 */
	int		(*mknod)	(inode_t *);	//inode -> status

	/**
	 * @brief Delete an inode
         *
	 * @warning Implementations must not modify the inode metadata from
	 * this function
         *
         * Implementations are recommended to free any data associated with 
         * the inode
	 * @param inode The inode to delete
         * @return In case of error: a valid error code, Otherwise 0
	 */
	int		(*rmnod)	(inode_t *);	//inode -> status

	/**
	 * @brief Read data from a file
	 * 
         * REQUIRED
	 * @warning Implementations must not modify the inode metadata from
	 * this function
         * @note The VFS will adjust count so that the whole requested area is 
	 * in the file
	 * @param inode       The inode for the file
	 * @param buffer      The buffer to store the data in
	 * @param file_offset The offset in the file to start reading at
	 * @param count       The number of bytes to read
	 * @param nread       A pointer to a variable in which the number of 
	 *			bytes read will be stored
	 * @return In case of error: a valid error code, Otherwise 0
	 */

	int		(*read_inode)	(inode_t *, void *, aoff_t, aoff_t, aoff_t *);//buffer, f_offset, length, nread

	/**
	 * @brief Write data to a file
	 * 
	 * @warning Implementations must not modify the inode metadata from
	 * this function
         *
         * @note The VFS layer will automatically adjust file size\n
	 * @param inode       The inode for the file
	 * @param buffer      The buffer containing the data to write
	 * @param file_offset The offset in the file to start writing at
	 * @param count       The number of bytes to write
	 * @param nwritten    A pointer to a variable in which the number of 
	 *			 bytes written will be stored
	 * @return In case of error: a valid error code, Otherwise 0
	 */
	int		(*write_inode)	(inode_t *, void *, aoff_t, aoff_t, aoff_t *);//buffer, f_offset, length, nwritten

	/**
	 * @brief Read directory entries from backing storage
	 * 
         * REQUIRED
	 * @warning Implementations must not modify the inode metadata from
	 * this function
	 * @warning Implementations must only return whole directory entries
	 * 
	 * @param inode       The inode for the directory
	 * @param buffer      The buffer to store the entries in
	 * @param file_offset The offset in the directory to start reading at
	 * @param count       The number of bytes to read
	 * @param nread	      A pointer to a variable in which the number of 
	 *			bytes read will be stored
	 * @return In case of error: a valid error code, Otherwise 0
	 */

	int		(*read_dir)	(inode_t *, void *, aoff_t, aoff_t, aoff_t *);//buffer, f_offset, length, nread 

	/** 
	 * @brief Find a directory entry
	 * 
         * REQUIRED
	 * @warning Implementations must not modify the inode metadata from
	 * this function
	 * 
	 * @param inode The directory to search
	 * @param name  The filename to match
	 * @return The directory entry matching name from the directory inode, 
	 *          if none, NULL is returned
	 */
	dirent_t *	(*find_dirent)	(inode_t *, char *);	//dir_inode_id, filename -> dirent_t  

	/**
	 * @brief Create directory structures
	 * 
	 * @warning Implementations must not modify the inode metadata from
	 * this function
	 * 
         * Implementations are not required to implement this function but must
         * provide a stub to allow directories to be created
	 * @param inode The inode for the new directory
	 * @return In case of error: a valid error code, Otherwise 0
	 */
	int		(*mkdir)	(inode_t *);		//dir_inode_id -> status

	/**
	 * @brief Create a directory entry
	 * 
	 * @warning Implementations must not modify the inode metadata from
	 * this function, except for the size field
	 * @warning Implementations are required to update the directory size 
         * field
	 * 
	 * @param inode  The inode for the directory
	 * @param name   The file name for the directory entry
	 * @param nod_id The inode id that the directory entry will point to
	 * @return In case of error: a valid error code, Otherwise 0
	 */

	int		(*link)		(inode_t *, char *, ino_t);	//dir_inode_id, filename, inode_id -> status

	/**
	 * @brief Delete a directory entry
	 * @warning Implementations must not modify the inode metadata from
	 * this function, except for the size field
	 * @warning Implementations are required to update the directory size 
         * field
	 * 
	 * @param inode - The inode for the directory
	 * @param name  - The file name of the directory entry to delete
	 * @return In case of error: a valid error code, Otherwise 0
	 */
	int		(*unlink)	(inode_t *, char *);	//dir_inode_id, filename

	/**
	 * @brief Resize a file
	 * @warning Implementations must not modify the inode metadata from
	 * this function
         * Implementations must fill the newly created space with zeroes and
         * free any truncated space
	 * 
	 * @param inode       The inode for the file
	 * @param size	      The new size of the file
	 * @return In case of error: a valid error code, Otherwise 0
	 */
	int		(*trunc_inode)	(inode_t *, aoff_t);
};

/** 
 * @brief An instance of a filesystem driver 
 * 
 * This structure describes an instance of a filesystem driver, it is returned 
 * by a filesystem's mount function and contains information on the interface
 * provided by the driver aswell as the mounted device.
 * In order to implement a filesystem driver you must provide two things:
 * \li A mount function of the form 
 *         fs_device_t fs_mount( dev_t device, uin32_t flags);
 * \li Implementations of the callbacks in fs_device_operations
 * 
 * For more information on the callbacks see the documentation of 
 * fs_device_operations
 *
 */
struct fs_device {
	/** Block device this filesystem resides on */
	uint32_t		id;
	/** Root inode ID */
	ino_t			root_inode_id;
	/** Pointer to the callback list */
	fs_device_operations_t *ops;
	/** Filesystem lock */
	semaphore_t	       *lock;	
	/** @brief Inode structure size for this filesystem
          *
          * In memory size of the inode structures returned by the callbacks
          * of this driver.
          */
	size_t 			inode_size;
};

#define MODE_READ  4
#define MODE_WRITE 2
#define MODE_EXEC  1

perm_class_t vfs_get_min_permissions(inode_t *inode, mode_t req_mode);

int vfs_have_permissions(inode_t *inode, mode_t req_mode);

inode_t *vfs_get_inode(fs_device_t *device, ino_t inode_id);

inode_t *vfs_effective_inode(inode_t * inode);

dirent_t *vfs_find_dirent(inode_t * inode, char * name);

int vfs_rmdir(char *path);

int vfs_mkdir(char *path, mode_t mode);

int vfs_mknod(char *path, mode_t mode, dev_t dev);

int vfs_unlink(char *path);

int vfs_link(char *oldpath, char *newpath);

int vfs_symlink(char *oldpath, char *newpath);

int vfs_mount(char *device, char *mountpoint, char *fstype, uint32_t flags);

inode_t *vfs_find_parent(char * path);

inode_t *vfs_find_inode(char * path);

inode_t *vfs_find_symlink(char * path);

dir_cache_t *vfs_dir_cache_mkroot(inode_t *root_inode);

void vfs_dir_cache_release(dir_cache_t *dirc);

dir_cache_t *vfs_find_dirc(char * path);

int vfs_initialize(fs_device_t * root_device);

int vfs_write(inode_t * inode , aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size, int non_block);

int vfs_read(inode_t * inode , aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size, int non_block);

int vfs_getdents(inode_t * inode , aoff_t file_offset, dirent_t * buffer, aoff_t count, aoff_t *read_size);

int vfs_truncate(inode_t * inode, aoff_t length);

#endif

