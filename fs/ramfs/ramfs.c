/**
 * fs/ramfs.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 18-04-2014 - Created
 */
#include "kernel/heapmm.h"
#include "kernel/vfs.h"
#include "kernel/streams.h"
#include "fs/ramfs.h"
#include "util/llist.h"
#include <string.h>
#include <sys/errno.h>
//HACKHACKHACK: Uses inode cache as storage

SVFUNC(ramfs_store_inode, inode_t *inode)
{
	if (!inode)
		THROWV(EFAULT);
	RETURNV;
}

SVFUNC(ramfs_mknod, inode_t *_inode)	//inode -> status
{
	ramfs_device_t *dev = (ramfs_device_t *) _inode->device;
	ramfs_inode_t *inode = (ramfs_inode_t *) _inode;

	inode->inode.id = dev->inode_id_ctr++;

	inode->block_list = (llist_t *) heapmm_alloc(sizeof(llist_t));

	if (!(inode->block_list))
		THROWV(ENOMEM);

	llist_create(inode->block_list);

	RETURNV;	
}

SVFUNC(ramfs_rmnod, inode_t *inode)	//inode -> status
{
	if (!inode)
		THROWV(EFAULT);
	RETURNV;
}

int ramfs_rawdir_search_iterator (llist_t *node, void *param)
{
	aoff_t start = (aoff_t) param;
	ramfs_dirent_t *block = (ramfs_dirent_t *) node;
	return (block->start == start);
}

SFUNC(aoff_t, ramfs_readdir, inode_t *_inode, void *_buffer, aoff_t f_offset, aoff_t length)//buffer, f_offset, length -> numbytes
{
	ramfs_dirent_t plholder;
	ramfs_inode_t *inode = (ramfs_inode_t *) _inode;
	uintptr_t buffer = (uintptr_t) _buffer;
	uintptr_t b_data;
	aoff_t pos = 0;
	aoff_t current_off;
	ramfs_dirent_t *_block;
	current_off = f_offset;
	for (;;) {
		_block = (ramfs_dirent_t *) llist_iterate_select(inode->dirent_list, &ramfs_rawdir_search_iterator, (void *) current_off);
		if (_block == NULL) {
			_block = &plholder;
			plholder.dir.d_reclen = sizeof(dirent_t);
			plholder.dir.name[0] = 0;
			plholder.dir.inode_id = 0;
			plholder.dir.device_id = _inode->device_id;
			plholder.start = current_off % (plholder.dir.d_reclen);
		}
		b_data = (uintptr_t) &(_block->dir);
		if ((pos + _block->dir.d_reclen) > length) {
			RETURN(pos);
		}
		memcpy((void *) buffer, (void *) (b_data),  _block->dir.d_reclen);
		pos += _block->dir.d_reclen;
		buffer += _block->dir.d_reclen;
		current_off += _block->dir.d_reclen;
	}	
	RETURN(pos);	
}

int ramfs_block_search_iterator (llist_t *node, void *param)
{
	aoff_t start = (aoff_t) param;
	ramfs_block_t *block = (ramfs_block_t *) node;
	return (block->start <= start) && ((block->start + block->length) > start);
}

SFUNC(aoff_t, ramfs_read_inode, inode_t *_inode, void *_buffer, aoff_t f_offset, aoff_t length)//buffer, f_offset, length -> numbytes
{
	ramfs_inode_t *inode = (ramfs_inode_t *) _inode;
	uintptr_t buffer = (uintptr_t) _buffer;
	uintptr_t b_data;
	aoff_t remaining_length = length;
	aoff_t block_off, current_off, in_blk_len;
	ramfs_block_t *_block;
	current_off = f_offset;
	for (;;) {
		_block = (ramfs_block_t *) llist_iterate_select(inode->block_list, &ramfs_block_search_iterator, (void *) current_off);
		if (_block == NULL)
			break;
		b_data = (uintptr_t) _block->data;
		block_off = current_off - _block->start;
		in_blk_len = _block->length - block_off;
		if (remaining_length < in_blk_len)
			in_blk_len = remaining_length;
		memcpy((void *) buffer, (void *) (b_data + block_off), (size_t) in_blk_len);
		remaining_length -= in_blk_len;
		current_off += in_blk_len;
		buffer += in_blk_len;
		if (remaining_length == 0)
			break;
	}	
	if (remaining_length)
		THROW(EIO, length - remaining_length);	
	else
		RETURN(length - remaining_length);
}

