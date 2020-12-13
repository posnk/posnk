/**
 * kernel/sc_time.c
 *
 * Part of P-OS kernel.
 *
 * Contains time related syscalls
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
	int r;

	/* Wait on the deadline */
	r = scheduler_wait(
		/* semaphore */ NULL,
		/* timeout   */ 10000000UL * a,
		/* flags     */ SCHED_WAITF_INTR | SCHED_WAITF_TIMEOUT );

	/* Handle interrupted wait */
	if ( r == SCHED_WAIT_INTR ) {
		syscall_errno = EINTR;
		return (uint32_t) -1;
	} else {
		return 0;
	}
}

SYSCALL_DEF1(usleep)
{
	int r;

	/* Wait on the deadline */
	r = scheduler_wait(
		/* semaphore */ NULL,
		/* timeout   */ a,
		/* flags     */ SCHED_WAITF_INTR | SCHED_WAITF_TIMEOUT );

	/* Handle interrupted wait */
	if ( r == SCHED_WAIT_INTR ) {
		syscall_errno = EINTR;
		return (uint32_t) -1;
	} else {
		return 0;
	}
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
	system_time_micros = system_time * 1000000UL;
	return 0;
}
