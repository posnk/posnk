/**
 * kernel/scheduler.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 03-04-2014 - Created
 */
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include "kernel/heapmm.h"
#include "kernel/process.h"
#include "kernel/synch.h"
#include "kernel/signals.h"
#include "kernel/streams.h"
#include "kernel/scheduler.h"
#include "kernel/earlycon.h"
#include "kdbg/dbgapi.h"
#include "kernel/paging.h"
#include "util/llist.h"
#include "config.h"

tid_t		  scheduler_tid_counter = 0;
scheduler_task_t *scheduler_current_task;
scheduler_task_t *scheduler_idle_task;
scheduler_task_t *scheduler_task_list;

spinlock_t scheduling_lock = 0;

void scheduler_init()
{
	scheduler_task_list = NULL;
	scheduler_current_task = 
		(scheduler_task_t *) heapmm_alloc(sizeof(scheduler_task_t));
	memset(scheduler_current_task, 0, sizeof(scheduler_task_t));
	
	scheduler_current_task->next = scheduler_current_task;
	scheduler_current_task->prev = scheduler_current_task;
	
	/* Initialize process info */
	scheduler_current_task->kernel_stack = NULL;

	/* Generate thread id */
	scheduler_current_task->tid = scheduler_tid_counter++;

	/* Task is not yet active */
	scheduler_current_task->active = 0;

	signal_init_task( scheduler_current_task );
	
	scheduler_current_task->state = PROCESS_READY;
	
	scheduler_task_list = scheduler_current_task;
	
	scheduler_init_task(scheduler_current_task);//TODO: Handle errors
}

/**
 * Changes the process a task belongs to
 */
void scheduler_reown_task( scheduler_task_t *task, process_info_t *process )
{
	/* If the task already belonged to a process */
	if ( task->process ) {
		/* Remove it from that processes thread list */
		llist_unlink( ( llist_t * ) task );
	}
	
	/* Set the tasks owner process */
	task->process = process;
	
	/* If the task is currently active, we need to switch the MMU
	 * over to the new process's address space */
	if ( task == scheduler_current_task )
		paging_switch_dir( task->process->page_directory );
	
	/* Finally, add the task to the owner's thread list */
	llist_add_end( &process->tasks, ( llist_t * ) task );
}

/**
 * Create a new task within the current address space, and
 * set it up to start executing the callee func with argument arg
 */
int scheduler_spawn( void *callee, void *arg, scheduler_task_t **t )
{
	int status, s;
	scheduler_task_t *new_task;

	/* Allocate task info block */
	new_task = (scheduler_task_t *) heapmm_alloc(sizeof(scheduler_task_t));
	
	if ( !new_task ) {
		status = ENOMEM;
		goto exitfail_a;
	}
		
	memset(new_task, 0, sizeof(scheduler_task_t));

	/* Allocate a task ID */
	new_task->tid = scheduler_tid_counter++;

	/* Initialize task signal handling */
	new_task->signal_altstack = scheduler_current_task->signal_altstack;
	new_task->signal_mask     = scheduler_current_task->signal_mask;

	/* Initialize process state */
	new_task->state = PROCESS_READY;

	/* Initialize task fields */
	status = scheduler_init_task( new_task );

	if ( status )
		goto exitfail_0;

	/* Get interrupt flag */
	s = disable();restore(s);

	/* Do architecture specific tasks (copy state, setup stack etc) */
	status = scheduler_do_spawn( new_task, callee, arg, s );
	
	if ( status )
		goto exitfail_1;
	
	/* Get a lock on the global scheduler state */
	s = spinlock_enter( &scheduling_lock );
	
	//TODO: Any reason we're not using the llist function here?
	new_task->prev = scheduler_task_list->prev;
	new_task->next = scheduler_task_list;
	new_task->next->prev = new_task;
	new_task->prev->next = new_task;
	
	/* Release lock on the scheduler state */
	spinlock_exit( &scheduling_lock, s );
		
	/* If the caller wanted a reference to the task, fill it */
	if ( t )
		*t = new_task;
			
	return 0;

exitfail_1:
	scheduler_free_task( new_task );

exitfail_0:
	heapmm_free( new_task, sizeof( scheduler_current_task ) );

exitfail_a:
	return status;
	
}

/**
 * Sets a task's state
 */
void scheduler_set_task_state( scheduler_task_t *task, int state )
{
	int s;
	
	s = spinlock_enter( &scheduling_lock );
	
	task->state = state;
	
	spinlock_exit( &scheduling_lock, s );
}

