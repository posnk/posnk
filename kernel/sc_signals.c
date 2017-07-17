/**
 * kernel/sc_signals.c
 *
 * Part of P-OS kernel.
 *
 * Contains signals related syscalls
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 26-05-2014 - Created
 */

#include <sys/errno.h>
#include "kernel/time.h"
#include "kernel/scheduler.h"
#include "kernel/syscall.h"
#include "kernel/permissions.h"
#include "kernel/signals.h"


SYSCALL_DEF2(signal)
{
	return (uint32_t) _sys_signal( a, ( void * ) b );
}

SYSCALL_DEF1(ssigex)
{
	scheduler_current_task->signal_handler_exit_func = (void *) a;
	return 0;
}

SYSCALL_DEF1(exitsig)
{
	if (!procvmm_check( (void *) a,1)){
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	exit_signal_handler( (void *) a );
	return 0;
}

SYSCALL_DEF3(sigprocmask)
{
	return _sys_sigprocmask( a, (void *) b, (void *) c);
}

SYSCALL_DEF3(sigaction)
{
	return _sys_sigaction( a, (void *) b, (void *) c);
}

SYSCALL_DEF2(sigaltstack)
{
	return _sys_sigaltstack( (void *) a, (void *) b);
}

SYSCALL_DEF1(sigsuspend)
{
	return _sys_sigsuspend( (void *) a );
}

SYSCALL_DEF1(sigpending)
{
	return _sys_sigpending( (void *) a );
}


