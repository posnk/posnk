/**
 * @file kernel/vfs.h
 *
 * Exposes the VFS API
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 09-04-2014 - Created
 * \li 12-07-2014 - Documented
 *
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
#include <sys/errno.h>

/**
 * @defgroup vfs VFS
 * The Virtual File System layer provides abstraction between the rest of the
 * kernel and the FS drivers, it handles path resolution, filesystem mounts and
 * all special file types.
 * @{
 */

#define PATHRES_FLAG_NOSYMLINK	(1<<0)
#define PATHRES_FLAG_PARENT	(1<<1)


/**
 * Bit definition for inode->mode, this file is readable
 */
#define MODE_READ  4

/**
 * Bit definition for inode->mode, this file is writable
 */
#define MODE_WRITE 2

/**
 * Bit definition for inode->mode, this file is executable/searchable
 */
#define MODE_EXEC  1

/**
 * @brief Describes a file currently cached by the kernel
 * @see inode
 */
typedef struct inode inode_t;

/**
 * @brief An entry in the path element cache
 * @see dirent
 */
typedef struct dirent dirent_t;

/**
 * @brief An entry in the path element cache
 * @see dir_cache
 */
typedef struct dir_cache dir_cache_t;

/** 
 * @brief An instance of a filesystem driver 
 * @see fs_device
 */
typedef struct fs_device fs_device_t;

/** 
 * @brief Contains callbacks for all filesystem driver functions 
 * @see fs_device_operations
 */
typedef struct fs_device_operations fs_device_operations_t;

/**
 * @brief A filesystem driver descriptor
 * @see fs_driver
 */
typedef struct fs_driver fs_driver_t;

/**
 * @brief A filesystem mount descriptor
 * @see fs_mount
 */
typedef struct fs_mount fs_mount_t;

typedef struct inode_ops inode_ops_t;

/**
 * Describes a file currently cached by the kernel
 */
struct inode {	
	/** Linked list node */
	llist_t		 node;
	
	/* Inode */
	/** Inode ID */
	ino_t		 id;
	
	/** Block device this inode resides on 		*/
	uint32_t	 device_id;
	
	/** Filesystem device this inode resides on */
	fs_device_t	*device;
	
	/** Mode (not necessarily fully correlated to disk, just for type */
	mode_t		 type;
	
	/** Reference count (for GC)*/
	uint32_t 	 usage_count;
	
	/** Open stream count */
	uint32_t	 open_count;
	semaphore_t	       *lock;	
	
	/** Data field ( set by implementation )	*/
	void		*data;
	
	/** Operations 								*/
	inode_ops_t	*ops;
	
};

/**
 * Describes a directory entry in a portable FS independent format
 */
struct dirent {
	/** The inode this entry points to */
	ino_t	 inode_id;
	/** The filesystem device this entry points to */
	dev_t	device_id;
	/** @brief The length of this dirent 
          * dirent structures are not always sizeof(dirent) long, most of the 
          * time they are smaller, this field indicates the actual size of the
          * structure but do not use it to calculate name length as padding 
          * after the name is allowed */
	unsigned short int d_reclen;//2 + 2 + 4 = 8 -> this struct is long alligned
	/** The name of the file described by this entry */
	char	 name[257];
}  __attribute__((packed));

struct inode_ops {
	

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
	 * @return The number of bytes read
	 */
	SFUNCPTR( aoff_t, read, inode_t *ino,
							void 	*buffer, 
							aoff_t	 offset, 
							aoff_t	 count);

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
	 * @return The number of bytes written
	 */
	SFUNCPTR( aoff_t, write,inode_t *ino,
							void 	*buffer, 
							aoff_t	 offset, 
							aoff_t	 count);

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
	SFUNCPTR( ino_t, findent, inode_t *ino, const char *name );

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
	 * @return The number of bytes read
	 */
	SFUNCPTR( aoff_t, readdir, inode_t *ino, void *buffer, 
											 aoff_t offset, 
											 aoff_t count);

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
	 */

	SVFUNCPTR( link, inode_t *ino, const char *name, ino_t ino_ptr);
	
	/**
	 * @brief Delete a directory entry
	 * @warning Implementations must not modify the inode metadata from
	 * this function, except for the size field
	 * @warning Implementations are required to update the directory size 
     * field
	 * 
	 * @param inode - The inode for the directory
	 * @param name  - The file name of the directory entry to delete
	 */
	SVFUNCPTR( unlink, inode_t *ino, const char *name );	