SFUNC(aoff_t, ramfs_write_inode, inode_t *_inode, const void *_buffer, aoff_t f_offset, aoff_t length)//buffer, f_offset, length -> numbytes
{
	ramfs_inode_t *inode = (ramfs_inode_t *) _inode;
	uintptr_t buffer = (uintptr_t) _buffer;
	uintptr_t b_data;
	aoff_t remaining_length = length;
	aoff_t block_off, current_off, in_blk_len;
	ramfs_block_t *_block;
	current_off = f_offset;
	for (;;) {
		_block = (ramfs_block_t *) llist_iterate_select(inode->block_list, &ramfs_block_search_iterator, (void *) current_off);
		if (_block == NULL)
			break;
		b_data = (uintptr_t) _block->data;
		block_off = current_off - _block->start;
		in_blk_len = _block->length - block_off;
		if (remaining_length < in_blk_len)
			in_blk_len = remaining_length;
		memcpy((void *) (b_data + block_off), (void *) buffer, (size_t) in_blk_len);
		remaining_length -= in_blk_len;
		current_off += in_blk_len;
		buffer += in_blk_len;
		if (remaining_length == 0) {
			RETURN(length);
		}
	}	
	_block = (ramfs_block_t *) heapmm_alloc(sizeof(ramfs_block_t));
	if (_block == NULL) {
		THROW(ENOSPC, length - remaining_length);
	}
	_block->start = current_off;
	_block->length = remaining_length;
	_block->data = heapmm_alloc((size_t) remaining_length);
	memcpy(_block->data, (void *) buffer, (size_t) remaining_length);
	llist_add_end(inode->block_list, (llist_t *) _block);
	RETURN(length);
}

SVFUNC(ramfs_trunc_inode, inode_t *_inode, aoff_t size)//buffer, f_offset, length -> numbytes
{
	ramfs_inode_t *inode = (ramfs_inode_t *) _inode;
	ramfs_block_t *_block;
	if (size > _inode->size) {
		_block = (ramfs_block_t *) heapmm_alloc(sizeof(ramfs_block_t));
		if (!_block)
			THROWV(ENOMEM);

		_block->start =  _inode->size;
		_block->length = size - _inode->size;

		_block->data = heapmm_alloc((size_t) _block->length);
		if (!_block->data) {
			heapmm_free(_block, sizeof(ramfs_block_t));
			THROWV(ENOMEM);
		}

		memset(_block->data, 0, (size_t) _block->length);

		llist_add_end(inode->block_list, (llist_t *) _block);
	} else if (size < _inode->size) {
		//PSEUDOCODE:
		//ITERATE OVER BLOCKS
		//   IF BLOCK STARTS PAST EOF DELETE BLOCK
		//   IF BLOCK EXTENDS PAST EOF TRUNCATE BLOCKS
		THROWV(EIO);
	}
	RETURNV;	
}

int ramfs_dirent_search_iterator (llist_t *node, void *param)
{
	ramfs_dirent_t *dirent = (ramfs_dirent_t *) node;
	return strcmp(dirent->dir.name, (char *) param) == 0;
}

SFUNC(dirent_t *, ramfs_find_dirent, inode_t *_inode, const char *name )	//dir_inode_id, filename -> dirent_t 
{
	ramfs_inode_t *inode = (ramfs_inode_t *) _inode;
	ramfs_dirent_t *dirent = (ramfs_dirent_t *) llist_iterate_select(inode->dirent_list, &ramfs_dirent_search_iterator, (void *) name);
	if (dirent == NULL)
		THROW(ENOENT, NULL);
	RETURN(&(dirent->dir));
}

