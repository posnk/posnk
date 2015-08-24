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
#include <assert.h>

#include <sys/errno.h>
#include <sys/types.h>

#include "fs/ext2.h"
#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/vfs.h"
#include "kernel/earlycon.h"
	

void *ext2_zero_buffer = NULL;

uint32_t ext2_divup(uint32_t a, uint32_t b)
{
	uint32_t result = a / b;
	if ( a % b ) { result++; }
	return result;
}

uint32_t ext2_roundup(uint32_t a, uint32_t b)
{//XXX: Works only with power-of-two values for B
	b--;
	return (a+b) & ~b;
}


void ext2_handle_error(ext2_device_t *device)
{
	assert(0/* EXT2 ERROR */);

}

SFUNC(aoff_t, ext2_read_block, ext2_device_t *dev, uint32_t block_ptr, uint32_t in_block, void *buffer, aoff_t count)
{
	aoff_t	block_addr;	
	assert( dev != NULL );
	assert( buffer != NULL );
	block_addr = (block_ptr << (10 + dev->superblock.block_size_enc)) + in_block; 
	CHAINRET(device_block_read, dev->dev_id, block_addr, buffer, count);	
}

SFUNC(aoff_t, ext2_write_block, ext2_device_t *dev, uint32_t block_ptr, uint32_t in_block, void *buffer, aoff_t count)
{
	aoff_t	block_addr;	
	assert( dev != NULL );
	assert( buffer != NULL );
	block_addr = (block_ptr << (10 + dev->superblock.block_size_enc)) + in_block; 
	CHAINRET(device_block_write, dev->dev_id, block_addr, buffer, count);	
}

SFUNC(ext2_block_group_desc_t *, ext2_load_bgd, ext2_device_t *device, uint32_t bg_id)
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

SVFUNC(ext2_store_bgd, ext2_device_t *device, uint32_t bg_id, ext2_block_group_desc_t *bgd)
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

void ext2_free_bgd(__attribute__((__unused__)) ext2_device_t *device, ext2_block_group_desc_t *bgd)
{
	heapmm_free(bgd, sizeof(ext2_block_group_desc_t));
}

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

SFUNC(uint32_t, ext2_allocate_indirect_block, ext2_device_t *device, ext2_inode_t *inode)
{
	int status;
	size_t block_size = 1024 << device->superblock.block_size_enc;
	aoff_t size;
	uint32_t id;

	status = ext2_alloc_block(device, 0, &id);//TODO: Prevent fragmentation, swap 0 for previous block in inode

	if (status)
		THROW(status, 0);

	inode->blocks += 2 << device->superblock.block_size_enc;

	if (!ext2_zero_buffer) {
		ext2_zero_buffer = heapmm_alloc(block_size);
		memset(ext2_zero_buffer, 0, block_size);
	}

	status = ext2_write_block(device, id, 0, ext2_zero_buffer, block_size, &size);
	if (status) {
		THROW(status, 0);
	}

	RETURN(id);
}

SFUNC(uint32_t, ext2_decode_block_id, ext2_device_t *device, ext2_inode_t *inode, uint32_t block_id)
{
	uint32_t indirect_count = 256 << device->superblock.block_size_enc;
	uint32_t indirect_id, indirect_off, indirect_rd;
	uint32_t s_indir_l = indirect_count;
	uint32_t d_indir_l = indirect_count * indirect_count;
	uint32_t s_indir_s = 12;
	uint32_t d_indir_s = s_indir_s + s_indir_l;
	uint32_t t_indir_s = d_indir_s + d_indir_l;
	int status;
	aoff_t rsize;	

	assert ( device != NULL );


	if (block_id >= t_indir_s) { //Triply indirect
		indirect_id = inode->block[14];
		indirect_off = (block_id - t_indir_s) / d_indir_l;

		if (!indirect_id)
			RETURN(0);

		status = ext2_read_block(device, indirect_id, indirect_off * 4, &indirect_rd, 4, &rsize);
		if (status || !indirect_rd) {
			THROW(status, 0);
		}

		indirect_id = indirect_rd;

		block_id -= indirect_off * d_indir_l + d_indir_l;
		
	} else if (block_id >= d_indir_s) {

		indirect_id = inode->block[13];

		if (!indirect_id)
			RETURN(0);
	}

	if (block_id >= d_indir_s) { //Doubly indirect
		indirect_off = (block_id - d_indir_s) / s_indir_l;

		status = ext2_read_block(device, indirect_id, indirect_off * 4, &indirect_rd, 4, &rsize);
		if (status || !indirect_rd) {
			THROW(status, 0);
		}

		indirect_id = indirect_rd;

		block_id -= s_indir_l + indirect_off * s_indir_l;

	} else if (block_id >= s_indir_s) {

		indirect_id = inode->block[12];

		if (!indirect_id)
			RETURN(indirect_id);

	}

	if (block_id >= s_indir_s) { //Singly Indirect

		indirect_off = block_id - s_indir_s;

		status = ext2_read_block(device, indirect_id, indirect_off * 4, &indirect_rd, 4, &rsize);
		if (status || !indirect_rd) {
			THROW(status, 0);
		}
		
		RETURN(indirect_rd);
	}

	RETURN(inode->block[block_id]);
}

