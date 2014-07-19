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

int ext2_read_block(ext2_device_t *dev, uint32_t block_ptr, uint32_t in_block, void *buffer, aoff_t count, aoff_t *read_size)
{
	int status;
	aoff_t	block_addr;	
	aoff_t	_read_size;
	size_t *rs = read_size ? read_size : (&_read_size);
	if (!dev)
		return EFAULT;
	block_addr = (block_ptr << (10 + dev->superblock.block_size_enc)) + in_block; 
	status = device_block_read(dev->dev_id, block_addr, buffer, count, rs);	
	if ((!status) && ((*rs) != count))
		return EIO;
	return status;
}

int ext2_write_block(ext2_device_t *dev, uint32_t block_ptr, uint32_t in_block, void *buffer, aoff_t count, aoff_t *write_size)
{
	int status;
	aoff_t	block_addr;	
	aoff_t	_write_size;
	aoff_t *ws = write_size ? write_size : (&_write_size);
	if (!dev)
		return EFAULT;
	block_addr = (block_ptr << (10 + dev->superblock.block_size_enc)) + in_block; 
	status = device_block_write(dev->dev_id, block_addr, buffer, count, ws);	
	if ((!status) && ((*ws) != count))
		return EIO;
	return status;
}

uint32_t ext2_decode_block_id(ext2_device_t *device, ext2_inode_t *inode, uint32_t block_id)
{
	uint32_t indirect_count = 256 << device->superblock.block_size_enc;
	uint32_t indirect_id = 0;
	int status, o, d;
	if (block_id > (12 + indirect_count * indirect_count + indirect_count)) { //Triply indirect
		status = ext2_read_block(device, 
					 inode->block[14],
				         ( (block_id - 12 - indirect_count * indirect_count - indirect_count) / (indirect_count * indirect_count) ) * 4,
					 &indirect_id,
					 4, NULL);
		if (status)
			return 0;
		
	} else {
		indirect_id = inode->block[13];
	}
	if (block_id > (11 + indirect_count)) { //Doubly indirect
		o = block_id - 12 - indirect_count;
		d = o / indirect_count;
		status = ext2_read_block(device, 
					 indirect_id,
				         (d % indirect_count) * 4,
					 &indirect_id,
					 4, NULL);

		if (status)
			return 0;

		status = ext2_read_block(device, 
					 indirect_id,
				         ( (o - d * indirect_count) % indirect_count) * 4,
					 &indirect_id,
					 4, NULL);
		if (status)
			return 0;

		return indirect_id;
	} else {
		indirect_id = inode->block[12];
	}
	if (block_id > 11) { //Singly Indirect
		status = ext2_read_block(device, 
					 indirect_id,
				         ( (block_id - 12) % indirect_count) * 4,
					 &indirect_id,
					 4, NULL);
		if (status)
			return 0;
		return indirect_id;
	}
	return inode->block[block_id];
}

int ext2_read_inode(inode_t *_inode, void *_buffer, aoff_t f_offset, aoff_t length, aoff_t *nread)
{
	ext2_device_t *device;
	ext2_vinode_t *inode;
	aoff_t count;
	aoff_t block_size;
	aoff_t in_blk_size;
	aoff_t in_blk;
	aoff_t in_file;
	uint32_t block_addr;	
	uint8_t *buffer = _buffer;
	int status;

	if (!_inode) {	
		*nread = 0;
		return EFAULT;
	}

	device = (ext2_device_t *) _inode->device;
	inode = (ext2_vinode_t *) _inode;

	block_size = 1024 << device->superblock.block_size_enc;

	for (count = 0; count < length; count += in_blk_size) {
		in_file = count + f_offset;
		in_blk = in_file % block_size;				
		in_blk_size = length - count;

		*nread = count;

		if (in_blk_size > block_size)
			in_blk_size = block_size;

		block_addr = ext2_decode_block_id (device, &(inode->inode), in_file / block_size);

		if (!block_addr)
			return EIO;

		status = ext2_read_block(device, block_addr, in_blk, &(buffer[count]), in_blk_size, NULL);

		if (status)
			return status;
	}
	
	*nread = count;

	return 0;
}

