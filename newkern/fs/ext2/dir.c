/**
 * fs/ext2/dir.c
 * 
 * Implements directories
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
