/**
 * fs/ext2.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-07-2014 - Created
 */

#include <stdint.h>
#include <string.h>

#include <sys/errno.h>
#include <sys/types.h>

#include "fs/ext2.h"
#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/vfs.h"
#include "kernel/earlycon.h"

int ext2_read_blocks(ext2_device_t *dev, uint32_t block_ptr, void *buffer, size_t count, size_t *read_size)
{
	off_t	block_addr;	
	size_t	_read_size;
	size_t *rs = read_size ? read_size : (&_read_size);
	if (!dev)
		return EFAULT;
	block_addr = block_ptr << (10 + dev->superblock.block_size_enc);
	return device_block_read(dev->dev_id, block_addr, buffer, count, rs);	
}

ext2_block_group_desc_t *ext2_load_bgd(ext2_device_t *device, uint32_t bg_id)
{
	//TODO: Cache BGDs
	ext2_block_group_desc_t *bgd;
	off_t bgd_addr;	
	size_t read_size;
	int status;

	if (!device)
		return NULL;

	bgd = heapmm_alloc(sizeof(ext2_block_group_desc_t));
	if (!bgd)
		return NULL;

	bgd_addr = (device->bgdt_block << (10 + device->superblock.block_size_enc)) + 
			bg_id * sizeof(ext2_block_group_desc_t);
	
	status = device_block_read(device->dev_id, bgd_addr, bgd, sizeof(ext2_block_group_desc_t), &read_size);
	
	if (status || (read_size != sizeof(ext2_block_group_desc_t))) {
		heapmm_free(bgd, sizeof(ext2_block_group_desc_t));
		return NULL;
	}

	return bgd;
}

void ext2_free_bgd(ext2_device_t *device, ext2_block_group_desc_t *bgd)
{
	heapmm_free(bgd, sizeof(ext2_block_group_desc_t));
}

ext2_inode_t *ext2_load_e2inode(ext2_device_t *device, uint32_t ino_id)
{
	ext2_block_group_desc_t *bgd;
	ext2_inode_t		*ino;
	off_t			index, ino_offset;
	size_t read_size;
	int status;

	if (!device)
		return NULL;

	ino = heapmm_alloc(device->superblock.inode_size);//TODO: Allocate at least sizeof(ext2_inode_t)
	if (!ino)
		return NULL;

	bgd = ext2_load_bgd(device, (ino_id - 1) / device->superblock.inodes_per_group);
	if (!bgd) {
		heapmm_free(ino, device->superblock.inode_size);//TODO: Allocate at least sizeof(ext2_inode_t)
		return NULL;
	}	

	index = (ino_id - 1) % device->superblock.inodes_per_group;
	ino_offset = (bgd->inode_table << (10 + device->superblock.block_size_enc)) + 
			index * device->superblock.inode_size;

	status = device_block_read(device->dev_id, 
				   ino_offset, 
				   ino,
				   device->superblock.inode_size, 
				   &read_size);
	
	if (status || (read_size != device->superblock.inode_size)) {
		heapmm_free(ino, device->superblock.inode_size);//TODO: Allocate at least sizeof(ext2_inode_t)
		ext2_free_bgd(device, bgd);
		return NULL;
	}
	
	ext2_free_bgd(device, bgd);
	return ino;
}

void ext2_free_e2inode(ext2_device_t *device, ext2_inode_t *ino)
{
	heapmm_free(ino, sizeof(ext2_inode_t));
}

inode_t *ext2_e2tovfs_inode(ext2_device_t *device, ext2_inode_t *ino, ino_t ino_id)
{
	inode_t *vfs_ino;
	if (!ino)
		return NULL;

	vfs_ino = heapmm_alloc(sizeof(inode_t));
	if (!vfs_ino)
		return NULL;

	vfs_ino->device_id = device->device.id;
	vfs_ino->id = ino_id;
	vfs_ino->device = (fs_device_t *) device;
	strcpy(vfs_ino->name, "ext2inode");
	vfs_ino->mount = NULL;
	vfs_ino->lock = semaphore_alloc();
	semaphore_up(vfs_ino->lock);
	
	vfs_ino->hard_link_count = (nlink_t) ino->link_count;
	vfs_ino->uid = (uid_t) ino->uid;
	vfs_ino->gid = (gid_t) ino->gid;

	vfs_ino->mode = (umode_t) (ino->mode & 0xFFF);

	vfs_ino->link_path[0] = 0;
	switch (EXT2_MODE_FMT(ino->mode)) {
		case EXT2_IFBLK:
			vfs_ino->mode |= S_IFBLK;
			vfs_ino->if_dev = (dev_t) ino->block[0];
			break;
		case EXT2_IFCHR:
			vfs_ino->mode |= S_IFCHR;
			vfs_ino->if_dev = (dev_t) ino->block[0];
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
			vfs_ino->mode |= S_IFREG;
			//TODO: Read link path
			break;
		default:
			vfs_ino->mode |= S_IFREG;
			debugcon_printf("ext2: inode %i has invalid mode: %i", (uint32_t)ino_id, (uint32_t)ino->mode);
			break;
	}
	vfs_ino->usage_count = 0;
	vfs_ino->size = ino->blocks * 512;
	vfs_ino->atime = (ktime_t) ino->atime;
	vfs_ino->ctime = (ktime_t) ino->ctime;
	vfs_ino->mtime = (ktime_t) ino->mtime;
	return vfs_ino;
}