SVFUNC(ext2_set_block_id, ext2_device_t *device, ext2_inode_t *inode, uint32_t block_id, uint32_t block_v)
{
	uint32_t indirect_count = 256 << device->superblock.block_size_enc;
	uint32_t indirect_id, indirect_off, indirect_rd;
	uint32_t s_indir_l = indirect_count;
	uint32_t d_indir_l = indirect_count * indirect_count;
	uint32_t s_indir_s = 12;
	uint32_t d_indir_s = s_indir_s + s_indir_l;
	uint32_t t_indir_s = d_indir_s + d_indir_l;
	int status;
	aoff_t rsize;	

	assert ( device != NULL );

	if (block_id >= t_indir_s) { //Triply indirect
		indirect_id = inode->block[14];
		indirect_off = (block_id - t_indir_s) / d_indir_l;

		if (!indirect_id) {

			status = ext2_allocate_indirect_block(device, inode, &indirect_id);

			if (status)
				THROWV(status);	

			inode->block[14] = indirect_id;
		}

		status = ext2_read_block(device, indirect_id, indirect_off * 4, &indirect_rd, 4, &rsize);
		if (status)
			THROWV(status);	

		if (!indirect_rd) {

			status = ext2_allocate_indirect_block(device, inode, &indirect_id);

			if (status)
				THROWV(status);	

			status = ext2_write_block(device, indirect_id, indirect_off * 4, &indirect_rd, 4, &rsize);
			if (status)
				THROWV(status);			
		}

		indirect_id = indirect_rd;

		block_id -= indirect_off * d_indir_l + d_indir_l;
		
	} else if (block_id >= d_indir_s) {

		indirect_id = inode->block[13];

		if (!indirect_id) {

			status = ext2_allocate_indirect_block(device, inode, &indirect_id);

			if (status)
				THROWV(status);	

			inode->block[13] = indirect_id;
		}
	}

	if (block_id >= d_indir_s) { //Doubly indirect
		indirect_off = (block_id - d_indir_s) / s_indir_l;

		status = ext2_read_block(device, indirect_id, indirect_off * 4, &indirect_rd, 4, &rsize);
		if (status)
			THROWV(status);	

		if (!indirect_rd) {

			status = ext2_allocate_indirect_block(device, inode, &indirect_id);

			if (status)
				THROWV(status);	

			status = ext2_write_block(device, indirect_id, indirect_off * 4, &indirect_rd, 4, &rsize);
			if (status)
				THROWV(status);			
		}

		indirect_id = indirect_rd;

		block_id -= s_indir_l + indirect_off * s_indir_l;

	} else if (block_id >= s_indir_s){

		indirect_id = inode->block[12];

		if (!indirect_id) {

			status = ext2_allocate_indirect_block(device, inode, &indirect_id);

			if (status)
				THROWV(status);	

			inode->block[12] = indirect_id;
		}

	}

	if (block_id >= s_indir_s) { //Singly Indirect

		indirect_off = block_id - s_indir_s;

		status = ext2_write_block(device, indirect_id, indirect_off * 4, &block_v, 4, &rsize);
		if (status)
			THROWV(status);	

		RETURNV;
	}

	inode->block[block_id] = block_v;

	RETURNV;
}

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