void scheduler_reap( scheduler_task_t *task )
{

	int s;

	assert( task != scheduler_task_list );

	s = spinlock_enter( &scheduling_lock );
	
	assert( task != scheduler_current_task );
	
	task->next->prev = task->prev;
	task->prev->next = task->next;
	
	spinlock_exit( &scheduling_lock, s );
	
    /* If this task belonged to a process, remove it from that processes
     * list. */
	if ( task->process ) {
		llist_unlink ((llist_t *) task);
	}
	
	scheduler_free_task( task );
	
	//TODO: Should this function check whether the process is still alive?
	
}

/**
 * Iterator function that tests for running tasks that are not the current
 */
int scheduler_may_run ( scheduler_task_t *task )
{

	if ( task->active )
		return 0;
    
	/* If the task belongs to a process */
	if ( task->process ) {

		/* Check if that process was stopped, and not immediately continued */
		if ( task->process->state == PROCESS_STOPPED ) {
			if ( !process_was_continued( task->process ) )
				return 0;
		} else if ( task->process->state != PROCESS_READY ) {
			return 0;
		}
	} 

	if (task->state == PROCESS_READY)
		return 1; /* Process is ready to run, select it */
	else if (task->state == PROCESS_INTERRUPTED)
		return 1; /* Process was waiting, but got interrupt, select it */
	else if (task->state == PROCESS_TIMED_OUT)
		return 1; /* Process was waiting, but timed out */
	else if (task->state == PROCESS_WAITING) {
		/* Task is waiting on something */

		/* Check if anything interrupted the wait */
		if ( process_was_interrupted(task) ) {
			
			/* TODO: Why are we re-checking this ? */
			if ( task->process && 
				 task->process->state == PROCESS_STOPPED )
				return 0;

			/* Update task state */
			task->state = PROCESS_INTERRUPTED;
			task->waiting_on = NULL;

			/* Select it for running */
			return 1;

		/* Check if the microsecond timeout was hit */
		} else if ( 
				(task->wait_timeout_u != 0) && 
				(task->wait_timeout_u <= system_time_micros) ) {

			/* Update task state */
			task->state = PROCESS_TIMED_OUT;
			task->waiting_on = NULL;

			/* Select it for running */
			return 1;//TODO: Signal timeout
        
		/* Check if the second timeout was hit */
		} else if (
				(task->wait_timeout_s != 0) && 
				(task->wait_timeout_s <= system_time) ) {

			/* Update task state */
			task->state = PROCESS_TIMED_OUT;
			task->waiting_on = NULL;            

			/* Select it for running */
			return 1;//TODO: Signal timeout
        
		/* If the task is waiting on a semaphore, try to acquire it.
		 * If it is not, or if the semaphore could not be acquired, the 
		 * process should keep waiting. */
		} else if ( 
				( task->waiting_on == NULL ) || 
				!semaphore_try_down(task->waiting_on) )
			return 0;

		task->state = PROCESS_READY;
		return 1;		
	} else
		return 0;
		
}

void schedule()
{
	
	int s, swd;
	scheduler_task_t *next_task;
	
#ifdef CONFIG_SERIAL_DEBUGGER_TRIG	
	if (debugcon_have_data())
		dbgapi_invoke_kdbg(0);
#endif
	
	/* Account task activations */
	scheduler_current_task->cpu_ticks++;
	
	if ( scheduler_current_task->process )
		scheduler_current_task->process->cpu_ticks++;

	//TODO: Increment ticks only on preemptive scheduler calls
	
	/* Acquire a lock on the scheduler state */
	s = spinlock_enter( &scheduling_lock );
	
	next_task = scheduler_current_task->next;

	/* Iterate over all tasks until we find one that may run */	
	do {
		if ( scheduler_may_run( next_task ) )
			break;
		next_task = next_task->next;
	} while ( next_task != scheduler_current_task );
	
	/* current_task refers to the previous task now */

	/* schedule() calls can be divided into two main classes:
	 *    pre-emptive: Current task is RUNNING, but a timer interrupt
	 *                 came. In this case, the task is still READY to run.
	 *    cooperative: The task became blocked, and is already marked with the
	 *                 applicable state. */
	if (scheduler_current_task->state == PROCESS_RUNNING)
		scheduler_current_task->state = PROCESS_READY;
	scheduler_current_task->active = 0;
	
	/* The only way the selected task is not runnable, is if there were no
	 * runnable tasks at all. In that case we select the idle task. */
	if ( !scheduler_may_run( next_task ) ) {
		next_task = scheduler_idle_task;
	}
	
	//TODO: Figure out why this was here
	//if ((scheduler_current_task->state != PROCESS_KILLED ) &&(scheduler_current_task->signal_pending != 0))
	//	scheduler_switch_task(scheduler_current_task);
		
	assert ( next_task != NULL );
	
	/* If the next_task is not READY, we should not set it to RUNNING yet. */	
	if ( next_task->state == PROCESS_READY ) 
		next_task->state = PROCESS_RUNNING;
	
	/* Set selected task active */
	next_task->active = 1;
	
	/* If we switched to a new task, perform a context switch */
	swd = next_task != scheduler_current_task;
	
	if ( swd ) { 
    	scheduler_switch_task( next_task );	/* ------------------------- */
    }

	spinlock_exit( &scheduling_lock, s );
	
	
}

