/**
 * fs/proc/proc.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 18-10-2015 - Created
 */

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <sys/errno.h>
#include <sys/types.h>

#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/vfs.h"
#include "kernel/earlycon.h"
#include "kernel/streams.h"

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

SFUNC( aoff_t, proc_finddir, inode_t *_inode, void *_buffer, char *name )
{
	proc_vinode_t	*inode;
	proc_dirent_t	*dirent;
	llist_t 	*_work;
	llist_t		*_end;
	
	assert( _inode != NULL );
	assert(   name != NULL );
	
	inode = ( proc_vinode_t * ) _inode;


	_end = 
}

SVFUNC( proc_link, inode_t *_inode, char *name, ino_t ino_id )
{	
	THROWV( EROFS );
}

stream_ops_t proc_dir_ops = {
	.close = ext2_dir_close,
	.read = ext2_dir_readwrite_stub,
	.write = ext2_dir_readwrite_stub,
	.readdir = ext2_dir_readdir,
	.ioctl = NULL,
	.seek  = NULL,
	.lseek = NULL,
	.chdir = NULL,
	.stat  = NULL,
	.chmod = NULL,
	.chown = NULL,
	.truncate = NULL
	
};

SVFUNC ( proc_open_inode, inode_t *inode, void *_stream )
{
	
	stream_info_t *stream = _stream;
	
	if (S_ISDIR(inode->mode)) {
		
		stream->type		= STREAM_TYPE_EXTERNAL;
		stream->ops  		= &ext2_dir_ops;
		stream->impl_flags  = 	STREAM_IMPL_FILE_CHDIR | 
								STREAM_IMPL_FILE_CHMOD |
								STREAM_IMPL_FILE_CHOWN |
								STREAM_IMPL_FILE_FSTAT |
								STREAM_IMPL_FILE_LSEEK |
								STREAM_IMPL_FILE_TRUNC;
		
	}
	
	return 0;
	
}

fs_device_operations_t ext2_ops = {
	&ext2_open_inode, //Open inode
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
	&ext2_sync,
};

SFUNC(fs_device_t *, proc_mount, dev_t device, uint32_t flags)
{
	int		 status;
	aoff_t	_read_size;

	proc_device_t *dev = heapmm_alloc(sizeof(proc_device_t));
	
	if (!dev)
		THROW(ENOMEM, NULL);

	dev->dev_id = device;	
	dev->device.id = device;
	dev->device.root_inode_id = PROC_ROOT_INODE;
	dev->device.ops = &ext2_ops;
	dev->device.lock = semaphore_alloc();
	if(!dev->device.lock) {
		heapmm_free(dev, sizeof(ext2_device_t));
		THROW(ENOMEM, NULL);
	}
	semaphore_up(dev->device.lock);
	dev->device.inode_size = sizeof(ext2_vinode_t);

	RETURN( (fs_device_t *) dev );
}

int proc_register()
{
	return vfs_register_fs("proc", &proc_mount);
}
