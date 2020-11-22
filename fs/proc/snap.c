/**
 * fs/proc/snap.c
 *
 * Implements directories
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 * Changelog:
 * 21-11-2020 - Created
 */

#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <sys/errno.h>
#include <sys/types.h>
#include <sys/dirent.h>

#include "fs/proc/proc.h"
#include "kernel/heapmm.h"
#include "kernel/vfs.h"
#include "kernel/streams.h"

SFUNC( proc_snap_t *, proc_snap_create,
                                aoff_t            alloc_size )
{
	proc_snap_t *snap;

	snap = heapmm_alloc( sizeof(proc_snap_t) );
	if ( !snap )
		THROW( ENOMEM, NULL );

	snap->size = 0;
	snap->alloc_size = alloc_size;
	snap->data = heapmm_alloc( alloc_size );

	if ( !snap->data ) {
		heapmm_free( snap, sizeof(proc_snap_t) );
		THROW( ENOMEM, NULL );
	}

	RETURN(snap);
}

void proc_snap_delete( proc_snap_t *snap )
{
	heapmm_free( snap->data, snap->alloc_size );
	heapmm_free( snap, sizeof(proc_snap_t) );
}

SVFUNC( proc_snap_trunc,
                                proc_snap_t *     snap,
                                aoff_t            size )
{
	aoff_t new_asz;
	void *new_data;
	assert( snap  != NULL );

	if ( size <= snap->size ) {
		memset( snap->data + size, 0, snap->size - size );
		snap->size = size;
		RETURNV;
	} else {
		if ( size > snap->alloc_size ) {
			new_asz  = (size + 0x1FF) & ~0x1FF;
			new_data = heapmm_realloc(
					snap->data,
					snap->alloc_size,
					new_asz );
			if ( !new_data )
				THROWV( ENOMEM );
			snap->alloc_size = new_asz;
			snap->data = new_data;
		}
		memset( snap->data + snap->size, 0, size - snap->size );
		snap->size = size;
		RETURNV;
	}

}

SFUNC( aoff_t, proc_snap_read,
                                proc_snap_t *     snap,
                                aoff_t            offset,
                                void *            buffer,
                                aoff_t            count )
{
	assert( snap   != NULL );
	assert( buffer != NULL );

	if ( offset >= snap->size )
		RETURN( 0 );

	if ( offset + count >= snap->size )
		count = snap->size - offset;

	memcpy( buffer, snap->data + offset, count );

	RETURN( count );
}

SFUNC( aoff_t, proc_snap_write,
                                proc_snap_t *     snap,
                                aoff_t            offset,
                                const void *      buffer,
                                aoff_t            count )
{
	errno_t status;
	assert( snap   != NULL );
	assert( buffer != NULL );

	if ( offset >= snap->size || offset + count >= snap->size ) {
		status = proc_snap_trunc( snap, offset + count );
		if ( status )
			THROW( status, 0 );
	}

	memcpy( snap->data + offset, buffer, count );

	RETURN( count );
}
#define SYS_DIRENT_SZ (8)

SVFUNC( proc_snap_appenddir,
                                proc_snap_t *     snap,
                                const char *      name,
                                ino_t             ino )
{
	errno_t status;
	aoff_t wrsz,base;
	proc_dirent_t dirent_hdr;

	assert( snap   != NULL );
	assert( name   != NULL );

	dirent_hdr.inode    = ino;
	dirent_hdr.name_len = strlen( name );
	dirent_hdr.rec_len  = dirent_hdr.name_len + sizeof( proc_dirent_t );

	base = snap->size;

	status = proc_snap_trunc( snap, base + dirent_hdr.rec_len );
	if ( status )
		THROWV( status );

	status = proc_snap_write(
		snap,
		base,
		&dirent_hdr,
		sizeof( proc_dirent_t ),
		&wrsz );

	if ( status )
		THROWV( status );

	if ( wrsz != sizeof( proc_dirent_t ) )
		THROWV( ENOSPC );

	status = proc_snap_write(
		snap,
		base+sizeof( proc_dirent_t ),
		name,
		dirent_hdr.name_len,
		&wrsz );

	assert( status == 0 && wrsz == dirent_hdr.name_len );

	RETURNV;

}

SFUNC( aoff_t, proc_snap_readdir,
				dev_t device,
				proc_snap_t *     snap,
				aoff_t *          offset,
				sys_dirent_t *    buffer,
				aoff_t            buflen)
{
	proc_dirent_t	dirent_hdr;
	aoff_t          name_offset;
	size_t          full_sz;

	assert( snap != NULL );
	assert( offset != NULL );
	assert( buffer != NULL );

	if ( SYS_DIRENT_SZ > buflen )
		THROW( E2BIG, SYS_DIRENT_SZ );

	if ( ( *offset + SYS_DIRENT_SZ ) > snap->size ) {
		RETURN( 0 );
	}

	memcpy( &dirent_hdr, snap->data + *offset, SYS_DIRENT_SZ );

	full_sz = dirent_hdr.name_len + SYS_DIRENT_SZ + 1;

	if ( full_sz > buflen )
		THROW( E2BIG, full_sz );

	name_offset = *offset + SYS_DIRENT_SZ;

	assert( name_offset + dirent_hdr.name_len <= snap->size );

	memcpy( buffer->d_name,
	        snap->data + name_offset,
	        dirent_hdr.name_len );


	buffer->d_name[ dirent_hdr.name_len ] = 0;
	buffer->d_ino                         = dirent_hdr.inode;
	buffer->d_dev                         = device;
	buffer->d_reclen                      = full_sz;

	*offset += dirent_hdr.rec_len;

	RETURN( full_sz );
}

