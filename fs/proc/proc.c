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
#include "fs/proc/proc.h"

SVFUNC( proc_sync, __attribute__((__unused__)) fs_device_t *device )
{

	RETURNV;

}

SVFUNC( proc_link, __attribute__((__unused__)) inode_t *_inode, __attribute__((__unused__)) const char *name, __attribute__((__unused__)) ino_t ino_id )
{
	THROWV( EROFS );
}

SVFUNC( proc_unlink, __attribute__((__unused__)) inode_t *_inode, __attribute__((__unused__)) const char *name )
{
	THROWV( EROFS );
}

SVFUNC(proc_mkdir, __attribute__((__unused__)) inode_t *_inode)
{
	THROWV( EROFS );
}

SFUNC(inode_t *, proc_load_inode, fs_device_t *device, ino_t id) {
	proc_file_t *file;
	inode_t *ino;

	if (!device)
		THROW(EFAULT, NULL);

	file = proc_get_file(id);
	if ( !file )
		THROW(EINVAL, NULL);

	ino = heapmm_alloc(sizeof(inode_t));
	if (!ino)
		THROW(ENOMEM, NULL);

	ino->device_id       = device->id;
	ino->id              = id;
	ino->device          = device;
	ino->mount           = NULL;
	semaphore_init(&ino->lock);
	semaphore_up(&ino->lock);
	ino->hard_link_count = (nlink_t) 1;
	ino->uid             = (uid_t) 0;
	ino->gid             = (gid_t) 0;
	ino->mode            = file->mode;
	ino->usage_count     = 0;
	ino->size            = file->size;
	ino->atime           = (ktime_t) 0;
	ino->ctime           = (ktime_t) 0;
	ino->mtime           = (ktime_t) 0;
	ino->dtime           = (ktime_t) 0;

	RETURN((inode_t *) ino);
}

SVFUNC(proc_store_inode,inode_t *_inode) {

	if (!_inode) {
		THROWV(EFAULT);
	}

	RETURNV;
}

SFUNC(snap_t *, proc_open_snap, ino_t inode ) {
	errno_t status;
	snap_t *snap;
	proc_file_t *file;

	file = proc_get_file( inode );
	if ( !file )
		THROWV(EINVAL);

	status = snap_create( 64, &snap );
	if (status)
		THROW(status, NULL);

	status = file->open( snap, inode );
	if (status) {
		snap_delete( snap );
		THROW(status, NULL);
	}

	RETURN( snap );

}

SVFUNC ( proc_open_inode, inode_t *inode, void *_stream )
{
	errno_t status;
	stream_info_t *stream = _stream;

	if (!inode)
		THROWV(EFAULT);

	stream->type		= STREAM_TYPE_EXTERNAL;
	if ( S_ISDIR( inode->mode ) ) {
		stream->ops  		= &proc_dir_ops;
	} else {
		stream->ops         = &proc_snapfile_ops;
	}
	stream->impl_flags  = 	STREAM_IMPL_FILE_CHDIR |
							STREAM_IMPL_FILE_CHMOD |
							STREAM_IMPL_FILE_CHOWN |
							//STREAM_IMPL_FILE_FSTAT |
							STREAM_IMPL_FILE_LSEEK |
							STREAM_IMPL_FILE_TRUNC;

	status = proc_open_snap( inode->id, (snap_t **)&(stream->impl) );
	if (status)
		THROWV(status);

	return 0;

}

fs_device_operations_t proc_ops = {
	&proc_open_inode,  //Open inode
	&proc_load_inode,  //Load inode
	&proc_store_inode, //Store inode
	NULL,              //Make inode
	NULL,              //Remove inode
	NULL,              //Read from file
	NULL,              //Write to file
	NULL,              //Read from directory
	&proc_finddir,     //Find directory entry
	&proc_mkdir,       //Make directory
	&proc_link,        //Make directory entry
	NULL,              //Remove directory entry
	NULL,              //Change file length
	&proc_sync,
};

fs_device_t proc_dev = {
	.id = 0xFF00,
	.root_inode_id = PROC_INO_ROOT,
	.ops = &proc_ops,
	.lock = 1,
	.inode_size = sizeof(inode_t)
};

SFUNC(fs_device_t *, proc_mount, __attribute__((__unused__)) dev_t device, __attribute__((__unused__)) uint32_t flags)
{
	RETURN( (fs_device_t *) &proc_dev );
}

int proc_register()
{
	proc_init_files();
	return vfs_register_fs("proc", &proc_mount);
}
