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

#include "fs/ext2/fsapi.h"
#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/vfs.h"
#include "kernel/earlycon.h"
#include "kernel/streams.h"

void ext2_handle_error( __attribute__((unused)) ext2_device_t *device)
{
	assert(0/* EXT2 ERROR */);

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
stream_ops_t ext2_dir_ops = {
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

SVFUNC ( ext2_open_inode, inode_t *inode, void *_stream )
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
	&ext2_unlink,//Remove directory entry
	&ext2_trunc_inode, //Change file length
	&ext2_sync,
};

SFUNC(fs_device_t *, ext2_mount, dev_t device, __attribute__((unused)) uint32_t flags)
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
		semaphore_free(dev->device.lock);
		heapmm_free(dev, sizeof(ext2_device_t));
		THROW(status, NULL);
	}

	if (dev->superblock.signature != 0xEF53) {
		debugcon_printf("ext2: superblock signature incorrect: %i!\n", dev->superblock.signature);
		semaphore_free(dev->device.lock);
		heapmm_free(dev, sizeof(ext2_device_t));
		THROW(EINVAL, NULL);
	}	

	if (dev->superblock.version_major == EXT2_VERSION_MAJOR_DYNAMIC) {

		if (dev->superblock.required_features & ~(EXT2_SUPPORTED_REQ_FEATURES)) {
			debugcon_printf("ext2: filesystem requires unsupported features, refusing to mount!\n");
			semaphore_free(dev->device.lock);
			heapmm_free(dev, sizeof(ext2_device_t));
			THROW(EINVAL, NULL);
		}

		if (dev->superblock.ro_force_features & ~(EXT2_SUPPORTED_ROF_FEATURES)) {
			debugcon_printf("ext2: filesystem requires unsupported features, mounting read-only!\n");
			//flags |= EXT2_MOUNT_FLAG_RO;
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
