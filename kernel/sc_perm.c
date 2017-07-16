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

uint32_t sys_getuid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f)
{
	return (uid_t) scheduler_current_task->uid;
}

uint32_t sys_geteuid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f)
{
	return (uid_t) scheduler_current_task->effective_uid;
}

uint32_t sys_getgid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f)
{
	return (gid_t) scheduler_current_task->gid;
}

uint32_t sys_getegid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f)
{
	return (gid_t) scheduler_current_task->effective_gid;
}

uint32_t sys_setuid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f)
{
	if ((a != scheduler_current_task->uid) && (scheduler_current_task->uid != 0)) {
		syscall_errno = EPERM;
		return -1;
	}
	scheduler_current_task->effective_uid = (uid_t) a;
	scheduler_current_task->uid = (uid_t) a;
	return 0;
}

uint32_t sys_setgid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f)
{
	if ((a != scheduler_current_task->gid) && (scheduler_current_task->gid != 0)) {
		syscall_errno = EPERM;
		return -1;
	}
	scheduler_current_task->effective_gid = (gid_t) a;
	scheduler_current_task->gid = (gid_t) a;
	return 0;
}

