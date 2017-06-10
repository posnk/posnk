/**
 * fs/ext2/inoblk.c
 * 
 * Implements functions for accessing blocks in inodes
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
	

void *ext2_zero_buffer = NULL;

SFUNC(uint32_t, ext2_allocate_indirect_block, 
					ext2_device_t *device, 
					ext2_inode_t *inode)
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

SFUNC(uint32_t, ext2_decode_block_id, 
					ext2_device_t *device, 
					ext2_inode_t *inode, 
					uint32_t block_id)
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

SVFUNC(ext2_set_block_id, ext2_device_t *device, 
							ext2_inode_t *inode, 
							uint32_t block_id, 
							uint32_t block_v)
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
