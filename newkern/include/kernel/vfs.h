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
#include "util/mrucache.h"
#include "util/hname.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <kobj.h>

/**
 * @defgroup vfs VFS
 * The Virtual File System layer provides abstraction between the rest of the
 * kernel and the FS drivers, it handles path resolution, filesystem mounts and
 * all special file types.
 * @{
 */

#define PATHRES_FLAG_NOSYMLINK	(1<<0)
#define PATHRES_FLAG_PARENT		(1<<1)


/**
 * Bit definitions for tflag_t
 */
#define TFLAG_ATIME		(1)
#define TFLAG_MTIME		(2)
#define TFLAG_CTIME		(4)

/**
 * Bit definitions for oflag_t
 */
#define OFLAG_READ		(4)
#define OFLAG_WRITE		(2)
#define OFLAG_IGNPERM	(1)

/**
 * Bit definitions for rflag_t
 */
#define RFLAG_ALLOWLNK	(1)

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
 * @brief Type for the flags parameter passed to FileHandle.read
 * Describes any important options that should be adhered to by the impl.
 */
typedef uint32_t	rflag_t;

/**
 * @brief Type for the flags parameter passed to FileHandle.write
 * Describes any important options that should be adhered to by the impl.
 */
typedef uint32_t	wflag_t;

/**
 * @brief Type for the flags parameter passed to FileHandle.map
 * Describes any important options that should be adhered to by the impl.
 */
typedef uint32_t	mflag_t;

/**
 * @brief Type for the flags parameter passed to pathresolution routines
 * Describes any important options that should be adhered to by the impl.
 */
typedef uint32_t	fflag_t;

/**
 * @brief Type for the flags parameter passed to <VFSObject>.open
 * Describes any important options that should be adhered to by the impl.
 */
typedef uint32_t	oflag_t;

/**
 * @brief Type for the flags parameter passed to Directory.create_*
 * Describes any important options that should be adhered to by the impl.
 */
typedef uint32_t	cflag_t;

/**
 * @brief Type for the flags parameter passed to Directory.remove_file
 * Describes any important options that should be adhered to by the impl.
 */
typedef uint32_t	dflag_t;

/**
 * @brief Type for the flags parameter passed to Filesystem.unmount
 * Describes any important options that should be adhered to by the impl.
 */
typedef uint32_t	uflag_t;

/**
 * @brief Type for the flags parameter passed to FSDriver.mount
 * Describes any important options that should be adhered to by the impl.
 */
typedef uint32_t	mflag_t;

/**
 * @brief Type for the flags parameter passed to file access time functions
 * Describes which time the implementation should operate on
 */
typedef uint32_t	tflag_t;

/**
 * @brief Type for the flags parameter passed to file access functions
 * Describes which kind of access is requested
 */
typedef uint32_t	aflag_t;

class_decl(FSDriver);
class_decl(FSPermissions);
class_decl(Directory);
class_decl(File);
class_decl(FileHandle);
class_decl(FileLink);
class_decl(DirHandle);
class_decl(Filesystem);

/**
 * Describes the result of a filesystem lookup
 */
struct fslookup {

	/**
	 * Holds the result if it was a directory
	 */
	Directory		*directory;

	/**
	 * Holds the result if it was a file
	 */
	File			*file;

};

/**
 * Describes the result of a filesystem lookup
 * @see struct fslookup
 */
typedef struct fslookup fslookup_t;

/**
 * Describes a filesystem driver
 */
