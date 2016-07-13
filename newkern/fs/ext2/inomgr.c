/**
 * fs/ext2/blkmgr.c
 * 
 * Implements functions for allocating and freeing inodes
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


SFUNC(uint32_t, ext2_alloc_inode, ext2_device_t *device)
{
	uint32_t i_count = device->superblock.inode_count;
	uint32_t inode_id = 11;
	uint32_t bgrp_id = 0;
	uint32_t bgrp_icnt = device->superblock.inodes_per_group;
	uint32_t bgrp_count = ext2_divup(i_count,bgrp_icnt);//DIVUP
	uint32_t bm_block_size = 256 << device->superblock.block_size_enc;
	uint32_t bm_block_icnt = bm_block_size * 32;
	uint32_t bgrp_bmcnt = ext2_divup(bgrp_icnt,bm_block_icnt);//DIVUP
	uint32_t bm_id;
	uint32_t inode_map[bm_block_size];
	uint32_t first_i = 1;
	uint32_t idx ,nb;
	aoff_t rsize;

	int status;

	ext2_block_group_desc_t *bgd;

	assert ( device != NULL );

	bgrp_id = (inode_id - first_i) / bgrp_icnt;
	idx = inode_id - first_i - bgrp_id * bgrp_icnt;
	bm_id = idx / bm_block_icnt;
	idx -= bm_id * bm_block_icnt;
	nb = idx % 32;
	idx -= nb;
	idx /= 32;

	for (; bgrp_id < bgrp_count; bgrp_id++) {
		status = ext2_load_bgd(device, bgrp_id, &bgd);
		if (status)
			THROW(status, 0);

		if (bgd->free_inode_count) {
			for (; bm_id < bgrp_bmcnt; bm_id++) {
				status = ext2_read_block(device, bgd->inode_bitmap + bm_id, 0, inode_map, bm_block_size * 4, &rsize);
				if (status)
					THROW(status, 0);
				for (; idx < bm_block_size; idx++) {
					if (inode_map[idx] != 0xFFFFFFFF) {					
						for (; nb < 32; nb++)
							if (!EXT2_BITMAP_GET(inode_map[idx], nb))
								goto found_it;
					}
					nb = 0;	
				}
				idx = 0;
			}
		}
		bm_id = 0;		
		ext2_free_bgd(device, bgd);
	}
	bgrp_id = 0;

	THROW(ENOSPC, 0);
found_it:

	inode_id = nb + idx * 32 + bm_id * bm_block_icnt + bgrp_id * bgrp_icnt + first_i;

	EXT2_BITMAP_SET(inode_map[idx], nb);
	status = ext2_write_block(device, bgd->inode_bitmap + bm_id, 0, inode_map, bm_block_size * 4, &rsize);
	if (status) {
		debugcon_printf("ext2: MAYDAY MAYDAY MAYDAY: write error (inode bitmap), FILESYSTEM IS PROBABLY CORRUPTED NOW!");
		ext2_handle_error(device);
		THROW(status, 0);
	}

	bgd->free_inode_count--;

	status = ext2_store_bgd(device, bgrp_id, bgd);
	if (status) {
		debugcon_printf("ext2: MAYDAY MAYDAY MAYDAY: write error (BGD), FILESYSTEM IS PROBABLY CORRUPTED NOW!");
		ext2_handle_error(device);
		THROW(status, 0);
	}
	ext2_free_bgd(device, bgd);

	device->superblock.free_inode_count--;

	RETURN(inode_id);
}

SVFUNC( ext2_free_inode, ext2_device_t *device, uint32_t inode_id)
{
	uint32_t i_count = device->superblock.inode_count;
	uint32_t bgrp_id = 0;
	uint32_t bgrp_icnt = device->superblock.inodes_per_group;
	uint32_t bm_block_size = 256 << device->superblock.block_size_enc;
	uint32_t bm_block_icnt = bm_block_size * 32;
	uint32_t bm_id;
	uint32_t inode_map[bm_block_size];
	uint32_t first_i = 1;
	uint32_t idx ,nb;
	aoff_t rsize;

	int status;

	ext2_block_group_desc_t *bgd;

	assert ( device != NULL );

	assert (inode_id < i_count);
	assert (inode_id > first_i);

	bgrp_id = (inode_id - first_i) / bgrp_icnt;
	idx = inode_id - first_i - bgrp_id * bgrp_icnt;
	bm_id = idx / bm_block_icnt;
	idx -= bm_id * bm_block_icnt;
	nb = idx % 32;
	idx -= nb;
	idx /= 32;
	
	status = ext2_load_bgd(device, bgrp_id, &bgd);
	if (status)
		THROWV(status);

	status = ext2_read_block(device, bgd->inode_bitmap + bm_id, 0, inode_map, bm_block_size * 4, &rsize);
	if (status)
		THROWV(status);

	EXT2_BITMAP_CLR(inode_map[idx], nb);

	status = ext2_write_block(device, bgd->inode_bitmap + bm_id, 0, inode_map, bm_block_size * 4, &rsize);
	if (status) {
		debugcon_printf("ext2: MAYDAY MAYDAY MAYDAY: write error (inode bitmap), FILESYSTEM IS PROBABLY CORRUPTED NOW!");
		ext2_handle_error(device);
		THROWV(status);
	}

	bgd->free_inode_count++;

	status = ext2_store_bgd(device, bgrp_id, bgd);
	if (status) {
		debugcon_printf("ext2: MAYDAY MAYDAY MAYDAY: write error (BGD), FILESYSTEM IS PROBABLY CORRUPTED NOW!");
		ext2_handle_error(device);
		THROWV(status);
	}
	ext2_free_bgd(device, bgd);

	device->superblock.free_inode_count++;

	RETURNV;
}
