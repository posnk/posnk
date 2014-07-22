/**
 * sys/ipc.h
 *
 * Part of P-OS.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 20-07-2014 - Created
 */

#ifndef __SYS_IPC_H__
#define __SYS_IPC_H__

#include <sys/types.h>

struct ipc_perm {
	uid_t	uid;
	gid_t	gid;
	uid_t	cuid;
	gid_t	cgid;
	mode_t	mode;
};

#define IPC_CREAT	(1<<9)
#define IPC_EXCL	(1<<10)
#define IPC_NOWAIT	(1<<11)

#define IPC_PRIVATE	(0xFFFFFFFE)

#define IPC_RMID	(0)
#define IPC_SET		(1)
#define IPC_STAT	(2)

#endif
