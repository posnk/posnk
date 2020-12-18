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
#include "fs/snap/snap.h"

#define PROC_INODE_FILE(InO)    (         ( ( InO       ) & 0x0FFF ) )
#define PROC_INODE_PID(InO)     ( (pid_t) ( ( InO >> 12 ) & 0xFFFF ) )
#define PROC_INODE_TID(InO)     ( (tid_t) ( ( InO >> 12 ) & 0xFFFF ) )
#define PROC_INODE( Pid, File ) ( (ino_t) ( (Pid << 12) | File ) )

#define PROC_INO_ROOT           ( 0x000 )
#define PROC_INO_P_DIR          ( 0x001 )
#define PROC_INO_P_NAME         ( 0x002 )
#define PROC_INO_P_STATE        ( 0x003 )
#define PROC_INO_P_TASKS        ( 0x004 )
#define PROC_INO_T_DIR          ( 0x005 )
#define PROC_INO_T_STATE        ( 0x006 )
#define PROC_INO_T_SYSCALL      ( 0x007 )
#define PROC_INO_T_MCONTEXT     ( 0x008 )

typedef errno_t (*proc_snapopen_t) ( snap_t *snap, ino_t inode );

#define PROC_FLAG_PID_UIDGID (1)

typedef struct proc_file {
	const char *    name;
	mode_t          mode;
	aoff_t          size;
	proc_snapopen_t open;
	uint32_t        flags;
} proc_file_t;

SFUNC( dirent_t *, proc_finddir, inode_t *_inode, const char * name );
SFUNC(snap_t *, proc_open_snap, ino_t inode );

void proc_init_files( void );

extern stream_ops_t proc_dir_ops;
extern stream_ops_t proc_snapfile_ops;

proc_file_t *proc_get_file( ino_t id );
errno_t snap_setstring( snap_t *snap, const char *str );
errno_t snap_setline( snap_t *snap, const char *str );
errno_t proc_process_open ( snap_t *snap, ino_t inode );
errno_t proc_task_open ( snap_t *snap, ino_t inode );
errno_t proc_tasks_list ( snap_t *snap, ino_t inode );
errno_t proc_proc_name_open ( snap_t *snap, ino_t inode );
errno_t proc_proc_state_open ( snap_t *snap, ino_t inode );
errno_t proc_proc_tasks_open ( snap_t *snap, ino_t inode );
errno_t proc_task_state_open ( snap_t *snap, ino_t inode );
errno_t proc_task_syscall_open ( snap_t *snap, ino_t inode );
errno_t proc_task_mcontext_open ( snap_t *snap, ino_t inode );

#endif
