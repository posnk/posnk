/**
 * fs/snap/snap.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 * Changelog:
 * 23-11-2020 - Created
 */

#ifndef __FS_SNAP_SNAP_H__
#define __FS_SNAP_SNAP_H__

#include <sys/types.h>
#include "kernel/vfs.h"
#include "util/llist.h"
#include "kernel/streams.h"

typedef struct {
	aoff_t           size;
	size_t           alloc_size;
	void *           data;
} snap_t;

struct snap_dirent {
	uint32_t inode;
	uint16_t rec_len;
	uint8_t  name_len;
	uint8_t  file_type;
}  __attribute__((packed)); //8 long

typedef struct snap_dirent snap_dirent_t;

SFUNC( snap_t *,snap_create, aoff_t alloc_size );

void snap_delete( snap_t *snap );

SVFUNC( snap_trunc, snap_t *snap, aoff_t size );

SFUNC( aoff_t, snap_read,
                                snap_t *     snap,
                                aoff_t            offset,
                                void *            buffer,
                                aoff_t            count );

SFUNC( aoff_t, snap_write,
                                snap_t *     snap,
                                aoff_t            offset,
                                const void *      buffer,
                                aoff_t            count );

SVFUNC( snap_appenddir,
                                snap_t *     snap,
                                const char *      name,
                                ino_t             ino );

SFUNC( aoff_t, snap_readdir,
				dev_t device,
				snap_t *     snap,
				aoff_t *          offset,
				sys_dirent_t *    buffer,
				aoff_t            buflen);
SFUNC( dirent_t *, proc_finddir, inode_t *_inode, const char * name );
SFUNC(snap_t *, proc_open_snap, ino_t inode );

#endif