void scheduler_spawnentry( void (*callee)(void *), void *arg, int s )
{
	/* We came here from a context switch, which can only originate
	 * in schedule(). This means that we still have a lock on the scheduler
	 * state that has to be released. */
	spinlock_exit( &scheduling_lock, s );
	
	/* Now that the scheduler is in a sane state, defer control to the callee */
	callee( arg );

}

/**
 * Mark the current task as the idle task.
 */
void scheduler_set_as_idle()
{
	scheduler_idle_task = scheduler_current_task;
	scheduler_idle_task->state = PROCESS_NO_SCHED;
}

int scheduler_wait_micros(ktime_t microtime)
{
	/* Setup wait deadline */
	scheduler_current_task->waiting_on = NULL;
	scheduler_current_task->wait_timeout_u = microtime;
	scheduler_current_task->wait_timeout_s = 0;
	scheduler_current_task->state = PROCESS_WAITING;

	/* Yield control */
	schedule();

	/* When we get back here, we either got interrupted or timed out */
	if (scheduler_current_task->state == PROCESS_INTERRUPTED) {
		scheduler_current_task->state = PROCESS_RUNNING;
		return 0;
	}
	scheduler_current_task->state = PROCESS_RUNNING;
	return 1;
}

int scheduler_wait_time(ktime_t time)
{
	scheduler_current_task->waiting_on = NULL;
	scheduler_current_task->wait_timeout_s = time;
	scheduler_current_task->wait_timeout_u = 0;
	scheduler_current_task->state = PROCESS_WAITING;
	schedule();
	if (scheduler_current_task->state == PROCESS_INTERRUPTED) {
		scheduler_current_task->state = PROCESS_RUNNING;
		return 0;
	}
	scheduler_current_task->state = PROCESS_RUNNING;
	return 1;
}

void scheduler_wait_on(semaphore_t *semaphore)
{
	//earlycon_printf("pid %i wants to wait on %x\n",scheduler_current_task->pid, semaphore);
	scheduler_current_task->waiting_on = semaphore;
	scheduler_current_task->wait_timeout_u = 0;
	scheduler_current_task->wait_timeout_s = 0;
	scheduler_current_task->state = PROCESS_WAITING;
	schedule();
	//earlycon_printf("pid %i came out of wait %x\n",scheduler_current_task->pid, semaphore);
}

void scheduler_wait_on_timeout(semaphore_t *semaphore, ktime_t seconds)
{
	//earlycon_printf("pid %i wants to wait on %x\n",scheduler_current_task->pid, semaphore);
	scheduler_current_task->waiting_on = semaphore;
	scheduler_current_task->wait_timeout_u = 0;
	scheduler_current_task->wait_timeout_s = system_time + seconds;
	scheduler_current_task->state = PROCESS_WAITING;
	schedule();
	//earlycon_printf("pid %i came out of wait %x\n",scheduler_current_task->pid, semaphore);
}

void scheduler_wait_on_to_ms(semaphore_t *semaphore, ktime_t micros)
{
	//earlycon_printf("pid %i wants to wait on %x\n",scheduler_current_task->pid, semaphore);
	scheduler_current_task->waiting_on = semaphore;
	scheduler_current_task->wait_timeout_u = system_time_micros+micros;
	scheduler_current_task->wait_timeout_s = 0;
	scheduler_current_task->state = PROCESS_WAITING;
	schedule();
	//earlycon_printf("pid %i came out of wait %x\n",scheduler_current_task->pid, semaphore);
}
