/**
 * fs/proc/proc.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 18-10-2015 - Created
 */

#ifndef __FS_PROC_PROC_H__
#define __FS_PROC_PROC_H__

#include <sys/types.h>
#include "kernel/vfs.h"
#include "util/llist.h"

#define PROC_INODE_PID(InO)	( (pid_t) ( ( InO << 16 ) & 0xFFFF ))
#define PROC_INODE_FILE(InO)	(         ( ( InO       ) & 0xFFFF ))

#define PROC_ROOT_INODE		(0)

typedef struct {
	inode_t		 vfs_inode;
	pid_t		 process_id;
	llist_t		*file_list;
} proc_vinode_t;

typedef struct {
	llist_t		 link;
	char		*name;
	ino_t		 ino_id;
	stream_ops_t	*ops;
} proc_dirent_t;

#endif