	/**
	 * @brief Delete an inode
     *
	 * @warning Implementations must not modify the inode metadata from
	 * this function
     *
     * Implementations are recommended to free any data associated with 
     * the inode
	 * @param inode The inode to delete
	 */
	SVFUNCPTR( rmnod, inode_t *ino );	

	/**
	 * @brief Create directory structures
	 * 
	 * @warning Implementations must not modify the inode metadata from
	 * this function
	 * 
     * Implementations are not required to implement this function but must
     * provide a stub to allow directories to be created
	 * @param inode The inode for the new directory
	 */
	SVFUNCPTR( mkdir, inode_t *ino );	

	/**
	 * @brief Write an inode to storage
     *
	 * @warning Implementations must not modify the inode metadata from
	 * this function
     *
	 * @param inode The inode to write 
	 */
	SVFUNCPTR( sync, inode_t *ino );

	/**
	 * @brief Get file information
     *
	 * @warning Implementations must not modify the inode metadata from
	 * this function
     *
	 * @param inode The inode to inquire about
	 */
	SFUNCPTR( struct stat, stat, inode_t *ino );

	/**
	 * @brief Try whether the requested access is allowed
     *
	 * @warning Implementations must not modify the inode metadata from
	 * this function
     *
	 * @param inode The inode to write 
	 */
	SVFUNCPTR( access, inode_t *ino, mode_t mode );

	/**
	 * @brief Resize a file
	 * @warning Implementations must not modify the inode metadata from
	 * this function
     * Implementations must fill the newly created space with zeroes and
     * free any truncated space
	 * 
	 * @param inode       The inode for the file
	 * @param size	      The new size of the file
	 */
	SVFUNCPTR( truncate, inode_t *ino, aoff_t size);
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
	SFUNCPTR( inode_t *, get_ino, fs_device_t *, ino_t );

	/**
	 * @brief Create an inode
     *
     * @warning Implementations must not depend on any fields in inode
     * being valid
     *
	 * Implementations must fill inode->id
	 * @param inode The inode to create
	 */
	SFUNCPTR( inode_t *, mknod, fs_device_t *, mode_t mode, dev_t dev);	

	/**
	 * @brief Synchronize the filesystem
	 *
         * Implementations must flush any global metadata to backing storage
	 * 
	 * @param inode       The inode for the file
	 * @param size	      The new size of the file
	 */
	SVFUNCPTR( sync, fs_device_t * );
};

/** 
 * @brief An instance of a filesystem driver 
 * 
 * This structure describes an instance of a filesystem driver, it is returned 
 * by a filesystem's mount function and contains information on the interface
 * provided by the driver aswell as the mounted device.
 * In order to implement a filesystem driver you must provide two things:
 * \li A mount function of the form 
 *         fs_device_t fs_mount( dev_t device, uint32_t flags);
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
};

/**
 * @brief A filesystem driver descriptor
 *
 * This structure describes a filesystem driver, it is used to store available
 * filesystem drivers in a table for use by vfs_mount
 */
struct fs_driver {
	/** Linked list node */
	llist_t		 link;
	/** Filesystem name (e.g. ext2 or ramfs) */
	char		*name;
	/** Mount callback @see fs_device */
	SFUNCPTR(fs_device_t *, mount, dev_t, uint32_t);
};

/**
 * @brief A mounted filesystem descriptor
 * 
 * This structure describes a filesystem that has been mounted at a mountpoint
 *
 */
struct fs_mount {

	/** Linked list node */
	llist_t		 link;

	/** Filesystem device */
	fs_device_t	*device;

	/** Mountpoint */
	ino_t		 mt_ino;
	uint32_t	 mt_dev;
	
	/** Rootdirectory */
	ino_t		 rt_ino;
	uint32_t	 rt_dev;

};


/** @name VFS API
 *  Public VFS functions
 */
///@{
void vfs_icache_initialize();
void vfs_cache_flush();

inode_t *vfs_inode_ref(inode_t *inode);

void vfs_inode_release(inode_t *inode);

SFUNC(dirent_t *, vfs_find_dirent, inode_t * inode, char * name);

int vfs_rmdir(char *path);

int vfs_mkdir(char *path, mode_t mode);

int vfs_mknod(char *path, mode_t mode, dev_t dev);

