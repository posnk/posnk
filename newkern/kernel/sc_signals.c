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

uint32_t sys_signal(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_signal( param[0], ( void * ) param [1] );
}

uint32_t sys_ssigex(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	scheduler_current_task->signal_handler_exit_func = (void *) param[0];
	return 0;
}

uint32_t sys_exitsig( uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	if (!procvmm_check( (void *) param[0],1)){
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	exit_signal_handler( (void *) param[0] );
	return 0;
}

uint32_t sys_sigprocmask(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return _sys_sigprocmask( param[0], (void *) param[1], (void *) param[2]);
}

uint32_t sys_sigaction(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return _sys_sigaction( param[0], (void *) param[1], (void *) param[2]);
}

uint32_t sys_sigaltstack(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return _sys_sigaltstack( (void *) param[0], (void *) param[1]);
}

uint32_t sys_sigsuspend(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return _sys_sigsuspend( (void *) param[0] );
}

uint32_t sys_sigpending(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return _sys_sigpending( (void *) param[0] );
}