SVFUNC( ext2_link, inode_t *_inode, char *name, ino_t ino_id)
{
	ext2_device_t *device;
	ext2_vinode_t *inode;

	size_t reclen,namelen, f_reclen;
	ext2_dirent_t dirent;	
	ext2_dirent_t f_dirent;	
	
	int status, m = 0;

	aoff_t split_offset, hole_offset, nread, pos, dsize, hole_size;

	assert( _inode != NULL );
	assert( name != NULL );

	device = (ext2_device_t *) _inode->device;
	inode = (ext2_vinode_t *) _inode;

	dsize = _inode->size;

	namelen = strlen(name);
	reclen = ext2_roundup(sizeof(ext2_dirent_t) + namelen, 4);

	split_offset = 0;

	for (pos = 0; pos < dsize; pos += f_dirent.rec_len) {
		status = ext2_read_inode(_inode, &f_dirent, pos, sizeof(ext2_dirent_t), &nread);
		if (status || (nread != sizeof(ext2_dirent_t)))
			THROWV(status ? status : EIO);
		
		f_reclen = ext2_roundup (sizeof(ext2_dirent_t) + f_dirent.name_len, 4);
		
		if ((((!f_dirent.inode) || (!f_dirent.name_len))) && (f_dirent.rec_len >= reclen)) {
			hole_offset = pos;
			hole_size = f_dirent.rec_len;
			m = 1;
			break;
		} else if ((f_dirent.rec_len - f_reclen) >= reclen) {
			split_offset = pos;
			hole_offset = pos + f_reclen;
			hole_size = f_dirent.rec_len - f_reclen;
			m = 2;
			break;
		}
		split_offset = pos;
	}

	if (m == 0) {
 		//Append new block to the end of the file, split_offset contains the offset of the last dirent
		hole_size = 1024 << device->superblock.block_size_enc;
		hole_offset = pos;
		_inode->size += hole_size;
	}

	dirent.rec_len = hole_size;
	dirent.name_len = namelen;
	dirent.inode = ino_id;
	//TODO: Set file type
	dirent.file_type = EXT2_FT_UNKNOWN;
	
	status = ext2_write_inode(_inode, name, hole_offset + sizeof(ext2_dirent_t), namelen, &nread);
	if (status || (nread != namelen))
		THROWV(status ? status : EIO);
	
	status = ext2_write_inode(_inode, &dirent, hole_offset, sizeof(ext2_dirent_t), &nread);
	if (status || (nread != sizeof(ext2_dirent_t)))
		THROWV(status ? status : EIO);
	
	if (m == 2){
		f_dirent.rec_len = hole_offset - split_offset;

		status = ext2_write_inode(_inode, &f_dirent, split_offset, sizeof(ext2_dirent_t), &nread);
		if (status || (nread != sizeof(ext2_dirent_t)))
			THROWV(status ? status : EIO);
	}

	RETURNV;
	
}

SFUNC(aoff_t, ext2_readdir, inode_t *_inode, void *_buffer, aoff_t f_offset, aoff_t length)
{///XXX: Dependent on : sizeof(vfs_dirent_t) == sizeof(ext2_dirent_t)
	int status;

	ext2_dirent_t *dirent;
	uint8_t *name;
	uint8_t *buffer= _buffer;

	dirent_t *vfs_dir;
	aoff_t pos, inode_nread;

	assert( _inode != NULL );
	assert( _buffer != NULL );

	if (f_offset >= _inode->size)
		RETURN(0);

	if ((f_offset + length) > _inode->size)
		length = _inode->size - f_offset;

	dirent = heapmm_alloc(sizeof(ext2_dirent_t));
	
	if (!dirent)
		THROW(ENOMEM, 0);

	for (pos = 0; pos < length; pos += dirent->rec_len) {

		status = ext2_read_inode(_inode, dirent, pos + f_offset, sizeof(ext2_dirent_t), &inode_nread);
		if (status || (inode_nread != sizeof(ext2_dirent_t))) {
			heapmm_free(dirent, sizeof(ext2_dirent_t));
			THROW(status ? status : EIO, 0);
		}

		if ((dirent->name_len + pos + 9) > length){
			heapmm_free(dirent, sizeof(ext2_dirent_t));
			RETURN(pos);
		}
	
		vfs_dir = (dirent_t *)&buffer[pos];

		name = (uint8_t *) vfs_dir->name;
		
		status = ext2_read_inode(_inode, name, pos + f_offset + sizeof(ext2_dirent_t), dirent->name_len, &inode_nread);

		if (status || (inode_nread != dirent->name_len)) {
			heapmm_free(dirent, sizeof(ext2_dirent_t));
			THROW(status ? status : EIO, 0);
		}

		name[dirent->name_len] = 0;
		vfs_dir->inode_id = dirent->inode;
		vfs_dir->device_id = _inode->device_id;
		vfs_dir->d_reclen = dirent->rec_len;
		
	}

	heapmm_free(dirent, sizeof(ext2_dirent_t));
	
	RETURN(pos);
}

