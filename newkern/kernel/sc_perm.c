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
