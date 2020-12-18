/**
 * kernel/signal.c
 *
 * Part of P-OS kernel.
 *
 * POSIX signal interface
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 28-05-2017 - Created
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

/**
 * The default behaviour for each signal
 * ( this is used when SIG_DFL is set )
 */
int signal_default_actions[] =
{
	SIGNAL_ACTION_IGNORE,   //UNDEFINED
	SIGNAL_ACTION_TERM,     //SIGHUP
	SIGNAL_ACTION_TERM,     //SIGINT
	SIGNAL_ACTION_ABORT,    //SIGQUIT
	SIGNAL_ACTION_ABORT,    //SIGILL
	SIGNAL_ACTION_ABORT,    //SIGTRAP
	SIGNAL_ACTION_ABORT,    //SIGABRT
	SIGNAL_ACTION_IGNORE,   //SIGWINCH
	SIGNAL_ACTION_ABORT,    //SIGFPE
	SIGNAL_ACTION_TERM,     //SIGKILL
	SIGNAL_ACTION_ABORT,    //SIGBUS
	SIGNAL_ACTION_ABORT,    //SIGSEGV
	SIGNAL_ACTION_ABORT,    //SIGSYS
	SIGNAL_ACTION_TERM,     //SIGPIPE
	SIGNAL_ACTION_TERM,     //SIGALRM
	SIGNAL_ACTION_TERM,     //SIGTERM
	SIGNAL_ACTION_TERM,     //SIGUSR1
	SIGNAL_ACTION_TERM,     //SIGUSR2
	SIGNAL_ACTION_IGNORE,   //SIGCHLD 18
	SIGNAL_ACTION_IGNORE,   //UNDEFINED 19
	SIGNAL_ACTION_IGNORE,   //UNDEFINED 20
	SIGNAL_ACTION_IGNORE,   //SIGURG 21
	SIGNAL_ACTION_TERM,     //SIGPOLL
	SIGNAL_ACTION_STOP,     //SIGSTOP
	SIGNAL_ACTION_STOP,     //SIGTSTP
	SIGNAL_ACTION_CONT,     //SIGCONT
	SIGNAL_ACTION_STOP,     //SIGTTIN
	SIGNAL_ACTION_STOP,     //SIGTTOU
	SIGNAL_ACTION_TERM,     //SIGVTALRM
	SIGNAL_ACTION_TERM,     //SIGPROF
	SIGNAL_ACTION_ABORT,    //SIGXCPU
	SIGNAL_ACTION_ABORT,    //SIGXFSZ
};

/** The default sigaction */
struct sigaction sig_action_default = {
	.sa_handler = SIG_DFL,
	.sa_flags	= 0,
	.sa_mask	= 0
};


void signal_init_process( process_info_t *task ) {
	int i;
	task->signal_pending = 0;
	for ( i = 0; i < 32; i++ )
		task->signal_actions[i] = sig_action_default;
}


void signal_init_task( scheduler_task_t *task ) {
	task->signal_mask = 0;
	task->signal_altstack.ss_flags = SS_DISABLE;
}

/**
 * Add signal to signal set
 * @return Error status
 */
inline int sigaddset( sigset_t *set, int sig )
{
	if ( set == NULL )
		return EFAULT;
	*set |= 1u << sig;
	return 0;
}

/**
 * Remove signal from signal set
 * @return Error status
 */
inline int sigdelset( sigset_t *set, int sig )
{
	if ( set == NULL )
		return EFAULT;
	*set &= ~(1u << sig);
	return 0;
}

/**
 * Test if signal is a member of a set
 */
inline int sigismember( const sigset_t *set, int sig )
{
	return *set & (1u << sig);
}

/**
 * Fill the signal set
 */
inline int sigfillset( sigset_t *set )
{
	*set = ~0;
	return 0;
}

/**
 * Clear the signal set
 */
inline int sigemptyset( sigset_t *set )
{
	*set = 0;
	return 0;
}
void process_handle_signal_scheduling( process_info_t *process );

/**
 * Send a signal to a process
 */
void process_send_signal(	process_info_t *process,
							int signal,
							struct siginfo info )
{
	assert( signal >= 0 && signal < 32 );

	debugcon_printf("sending signal to pid %i : %i\n",process->pid, signal);

	if (signal == 0)
		return;

	//TODO: Possible race condition
	/* If we are the first to raise this signal, set siginfo */
	if ( ! sigismember( &process->signal_pending, signal ) )
		process->signal_info[ signal ] = info;

	sigaddset( &process->signal_pending, signal );

	/* SIGCONT will discard all stopping signals */
	if ( signal == SIGCONT ) {
		sigdelset( &process->signal_pending, SIGSTOP );
		sigdelset( &process->signal_pending, SIGTSTP );
		sigdelset( &process->signal_pending, SIGTTIN );
		sigdelset( &process->signal_pending, SIGTTOU );
	}

	/* Stopping signals will discard SIGCONT */
	if (	signal == SIGSTOP ||
			signal == SIGTSTP ||
			signal == SIGTTIN ||
			signal == SIGTTOU ) {
		sigdelset( &process->signal_pending, SIGCONT );
	}

	process_handle_signal_scheduling( process );

}

