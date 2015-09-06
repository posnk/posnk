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
#include "kernel/streams.h"


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



SFUNC( aoff_t, ext2_ireaddir, 
				inode_t *inode,
				aoff_t *offset,
				sys_dirent_t *buffer, 
				aoff_t buflen)
{
	ext2_dirent_t	dirent_hdr;
	errno_t			status;
	aoff_t			inode_nread;
	
	if ( sizeof(sys_dirent_t) > buflen )
		THROW( E2BIG, sizeof(sys_dirent_t) );
	
	if ( ( *offset + sizeof(ext2_dirent_t) ) > inode->size ) {
		THROW( ENMFILE, 0 );
	}
		
	status = ext2_read_inode(	inode, 
								&dirent_hdr, 
								*offset, 
								sizeof(ext2_dirent_t), 
								&inode_nread);
	
	if ( status ) 
		THROW ( status, 0 );
		
	if ( inode_nread < sizeof( ext2_dirent_t ) )
		THROW ( EIO, 0 );

	if ((dirent_hdr.name_len + 9) > buflen)
		THROW( E2BIG, dirent_hdr.name_len + 9 );
		
	status = ext2_read_inode( 	inode, 
								buffer->d_name, 
								*offset + sizeof(ext2_dirent_t), 
								dirent_hdr.name_len, 
								&inode_nread);

	if (status || (inode_nread != dirent_hdr.name_len)) 
		THROW(status ? status : EIO, 0);

	buffer->d_name[dirent_hdr.name_len] = 0;
	buffer->d_ino = dirent_hdr.inode;
	buffer->d_dev = inode->device_id;
	buffer->d_reclen = 11 + dirent_hdr.name_len;
		
	*offset += dirent_hdr.rec_len;
		
	RETURN( dirent_hdr.name_len + 11);
}

SFUNC(dirent_t *, ext2_finddir, inode_t *_inode, char * name)
{
	sys_dirent_t	*dirent;
	errno_t			status;
	aoff_t			nread;
	aoff_t 			offset = 0;
	
	dirent = heapmm_alloc( sizeof ( sys_dirent_t ) );
	
	if ( dirent == NULL )
		THROW( ENOMEM, NULL );
	
	while ( (status = ext2_ireaddir(	_inode, 
										&offset, 
										dirent, 
										sizeof(sys_dirent_t), 
										&nread ) ) == 0 ) {
		if ( strcmp ( dirent->d_name, name ) == 0 )
			RETURN( ( dirent_t * ) dirent);
											
	}
	
	assert ( status != E2BIG );
	
	if ( status == ENMFILE )
		status = ENOENT;
	
	heapmm_free( dirent, sizeof ( sys_dirent_t ));
	
	THROW( status, NULL );
	
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

SFUNC( aoff_t, ext2_dir_readwrite_stub, 	
					__attribute__((__unused__)) stream_info_t *stream,
					__attribute__((__unused__)) void *buffer,
					__attribute__((__unused__)) aoff_t length )
{
	THROW(EISDIR, 0);
}

SVFUNC( ext2_dir_close, stream_info_t *stream )
{

	stream->inode->open_count--;
	
	/* Release the inode */
	vfs_inode_release(stream->inode);
	
	/* Release the inode */
	vfs_dir_cache_release(stream->dirc);

	RETURNV;

}

SFUNC( aoff_t, ext2_dir_readdir, stream_info_t *stream, sys_dirent_t *buffer, aoff_t buflen)
{
	CHAINRET( ext2_ireaddir, stream->inode, &stream->offset, buffer, buflen );
}
