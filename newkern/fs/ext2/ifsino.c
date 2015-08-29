/**
 * fs/ext2/ifsino.c
 * 
 * Implements IFS interface to inodes
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 29-08-2015 - Created
 */

#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <sys/errno.h>
#include <sys/types.h>

#include "fs/ext2/fsapi.h"
#include "kernel/heapmm.h"
#include "kernel/vfs.h"

void ext2_e2tovfs_inode(ext2_device_t *device, ext2_vinode_t *_ino, ino_t ino_id)
{
	ext2_inode_t *ino;
	inode_t *vfs_ino;

	ino = &(_ino->inode);
	vfs_ino = &(_ino->vfs_ino);
	vfs_ino->node.next = NULL;
	vfs_ino->node.prev = NULL;
	vfs_ino->device_id = device->device.id;
	vfs_ino->id = ino_id;
	vfs_ino->device = (fs_device_t *) device;
	vfs_ino->mount = NULL;
	vfs_ino->lock = semaphore_alloc();
	semaphore_up(vfs_ino->lock);
	
	vfs_ino->hard_link_count = (nlink_t) ino->link_count;
	vfs_ino->uid = (uid_t) ino->uid;
	vfs_ino->gid = (gid_t) ino->gid;

	vfs_ino->mode = (umode_t) (ino->mode & 0xFFF);

	switch (EXT2_MODE_FMT(ino->mode)) {
		case EXT2_IFBLK:
			vfs_ino->mode |= S_IFBLK;
			vfs_ino->if_dev = EXT2_DEV_DECODE(ino->block[0]);
			break;
		case EXT2_IFCHR:
			vfs_ino->mode |= S_IFCHR;
			vfs_ino->if_dev = EXT2_DEV_DECODE(ino->block[0]);
			break;
		case EXT2_IFDIR:
			vfs_ino->mode |= S_IFDIR;
			break;
		case EXT2_IFSOCK:
			vfs_ino->mode |= S_IFSOCK;
			break;
		case EXT2_IFIFO:
			vfs_ino->mode |= S_IFIFO;
			break;
		case EXT2_IFREG:
			vfs_ino->mode |= S_IFREG;
			break;
		case EXT2_IFLNK:
			vfs_ino->mode |= S_IFLNK;
			break;
		default:
			vfs_ino->mode |= S_IFREG;
			debugcon_printf("ext2: inode %i has invalid mode: %i", (uint32_t)ino_id, (uint32_t)ino->mode);
			break;
	}
	vfs_ino->usage_count = 0;
	vfs_ino->size = ino->size;
	vfs_ino->atime = (ktime_t) ino->atime;
	vfs_ino->ctime = (ktime_t) ino->ctime;
	vfs_ino->mtime = (ktime_t) ino->mtime;
}

void ext2_vfstoe2_inode(ext2_vinode_t *_ino, ino_t ino_id)
{
	
	ext2_inode_t *ino;
	inode_t *vfs_ino;

	ino = &(_ino->inode);
	vfs_ino = &(_ino->vfs_ino);
	//semaphore_down(vfs_ino->lock);
	
	ino->link_count = vfs_ino->hard_link_count;
	ino->uid = vfs_ino->uid;
	ino->gid = vfs_ino->gid;

	ino->mode = vfs_ino->mode & 0xFFF;

	switch (vfs_ino->mode & S_IFMT) {
		case S_IFBLK:
			ino->mode |= EXT2_IFBLK;
			ino->block[0] = EXT2_DEV_ENCODE(vfs_ino->if_dev);
			break;
		case S_IFCHR:
			ino->mode |= EXT2_IFCHR;
			ino->block[0] = EXT2_DEV_ENCODE(vfs_ino->if_dev);
			break;
		case S_IFDIR:
			ino->mode |= EXT2_IFDIR;
			break;
		case S_IFSOCK:
			ino->mode |= EXT2_IFSOCK;
			break;
		case S_IFIFO:
			ino->mode |= EXT2_IFIFO;
			break;
		case S_IFREG:
			ino->mode |= EXT2_IFREG;
			break;
		case S_IFLNK:
			ino->mode |= EXT2_IFLNK;
			break;
		default:
			ino->mode |= S_IFREG;
			debugcon_printf("ext2: vfs inode %i has invalid mode: %i", (uint32_t)ino_id, (uint32_t)ino->mode);
			break;
	}

	ino->size = vfs_ino->size;
	ino->atime = vfs_ino->atime;
	ino->ctime = vfs_ino->ctime;
	ino->mtime = vfs_ino->mtime;

	//semaphore_up(vfs_ino->lock);
}



SFUNC(inode_t *, ext2_load_inode, fs_device_t *device, ino_t id) {
	ext2_vinode_t *ino;
	ext2_device_t *_dev = (ext2_device_t *) device;
	errno_t status;

	if (!_dev)
		THROW(EFAULT, NULL);

	ino = heapmm_alloc(sizeof(ext2_vinode_t));
	if (!ino)
		THROW(ENOMEM, NULL);

	status = ext2_load_e2inode(_dev, &(ino->inode), id);
	if (status)
		THROW(status, NULL);

	ext2_e2tovfs_inode(_dev, ino, id);

	RETURN((inode_t *) ino);
}

SVFUNC(ext2_store_inode,inode_t *_inode) {
	ext2_device_t *device;
	ext2_vinode_t *inode;

	if (!_inode) {	
		THROWV(EFAULT);
	}

	device = (ext2_device_t *) _inode->device;
	inode = (ext2_vinode_t *) _inode;

	ext2_vfstoe2_inode(inode, _inode->id);

	//debugcon_printf("ext2: storing inode %i\n", _inode->id);

	CHAINRETV(ext2_store_e2inode, device, &(inode->inode), _inode->id);
}