class_defn(FSDriver)
{

	/**
	 * Destroy the object
	 */
	SOMDECL(FSDriver, destroy);	
	
	/**
	 * Mount a filesystem
	 * @param target	String specifying the filesystem to mount
	 * @param flags		Options indicating how to mount the filesystem
	 */
	SMDECL(Filesystem *, FSDriver, mount_named, 
						char *  /* target */,
						mflag_t /* flags */
			);	
	
	/**
	 * Mount a filesystem
	 * @param target	Block device id specifying the filesystem to mount
	 * @param flags		Options indicating how to mount the filesystem
	 */
	SMDECL(Filesystem *, FSDriver, mount_device, 
						dev_t	/* target */,
						mflag_t /* flags */
			);	
	
	method_end_o(FSDriver, llist_t);
	
	/** The name of this filesystem */
	char *		 name;
	
	/** Implementation specific data */
	void		*impl;
	
	/** Number of mounted instances of this driver */
	int			 mntcount;
	
};

/**
 * Describes a filesystem
 */
class_defn(Filesystem)
{

	/**
	 * Destroy the object
	 */
	SOMDECL(Filesystem, destroy);	
	
	/**
	 * Unmount the filesystem
	 * @param flags	Options indicating how to unmount the filesystem
	 */
	SVMDECL(Filesystem, unmount, 
						uflag_t /* flags */
			);
	
	/**
	 * Attach the filesystem
	 * @param mountpoint	The mountpoint to attach the filesystem to
	 */
	SVMDECL(Filesystem, attach, 
						Directory * /* mountpoint */
			);	
	
	/**
	 * Sync the filesystem
	 */
	SOMDECL(Filesystem, sync );	
	
	method_end_o(Filesystem, llist_t);

	/** Lock */
	semaphore_t	*lock;

	/** Implementation specific data */
	void		*impl;
	
	/** Root directory */
	Directory	*root;
	
	/**
	 * Mount point 
	 * @note This is NULL before the file system is attached to a mountpoint
	 * @note This is also NULL for the root file system
	 */
	Directory	*mountpoint;
		
	/** Driver reference */
	FSDriver	*driver;

	/** File list */
	llist_t		 files;
	
};

/**
 * Describes a file's permissions
 */
class_defn(FSPermissions) {

	/**
	 * Tries to access the file
	 * @param		flags	The type of access requested
	 * @exception	EACCES	The current user does not have sufficient rights to
	 * 						access the file
	 */
	SVMDECL(FSPermissions, access, aflag_t);	

	/**
	 * Set the owning user
	 * @param		uid		The user id of the new owner
	 */
	SVMDECL(FSPermissions, set_owner_uid, uid_t);

	/**
	 * Set the owning group
	 * @param		gid		The group id of the new owner
	 */
	SVMDECL(FSPermissions, set_owner_gid, gid_t);

	/**
	 * Set object to match the UNIX permissions field passed
	 * @param		mode	The UNIX permissions field to be set
	 */
	SVMDECL(FSPermissions, set_unix_perm, umode_t);

	/**
	 * Destroy the object
	 */
	SOMDECL(FSPermissions, destroy);	
	
	method_end(FSPermissions);
	
	/** Implementation specific data */
	void		*impl;
	
};

/**
 * Represents an open file
 */
