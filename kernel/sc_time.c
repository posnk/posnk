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

uint32_t sys_time(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	return (uint32_t) system_time;
}

uint32_t sys_sleep(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	if (!scheduler_wait_time(system_time + (ktime_t) a)) {
		syscall_errno = EINTR;
		return (uint32_t) -1;
	}
	return 0;
}

uint32_t sys_usleep(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	if (!scheduler_wait_micros(system_time_micros + (ktime_t) a)) {
		syscall_errno = EINTR;
		return (uint32_t) -1;
	}
	return 0;
}

uint32_t sys_stime(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
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
