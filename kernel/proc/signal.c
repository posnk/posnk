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
	task->signal_pending = 0;
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

	if (signal == 0)
		return;

	printf( CON_TRACE, "Sending signal %i to pid %i",
	        signal, process->pid );

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


/**
 * Send a signal to a thread
 */
void thread_send_signal(	scheduler_task_t *task,
							int signal,
							struct siginfo info )
{
	process_info_t *process;

	assert( task != NULL );

	process = task->process;
	assert( process != NULL );

	assert( signal >= 0 && signal < 32 );

	if (signal == 0)
		return;

	printf( CON_TRACE, "Sending signal %i to tid %i",
	        signal, task->tid );

	//TODO: Possible race condition
	/* If we are the first to raise this signal, set siginfo */
	if ( !sigismember( &task->signal_pending, signal ) )
		task->signal_info[ signal ] = info;

	sigaddset( &task->signal_pending, signal );

	/* SIGCONT will discard all stopping signals (even per thread?) */
	if ( signal == SIGCONT ) {//TODO: Fix this
		sigdelset( &process->signal_pending, SIGSTOP );
		sigdelset( &process->signal_pending, SIGTSTP );
		sigdelset( &process->signal_pending, SIGTTIN );
		sigdelset( &process->signal_pending, SIGTTOU );
		sigdelset( &task->signal_pending, SIGSTOP );
		sigdelset( &task->signal_pending, SIGTSTP );
		sigdelset( &task->signal_pending, SIGTTIN );
		sigdelset( &task->signal_pending, SIGTTOU );
	}

	/* Stopping signals will discard SIGCONT */
	if (	signal == SIGSTOP ||
			signal == SIGTSTP ||
			signal == SIGTTIN ||
			signal == SIGTTOU ) {
		sigdelset( &process->signal_pending, SIGCONT );
		sigdelset( &task->signal_pending, SIGCONT );
	}

	process_handle_signal_scheduling( process );

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
	sigset_t sig_pending = process->signal_pending;

	if ( sig_pending == 0 )
		return;

	if ( 	sigismember( &sig_pending, SIGCONT ) ||
			sigismember( &sig_pending, SIGKILL ) ) {
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
	sigset_t sig_pending;

	if ( !task->process )
		return;

	sig_pending = task->signal_pending | task->process->signal_pending;

	/* Apply signal mask */
	sig_pending &= ~task->signal_mask;

	for ( sig = 0; sig < 32; sig++ ) {

		if ( !sigismember( &sig_pending, sig ) )
			continue;

		act = &task->process->signal_actions[sig];

		if ( act->sa_handler == SIG_IGN )
			continue;

		if ( act->sa_handler != SIG_DFL ) {
			/* Signal had a handler registered, interrupt any waits */
			scheduler_interrupt_task( task );
			return;
		}

	}

	/* This would imply resuming a task even if a signal was ignored */
	if ( sig_pending )
		scheduler_interrupt_task( task );
}

int process_signal_handler_default( int signal, int thread )
{

	process_info_t *cproc;
	scheduler_task_t *ctask;

	ctask = scheduler_current_task;
	cproc = ctask->process;

	assert( ctask != NULL );

	cproc->last_signal = signal;

	switch ( signal_default_actions[signal] ) {
		case SIGNAL_ACTION_ABORT:
			//TODO: Dump core
		case SIGNAL_ACTION_TERM:
			cproc->term_cause = PROCESS_TERM_SIGNAL;
			cproc->state = PROCESS_KILLED;
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

/**
 * Handles a single pending signal
 * @param	sig	The signal number
 * @return	Non zero when more signals can be handled before returning to user
 *			mode
 */
int process_handle_signal( int sig, int thread )
{

	process_info_t *cproc;
	scheduler_task_t *ctask;
	struct sigaction *act;
	struct siginfo *info;

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
		return process_signal_handler_default( sig, thread );

	} else {

		/* We have a real handler in place, this architecture specific */
		/* function should take care of the relevant sa_flags */
		//TODO: Implement siginfo and ucontext

		info = thread ? &ctask->signal_info[sig] :
		                &cproc->signal_info[sig];

		invoke_signal_handler( sig, info, NULL );

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

	//TODO: Find out why i thought this was important
	//Hunch: Interrupting a system call might not immediately return to userland
	//		 hence, we should hold off processing signals any further until it
	//		 has handled the interrupted state.
	if ( ctask->state & TASK_STATE_INTERRUPTED )
		return;

	/* Determine which signals we need to handle per thread */
	sig_active = ctask->signal_pending & ~ctask->signal_mask;

	/* Iterate over the signal set */
	for ( sig = 0; sig < 32; sig++ ) {

		/* Only handle pending signals */
		if ( !sigismember( &sig_active, sig ) )
			continue;

		/* Remove the signal from the pending set */
		sigdelset( &ctask->signal_pending, sig );

		debugcon_printf( "handling signal: %i\n", sig );

		/* Handle the signal */
		if ( !process_handle_signal( sig, /* thread */ 1 ) )
			break;

	}

	/* Determine which signals we need to handle per process */
	sig_active = ctask->signal_pending & ~ctask->signal_mask;

	/* Iterate over the signal set */
	for ( sig = 0; sig < 32; sig++ ) {

		/* Only handle pending signals */
		if ( !sigismember( &sig_active, sig ) )
			continue;

		/* Remove the signal from the pending set */
		sigdelset( &cproc->signal_pending, sig );

		debugcon_printf( "handling signal: %i\n", sig );

		/* Handle the signal */
		if ( !process_handle_signal( sig, /* thread */ 0 ) )
			break;

	}

}

typedef struct sig_pgrp_param {
	pid_t	group;
	int	signal;
	struct siginfo info;
} sig_pgrp_param_t;

uint32_t  process_signal_pgroup_numdone = 0;

/**
 * Iterator function that signals up processes with the given pgid
 */
int process_sig_pgrp_iterator (llist_t *node, void *param )
{
	sig_pgrp_param_t *p = (sig_pgrp_param_t *) param;
	process_info_t *process = (process_info_t *) node;
	if ((process->pid > 1) && (process->pid != 2) && (process->pgid == p->group)) {
		if (get_perm_class(process->uid, process->gid) != PERM_CLASS_OWNER){
			syscall_errno = EPERM;
			return 0;
		}
		process_send_signal(process, p->signal, p->info);
		process_signal_pgroup_numdone++;
	}
	return 0;
}

/* TODO: Add some checks for existence to signal to caller */
int process_signal_pgroup(pid_t pid, int signal, struct siginfo info)
{
	sig_pgrp_param_t param;
	param.group = pid;
	param.signal = signal;
	param.info = info;
	process_signal_pgroup_numdone = 0;
	llist_iterate_select( process_list, &process_sig_pgrp_iterator, (void *) &param);
	debugcon_printf(" sent %i to %i tasks in group %i\n", signal, process_signal_pgroup_numdone, pid);
	return process_signal_pgroup_numdone;
}



