/**
 * kernel/signal_api.c
 *
 * Part of P-OS kernel.
 *
 * POSIX signal interface
 *
 * Written by Peter Bosch <me@pbx.sh>
 *
 */

#include <signal.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "kernel/earlycon.h"
#include "kernel/scheduler.h"
#include "kernel/streams.h"
#include "kernel/process.h"
#include "kernel/signals.h"
#include "kernel/syscall.h"

void (*_sys_signal(	int sig, void (*disp)(int) ))( int )
{
	void *old;

	/* Verify the signal number */
	if ( sig < 0 || sig >= 32 || sig == SIGKILL || sig == SIGSTOP ) {
		syscall_errno = EINVAL;
		return SIG_ERR;
	}

	old = scheduler_current_task->process->signal_actions[sig].sa_handler;

	scheduler_current_task->process->signal_actions[sig]
		= sig_action_default;
	scheduler_current_task->process->signal_actions[sig].sa_handler
		= disp;

	return old;
}

int _sys_sigaction(	int sig,
					const struct sigaction *act,
					struct sigaction *oact )
{

	/* Verify the signal number */
	if ( sig < 0 || sig >= 32 || sig == SIGKILL || sig == SIGSTOP ) {
		syscall_errno = EINVAL;
		return -1;
	}

	/* Copy the old action to userland if requested */
	if ( oact != NULL ) {
		if ( !procvmm_check( oact, sizeof( struct sigaction ) ) ) {
			syscall_errno = EFAULT;
			return -1;
		}
		memcpy( oact,
				&scheduler_current_task->process->signal_actions[sig],
				sizeof( struct sigaction ) );
	}

	/* Copy the new action from userland if requested */
	if (  act != NULL ) {
		if ( !procvmm_check(  act, sizeof( struct sigaction ) ) ) {
			syscall_errno = EFAULT;
			return -1;
		}
		memcpy( &scheduler_current_task->process->signal_actions[sig],
				 act,
				sizeof( struct sigaction ) );
	}

	return 0;

}

int _sys_sigaltstack(	const stack_t *ss,
						stack_t *oss )
{

	/* Copy the old stack to userland if requested */
	if ( oss != NULL ) {
		if ( !procvmm_check( oss, sizeof( stack_t ) ) ) {
			syscall_errno = EFAULT;
			return -1;
		}
		memcpy( oss,
				&scheduler_current_task->signal_altstack,
				sizeof( stack_t ) );
	}

	/* Copy the new stack from userland if requested */
	if (  ss != NULL ) {
		if ( !procvmm_check(  ss, sizeof( stack_t ) ) ) {
			syscall_errno = EFAULT;
			return -1;
		}

		if ( ss->ss_size < MINSIGSTKSZ ) {
			syscall_errno = ENOMEM;
			return -1;
		}

		if ( ss->ss_flags & ~SS_DISABLE ) {
			syscall_errno = EINVAL;
			return -1;
		}

		//TODO: Check if the stack is usable?

		if ( scheduler_current_task->signal_altstack.ss_flags & SS_ONSTACK ) {
			syscall_errno = EPERM;
			return -1;
		}

		memcpy( &scheduler_current_task->signal_altstack,
				 ss,
				sizeof( stack_t ) );
	}

	return 0;

}

int _sys_sigprocmask(	int how,
						const sigset_t *set,
						sigset_t *oset )
{

	/* Copy the old mask to userland if requested */
	if ( oset != NULL ) {
		if ( !procvmm_check( oset, sizeof( sigset_t ) ) ) {
			syscall_errno = EFAULT;
			return -1;
		}
		memcpy( oset,
				&scheduler_current_task->signal_mask,
				sizeof( sigset_t ) );
	}

	/* If we don't have to update, return */
	if (  set == NULL )
		return 0;

 	if ( !procvmm_check(  set, sizeof( sigset_t ) ) ) {
		syscall_errno = EFAULT;
		return -1;
	}

	switch ( how ) {

		case SIG_BLOCK:
			scheduler_current_task->signal_mask |=  *set;
			break;

		case SIG_SETMASK:
			scheduler_current_task->signal_mask  =  *set;
			break;

		case SIG_UNBLOCK:
			scheduler_current_task->signal_mask &= ~*set;
			break;

		default:
			syscall_errno = EINVAL;
			return -1;
	}

	scheduler_current_task->signal_mask &= ~( 1 << SIGKILL );
	scheduler_current_task->signal_mask &= ~( 1 << SIGSTOP );

	return 0;

}

int _sys_sigsuspend( const sigset_t *sigmask )
{
	semaphore_t sem = 0;
	sigset_t before;

 	if ( !procvmm_check( sigmask, sizeof( sigset_t ) ) ) {
		syscall_errno = EFAULT;
		return -1;
	}

	before = scheduler_current_task->signal_mask;

	scheduler_current_task->signal_mask  = *sigmask;
	scheduler_current_task->signal_mask &= ~( 1 << SIGKILL );
	scheduler_current_task->signal_mask &= ~( 1 << SIGSTOP );

	semaphore_ndown(
		/* semaphore */ &sem,
		/* timeout   */ 0,
		/* flags     */ SCHED_WAITF_INTR );

	scheduler_current_task->signal_mask = before;

	syscall_errno = EINTR;
	return -1;
}

int _sys_sigpending( sigset_t *set )
{

 	if ( !procvmm_check(  set, sizeof( sigset_t ) ) ) {
		syscall_errno = EFAULT;
		return -1;
	}

	*set =	scheduler_current_task->signal_mask &
			scheduler_current_task->process->signal_pending;

	return 0;

}
