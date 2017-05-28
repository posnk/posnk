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

#define SIG_ERR (0xFFFFFFFF)


#define SIG_SETMASK 0	/* set mask with sigprocmask() */
#define SIG_BLOCK 1	/* set of signals to block */
#define SIG_UNBLOCK 2	/* set of signals to, well, unblock */


uint32_t sys_signal(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	void *oldval;
	int signum = (int) param[0];
	if (signum < 1 || signum >= 32) {
		syscall_errno = EINVAL;
		return (uint32_t) SIG_ERR;
	}
	oldval = scheduler_current_task->signal_handler_table[signum];
	scheduler_current_task->signal_handler_table[signum] = (void *) param[1];
	return (uint32_t) oldval;
}

uint32_t sys_ssigex(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	scheduler_current_task->signal_handler_exit_func = (void *) param[0];
	return 0;
}

uint32_t sys_exitsig( uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	scheduler_exit_signal_handler( (void *) param[0] );
	return 0;
}

uint32_t sys_sigprocmask(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	uint32_t oldval,t;
	oldval = scheduler_current_task->signal_mask_bitmap;
	if (!copy_user_to_kern((void *)param[1], &t, sizeof(uint32_t))) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	switch(param[0]) {
		case SIG_SETMASK:
			scheduler_current_task->signal_mask_bitmap = t;
			break;
		case SIG_BLOCK:
			scheduler_current_task->signal_mask_bitmap |= t;
			break;
		case SIG_UNBLOCK:
			scheduler_current_task->signal_mask_bitmap &= ~t;
			break;
	}
	copy_kern_to_user(&oldval, (void *)param[2], sizeof(uint32_t));
	return 0;
}