SFUNC(dirent_t *, ext2_finddir, inode_t *_inode, char * name)
{
	uint8_t *buffer = heapmm_alloc(1024);
	int status;
	aoff_t nread = 1;
	aoff_t fpos = 0;
	aoff_t pos;
	dirent_t *dirent;
	dirent_t *pp;

	assert( _inode != NULL );
	assert( name != NULL );

	if (!buffer)
		THROW(ENOMEM, NULL);
	for (fpos = 0; nread != 0; fpos += nread) {
		status = ext2_readdir(_inode, buffer, fpos, 1024, &nread);
		if (status) {
			heapmm_free(buffer, 1024);
			THROW(status, NULL);
		}
		for (pos = 0; pos < nread; pos += dirent->d_reclen) {
			dirent = (dirent_t *) &(buffer[pos]);
			if (strcmp(name, dirent->name) == 0){
				pp = heapmm_alloc(sizeof(dirent_t));	
				dirent->d_reclen = (dirent->d_reclen > sizeof(dirent_t)) ? sizeof(dirent_t) : dirent->d_reclen;
				memcpy(pp, dirent,dirent->d_reclen);
				heapmm_free(buffer, 1024);
				RETURN(pp);
			}
		}
	}
	heapmm_free(buffer, 1024);
	THROW(ENOENT, NULL);
}

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

SVFUNC(ext2_mkdir, inode_t *_inode) {
	ext2_block_group_desc_t *bgd;
	ext2_device_t *device;
	int st;
	if (!_inode) {	
		THROWV(EFAULT);
	}

	device = (ext2_device_t *) _inode->device;

	st = ext2_load_bgd(device, (_inode->id - 1) / device->superblock.inodes_per_group, &bgd);

	if (st)
		THROWV(st);

	bgd->used_dir_count++;
	
	st = ext2_store_bgd(device, (_inode->id - 1) / device->superblock.inodes_per_group, bgd);
	
	ext2_free_bgd(device, bgd);
	
	THROWV(st);
}

SVFUNC( ext2_sync, fs_device_t *device )
{
	int status;
	aoff_t	_read_size;
	ext2_device_t *dev;

	dev = (ext2_device_t *) device;

	status = device_block_write(dev->dev_id, 1024, &(dev->superblock), 1024, &_read_size);

	//TODO: Update alternate superblocks

	if ( status ) {
		debugcon_printf("ext2: could not write superblock, error:%i, read:%i!\n", status, _read_size);
		THROWV( status );
	}

	if (_read_size != 1024) {
		debugcon_printf("ext2: could not write superblock, read:%i!\n", status, _read_size);
		THROWV( EIO );
	}

	RETURNV;	

}

fs_device_operations_t ext2_ops = {
	&ext2_load_inode,//Load inode
	&ext2_store_inode,//Store inode
	&ext2_mknod,//Make inode
	NULL,//Remove inode
	&ext2_read_inode,//Read from file
	&ext2_write_inode,//Write to file
	&ext2_readdir,//Read from directory
	&ext2_finddir,//Find directory entry
	&ext2_mkdir,//Make directory
	&ext2_link,//Make directory entry
	NULL,//Remove directory entry
	&ext2_trunc_inode, //Change file length
	&ext2_sync
};

SFUNC(fs_device_t *, ext2_mount, dev_t device, uint32_t flags)
{
	int status;
	aoff_t	_read_size;

	ext2_device_t *dev = heapmm_alloc(sizeof(ext2_device_t));
	
	if (!dev)
		THROW(ENOMEM, NULL);

	dev->dev_id = device;	
	dev->device.id = device;
	dev->device.root_inode_id = EXT2_ROOT_INODE;
	dev->device.ops = &ext2_ops;
	dev->device.lock = semaphore_alloc();
	if(!dev->device.lock) {
		heapmm_free(dev, sizeof(ext2_device_t));
		THROW(ENOMEM, NULL);
	}
	semaphore_up(dev->device.lock);
	dev->device.inode_size = sizeof(ext2_vinode_t);

	status = device_block_read(dev->dev_id, 1024, &(dev->superblock), 1024, &_read_size);

	//TODO: Implement fallback to alternative superblock

	if (_read_size != 1024) {
		debugcon_printf("ext2: could not read superblock, error:%i, read:%i!\n", status, _read_size);
		heapmm_free(dev, sizeof(ext2_device_t));
		THROW(status, NULL);
	}

	if (dev->superblock.signature != 0xEF53) {
		debugcon_printf("ext2: superblock signature incorrect: %i!\n", dev->superblock.signature);
		heapmm_free(dev, sizeof(ext2_device_t));
		THROW(EINVAL, NULL);
	}	

	if (dev->superblock.version_major == EXT2_VERSION_MAJOR_DYNAMIC) {

		if (dev->superblock.required_features & ~(EXT2_SUPPORTED_REQ_FEATURES)) {
			debugcon_printf("ext2: filesystem requires unsupported features, refusing to mount!\n");
			heapmm_free(dev, sizeof(ext2_device_t));
			THROW(EINVAL, NULL);
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

	RETURN( (fs_device_t *) dev );
}

int ext2_register()
{
	return vfs_register_fs("ext2", &ext2_mount);
}