inode_t *ext2_load_inode(fs_device_t *device, ino_t id) {
	inode_t *vfs_ino;
	ext2_inode_t *ino;
	ext2_device_t *_dev = (ext2_device_t *) device;

	if (!_dev)
		return NULL;

	ino = ext2_load_e2inode(_dev, id);
	if(!ino)
		return NULL;

	vfs_ino = ext2_e2tovfs_inode(_dev, ino, id);

	ext2_free_e2inode(_dev, ino);
	return vfs_ino;
}

fs_device_operations_t ext2_ops = {
	&ext2_load_inode,
	NULL,//Store inode
	NULL,//Make inode
	NULL,//Remove inode
	NULL,//Read from file
	NULL,//Write to file
	NULL,//Read from directory
	NULL,//Find directory entry
	NULL,//Make directory
	NULL,//Make directory entry
	NULL //Remove directory entry
};

fs_device_t *ext2_mount(dev_t device, uint32_t flags)
{
	int status;
	size_t	_read_size;

	ext2_inode_t *root_ino;
	ext2_device_t *dev = heapmm_alloc(sizeof(ext2_device_t));
	
	if (!dev)
		return NULL;

	dev->dev_id = device;	
	dev->device.id = device;
	dev->device.root_inode_id = EXT2_ROOT_INODE;
	dev->device.ops = &ext2_ops;
	dev->device.lock = semaphore_alloc();
	if(!dev->device.lock) {
		heapmm_free(dev, sizeof(ext2_device_t));
		return 0;
	}
	semaphore_up(dev->device.lock);
	dev->device.inode_size = sizeof(inode_t);

	status = device_block_read(dev->dev_id, 1024, &(dev->superblock), 1024, &_read_size);

	//TODO: Implement fallback to alternative superblock

	if (_read_size != 1024) {
		debugcon_printf("ext2: could not read superblock, error:%i, read:%i!\n", status, _read_size);
		heapmm_free(dev, sizeof(ext2_device_t));
		return 0;
	}

	if (dev->superblock.signature != 0xEF53) {
		debugcon_printf("ext2: superblock signature incorrect: %i!\n", dev->superblock.signature);
		heapmm_free(dev, sizeof(ext2_device_t));
		return 0;
	}	

	if (dev->superblock.version_major == EXT2_VERSION_MAJOR_DYNAMIC) {

		if (dev->superblock.required_features & ~(EXT2_SUPPORTED_REQ_FEATURES)) {
			debugcon_printf("ext2: filesystem requires unsupported features, refusing to mount!\n");
			heapmm_free(dev, sizeof(ext2_device_t));
			return 0;
		}

		if (dev->superblock.ro_force_features & ~(EXT2_SUPPORTED_ROF_FEATURES)) {
			debugcon_printf("ext2: filesystem requires unsupported features, mounting read-only!\n");
			flags |= EXT2_MOUNT_FLAG_RO;
		}

		debugcon_printf("ext2: mounting %s\n", dev->superblock.volume_name);

	} else {
		dev->superblock.first_inode = EXT2_NO_EXT_FIRST_INO;
		dev->superblock.inode_size = EXT2_NO_EXT_INODE_SIZE;		
	}

	dev->bgdt_block = dev->superblock.block_size_enc ? 1 : 2;

	root_ino = ext2_load_e2inode(dev, EXT2_ROOT_INODE);

	if (!root_ino) {
		debugcon_printf("ext2: unable to read root inode!\n");
		heapmm_free(dev, sizeof(ext2_device_t));
		return 0;
	}

	ext2_free_e2inode(dev, root_ino);

	debugcon_printf("ext2: root inode ctime: %i\n", root_ino->ctime);

	return (fs_device_t *) dev;
}
