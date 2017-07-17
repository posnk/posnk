/**
 * kernel/sc_time.c
 *
 * Part of P-OS kernel.
 *
 * Contains process related syscalls
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 24-05-2014 - Created
 */

#include <sys/errno.h>
#include "kernel/time.h"
#include "kernel/scheduler.h"
#include "kernel/syscall.h"
#include "kernel/permissions.h"

SYSCALL_DEF0(time)
{
	return (uint32_t) system_time;
}

SYSCALL_DEF1(sleep)
{
	if (!scheduler_wait_time(system_time + (ktime_t) a)) {
		syscall_errno = EINTR;
		return (uint32_t) -1;
	}
	return 0;
}

SYSCALL_DEF1(usleep)
{
	if (!scheduler_wait_micros(system_time_micros + (ktime_t) a)) {
		syscall_errno = EINTR;
		return (uint32_t) -1;
	}
	return 0;
}

SYSCALL_DEF1(stime)
{
	time_t t;
	if (!copy_user_to_kern((void *)a, &t, sizeof(time_t)))
	{
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	if (get_effective_uid() != 0) {
		syscall_errno = EPERM;
		return (uint32_t) -1;
	}
	system_time = t;
	system_time_micros = system_time * 1000000;
	return 0;
}
