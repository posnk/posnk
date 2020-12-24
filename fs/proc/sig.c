/**
 * fs/proc/sig.c
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
#include <sys/procfs.h>

#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/vfs.h"
#include "kernel/earlycon.h"
#include "kernel/streams.h"
#include "fs/proc/proc.h"

static SFUNC( aoff_t, sig_read,
       stream_info_t *stream,
       void *buffer,
       aoff_t length )
{
    procsig_t *out;
    struct process_debug_sig *sig;
    process_info_t *proc;
    pid_t pid;

    pid = (pid_t) stream->impl;

    if ( length != sizeof(procsig_t) )
        THROW( EINVAL, -1 );
    out = buffer;

    proc = process_get( pid );

    if ( proc == NULL )
        THROW( EINVAL, -1 );

    sig = process_debug_getsig( proc );
    if ( !sig )
        RETURN( 0 );

    out->signal = sig->signal;
    out->info = sig->info;
    if ( sig->task )
        out->task = sig->task->tid;
    else
        out->task = 0;

    RETURN(length);
}

static SFUNC( aoff_t, sig_write,
       stream_info_t *stream,
       const void *buffer,
       aoff_t length )
{
    const procsig_t *in;
    struct process_debug_sig sig;
    process_info_t *proc;
    pid_t pid;

    pid = (pid_t) stream->impl;
    proc = process_get( pid );
    if ( proc == NULL )
        THROW( EINVAL, -1 );

    if ( length == 1 ){
        if ( *(const char*) buffer )
            process_debug_start( proc );
        else
            process_debug_stop( proc );
        RETURN( length );
    } else if ( length != sizeof(procsig_t) )
        THROW( EINVAL, -1 );
    in = buffer;


    sig.signal = in->signal;
    sig.info = in->info;

    if ( in->task )
        sig.task = scheduler_get_task( in->task );
    else
        sig.task = NULL;

    process_deliver_signal( proc, &sig );

    RETURN(length);
}

static SVFUNC( sig_close, stream_info_t *stream )
{

    process_info_t *proc;
    pid_t pid;

    pid = (pid_t) stream->impl;

    proc = process_get( pid );

    if ( proc ) {
        process_debug_detach( proc );
    }

    stream->inode->open_count--;

    /* Release the inode */
    vfs_inode_release(stream->inode);

    /* Release the inode */
    vfs_dir_cache_release(stream->dirc);

    RETURNV;

}

static stream_ops_t sig_ops = {
        .close = sig_close,
        .read  = sig_read,
        .write = sig_write,
};

SVFUNC ( proc_open_sig_inode, inode_t *inode, stream_info_t *stream )
{

    process_info_t *proc;
    pid_t pid;

    pid = PROC_INODE_PID( inode->id );

    proc = process_get( pid );
    if ( proc == NULL )
        THROWV( EINVAL );

    stream->type		= STREAM_TYPE_EXTERNAL;
    stream->ops         = &sig_ops;
    stream->impl_flags  =  STREAM_IMPL_FILE_CHDIR |
                           STREAM_IMPL_FILE_CHMOD |
                           STREAM_IMPL_FILE_CHOWN |
                           STREAM_IMPL_FILE_TRUNC;

    stream->impl        = (void *) pid;

    return process_debug_attach( proc );


}