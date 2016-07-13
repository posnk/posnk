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
#include "kernel/earlycon.h"

SVFUNC(ext2_shrink_inode, ext2_device_t *device, ext2_inode_t *inode, aoff_t old_size, aoff_t new_size)
{
	aoff_t count;
	aoff_t length = old_size - new_size;
	aoff_t block_size;
	aoff_t in_blk;
	aoff_t in_blk_size;
	aoff_t in_file;
	uint32_t block_addr;	
	int status;

	block_size = 1024 << device->superblock.block_size_enc;

	for (count = 0; count < length; count += in_blk_size) {
		in_file = count + new_size;
		in_blk = in_file % block_size;				
		in_blk_size = length - count;

		if (in_blk_size > (block_size - in_blk))
			in_blk_size = block_size - in_blk;
	
		if (in_blk_size != block_size)
			continue;
	
		status = ext2_decode_block_id (device, inode, in_file / block_size, &block_addr);

		if (status)
			THROWV(status);

		if (block_addr) {
			debugcon_printf("ext2: shrinking file!\n");
			status = ext2_free_block(device, block_addr);
			if (status) 
				THROWV(status);

			status = ext2_set_block_id(device, inode, in_file / block_size, 0);

			if (status)
				THROWV(status);

			inode->blocks -= 2 << device->superblock.block_size_enc;;
		}
	}

	//TODO: Dealloc indirect blocks

	RETURNV;
}

	
SVFUNC(ext2_trunc_inode, inode_t *_inode, aoff_t size)//buffer, f_offset, length -> numbytes
{
	ext2_device_t *device;
	ext2_vinode_t *inode;

	if (!_inode) {	
		THROWV(EFAULT);
	}

	device = (ext2_device_t *) _inode->device;
	inode = (ext2_vinode_t *) _inode;

	if (size > _inode->size) {
		//TODO: Implement explicit file growth
	} else if (size < _inode->size) {
		CHAINRETV(ext2_shrink_inode, device, &(inode->inode), _inode->size, size);
	}
	RETURNV;	
}

SFUNC(aoff_t, ext2_write_inode, inode_t *_inode, void *_buffer, aoff_t f_offset, aoff_t length)
{
	ext2_device_t *device;
	ext2_vinode_t *inode;
	aoff_t count;
	aoff_t block_size;
	aoff_t in_blk_size;
	aoff_t rsize;
	aoff_t in_blk;
	aoff_t in_file;
	uint32_t block_addr, p_block_addr;	
	uint8_t *buffer = _buffer;
	int status;

	assert( _inode != NULL );
	assert( buffer != NULL );

	device = (ext2_device_t *) _inode->device;
	inode = (ext2_vinode_t *) _inode;

	block_size = 1024 << device->superblock.block_size_enc;
	
	p_block_addr = 0;

	for (count = 0; count < length; count += in_blk_size) {
		in_file = count + f_offset;
		in_blk = in_file % block_size;				
		in_blk_size = length - count;

		if (in_blk_size > (block_size - in_blk))
			in_blk_size = block_size - in_blk;

		status = ext2_decode_block_id (device, &(inode->inode), in_file / block_size, &block_addr);
		if (status)
			THROW(status, 0);

		if (!block_addr) {
			//debugcon_printf("ext2: growing file!\n");
			status = ext2_alloc_block(device, p_block_addr, &block_addr);
			if (status) 
				THROW(status, 0);

			status = ext2_set_block_id(device, &(inode->inode), in_file / block_size, block_addr);
			if (status)
				THROW(status, 0);

			inode->inode.blocks += 2 << device->superblock.block_size_enc;
		}

		status = ext2_write_block(device, block_addr, in_blk, &(buffer[count]), in_blk_size, &rsize);

		if (status)
			THROW(status, 0);

		p_block_addr = block_addr;
	}
	
	RETURN(count);
}

SFUNC(aoff_t, ext2_read_inode, inode_t *_inode, void *_buffer, aoff_t f_offset, aoff_t length)
{
	ext2_device_t *device;
	ext2_vinode_t *inode;
	aoff_t count;
	aoff_t block_size;
	aoff_t in_blk_size;
	aoff_t in_blk;
	aoff_t in_file;
	aoff_t rsize;
	uint32_t block_addr;	
	uint8_t *buffer = _buffer;
	int status;

	assert( _inode != NULL );
	assert( buffer != NULL );

	device = (ext2_device_t *) _inode->device;
	inode = (ext2_vinode_t *) _inode;

	if (f_offset > _inode->size)
		return EINVAL;

	if ((length + f_offset) > _inode->size)
		length = _inode->size - f_offset;

	block_size = 1024 << device->superblock.block_size_enc;

	for (count = 0; count < length; count += in_blk_size) {
		in_file = count + f_offset;
		in_blk = in_file % block_size;				
		in_blk_size = length - count;

		if (in_blk_size > (block_size - in_blk))
			in_blk_size = block_size - in_blk;

		status = ext2_decode_block_id (device, &(inode->inode), in_file / block_size, &block_addr);
		if (status)
			THROW(status, 0);

		if (block_addr) {
			status = ext2_read_block(device, block_addr, in_blk, &(buffer[count]), in_blk_size, &rsize);
		
			if (status)
				THROW(status, 0);
		} else {
			memset(&(buffer[count]), 0, in_blk_size);
		}
	}
	
	RETURN(count);
}

SVFUNC(ext2_mknod, inode_t *_inode) {
	ext2_device_t *device;
	ext2_vinode_t *inode;
	int status;


	if (!_inode) {	
		THROWV(EFAULT);
	}

	device = (ext2_device_t *) _inode->device;
	inode = (ext2_vinode_t *) _inode;

	status = ext2_alloc_inode(device, (uint32_t *) &(_inode->id));
	if (status)
		THROWV(status);

	memset(&(inode->inode), 0, sizeof(ext2_inode_t));

	ext2_vfstoe2_inode(inode, _inode->id);

	CHAINRETV(ext2_store_e2inode,device, &(inode->inode), _inode->id);
}