int vfs_unlink(char *path);

int vfs_link(char *oldpath, char *newpath);

int vfs_symlink(char *oldpath, char *newpath);

int vfs_readlink(inode_t *inode, char * buffer, size_t size, size_t *read_size);

int vfs_write(inode_t * inode , aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size, int non_block);

int vfs_read(inode_t * inode , aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size, int non_block);

int vfs_getdents(inode_t * inode , aoff_t file_offset, dirent_t * buffer, aoff_t count, aoff_t *read_size);

int vfs_truncate(inode_t * inode, aoff_t length);

int vfs_chroot(dir_cache_t *dirc);

int vfs_chdir(dir_cache_t *dirc);

SFUNC(char *, vfs_get_filename, const char *path);

///@}


/** @name VFS Internal
 *  Utility functions for use by VFS functions only
 */
///@{
void vfs_inode_cache(inode_t *inode);
inode_t *vfs_get_cached_inode(fs_device_t *device, ino_t inode_id);

perm_class_t vfs_get_min_permissions(inode_t *inode, mode_t req_mode);

int vfs_have_permissions(inode_t *inode, mode_t req_mode);

SFUNC(inode_t *, vfs_get_inode, fs_device_t *device, ino_t inode_id);

inode_t *vfs_effective_inode(inode_t * inode);

///@}

int vfs_initialize(dev_t root_device, char *root_fs_type);

///@}

SFUNC( inode_t *, ifs_load_inode, fs_device_t * device, ino_t id );
SVFUNC( ifs_store_inode, inode_t * inode );
SVFUNC( ifs_mknod, inode_t * inode );
SVFUNC( ifs_rmnod, inode_t * inode);
SVFUNC( ifs_mkdir, inode_t * inode);
SFUNC( dirent_t *, ifs_find_dirent, inode_t * inode, char * name);
SVFUNC( ifs_link, inode_t * inode , char * name , ino_t nod_id );
SVFUNC( ifs_unlink, inode_t * inode , char * name );
SFUNC( aoff_t, ifs_read_dir, inode_t * inode, void * buffer, aoff_t file_offset, aoff_t count );
SFUNC( aoff_t, ifs_read, inode_t * inode, void * buffer, aoff_t file_offset, aoff_t count );
SFUNC( aoff_t, ifs_write, inode_t * inode, void * buffer, aoff_t file_offset, aoff_t count );
SVFUNC( ifs_truncate, inode_t * inode, aoff_t size);
SVFUNC( ifs_sync, fs_device_t *device );
SFUNC(dir_cache_t *, vfs_find_dirc_parent, char * path);
SFUNC(dir_cache_t *, vfs_find_dirc_parent_at, dir_cache_t *curdir, char * path);
SFUNC(dir_cache_t *,_vfs_find_dirc_at,dir_cache_t *curdir,char * path,int flags,
					int recurse_level );
SFUNC(dir_cache_t *, vfs_find_dirc_symlink_at, dir_cache_t *curdir,char * path);
SFUNC(dir_cache_t *, vfs_find_dirc_symlink, char * path);
SFUNC(dir_cache_t *, vfs_find_dirc, char * path);
SFUNC(dir_cache_t *, vfs_find_dirc_at, dir_cache_t *curdir, char * path);
SFUNC(inode_t *, vfs_find_parent, char * path);
SFUNC(inode_t *, vfs_find_inode, char * path);
SFUNC(inode_t *, vfs_find_symlink, char * path);
SVFUNC(vfs_sync_inode, inode_t *inode);
fs_mount_t	*vfs_get_mount_by_root( uint32_t dev, ino_t ino );
fs_mount_t	*vfs_get_mount_by_mountpoint( uint32_t dev, ino_t ino );
SVFUNC(vfs_mount, char *device, char *mountpoint, char *fstype, uint32_t flags);
SVFUNC(vfs_do_mount, fs_driver_t *driver, dev_t device, inode_t *mountpoint, uint32_t flags);
SFUNC(fs_driver_t *, vfs_get_driver, char *fstype);
SVFUNC( vfs_register_fs, 
		const char *name, 
		SFUNCPTR(fs_device_t *, mnt_cb, dev_t, uint32_t) );
void vfs_ifsmgr_initialize( void );
void vfs_mount_initialize ( void );
void vfs_sync_filesystems( void );
#endif

