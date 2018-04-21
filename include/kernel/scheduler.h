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

typedef uint32_t    tid_t;

typedef struct task scheduler_task_t;

#include "kernel/paging.h"
#include "kernel/process.h"
#include "kernel/synch.h"
#include "kernel/time.h"
#include "util/llist.h"

#define PROCESS_RUNNING 	0
#define PROCESS_WAITING 	1
#define PROCESS_READY		2
#define PROCESS_NO_SCHED	3
#define PROCESS_KILLED		4
#define PROCESS_INTERRUPTED	5
#define PROCESS_TIMED_OUT	6
#define PROCESS_STOPPED		7

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
	int			    state;
	
	sigset_t        signal_mask;
	stack_t         signal_altstack;
	
	ticks_t		 cpu_ticks;
	
	int				active;
	
	/** The process this task belongs to */
	process_info_t *process;
	
	/** Processor state */
	void		   *arch_state;
	
	/** The kernel stack */
	void           *kernel_stack;
	
	uint32_t	 in_syscall;

	/** Wait target */
	semaphore_t    *waiting_on;
	ktime_t         wait_timeout_u;//In microseconds
	ktime_t         wait_timeout_s;//In microseconds

};

extern scheduler_task_t *scheduler_current_task;

int scheduler_init_task(scheduler_task_t *new_task);

void scheduler_reown_task( scheduler_task_t *task, process_info_t *process );

void scheduler_switch_task(scheduler_task_t *new_task);

void scheduler_fork_main( void * arg );

void scheduler_spawnentry( void (*callee)(void *), void *arg, int s );

int scheduler_do_spawn( scheduler_task_t *new_task, void *callee, void *arg, int s );

int scheduler_free_task ( scheduler_task_t *task );

int scheduler_spawn( void *callee, void *arg, scheduler_task_t **t );

void scheduler_init(void);

void scheduler_reap( scheduler_task_t *task );

void scheduler_wait_on(semaphore_t *semaphore);

void scheduler_wait_on_timeout(semaphore_t *semaphore, ktime_t seconds);

void scheduler_wait_on_to_ms(semaphore_t *semaphore, ktime_t micros);

int scheduler_wait_time(ktime_t time);

int scheduler_wait_micros(ktime_t microtime);

int scheduler_fork(void);

void schedule(void);

void scheduler_set_as_idle(void);

#endif