SVFUNC(ramfs_mkdir, inode_t *_inode)
{
	ramfs_inode_t *inode = (ramfs_inode_t *) _inode;

	inode->dirent_list = (llist_t *) heapmm_alloc(sizeof(llist_t));
	if (!(inode->dirent_list))
		THROWV(ENOMEM);

	llist_create(inode->dirent_list);
	RETURNV;
}

SFUNC(inode_t *, ramfs_load_inode, fs_device_t *device, ino_t id)
{
	//ramfs_device_t *dev = (ramfs_device_t *) device;
	ramfs_inode_t *inode;
	if (id == 0) {
		inode = (ramfs_inode_t *) heapmm_alloc(sizeof(ramfs_inode_t));
		memset(inode, 0, device->inode_size);
		inode->inode.device = device;
		inode->inode.device_id = device->id;
		inode->inode.mode = S_IFDIR | 0777;
		inode->inode.if_dev = 0;
		inode->inode.uid = 0; //root
		inode->inode.gid = 0; //root
		inode->inode.lock = semaphore_alloc();
		semaphore_up(inode->inode.lock);
		ramfs_mknod((inode_t *) inode);
		ramfs_mkdir((inode_t *) inode);
		RETURN((inode_t *) inode);
	}
	THROW(ENOTSUP, NULL);
}//inode id -> inode

SVFUNC(ramfs_link, inode_t * _dir, const char *_name, ino_t inode_id)	//dir_inode_id, filename, inode_id -> status
{
	ramfs_dirent_t *dirent = (ramfs_dirent_t *) heapmm_alloc(sizeof(ramfs_dirent_t));
	ramfs_inode_t *_d_inode = (ramfs_inode_t *) _dir;

	if (!dirent)
		THROWV(ENOSPC);

	strcpy(dirent->dir.name, _name);
	dirent->dir.inode_id = inode_id;
	dirent->dir.device_id = _dir->device_id;
	dirent->start = _dir->size;
	dirent->dir.d_reclen = sizeof(dirent_t);
	_dir->size += sizeof(dirent_t);

	llist_add_end(_d_inode->dirent_list, (llist_t *) dirent);
	RETURNV;	
}

SVFUNC(ramfs_unlink, inode_t *_inode, const char *name )	//dir_inode_id, filename
{
	ramfs_inode_t *inode = (ramfs_inode_t *) _inode;
	ramfs_dirent_t *dirent = (ramfs_dirent_t *) llist_iterate_select(inode->block_list, &ramfs_dirent_search_iterator, (char *) name);
	if (!dirent)
		THROWV(ENOENT);
	llist_unlink((llist_t *) dirent);
	heapmm_free(dirent, sizeof(ramfs_dirent_t));
	RETURNV;
}

uint32_t ramfs_device_ctr = 0x0F00;

SFUNC( aoff_t, ramfs_dir_read_stub, 	
					__attribute__((__unused__)) stream_info_t *stream,
					__attribute__((__unused__)) void *buffer,
					__attribute__((__unused__)) aoff_t length )
{
	THROW(EISDIR, 0);
}

SFUNC( aoff_t, ramfs_dir_write_stub, 	
					__attribute__((__unused__)) stream_info_t *stream,
					__attribute__((__unused__)) const void *buffer,
					__attribute__((__unused__)) aoff_t length )
{
	THROW(EISDIR, 0);
}

SVFUNC( ramfs_dir_close, stream_info_t *stream )
{

	stream->inode->open_count--;
	
	/* Release the inode */
	vfs_inode_release(stream->inode);
	
	/* Release the inode */
	vfs_dir_cache_release(stream->dirc);

	RETURNV;

}

