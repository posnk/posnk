/**
 * fs/ext2/blkio.c
 * 
 * Implements block device access functions for the ext2 filesystem
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

SFUNC(aoff_t, ext2_read_block, 
					ext2_device_t *dev, 
					uint32_t block_ptr, 
					uint32_t in_block, 
					void *buffer, 
					aoff_t count)
{
	aoff_t	block_addr;	
	
	assert( dev != NULL );
	assert( buffer != NULL );
	
	block_addr = (block_ptr << (10 + dev->superblock.block_size_enc)) + in_block; 
	
	CHAINRET(device_block_read, dev->dev_id, block_addr, buffer, count);	
}

SFUNC(aoff_t, ext2_write_block, 
					ext2_device_t *dev, 
					uint32_t block_ptr, 
					uint32_t in_block, 
					const void *buffer, 
					aoff_t count)
{
	aoff_t	block_addr;	
	
	assert( dev != NULL );
	assert( buffer != NULL );
	
	block_addr = (block_ptr << (10 + dev->superblock.block_size_enc)) + in_block; 
	
	CHAINRET(device_block_write, dev->dev_id, block_addr, buffer, count);	
}