class_defn(FileHandle) {

	/**
	 * Close a file handle
	 */
	SOMDECL(FileHandle, close);

	/**
	 * Read synchronously from a file
	 * 
	 * @param buffer The buffer to load the file's contents into
	 * @param offset The offset in the file to read
	 * @param length The number of bytes to read
	 * @param flags	 Options for reading the file
	 * @return The number of bytes actually read
	 */
	SMDECL(aoff_t, FileHandle, read, 
					void *  /* buffer */,   
					aoff_t  /* offset */,
					aoff_t  /* length */,
					rflag_t /* flags */
				);
				
	/**
	 * Write synchronously to a file
	 * 
	 * @param buffer The buffer to load the file's contents into
	 * @param offset The offset in the file to read
	 * @param length The number of bytes to read
	 * @param flags	 Options for writing the file
	 * @return The number of bytes actually read
	 */
	SMDECL(aoff_t, FileHandle, write, 
					void *  /* buffer */,   
					aoff_t  /* offset */,
					aoff_t  /* length */,
					wflag_t /* flags */
				);
				
	/**
	 * Map a file to the process memory the file range to
	 * @param offset The offset in the file to map there
	 * @param length The number of bytes to map
	 * @param flags	 Options for mapping the file
	 */
	SVMDECL(FileHandle,	map,
					void *  /* target */,
					aoff_t	/* offset */,
					aoff_t	/* length */,
					mflag_t	/* flags */
				);

	/**
	 * Unmap a file from the process memory space
	 * @param target The address the file was mapped to
	 * @param length The number of bytes that were mapped
	 */
	SVMDECL(FileHandle,	unmap,
					void *  /* target */,
					aoff_t	/* length */
				);
			
	//TODO: Add file inspection interface (see: stat/fstat/lstat)

	/**
	 * Get a file's size
	 * @return The size of the file
	 */
	SNMDECL(aoff_t, FileHandle, get_size);
	
	/**
	 * Flush a file's contents to disk from the cache
	 */
	SOMDECL(FileHandle, sync);
	
	method_end_o(FileHandle, llist_t);

	/** File reference */
	File		*file;

	/** Lock */
	semaphore_t	*lock;

	/** IFS state pointer */
	void		*state;
	
	/** Reference count */
	int			refcount;

};

/**
 * Represents an open directory
 */
class_defn(DirHandle) {

	/**
	 * Close a directory handle
	 */
	SOMDECL(DirHandle, close);

	//TODO: Come up with a good readdir interface
	
	/**
	 * Refresh the handle to the current directory state
	 */
	SOMDECL(DirHandle, sync);
	
	method_end_o(DirHandle, llist_t);

	/** Directory reference */
	Directory	*directory;

	/** Lock */
	semaphore_t	*lock;

	/** IFS state pointer */
	void		*state;

};

/**
 * Describes a directory
 */
class_defn(Directory) {
	
	/** 
	 * Gets a file or subdirectory from the directory
	 * @param filename	The name of the file to find
	 * @param flags		Options indicating how to look up the file
	 */
	SMDECL( fslookup_t, Directory, get_file, 
					hname_t */* filename */, 
					fflag_t  /* flags */
			);
	
	/** 
	 * Opens a reference to this directory
	 * @param flags		Options indicating how to open the file
	 * @return			A new handle to the directory
	 */
	SMDECL( DirHandle *, Directory, open, 
					oflag_t /* flags */
			);	
	
	/** 
	 * Creates a new regular file in the directory
	 * @param filename	The name of the file to create
	 * @param flags		Options indicating how to create the file
	 * @note			Must only be called while directory is locked
	 * @return			The newly created file object
	 */	
	SMDECL( File *, Directory, create_file,
					char *	/* filename */,
					cflag_t /* flags */
			);	
	/** 
	 * Creates a new device file in the directory
	 * @param filename	The name of the file to create
	 * @param flags		Options indicating how to create the file
	 * @param device	The device number the new file should point to
	 * @note			Must only be called while directory is locked
	 * @return			The newly created file object
	 */
	SMDECL( File *, Directory, create_device,
					char *	/* filename */,
					cflag_t /* flags */,
					dev_t	/* device */
			);	
	/** 
	 * Creates a new named pipe in the directory
	 * @param filename	The name of the pipe to create
	 * @param flags		Options indicating how to create the pipe
	 * @note			Must only be called while directory is locked
	 * @return			The newly created file object
	 */				
	SMDECL( File *, Directory, create_pipe,
					char *	/* filename */,
					cflag_t /* flags */
			);	
	/** 
	 * Creates a new symbolic link in the directory
	 * @param filename	The name of the link to create
	 * @param flags		Options indicating how to create the link
	 * @param target	The path to the target
	 * @note			Must only be called while directory is locked
	 * @return			The newly created file object
	 */				
	SMDECL( File *, Directory, create_link,
					char *	/* filename */,
					cflag_t /* flags */,
					char * /* target */
			);	
			
	/** 
	 * Creates a new (hard) link to a file in the directory
	 * @param filename	The name of the link to create
	 * @param file		The file to link to
	 * @note			Must only be called while directory and file are locked
	 */	
	SVMDECL( Directory, link_file,
					char *	/* filename */,
					File *	/* file */
			);
	
	/**
	 * Deletes a file or subdirectory from the directory listing
	 * @param filename	The name of the file to remove
	 * @param flags		Options indicating how to delete the file
	 * @note The file might not be truely deleted until all other links are gone
	 * 		 and all handles to it have been closed
	 */
	SVMDECL( Directory, remove_file, 
					char *		/* filename */,
					dflag_t		/* flags */
			);
			
	//TODO: Add file inspection interface (see: stat/fstat/lstat)

	/**
	 * Destroy the object in memory (does not affect the actual directory)
	 * @warning This method should only be called by the directory caching code
	 */
	SOMDECL(Directory, destroy);

	/**
	 * Delete the directory from backing storage
	 * @warning This method should only be called after all references (links
	 * 			and handles) are gone.
	 */
	SOMDECL(Directory, delete);
	
	method_end_o(Directory, llist_t);
	
	/**
	 * The hashed name for the directory
	 */
	hname_t			*name;
	
	/**
	 * The directory containing this directory
	 * If this is a filesystem root, this will be zero
	 */
	Directory		*parent;
	
	/**
	 * The directory mounted here
	 */
	Directory		*mount;
	
	/**
	 * List containing subdirectories currently loaded in memory
	 * Entry type llist_t:>Directory
	 */
	llist_t			 subdirectories;
	
	/**
	 * List containing linked files currently loaded in memory
	 * Entry type llist_t:>FileLink
	 */
	llist_t			 files;
		
	/**
	 * The filesystem containing this directory
	 */
	Filesystem		*fs;
	
	/**
	 * The filesystem permissions object representing this directory
	 */
	FSPermissions	*permissions;

	/** 
	 * Lock
	 */
	semaphore_t		*lock;

	/** 
	 * IFS state pointer 
	 */
	void			*state;
	
	/** 
	 * Reference count 
	 */
	int				 refcount;
	
	/** 
	 * Handle count 
	 */
	int				 hndcount;
	
};

