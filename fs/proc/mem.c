/**
 * fs/proc/mem.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
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

physmap_t *map_page_other( pid_t pid, void *addr ) {
    physaddr_t  fr;
    process_info_t *proc;

    proc = process_get( pid );

    if ( proc == NULL )
        return NULL;

    fr = paging_get_physical_address_other(
            proc->page_directory,
            addr );

    if ( !fr )
        return NULL;

    fr = PAGE_ROUND_DOWN( fr );

    return paging_map_phys_range( fr, PAGE_SIZE, PAGING_PAGE_FLAG_RW );

}

SFUNC( aoff_t, mem_read,
       stream_info_t *stream,
       void *buffer,
       aoff_t length )
{
    physaddr_t inpage;
    physmap_t *map;

    map = map_page_other( (pid_t) stream->impl, (void *) stream->offset );

    if ( map == NULL )
        THROW( EINVAL, -1 );

    inpage = stream->offset & PAGE_ADDR_MASK;

    if ( inpage + length > PAGE_SIZE )
        length = PAGE_SIZE - inpage;

    memcpy( buffer, map->virt + inpage, length );

    stream->offset += length;

    paging_unmap_phys_range( map );

    RETURN( length );
}

SFUNC( aoff_t, mem_write,
       stream_info_t *stream,
       const void *buffer,
       aoff_t length )
{
    physaddr_t inpage;
    physmap_t *map;

    map = map_page_other( (pid_t) stream->impl, (void *) stream->offset );

    if ( map == NULL )
        THROW( EINVAL, -1 );

    inpage = stream->offset & PAGE_ADDR_MASK;

    if ( inpage + length > PAGE_SIZE )
        length = PAGE_SIZE - inpage;

    memcpy( map->virt + inpage, buffer, length );

    stream->offset += length;

    paging_unmap_phys_range( map );

    RETURN( length );
}

SVFUNC( mem_close, stream_info_t *stream )
{


    stream->inode->open_count--;

    /* Release the inode */
    vfs_inode_release(stream->inode);

    /* Release the inode */
    vfs_dir_cache_release(stream->dirc);

    RETURNV;

}


static stream_ops_t mem_ops = {
        .close = mem_close,
        .read = mem_read,
        .write =  mem_write,
};



SVFUNC ( proc_open_mem_inode, inode_t *inode, stream_info_t *stream )
{
    stream->type		= STREAM_TYPE_EXTERNAL;
    stream->ops         = &mem_ops;
    stream->impl_flags  =  STREAM_IMPL_FILE_CHDIR |
                           STREAM_IMPL_FILE_CHMOD |
                           STREAM_IMPL_FILE_CHOWN |
                           //STREAM_IMPL_FILE_FSTAT |
                           STREAM_IMPL_FILE_LSEEK |
                           STREAM_IMPL_FILE_TRUNC;

    stream->impl        = (void *) PROC_INODE_PID( inode->id );

    return 0;

}