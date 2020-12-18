/**
 * kernel/scheduler.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-04-2014 - Created
 */

#ifndef __KERNEL_SCHEDULER_H__
#define __KERNEL_SCHEDULER_H__

#include <stddef.h>
#include <stdint.h>
#include <sys/mcontext.h>

typedef uint32_t    tid_t;

typedef struct task scheduler_task_t;

#include "kernel/paging.h"
#include "kernel/process.h"
#include "kernel/synch.h"
#include "kernel/time.h"
#include "util/llist.h"

#define TASK_STATE_RUNNING	(1 << 0)
#define TASK_STATE_READY	(1 << 1)
#define TASK_STATE_STOPPED	(1 << 2)
#define TASK_STATE_TIMEDWAIT_US	(1 << 3)
#define TASK_STATE_TIMEDWAIT	TASK_STATE_TIMEDWAIT_US
#define TASK_STATE_BLOCKED	(1 << 5)
#define TASK_STATE_INTERRUPT	(1 << 6)
#define TASK_STATE_INTERRUPTED	(1 << 8)
#define TASK_STATE_TIMED_OUT	(1 << 9)

#define SCHED_WAIT_OK		(0)
#define SCHED_WAIT_INTR		(-2)
#define SCHED_WAIT_TIMEOUT	(-1)

#define SCHED_WAITF_INTR        (1 << 1)
#define SCHED_WAITF_TIMEOUT     (1 << 2)

#define TASK_GLOBAL         (1<<0)

/**
 * Describes a thread
 */
struct task {

	llist_t         node;

	/** Previous task in the scheduling queue */
	struct task    *prev;

	/** Next task in the scheduling queue */
	struct task    *next;

	/** The task id */
	tid_t           tid;

	/** The task flags */
	int             flags;

	/* Scheduling state */
	int	        state;

	sigset_t        signal_mask;
	stack_t         signal_altstack;

	ticks_t         cpu_ticks;
	ticks_t         cpu_end;

	int		active;

	/** The process this task belongs to */
	process_info_t *process;

	/** Processor state */
	void           *arch_state;

	/** The kernel stack */
	void           *kernel_stack;

	uint32_t        in_syscall;

	/** Wait target */
	semaphore_t    *waiting_on;
	ktime_t         wait_timeout_u;//In microseconds

};

void scheduler_get_mcontext( const scheduler_task_t *_task,
                              mcontext_t *ctx );

void scheduler_set_mcontext( scheduler_task_t *_task,
                             const mcontext_t *ctx );

size_t scheduler_get_state_size( );

extern scheduler_task_t *scheduler_current_task;

int scheduler_init_task(scheduler_task_t *new_task);

void scheduler_reown_task( scheduler_task_t *task, process_info_t *process );

void scheduler_switch_task(scheduler_task_t *new_task);

void scheduler_stop_task( scheduler_task_t *task );

void scheduler_continue_task( scheduler_task_t *task );

void scheduler_interrupt_task( scheduler_task_t *task );

scheduler_task_t *scheduler_get_task( tid_t tid );

void scheduler_fork_main( void * arg );

void scheduler_spawnentry( void (*callee)(void *), void *arg, int s );

int scheduler_do_spawn( scheduler_task_t *new_task, void *callee, void *arg, int s );

int scheduler_free_task ( scheduler_task_t *task );

int scheduler_spawn( void *callee, void *arg, scheduler_task_t **t );

void scheduler_init(void);

void scheduler_reap( scheduler_task_t *task );

int scheduler_wait( semaphore_t *semaphore, utime_t timeout, int flags );

int scheduler_fork(void);

void schedule(void);

void scheduler_set_as_idle(void);

#endif
