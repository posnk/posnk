/**
 * fs/proc/snap.c
 *
 * Implements file/directory snapshots
 * These are used to avoid deadlocks by providing an unique copy
 * of a file or directory on open().
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

#include "fs/snap/snap.h"
#include "kernel/heapmm.h"
#include "kernel/vfs.h"
#include "kernel/streams.h"

/**
 * Creates a new, empty snap.
 *
 * @param alloc_size    Size of the initial allocation for the snap
 * @return              The snap that was created. To be freed using
 *                      snap_delete.
 * @exception ENOMEM    If either the descriptor or data could not
 *                      be allocated.
 */
SFUNC( snap_t *, snap_create,
                                aoff_t            alloc_size )
{
	snap_t *snap;

	/* Allocate the snap info block */
	snap = heapmm_alloc( sizeof(snap_t) );
	if ( !snap )
		THROW( ENOMEM, NULL );

	/* Initialize fields */
	snap->size = 0;
	snap->alloc_size = alloc_size;
	snap->data = heapmm_alloc( alloc_size );

	/* Handle data allocation failure */
	if ( !snap->data ) {
		heapmm_free( snap, sizeof(snap_t) );
		THROW( ENOMEM, NULL );
	}

	RETURN(snap);
}

/**
 * Deletes a snap.
 * @param snap          The snap to delete.
 */
void snap_delete( snap_t *snap )
{
	heapmm_free( snap->data, snap->alloc_size );
	heapmm_free( snap, sizeof(snap_t) );
}

/**
 * @brief Change the size of a snap
 * This function zeros out newly allocated or deallocated memory for the
 * snap. If an error occurs, the snap is not modified.
 * @param snap        The snap to modify
 * @param size        The new size for the snap.
 * @exception ENOMEM  Not enough memory was available to enlarge the snap.
 */
SVFUNC( snap_trunc,
                                snap_t *     snap,
                                aoff_t            size )
{
	aoff_t new_asz;
	void *new_data;
	assert( snap  != NULL );
	/* Check if the snap needs to shrink or grow */
	if ( size <= snap->size ) {
		/* Snap is being shrunk */

		/* Zero out data that is no longer being used */
		memset( snap->data + size, 0, snap->size - size );

		/* Set new size, excess data storage is not deallocated */
		snap->size = size;
		RETURNV;
	} else {
		/* Snap is being grown */

		if ( size > snap->alloc_size ) {
			/* There is not enough size in the snap's current */
			/* allocation. Reallocate data buffer. */

			/* Data buffer allocation size is rounded up to 512 b */
			new_asz  = (size + 0x1FF) & ~0x1FF;
			new_data = heapmm_realloc(
					snap->data,
					snap->alloc_size,
					new_asz );
			if ( !new_data )
				THROWV( ENOMEM );

			/* Update snap pointers */
			snap->alloc_size = new_asz;
			snap->data = new_data;
		}

		/* Zero the grown region of the snap */
		memset( snap->data + snap->size, 0, size - snap->size );

		/* Set the new snap size */
		snap->size = size;
		RETURNV;
	}

}


/**
 * @brief Read data from a snap
 * @param snap     The snap to read from
 * @param offset   The offset in the snap to read
 * @param buffer   The buffer to copy the data into
 * @param count    The number of bytes to read
 * @return         The number of bytes read
 */
SFUNC( aoff_t, snap_read,
                                snap_t *     snap,
                                aoff_t            offset,
                                void *            buffer,
                                aoff_t            count )
{
	assert( snap   != NULL );
	assert( buffer != NULL );

	/* If all of the read is beyond the end of the snap, read nothing */
	if ( offset >= snap->size )
		RETURN( 0 );

	/* If part of the read is beyond the end, do a short read */
	if ( offset + count >= snap->size )
		count = snap->size - offset;

	/* Copy over the data */
	memcpy( buffer, snap->data + offset, count );

	RETURN( count );
}

/**
 * @brief Write data to a snap
 * @param snap     The snap to write to
 * @param offset   The offset in the snap to write
 * @param buffer   The buffer to copy the data from
 * @param count    The number of bytes to write
 * @return         The number of bytes written
 */
