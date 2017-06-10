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

uint32_t sys_getuid(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uid_t) scheduler_current_task->uid;
}

uint32_t sys_geteuid(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uid_t) scheduler_current_task->effective_uid;
}

uint32_t sys_getgid(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (gid_t) scheduler_current_task->gid;
}

uint32_t sys_getegid(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (gid_t) scheduler_current_task->effective_gid;
}

uint32_t sys_setuid(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	if ((param[0] != scheduler_current_task->uid) && (scheduler_current_task->uid != 0)) {
		syscall_errno = EPERM;
		return -1;
	}
	scheduler_current_task->effective_uid = (uid_t) param[0];
	scheduler_current_task->uid = (uid_t) param[0];
	return 0;
}

uint32_t sys_setgid(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	if ((param[0] != scheduler_current_task->gid) && (scheduler_current_task->gid != 0)) {
		syscall_errno = EPERM;
		return -1;
	}
	scheduler_current_task->effective_gid = (gid_t) param[0];
	scheduler_current_task->gid = (gid_t) param[0];
	return 0;
}

