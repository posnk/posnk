/**
 * kernel/signals.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-04-2014 - Created
 */

#ifndef __KERNEL_SIGNALS_H__
#define __KERNEL_SIGNALS_H__

#include <signal.h>
#include "kernel/process.h"

#define SIGNAL_ACTION_ABORT		0
#define SIGNAL_ACTION_TERM		1
#define SIGNAL_ACTION_CONT		2
#define SIGNAL_ACTION_IGNORE	3
#define SIGNAL_ACTION_STOP		4

typedef struct {
	/** The signal we are returning from */
	int			sr_sig;
	/** Signal mask to restore after return */
	sigset_t	sr_mask;
	/** Miscellaneous flags */
	int			sr_flags;
} sigrestore_info_t;

void	do_signals();

int		invoke_signal_handler( int signal, siginfo_t *info, void *context );

void	exit_signal_handler( void *ctx );

void	signal_init_process( process_info_t *task );

void	signal_init_task( scheduler_task_t *task );

void (*_sys_signal(	int sig, void (*disp)(int) ))( int );

int _sys_sigaction(	int sig, 
					const struct sigaction *act, 
					struct sigaction *oact );

int _sys_sigaltstack(	const stack_t *ss, 
						stack_t *oss );

int _sys_sigprocmask(	int how, 
						const sigset_t *set, 
						sigset_t *oset );

int _sys_sigsuspend( const sigset_t *sigmask );

int _sys_sigpending( sigset_t *set );

#endif
