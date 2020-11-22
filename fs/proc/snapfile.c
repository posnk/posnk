/**
 * fs/proc/snapfile.c
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

#include "fs/proc/proc.h"
#include "kernel/heapmm.h"
#include "kernel/vfs.h"
#include "kernel/streams.h"

SFUNC( aoff_t, proc_snapfile_read,
					stream_info_t *stream,
					void *buffer,
					aoff_t length )
{
	aoff_t sz;
	errno_t status;

	status = proc_snap_read(
	                         stream->impl,
	                         stream->offset,
	                         buffer,
	                         length,
	                         &sz );

	stream->offset += sz;

	if ( status )
		THROW( status, 0 );

	RETURN( sz );
}

SFUNC( aoff_t, proc_snapfile_write,
					stream_info_t *stream,
					const void *buffer,
					aoff_t length )
{
	aoff_t sz;
	errno_t status;

	status = proc_snap_write(
	                         stream->impl,
	                         stream->offset,
	                         buffer,
	                         length,
	                         &sz );

	stream->offset += sz;

	if ( status )
		THROW( status, 0 );

	RETURN( sz );
}

SVFUNC( proc_snapfile_close, stream_info_t *stream )
{

	/* Delete the snap */
	proc_snap_delete( stream->impl );

	stream->inode->open_count--;

	/* Release the inode */
	vfs_inode_release(stream->inode);

	/* Release the inode */
	vfs_dir_cache_release(stream->dirc);

	RETURNV;

}

stream_ops_t proc_snapfile_ops = {
	.close    = proc_snapfile_close,
	.read     = proc_snapfile_read,
	.write    = proc_snapfile_write,
	.readdir  = NULL,
	.ioctl    = NULL,
	.seek     = NULL,
	.lseek    = NULL,
	.chdir    = NULL,
	.stat     = NULL,
	.chmod    = NULL,
	.chown    = NULL,
	.truncate = NULL
};

