/**
 * fs/ext2/blkmgr.c
 * 
 * Implements functions for allocating and freeing blocks
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


SVFUNC(ext2_free_block, ext2_device_t *device, uint32_t block_id)
{
	uint32_t b_count = device->superblock.block_count;
	uint32_t bgrp_id = 0;
	uint32_t bgrp_bcnt = device->superblock.blocks_per_group;
	uint32_t bm_block_size = 256 << device->superblock.block_size_enc;
	uint32_t bm_block_bcnt = bm_block_size * 32;
	uint32_t bm_id;
	uint32_t block_map[bm_block_size];
	uint32_t first_b = device->superblock.block_size_enc ? 0 : 1;
	uint32_t idx ,nb;
	aoff_t rsize;
	int status;
	ext2_block_group_desc_t *bgd;

	assert ( device != NULL );
	assert (block_id < b_count);
	assert (block_id > first_b);

	bgrp_id = (block_id - first_b) / bgrp_bcnt;
	idx = block_id - first_b - bgrp_id * bgrp_bcnt;
	bm_id = idx / bm_block_bcnt;
	idx -= bm_id * bm_block_bcnt;
	nb = idx % 32;
	idx -= nb;
	idx /= 32;
	
	status = ext2_load_bgd(device, bgrp_id, &bgd);
	if (status)
		THROWV(status);

	status = ext2_read_block(device, bgd->block_bitmap + bm_id, 0, block_map, bm_block_size * 4, &rsize);
	if (status)
		THROWV(status);

	EXT2_BITMAP_CLR(block_map[idx], nb);

	status = ext2_write_block(device, bgd->block_bitmap + bm_id, 0, block_map, bm_block_size * 4, &rsize);
	if (status) {
		debugcon_printf("ext2: MAYDAY MAYDAY MAYDAY: write error (block bitmap), FILESYSTEM IS PROBABLY CORRUPTED NOW!");
		ext2_handle_error(device);
		THROWV(status);
	}

	bgd->free_block_count++;

	status = ext2_store_bgd(device, bgrp_id, bgd);
	if (status) {
		debugcon_printf("ext2: MAYDAY MAYDAY MAYDAY: write error (BGD), FILESYSTEM IS PROBABLY CORRUPTED NOW!");
		ext2_handle_error(device);
		THROWV(status);
	}
	ext2_free_bgd(device, bgd);

	device->superblock.free_block_count++;

	RETURNV;
}

SFUNC(uint32_t, ext2_alloc_block, ext2_device_t *device, uint32_t start)
{
	uint32_t b_count = device->superblock.block_count;
	uint32_t block_id = start;
	uint32_t bgrp_id = 0;
	uint32_t bgrp_bcnt = device->superblock.blocks_per_group;
	uint32_t bgrp_count = ext2_divup(b_count,bgrp_bcnt);//DIVUP
	uint32_t bm_block_size = 256 << device->superblock.block_size_enc;
	uint32_t bm_block_bcnt = bm_block_size * 32;
	uint32_t bgrp_bmcnt = ext2_divup(bgrp_bcnt,bm_block_bcnt);//DIVUP
	uint32_t bm_id;
	uint32_t block_map[bm_block_size];
	uint32_t first_b = device->superblock.block_size_enc ? 0 : 1;
	uint32_t idx ,nb;
	aoff_t rsize;
	int status;
	ext2_block_group_desc_t *bgd;

	assert ( device != NULL );
	assert (start < b_count);

	if (start == 0)
		block_id = first_b;

	bgrp_id = (block_id - first_b) / bgrp_bcnt;
	idx = block_id - first_b - bgrp_id * bgrp_bcnt;
	bm_id = idx / bm_block_bcnt;
	idx -= bm_id * bm_block_bcnt;
	nb = idx % 32;
	idx -= nb;
	idx /= 32;
	for (; bgrp_id < bgrp_count; bgrp_id++) {
		status = ext2_load_bgd(device, bgrp_id, &bgd);
		if (status)
			THROW(status, 0);

		if (bgd->free_block_count) {
			for (; bm_id < bgrp_bmcnt; bm_id++) {
				status = ext2_read_block(device, bgd->block_bitmap + bm_id, 0, block_map, bm_block_size * 4, &rsize);
				if (status)
					THROW(status, 0);
				for (; idx < bm_block_size; idx++) {
					if (block_map[idx] != 0xFFFFFFFF) {					
						for (; nb < 32; nb++)
							if (!EXT2_BITMAP_GET(block_map[idx], nb))
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
	for (; bgrp_id < bgrp_count; bgrp_id++) {
		status = ext2_load_bgd(device, bgrp_id, &bgd);
		if (status)
			THROW(status, 0);

		if (bgd->free_block_count) {
			for (; bm_id < bgrp_bmcnt; bm_id++) {
				status = ext2_read_block(device, bgd->block_bitmap + bm_id, 0, block_map, bm_block_size * 4, &rsize);
				if (status)
					THROW(status, 0);
				for (; idx < bm_block_size; idx++) {
					if (block_map[idx] != 0xFFFFFFFF) {					
						for (; nb < 32; nb++)
							if (!EXT2_BITMAP_GET(block_map[idx], nb))
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

	THROW(ENOSPC, 0);
found_it:

	block_id = nb + idx * 32 + bm_id * bm_block_bcnt + bgrp_id * bgrp_bcnt + first_b;

	EXT2_BITMAP_SET(block_map[idx], nb);
	status = ext2_write_block(device, bgd->block_bitmap + bm_id, 0, block_map, bm_block_size * 4, &rsize);
	if (status) {
		debugcon_printf("ext2: MAYDAY MAYDAY MAYDAY: write error (block bitmap), FILESYSTEM IS PROBABLY CORRUPTED NOW!");
		ext2_handle_error(device);
		THROW(status, 0);
	}

	bgd->free_block_count--;

	status = ext2_store_bgd(device, bgrp_id, bgd);
	if (status) {
		debugcon_printf("ext2: MAYDAY MAYDAY MAYDAY: write error (BGD), FILESYSTEM IS PROBABLY CORRUPTED NOW!");
		ext2_handle_error(device);
		THROW(status, 0);
	}
	ext2_free_bgd(device, bgd);

	device->superblock.free_block_count--;
	
	memset(block_map, 0, bm_block_size * 4);

	status = ext2_write_block(device, block_id, 0, block_map, bm_block_size * 4, &rsize);
	if (status) {
		debugcon_printf("ext2: MAYDAY MAYDAY MAYDAY: write error (block clear)!");
		ext2_handle_error(device);
		THROW(status, 0);
	}

	RETURN(block_id);
}