int process_signal_handler_default(int signal)
{

	process_info_t *cproc;
	scheduler_task_t *ctask;

	ctask = scheduler_current_task;
	cproc = ctask->process;

	assert( ctask != NULL );

	//debugcon_printf("sigrecv: %i\n", scheduler_current_task->pid);
	cproc->last_signal = signal;

	switch (signal_default_actions[signal]) {
		case SIGNAL_ACTION_ABORT:
			//TODO: Dump core
		case SIGNAL_ACTION_TERM:
			cproc->term_cause = PROCESS_TERM_SIGNAL;
			cproc->state = PROCESS_KILLED;
			//earlycon_printf("killed: %i\n", scheduler_current_task->pid);
			break;
		case SIGNAL_ACTION_STOP:
			process_stop( cproc );
			process_child_event( cproc, PROCESS_CHILD_STOPPED );
			schedule();
			return 1;
		case SIGNAL_ACTION_IGNORE:
		default:
			return 1;
	}

	if ( cproc->state == PROCESS_KILLED ) {
		debugcon_printf( "killedby: %i\n", signal );
		process_child_event( cproc, PROCESS_CHILD_KILLED);
		stream_do_close_all( cproc );
		procvmm_clear_mmaps();
		process_deschedule( cproc );
		schedule();
		return 0;
	}

	return 0;

}
void thread_handle_signal_scheduling( scheduler_task_t *task );

/**
 * This function handles process-wide signal scheduling updates,
 * such as continue.
 *
 * @param process	The process to check
 */
void process_handle_signal_scheduling( process_info_t *process )
{
	llist_t *c, *n, *h;

	/* SIGKILL and SIGCONT resume the process regardless of the mask value */
	sigset_t sig_active = process->signal_pending;

	if ( sig_active == 0 )
		return;

	if ( 	sigismember( &sig_active, SIGCONT ) ||
			sigismember( &sig_active, SIGKILL ) ) {
		//TODO: Should SIGKILL interrupt the syscall currently being executed?
		if ( process->state == PROCESS_STOPPED )
			process_continue( process );
	}

	h = &process->tasks;

	for ( c = h->next, n = c->next; c != h; c = n, n = c->next )
		thread_handle_signal_scheduling( ( scheduler_task_t * ) c );

}

void thread_handle_signal_scheduling( scheduler_task_t *task )
{
	int sig;
	struct sigaction *act;

	//TODO: Figure out whether to interrupt calls on an SIG_IGN action
	sigset_t sig_active;

	if ( !task->process )
		return;

	sig_active = task->process->signal_pending & ~task->signal_mask;

	if ( sig_active == 0 )
		return;

	for ( sig = 0; sig < 32; sig++ ) {

		if ( !sigismember( &sig_active, sig ) )
			continue;

		act = &task->process->signal_actions[sig];

		if ( act->sa_handler == SIG_IGN )
			continue;

		if ( act->sa_handler != SIG_DFL ) {
			/* Signal had a handler registered, interrupt any waits */
			scheduler_interrupt_task( task );
			return;
		}

/*		if ( signal_default_actions[ sig ] == SIGNAL_ACTION_STOP ) {
			sigdelset( &task->process->signal_pending, sig );
			process_stop( task->process );
			process_child_event( task->process, PROCESS_CHILD_STOPPED );
			return;
		}*/
	}

	/* This would imply resuming a task even if a signal was ignored */
	if ( sig_active )
		scheduler_interrupt_task( task );
}

/**
 * Handles a single pending signal
 * @param	sig	The signal number
 * @return	Non zero when more signals can be handled before returning to user
 *			mode
 */
int process_handle_signal( int sig )
{

	process_info_t *cproc;
	scheduler_task_t *ctask;
	struct sigaction *act;

	ctask = scheduler_current_task;
	cproc = ctask->process;

	assert( cproc != NULL );

	/* Get the sigaction for the signal */
	act = &cproc->signal_actions[ sig ];

	/* Check for the magic dispositions */
	if ( act->sa_handler == SIG_IGN ) {

		/* Check whether we have to reset the handler */
		//TODO: Check if this is the right way to interpret POSIX
		if ( act->sa_flags & SA_RESETHAND )
			act->sa_handler = SIG_DFL;

		/* The signal should be ignored */
		return 1;

	} else if ( act->sa_handler == SIG_DFL ) {

		/* The default action should be invoked */
		return process_signal_handler_default( sig );

	} else {

		/* We have a real handler in place, this architecture specific */
		/* function should take care of the relevant sa_flags */
		//TODO: Implement siginfo and ucontext
		invoke_signal_handler( sig, &cproc->signal_info[sig], NULL );

		return 0;

	}

}

/**
 * Handle all unmasked pending signals
 */
void do_signals()
{
	process_info_t *cproc;
	scheduler_task_t *ctask;

	int sig;
	sigset_t sig_active;

	ctask = scheduler_current_task;
	cproc = ctask->process;

	if ( !cproc )
		return;

	/* Determine which signals we need to handle */
	sig_active = cproc->signal_pending & ~ctask->signal_mask;

	/* If there are none, we have nothing to do */
	if (sig_active == 0)
		return;

	debugcon_printf("sighandle: %i\n", cproc->pid);

	//TODO: Find out why i thought this was important
	//Hunch: Interrupting a system call might not immediately return to userland
	//		 hence, we should hold off processing signals any further until it
	//		 has handled the interrupted state.
	if ( ctask->state & TASK_STATE_INTERRUPTED )
		return;

	/* Iterate over the signal set */
	for ( sig = 0; sig < 32; sig++ ) {

		/* Only handle pending signals */
		if ( !sigismember( &sig_active, sig ) )
			continue;

		/* Remove the signal from the pending set */
		sigdelset( &cproc->signal_pending, sig );

		debugcon_printf( "handling signal: %i\n", sig );

		/* Handle the signal */
		if ( !process_handle_signal( sig ) )
			break;

	}

}

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