SFUNC( aoff_t, ramfs_dir_readdir, stream_info_t *stream, sys_dirent_t *buffer, aoff_t buflen)
{
	ramfs_dirent_t plholder;
	ramfs_inode_t *inode = (ramfs_inode_t *) stream->inode;
	aoff_t current_off;
	
	ramfs_dirent_t *_block;
	
	current_off = stream->offset;
	
	if ( current_off > stream->inode->size )
		RETURN(0);
	
	_block = (ramfs_dirent_t *) llist_iterate_select(inode->dirent_list, &ramfs_rawdir_search_iterator, (void *) current_off);
	
	if (_block == NULL) {
		_block = &plholder;
		plholder.dir.d_reclen = sizeof(dirent_t);
		plholder.dir.name[0] = 0;
		plholder.dir.inode_id = 0;
		plholder.dir.device_id = stream->inode->device_id;
		plholder.start = current_off % (plholder.dir.d_reclen);
	}
	
	if (_block->dir.d_reclen > buflen) {
		THROW(E2BIG, _block->dir.d_reclen);
	}
	
	memcpy(buffer, &_block->dir, _block->dir.d_reclen);
	stream->offset += _block->dir.d_reclen;
	
	RETURN(_block->dir.d_reclen);
}

fs_device_operations_t *ramfs_ops;
stream_ops_t ramfs_dir_ops = {
	.close = ramfs_dir_close,
	.read = ramfs_dir_read_stub,
	.write = ramfs_dir_write_stub,
	.readdir = ramfs_dir_readdir,
	.ioctl = NULL,
	.seek  = NULL,
	.lseek = NULL,
	.chdir = NULL,
	.stat  = NULL,
	.chmod = NULL,
	.chown = NULL,
	.truncate = NULL
	
};

SVFUNC ( ramfs_open_inode, inode_t *inode, void *_stream )
{
	
	stream_info_t *stream = _stream;
	
	if (S_ISDIR(inode->mode)) {
		
		stream->type		= STREAM_TYPE_EXTERNAL;
		stream->ops  		= &ramfs_dir_ops;
		stream->impl_flags  = 	STREAM_IMPL_FILE_CHDIR | 
								STREAM_IMPL_FILE_CHMOD |
								STREAM_IMPL_FILE_CHOWN |
								STREAM_IMPL_FILE_FSTAT |
								STREAM_IMPL_FILE_LSEEK |
								STREAM_IMPL_FILE_TRUNC;
		
	}
	
	return 0;
	
}

SFUNC(fs_device_t *, ramfs_mount, 
			__attribute__((__unused__)) dev_t device,
			__attribute__((__unused__)) uint32_t flags)
{
	ramfs_device_t *dev = (ramfs_device_t *) heapmm_alloc(sizeof(ramfs_device_t));
	if (ramfs_ops == NULL) {
		ramfs_ops = (fs_device_operations_t *) heapmm_alloc(sizeof(fs_device_operations_t));
		ramfs_ops->load_inode = &ramfs_load_inode;
		ramfs_ops->store_inode = &ramfs_store_inode;
		ramfs_ops->mknod = &ramfs_mknod;
		ramfs_ops->rmnod = &ramfs_rmnod;
		ramfs_ops->read_inode = &ramfs_read_inode;
		ramfs_ops->read_dir = &ramfs_readdir;
		ramfs_ops->write_inode = &ramfs_write_inode;
		ramfs_ops->find_dirent = &ramfs_find_dirent;
		ramfs_ops->mkdir = &ramfs_mkdir;
		ramfs_ops->link = &ramfs_link;
		ramfs_ops->unlink = &ramfs_unlink;
		ramfs_ops->trunc_inode = &ramfs_trunc_inode;
		ramfs_ops->open_inode = &ramfs_open_inode;
	}
	dev->inode_id_ctr = 0;
	dev->device.id = ramfs_device_ctr;
	dev->device.root_inode_id = 0;
	dev->device.lock = semaphore_alloc();
	dev->device.ops = ramfs_ops;
	dev->device.inode_size = sizeof(ramfs_inode_t);
	ramfs_device_ctr = ramfs_device_ctr + 1;
	RETURN( (fs_device_t *)dev );
}

int ramfs_register()
{
	return vfs_register_fs("ramfs", &ramfs_mount);
}