/**
 * Represents a linkback from a file to a link
 */
typedef struct {
	llist_t			 node;
	FileLink		*link;
} flinkback_t;

/**
 * Represents a link to a file
 */
class_defn(FileLink) {
	
	/**
	 * Destroy this file link object
	 */ 
	SOMDECL(FileLink, destroy);
	
	method_end_o(FileLink, llist_t);

	/**
	 * The filename associated with this link
	 */
	hname_t		*name;
	
	/**
	 * The directory containing this link
	 */	
	Directory	*parent;
	
	/**
	 * The file this link refers to
	 */
	File		*file;	

	/**
	 * Linkback to this link from the target
	 */
	flinkback_t  linkback;
	
};

/**
 * Describes a file
 */
class_defn(File) {
	
	/** Opens a reference to this file
	 * @param flags		Options indicating how to open the file
	 */
	SMDECL( FileHandle *, File, open, 
					oflag_t /* flags */
			);	
	
	/** Reads the link target from a symlink
	 * @param buffer	The buffer to read to
	 * @param length	How many characters to read
	 * @param flags		Options indicating how to open the file for reading
	 * @return			The number of characters read
	 */
	SMDECL( aoff_t, File, readlink, 
					char *	/* buffer	*/,
					aoff_t	/* length	*/,
					oflag_t /* flags 	*/
			);

	/**
	 * Get a file's size
	 * @return The size of the file
	 */
	SNMDECL(aoff_t, File, get_size);

	/**
	 * Get a file's link count
	 * @return The link count
	 */
	SNMDECL(int, File, get_link_count);

	/**
	 * Check if this file is a symbolic link
	 * @return nonzero if the file is a symbolic link
	 */
	SNMDECL(int, File, is_symlink);

	/**
	 * Check if this file is a block device
	 * @return nonzero if the file is a block device
	 */
	SNMDECL(int, File, is_blkdev);

	/**
	 * Gets the device this file points to
	 * @return The device this file points to
	 */
	SNMDECL(dev_t, File, get_device);

	/**
	 * Destroy the object
	 */
	SOMDECL(File, destroy);
	
	/**
	 * Delete the file from backing storage
	 * @warning This method should only be called after all references (links
	 * 			and handles) are gone.
	 */
	SOMDECL(File, delete);
	
	method_end_o(File, llist_t);
	
	/**
	 * The filesystem containing this file
	 */
	Filesystem		*fs;
	
	/**
	 * The filesystem permissions object representing this file
	 */
	FSPermissions	*permissions;

	/** Lock */
	semaphore_t		*lock;

	/** 
	 * IFS state pointer 
	 */
	void			*state;
	
	/** 
	 * Reference count 
	 */
	int				 refcount;
	
	/** 
	 *Handle count 
	 */
	int				 hndcount;
	
	/**
	 * Loaded link list
	 */ 	
	llist_t			 links;
	
};

