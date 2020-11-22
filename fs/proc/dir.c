/**
 * fs/proc/dir.c
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

SFUNC( dirent_t *, proc_finddir, inode_t *_inode, const char * name )
{
	proc_snap_t     *snap;
	sys_dirent_t	*dirent;
	errno_t			status;
	aoff_t			nread;
	aoff_t 			offset = 0;

	dirent = heapmm_alloc( sizeof ( sys_dirent_t ) );

	if ( dirent == NULL )
		THROW( ENOMEM, NULL );

	status = proc_open_snap( _inode->id, &snap );
	if( status )
		THROW( status, NULL );

	while ( (status = proc_snap_readdir(_inode->device_id,
	                                    snap,
										&offset,
										dirent,
										sizeof(sys_dirent_t),
										&nread ) ) == 0 && nread != 0) {
		if ( strcmp ( dirent->d_name, name ) == 0 )
			RETURN( ( dirent_t * ) dirent);

	}

	proc_snap_delete( snap );

	assert ( status != E2BIG );

	if ( status == 0 )
		status = ENOENT;

	heapmm_free( dirent, sizeof ( sys_dirent_t ));

	THROW( status, NULL );

}
SFUNC( aoff_t, proc_dir_read_stub,
					__attribute__((__unused__)) stream_info_t *stream,
					__attribute__((__unused__)) void *buffer,
					__attribute__((__unused__)) aoff_t length )
{
	THROW(EISDIR, 0);
}

SFUNC( aoff_t, proc_dir_write_stub,
					__attribute__((__unused__)) stream_info_t *stream,
					__attribute__((__unused__)) const void *buffer,
					__attribute__((__unused__)) aoff_t length )
{
	THROW(EISDIR, 0);
}

SVFUNC( proc_dir_close, stream_info_t *stream )
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

SFUNC( aoff_t, proc_dir_readdir,
                                 stream_info_t *stream,
                                 sys_dirent_t *buffer,
                                 aoff_t buflen)
{
	CHAINRET(
		proc_snap_readdir,
		stream->inode->device_id,
		stream->impl,
		&stream->offset,
		buffer,
		buflen );
}

stream_ops_t proc_dir_ops = {
	.close    = proc_dir_close,
	.read     = proc_dir_read_stub,
	.write    = proc_dir_write_stub,
	.readdir  = proc_dir_readdir,
	.ioctl    = NULL,
	.seek     = NULL,
	.lseek    = NULL,
	.chdir    = NULL,
	.stat     = NULL,
	.chmod    = NULL,
	.chown    = NULL,
	.truncate = NULL
};