SFUNC( aoff_t, snap_write,
                                snap_t *     snap,
                                aoff_t            offset,
                                const void *      buffer,
                                aoff_t            count )
{
	errno_t status;
	assert( snap   != NULL );
	assert( buffer != NULL );

	/* If the write is beyond the current end of the snap, grow the snap */
	if ( offset >= snap->size || offset + count >= snap->size ) {
		status = snap_trunc( snap, offset + count );
		if ( status )
			THROW( status, 0 );
	}

	/* Copy over the data */
	memcpy( snap->data + offset, buffer, count );

	RETURN( count );
}
#define SYS_DIRENT_SZ (8)

/**
 * @brief Append a new dirent to a snap
 * @param snap        The snap to append to
 * @param name        The filename to write into the new dirent
 * @param ino         The inode number the dirent should refer too
 * @exception ENOMEM  If there was not enough space to grow the snap.
 */
SVFUNC( snap_appenddir,
                                snap_t *     snap,
                                const char *      name,
                                ino_t             ino )
{
	errno_t status;
	aoff_t wrsz,base;
	snap_dirent_t dirent_hdr;

	assert( snap   != NULL );
	assert( name   != NULL );

	/* Fill dirent header */
	dirent_hdr.inode    = ino;
	dirent_hdr.name_len = strlen( name );
	dirent_hdr.rec_len  = dirent_hdr.name_len + sizeof( snap_dirent_t );

	/* The dirent will be written to the end of the snap */
	base = snap->size;

	/* Grow the snap first, so any errors will happen before modifying
	 * the snap */
	status = snap_trunc( snap, base + dirent_hdr.rec_len );
	if ( status )
		THROWV( status );

	/* Write out the dirent header */
	status = snap_write(
		snap,
		base,
		&dirent_hdr,
		sizeof( snap_dirent_t ),
		&wrsz );

	if ( status )
		THROWV( status );

	if ( wrsz != sizeof( snap_dirent_t ) )
		THROWV( ENOSPC );

	/* Write out the dirent name */
	status = snap_write(
		snap,
		base + sizeof( snap_dirent_t ),
		name,
		dirent_hdr.name_len,
		&wrsz );

	/* Any error here is a serious issue as the snap is now corrupt */
	assert( status == 0 && wrsz == dirent_hdr.name_len );

	RETURNV;

}

/**
 * @brief Read a single dirent from the snap
 * @param device      The device number to set in the sys dirent.
 * @param offset      Input/output parameter for the directory offset.
 * @param buffer      The output to copy the dirent into
 * @param buflen      The size of the buffer.
 * @exception E2BIG   The buffer was too small to even fit the dirent header.
 */
SFUNC( aoff_t, snap_readdir,
				dev_t device,
				snap_t *     snap,
				aoff_t *          offset,
				sys_dirent_t *    buffer,
				aoff_t            buflen)
{
	snap_dirent_t	dirent_hdr;
	aoff_t          name_offset;
	size_t          full_sz;

	assert( snap != NULL );
	assert( offset != NULL );
	assert( buffer != NULL );

	/* If the buffer can not fit the dirent header, throw E2BIG */
	if ( SYS_DIRENT_SZ > buflen )
		THROW( E2BIG, SYS_DIRENT_SZ );

	/* If the offset is beyond the end of the snap, read 0 */
	if ( ( *offset + SYS_DIRENT_SZ ) > snap->size ) {
		RETURN( 0 );
	}

	/* Copy the header into the temporary store */
	memcpy( &dirent_hdr, snap->data + *offset, SYS_DIRENT_SZ );

	/* Compute the full dirent size */
	full_sz = dirent_hdr.name_len + SYS_DIRENT_SZ + 1;

	/* Check if the full dirent will fit into the buffer */
	if ( full_sz > buflen )
		THROW( E2BIG, full_sz );

	/* Compute the start offset of the dirent name field within the snap */
	name_offset = *offset + SYS_DIRENT_SZ;

	/* Assert that the snap dirent was consistent. */
	assert( name_offset + dirent_hdr.name_len <= snap->size );

	/* Copy out the name */
	memcpy( buffer->d_name,
	        snap->data + name_offset,
	        dirent_hdr.name_len );

	/* Copy out the header data into buffer in the system dirent format */
	buffer->d_name[ dirent_hdr.name_len ] = 0;
	buffer->d_ino                         = dirent_hdr.inode;
	buffer->d_dev                         = device;
	buffer->d_reclen                      = full_sz;

	/* Move forward the read offset */
	*offset += dirent_hdr.rec_len;

	RETURN( full_sz );
}


