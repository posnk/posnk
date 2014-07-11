/**
 * fs/ramfs.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 18-04-2014 - Created
 */

#include "kernel/vfs.h"

#ifndef __FS_RAMFS_H__
#define __FS_RAMFS_H__

typedef struct ramfs_inode ramfs_inode_t;

typedef struct ramfs_block ramfs_block_t;

typedef struct ramfs_device ramfs_device_t;

typedef struct ramfs_dirent ramfs_dirent_t;

struct ramfs_inode {
	inode_t inode;
	llist_t	*block_list;
	llist_t *dirent_list;
};

struct ramfs_block {	
	llist_t  node;
	inode_t  *inode;
	aoff_t	 start;
	aoff_t	 length;
	void	*data;
};

struct ramfs_device {
	fs_device_t device;
	ino_t	  inode_id_ctr;
};

struct ramfs_dirent {
	llist_t  node;
	dirent_t dir;
	aoff_t	 start;
};

fs_device_t *ramfs_create();

#endif