SVFUNC( dcache_initialize, int max_entries );
SVFUNC( dcache_add_dir, Directory *dir );
SVFUNC( fcache_initialize, int max_entries );
SVFUNC( fcache_add_link, Directory *parent, File *file, hname_t *name );
SVFUNC( fcache_add_file, File *file );
File *file_ref( File *file );
void file_release( File *file );
Directory * directory_ref( Directory *dir );
void directory_release( Directory *dir );
void vfs_mount_initialize ( void );
Filesystem	*vfs_get_mount_by_mountpoint( Directory *mountpoint );
void vfs_reg_mount( Filesystem *filesystem );
void vfs_unreg_mount( Filesystem *filesystem );

SVFUNC(vfs_do_mount_device,	
						FSDriver *driver, 
						dev_t device, 
						Directory *mountpoint, 
						mflag_t flags
		);

SVFUNC(vfs_do_mount_named,	
						FSDriver *driver, 
						char * target, 
						Directory *mountpoint, 
						mflag_t flags
		);
void vfs_sync_filesystems( void );
SFUNC(FSDriver *, vfs_get_driver, char *fstype);
SVFUNC( vfs_register_fs, FSDriver *driver );
void vfs_ifsmgr_initialize( void );
SFUNC(fslookup_t, vfs_find_parent, char * path);
SFUNC(fslookup_t, vfs_find_parent_at, Directory *curdir, char * path);
SFUNC(fslookup_t, vfs_find_symlink, char * path);
SFUNC(fslookup_t, vfs_find_symlink_at, Directory *curdir, char * path);
SFUNC(fslookup_t, vfs_find, char * path);
SFUNC(fslookup_t, vfs_find_at, Directory *curdir, char * path);
void vfs_bump_utimes_dir( Directory *dir , tflag_t flags );
void vfs_bump_utimes_file( File *file , tflag_t flags );
int vfs_chroot(Directory *dirc);
int vfs_chdir(Directory *dirc);
SVFUNC(vfs_mount, char *device, char *mountpoint, char *fstype, uint32_t flags);
/** @name VFS API
 *  Public VFS functions
 */
///@{
void vfs_cache_flush();
/*
void vfs_icache_initialize();
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
*/


SFUNC(char *, vfs_get_filename, const char *path);
///@}


/** @name VFS Internal
 *  Utility functions for use by VFS functions only
 */
///@{
	/*
perm_class_t vfs_get_min_permissions(inode_t *inode, mode_t req_mode);

int vfs_have_permissions(inode_t *inode, mode_t req_mode);

inode_t *vfs_effective_inode(inode_t * inode);*/

///@}

int vfs_initialize(dev_t root_device, char *root_fs_type);

///@}
/*
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
SVFUNC(vfs_sync_inode, inode_t *inode);
*/
#endif

