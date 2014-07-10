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
#include "kernel/synch.h"
#include "kernel/permissions.h"
#include "kernel/pipe.h"
#include "kernel/time.h"
#include "util/llist.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifndef __KERNEL_VFS_H__
#define __KERNEL_VFS_H__

typedef struct inode inode_t;

typedef struct dirent dirent_t;

typedef struct dir_cache dir_cache_t;

typedef struct fs_device fs_device_t;

typedef struct fs_device_operations fs_device_operations_t;

struct inode {	
	llist_t		 node;
	/* Inode */
	uint32_t	 device_id;
	ino_t		 id;
	fs_device_t	*device;
	char	 	 name[CONFIG_FILE_MAX_NAME_LENGTH];
	nlink_t 	 hard_link_count;
	/* Permissions */
	uid_t	 	 uid;
	gid_t	 	 gid;
	umode_t	 	 mode;
	/* Special file */
	dev_t	 	 if_dev;
	char 	 	 link_path[CONFIG_FILE_MAX_NAME_LENGTH];
	pipe_info_t 	*fifo;
	/* Mounting */
	inode_t	 	*mount;
	/* Synchronization */
	semaphore_t 	*lock;
	uint32_t 	 usage_count;
	/* Content */
	off_t	 	 size;	
	/* Access times */
	ktime_t		 atime;
	ktime_t		 mtime;
	ktime_t		 ctime;
};

struct dirent {
	char	 name[CONFIG_FILE_MAX_NAME_LENGTH];
	ino_t	 inode_id;
	uint32_t device_id;
	unsigned short int d_reclen;//2 + 2 + 4 = 8 -> this struct is long alligned
};

struct dir_cache {
	dir_cache_t	*parent;
	inode_t		*inode;
	uint32_t	 usage_count;
};

struct fs_device_operations {

	inode_t *	(*load_inode)	(fs_device_t *, ino_t);//inode id -> inode
	int		(*store_inode)	(inode_t *);//inode -> status
	int		(*mknod)	(inode_t *);	//inode -> status
	int		(*rmnod)	(inode_t *);	//inode -> status

	int		(*read_inode)	(inode_t *, void *, off_t, off_t);//buffer, f_offset, length -> numbytes
	int		(*write_inode)	(inode_t *, void *, off_t, off_t);//buffer, f_offset, length -> numbytes

	int		(*read_dir)	(inode_t *, void *, off_t, off_t);//buffer, f_offset, length -> numbytes
	dirent_t *	(*find_dirent)	(inode_t *, char *);	//dir_inode_id, filename -> dirent_t  
	int		(*mkdir)	(inode_t *);		//dir_inode_id -> status
	int		(*link)		(inode_t *, char *, ino_t);	//dir_inode_id, filename, inode_id -> status
	int		(*unlink)	(inode_t *, char *);	//dir_inode_id, filename
};

struct fs_device {
	uint32_t		id;
	ino_t			root_inode_id;
	fs_device_operations_t *ops;
	semaphore_t	       *lock;	
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

inode_t *vfs_find_parent(char * path);

inode_t *vfs_find_inode(char * path);

inode_t *vfs_find_symlink(char * path);

dir_cache_t *vfs_dir_cache_mkroot(inode_t *root_inode);

void vfs_dir_cache_release(dir_cache_t *dirc);

dir_cache_t *vfs_find_dirc(char * path);

int vfs_initialize(fs_device_t * root_device);

int vfs_read(inode_t * inode , off_t file_offset, void * buffer, size_t count, size_t *read_size, int non_block);
int vfs_write(inode_t * inode , off_t file_offset, void * buffer, size_t count, size_t *read_size, int non_block);

int vfs_getdents(inode_t * inode , off_t file_offset, dirent_t * buffer, size_t count, size_t *read_size);

#endif