int ext2_readdir(inode_t *_inode, void *_buffer, aoff_t f_offset, aoff_t length, aoff_t *nread)
{
	int status;

	ext2_dirent_t *dirent;
	uint8_t *name;
	uint8_t *buffer= _buffer;

	dirent_t *vfs_dir;
	aoff_t pos, inode_nread;

	*nread = 0;

	if (!_inode) 
		return EFAULT;

	if (f_offset >= _inode->size)
		return EINVAL;

	if ((f_offset + length) > _inode->size)
		length = _inode->size - f_offset;

	dirent = heapmm_alloc(sizeof(ext2_dirent_t));
	
	if (!dirent)
		return ENOMEM;

	for (pos = 0; pos < length; pos += dirent->rec_len) {

		*nread = pos;

		status = ext2_read_inode(_inode, dirent, pos + f_offset, sizeof(ext2_dirent_t), &inode_nread);
		if (status || (inode_nread != sizeof(ext2_dirent_t))) {
			heapmm_free(dirent, sizeof(ext2_dirent_t));
			return status ? status : EIO;
		}

		if ((dirent->rec_len + pos) > length){
			heapmm_free(dirent, sizeof(ext2_dirent_t));
			return 0;
		}
	
		vfs_dir = (dirent_t *)&buffer[pos];

		name = (uint8_t *) vfs_dir->name;
		
		status = ext2_read_inode(_inode, name, pos + f_offset + sizeof(ext2_dirent_t), dirent->name_len, &inode_nread);

		if (status || (inode_nread != dirent->name_len)) {
			heapmm_free(dirent, sizeof(ext2_dirent_t));
			return status ? status : EIO;
		}

		name[dirent->name_len] = 0;
		vfs_dir->inode_id = dirent->inode;
		vfs_dir->device_id = _inode->device_id;
		vfs_dir->d_reclen = dirent->rec_len;
		
	}

	heapmm_free(dirent, sizeof(ext2_dirent_t));

	*nread = pos;

	return 0;
}

dirent_t *ext2_finddir(inode_t *_inode, char * name)
{
	uint8_t *buffer = heapmm_alloc(1024);
	int status;
	aoff_t nread = 1;
	aoff_t fpos = 0;
	aoff_t pos;
	dirent_t *dirent;
	dirent_t *pp;
	if (!buffer)
		return NULL;
	for (fpos = 0; nread != 0; fpos += nread) {
		status = ext2_readdir(_inode, buffer, fpos, 1024, &nread);
		if (status) {
			heapmm_free(buffer, 1024);
			return NULL;
		}
		for (pos = 0; pos < nread; pos += dirent->d_reclen) {
			dirent = (dirent_t *) &(buffer[pos]);
			if (strcmp(name, dirent->name) == 0){
				pp = heapmm_alloc(sizeof(dirent_t));	
				dirent->d_reclen = (dirent->d_reclen > sizeof(dirent_t)) ? sizeof(dirent_t) : dirent->d_reclen;
				memcpy(pp, dirent,dirent->d_reclen);
				heapmm_free(buffer, 1024);
				return pp;
			}
		}
	}
	heapmm_free(buffer, 1024);
	return NULL;
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

int ext2_load_e2inode(ext2_device_t *device, ext2_inode_t *ino, uint32_t ino_id)
{
	ext2_block_group_desc_t *bgd;
	off_t			index, ino_offset;
	size_t read_size;
	int status;

	if (!device)
		return 0;

	bgd = ext2_load_bgd(device, (ino_id - 1) / device->superblock.inodes_per_group);
	if (!bgd) {
		return 0;
	}	

	index = (ino_id - 1) % device->superblock.inodes_per_group;
	ino_offset = (bgd->inode_table << (10 + device->superblock.block_size_enc)) + 
			index * device->superblock.inode_size;

	status = device_block_read(device->dev_id, 
				   ino_offset, 
				   ino,
				   device->inode_load_size, 
				   &read_size);
	
	if (status || (read_size != device->inode_load_size)) {
		ext2_free_bgd(device, bgd);
		return 0;
	}
	
	ext2_free_bgd(device, bgd);
	return 1;
}

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
}

inode_t *ext2_load_inode(fs_device_t *device, ino_t id) {
	ext2_vinode_t *ino;
	ext2_device_t *_dev = (ext2_device_t *) device;

	if (!_dev)
		return NULL;

	ino = heapmm_alloc(sizeof(ext2_vinode_t));
	if (!ino)
		return NULL;

	if(!ext2_load_e2inode(_dev, &(ino->inode), id))
		return NULL;

	ext2_e2tovfs_inode(_dev, ino, id);

	return (inode_t *) ino;
}

fs_device_operations_t ext2_ops = {
	&ext2_load_inode,
	NULL,//Store inode
	NULL,//Make inode
	NULL,//Remove inode
	&ext2_read_inode,//Read from file
	NULL,//Write to file
	&ext2_readdir,//Read from directory
	&ext2_finddir,//Find directory entry
	NULL,//Make directory
	NULL,//Make directory entry
	NULL,//Remove directory entry
	NULL //Change file length
};

fs_device_t *ext2_mount(dev_t device, uint32_t flags)
{
	int status;
	aoff_t	_read_size;

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
	dev->device.inode_size = sizeof(ext2_vinode_t);

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

	dev->inode_load_size = (sizeof(ext2_inode_t) > dev->superblock.inode_size) ?
				dev->superblock.inode_size : sizeof(ext2_inode_t);

	dev->bgdt_block = dev->superblock.block_size_enc ? 1 : 2;

	return (fs_device_t *) dev;
}
