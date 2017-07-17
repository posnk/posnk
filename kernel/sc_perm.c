/**
 * kernel/sc_process.c
 *
 * Part of P-OS kernel.
 *
 * Contains process related syscalls
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-04-2014 - Created
 */

#include "kernel/heapmm.h"
#include "kernel/physmm.h"
#include "kernel/paging.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "kernel/permissions.h"
#include "kernel/synch.h"
#include "kernel/syscall.h"
#include <sys/errno.h>

SYSCALL_DEF0(getuid)
{
	return (uid_t) scheduler_current_task->uid;
}

SYSCALL_DEF0(geteuid)
{
	return (uid_t) scheduler_current_task->effective_uid;
}

SYSCALL_DEF0(getgid)
{
	return (gid_t) scheduler_current_task->gid;
}

SYSCALL_DEF0(getegid)
{
	return (gid_t) scheduler_current_task->effective_gid;
}

SYSCALL_DEF1(setuid)
{
	if ((a != scheduler_current_task->uid) && (scheduler_current_task->uid != 0)) {
		syscall_errno = EPERM;
		return -1;
	}
	scheduler_current_task->effective_uid = (uid_t) a;
	scheduler_current_task->uid = (uid_t) a;
	return 0;
}

SYSCALL_DEF1(setgid)
{
	if ((a != scheduler_current_task->gid) && (scheduler_current_task->gid != 0)) {
		syscall_errno = EPERM;
		return -1;
	}
	scheduler_current_task->effective_gid = (gid_t) a;
	scheduler_current_task->gid = (gid_t) a;
	return 0;
}

