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

uint32_t sys_signal(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	return (uint32_t) _sys_signal( a, ( void * ) b );
}

uint32_t sys_ssigex(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	scheduler_current_task->signal_handler_exit_func = (void *) a;
	return 0;
}

uint32_t sys_exitsig( uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	if (!procvmm_check( (void *) a,1)){
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	exit_signal_handler( (void *) a );
	return 0;
}

uint32_t sys_sigprocmask(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	return _sys_sigprocmask( a, (void *) b, (void *) c);
}

uint32_t sys_sigaction(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	return _sys_sigaction( a, (void *) b, (void *) c);
}

uint32_t sys_sigaltstack(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	return _sys_sigaltstack( (void *) a, (void *) b);
}

uint32_t sys_sigsuspend(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	return _sys_sigsuspend( (void *) a );
}

uint32_t sys_sigpending(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{
	return _sys_sigpending( (void *) a );
}


