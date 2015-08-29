/**
 * fs/ext2/inoio.c
 * 
 * Implements inode access functions for the ext2 filesystem
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

#include <sys/errno.h>
#include <sys/types.h>

#include "fs/ext2/fsapi.h"
#include "kernel/device.h"
#include "kernel/vfs.h"



SVFUNC(ext2_load_e2inode, ext2_device_t *device, ext2_inode_t *ino, uint32_t ino_id)
{
	ext2_block_group_desc_t *bgd;
	off_t			index, ino_offset;
	size_t read_size;
	int status;

	assert( device != NULL );
	assert( ino != NULL );

	status = ext2_load_bgd(device, (ino_id - 1) / device->superblock.inodes_per_group, &bgd);
	if (status) {
		THROWV(status);
	}	

	index = (ino_id - 1) % device->superblock.inodes_per_group;
	ino_offset = (bgd->inode_table << (10 + device->superblock.block_size_enc)) + 
			index * device->superblock.inode_size;

	status = device_block_read(device->dev_id, 
				   ino_offset, 
				   ino,
				   device->inode_load_size, 
				   &read_size);
	
	if (status) {
		ext2_free_bgd(device, bgd);
		THROWV(status);
	}
	if (read_size != device->inode_load_size) {
		ext2_free_bgd(device, bgd);
		THROWV(EIO);//TODO: Better error handling
	}
	
	ext2_free_bgd(device, bgd);
	RETURNV;
}

SVFUNC(ext2_store_e2inode, ext2_device_t *device, ext2_inode_t *ino, uint32_t ino_id)
{
	ext2_block_group_desc_t *bgd;
	off_t			index, ino_offset;
	size_t read_size;
	int status;

	assert( device != NULL );
	assert( ino != NULL );

	status = ext2_load_bgd(device, (ino_id - 1) / device->superblock.inodes_per_group, &bgd);
	if (!bgd) {
		THROWV(status);
	}	

	index = (ino_id - 1) % device->superblock.inodes_per_group;
	ino_offset = (bgd->inode_table << (10 + device->superblock.block_size_enc)) + 
			index * device->superblock.inode_size;

	status = device_block_write(device->dev_id, 
				   ino_offset, 
				   ino,
				   device->inode_load_size, 
				   &read_size);
	
	if (status){
		ext2_free_bgd(device, bgd);
		THROWV(status);
	}
	if (read_size != device->inode_load_size) {
		ext2_free_bgd(device, bgd);
		THROWV(EIO);//TODO: Better error handling
	}
	
	ext2_free_bgd(device, bgd);
	RETURNV;
}
