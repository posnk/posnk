/**
 * fs/ext2/bgd.c
 * 
 * Implements functions for manipulating block groups
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
#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/vfs.h"

SFUNC(ext2_block_group_desc_t *, ext2_load_bgd, 
									ext2_device_t *device, 
									uint32_t bg_id)
{
	//TODO: Cache BGDs
	ext2_block_group_desc_t *bgd;
	off_t bgd_addr;	
	aoff_t read_size;
	int status;

	assert ( device != NULL );

	bgd = heapmm_alloc(sizeof(ext2_block_group_desc_t));
	if (!bgd)
		THROW ( ENOMEM, 0 );

	bgd_addr = (device->bgdt_block << (10 + device->superblock.block_size_enc)) + 
			bg_id * sizeof(ext2_block_group_desc_t);
	
	status = device_block_read(device->dev_id, bgd_addr, bgd, sizeof(ext2_block_group_desc_t), &read_size);
	
	if (status || (read_size != sizeof(ext2_block_group_desc_t))) {
		heapmm_free(bgd, sizeof(ext2_block_group_desc_t));
		THROW(status, NULL);
	}

	RETURN(bgd);
}

SVFUNC(ext2_store_bgd, 
									ext2_device_t *device, 
									uint32_t bg_id, 
									ext2_block_group_desc_t *bgd)
{
	//TODO: Cache BGDs
	off_t bgd_addr;	
	aoff_t read_size;
	int status;

	assert ( device != NULL );

	bgd_addr = (device->bgdt_block << (10 + device->superblock.block_size_enc)) + 
			bg_id * sizeof(ext2_block_group_desc_t);
	
	status = device_block_write(device->dev_id, bgd_addr, bgd, sizeof(ext2_block_group_desc_t), &read_size);
	
	if (status || (read_size != sizeof(ext2_block_group_desc_t))) {
		THROWV(status);
	}

	RETURNV;
}

void ext2_free_bgd(	__attribute__((__unused__)) ext2_device_t *device, 
					ext2_block_group_desc_t *bgd)
{
	heapmm_free(bgd, sizeof(ext2_block_group_desc_t));
}
