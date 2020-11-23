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
#include "kernel/streams.h"

#define PROC_INODE_FILE(InO)	(         ( ( InO       ) & 0x0FFF ) )
#define PROC_INODE_PID(InO)	( (pid_t) ( ( InO >> 12 ) & 0xFFFF ) )
#define PROC_INODE_TID(InO)	( (tid_t) ( ( InO >> 12 ) & 0xFFFF ) )
#define PROC_INODE( Pid, File ) ( (ino_t) ( (Pid << 12) | File ) )
#define PROC_ROOT_INODE		( 0 )

typedef struct {
	inode_t		 vfs_inode;
	pid_t		 process_id;
	llist_t		*file_list;
} proc_vinode_t;

typedef struct {
	aoff_t           size;
	size_t           alloc_size;
	void *           data;
} proc_snap_t;

struct proc_dirent {
	uint32_t inode;
	uint16_t rec_len;
	uint8_t  name_len;
	uint8_t  file_type;
}  __attribute__((packed)); //8 long

typedef struct proc_dirent proc_dirent_t;

typedef errno_t (*proc_snapopen_t) ( proc_snap_t *snap, ino_t inode );

#define PROC_FLAG_PID_UIDGID (1)

typedef struct proc_file {
	const char *    name;
	mode_t          mode;
	aoff_t          size;
	proc_snapopen_t open;
	uint32_t        flags;
} proc_file_t;

SFUNC( proc_snap_t *,proc_snap_create,
                                aoff_t            alloc_size );

void proc_snap_delete( proc_snap_t *snap );

SVFUNC( proc_snap_trunc,
                                proc_snap_t *     snap,
                                aoff_t            size );

SFUNC( aoff_t, proc_snap_read,
                                proc_snap_t *     snap,
                                aoff_t            offset,
                                void *            buffer,
                                aoff_t            count );

SFUNC( aoff_t, proc_snap_write,
                                proc_snap_t *     snap,
                                aoff_t            offset,
                                const void *      buffer,
                                aoff_t            count );

SVFUNC( proc_snap_appenddir,
                                proc_snap_t *     snap,
                                const char *      name,
                                ino_t             ino );

SFUNC( aoff_t, proc_snap_readdir,
				dev_t device,
				proc_snap_t *     snap,
				aoff_t *          offset,
				sys_dirent_t *    buffer,
				aoff_t            buflen);
SFUNC( dirent_t *, proc_finddir, inode_t *_inode, const char * name );
SFUNC(proc_snap_t *, proc_open_snap, ino_t inode );

extern stream_ops_t proc_dir_ops;
extern stream_ops_t proc_snapfile_ops;

proc_file_t *proc_get_file( ino_t id );
errno_t proc_snap_setstring( proc_snap_t *snap, const char *str );
errno_t proc_snap_setline( proc_snap_t *snap, const char *str );
errno_t proc_process_open ( proc_snap_t *snap, ino_t inode );
errno_t proc_task_open ( proc_snap_t *snap, ino_t inode );
errno_t proc_tasks_list ( proc_snap_t *snap, ino_t inode );
errno_t proc_proc_name_open ( proc_snap_t *snap, ino_t inode );
errno_t proc_proc_state_open ( proc_snap_t *snap, ino_t inode );
errno_t proc_proc_tasks_open ( proc_snap_t *snap, ino_t inode );
errno_t proc_task_state_open ( proc_snap_t *snap, ino_t inode );
errno_t proc_task_syscall_open ( proc_snap_t *snap, ino_t inode );
errno_t proc_task_cpustate_open ( proc_snap_t *snap, ino_t inode );

#endif
