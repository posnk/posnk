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

pid_t		  scheduler_pid_counter = 0;
scheduler_task_t *scheduler_current_task;
scheduler_task_t *scheduler_idle_task;
llist_t		 *scheduler_task_list;

void scheduler_init()
{
	scheduler_task_list = (llist_t *) heapmm_alloc(sizeof(llist_t));
	llist_create(scheduler_task_list);
	scheduler_current_task = 
		(scheduler_task_t *) heapmm_alloc(sizeof(scheduler_task_t));
	memset(scheduler_current_task, 0, sizeof(scheduler_task_t));
	/* Initialize process info */
	scheduler_current_task->uid  = 0;
	scheduler_current_task->gid  = 0;
	scheduler_current_task->pgid = scheduler_pid_counter;
	scheduler_current_task->sid = scheduler_pid_counter;
	scheduler_current_task->pid = scheduler_pid_counter++;
	scheduler_current_task->parent_pid = 0;
	scheduler_current_task->name = heapmm_alloc(CONFIG_PROCESS_MAX_NAME_LENGTH);

	scheduler_current_task->fd_table = heapmm_alloc(sizeof(llist_t));
	llist_create(scheduler_current_task->fd_table);

	scheduler_current_task->memory_map = heapmm_alloc(sizeof(llist_t));
	llist_create(scheduler_current_task->memory_map);

	strcpy(scheduler_current_task->name, CONFIG_SYSTEM_PROCESS_NAME);
	/* Initialize process memory info */
	scheduler_current_task->heap_start	= (void *) 0xE0000000;
	scheduler_current_task->heap_end	= (void *) 0x12345678;
		// TOTALLY NOT RELEVANT ON PROCESS ZERO
	scheduler_current_task->stack_bottom	= (void *) 0x12345678;
	scheduler_current_task->stack_top	= (void *) 0x12345678;
	scheduler_current_task->kernel_stack = (void *) 0x12345678;

	signal_init_task( scheduler_current_task );

	/* Initialize process state */
	scheduler_current_task->page_directory = paging_active_dir;

	scheduler_current_task->child_sema	= semaphore_alloc();

	scheduler_current_task->child_events = heapmm_alloc(sizeof(llist_t));
	llist_create(scheduler_current_task->child_events);

	scheduler_current_task->state = PROCESS_READY;
	llist_add_end(scheduler_task_list, (llist_t *) scheduler_current_task);
	scheduler_init_task(scheduler_current_task);//TODO: Handle errors
}

int scheduler_spawn( void *callee, void *arg )
{
	scheduler_task_t *new_task = (scheduler_task_t *) heapmm_alloc(sizeof(scheduler_task_t));
	memset(new_task, 0, sizeof(scheduler_task_t));

	/* Initialize process info */
	new_task->pid  = scheduler_pid_counter++;
	new_task->uid  = scheduler_current_task->uid;
	new_task->gid  = scheduler_current_task->gid;
	new_task->effective_uid  = scheduler_current_task->effective_uid;
	new_task->effective_gid  = scheduler_current_task->effective_gid;
	new_task->pgid = scheduler_current_task->pgid;
	new_task->sid = scheduler_current_task->sid;
	new_task->parent_pid = scheduler_current_task->pid;

	new_task->name = heapmm_alloc(CONFIG_PROCESS_MAX_NAME_LENGTH);
	strcpy(new_task->name, scheduler_current_task->name);

	new_task->fd_table = heapmm_alloc(sizeof(llist_t));
	llist_create(new_task->fd_table);
	stream_copy_fd_table (new_task->fd_table);
	new_task->fd_ctr = scheduler_current_task->fd_ctr;

	new_task->memory_map = heapmm_alloc(sizeof(llist_t));
	llist_create(new_task->memory_map);
	procvmm_copy_memory_map (new_task->memory_map);

	new_task->current_directory = vfs_dir_cache_ref(scheduler_current_task->current_directory);
	new_task->root_directory = vfs_dir_cache_ref(scheduler_current_task->root_directory);
	new_task->root_directory->usage_count++;
	new_task->current_directory->usage_count++;
	/* Initialize proces signal handling */
	memcpy( new_task->signal_actions,
			scheduler_current_task->signal_actions, 
			sizeof(struct sigaction[32]));
	new_task->signal_mask = scheduler_current_task->signal_mask;

	/* Initialize process memory info */
	new_task->heap_start	= scheduler_current_task->heap_start;
	new_task->heap_end	= scheduler_current_task->heap_end;
	new_task->heap_max	= scheduler_current_task->heap_max;
	new_task->stack_bottom	= scheduler_current_task->stack_bottom;
	new_task->stack_top	= scheduler_current_task->stack_top;
	new_task->child_sema	= semaphore_alloc();

	new_task->child_events = heapmm_alloc(sizeof(llist_t));
	llist_create(new_task->child_events);

	/* Initialize process state */
	new_task->state = PROCESS_READY;

	scheduler_init_task(new_task); //TODO: Handle errors

	if (!scheduler_do_spawn( new_task, callee, arg )) {
		llist_add_end(scheduler_task_list, (llist_t *) new_task);
		return new_task->pid;
	}

	return -1;
}

int scheduler_fork()
{
	return scheduler_spawn( scheduler_fork_main, NULL );
}

/**
 * Iterator function that tests for running tasks that are not the current
 */
int scheduler_next_task_iterator (llist_t *node, __attribute__((__unused__)) void *param)
{
	scheduler_task_t *task = (scheduler_task_t *) node;
	if (task == scheduler_current_task)
		return 0;
	else if (task->state == PROCESS_READY)
		return 1;
	else if (task->state == PROCESS_INTERRUPTED)
		return 1;
	else if (task->state == PROCESS_TIMED_OUT)
		return 1;
	else if (task->state == PROCESS_STOPPED) {
		return process_was_continued( task );
	} else if (task->state == PROCESS_WAITING) {
		//earlycon_printf("pid %i is waiting on %x\n",task->pid, task->waiting_on);
		if (process_was_interrupted(task)) {
			//TODO: Figure out a less ugly way of doing this
			if ( task->state == PROCESS_STOPPED )
				return 0;
			task->state = PROCESS_INTERRUPTED;
			task->waiting_on = NULL;
			return 1;
		} else if ( (task->wait_timeout_u != 0) && (task->wait_timeout_u <= system_time_micros) ) {
			task->state = PROCESS_TIMED_OUT;
			task->waiting_on = NULL;
			return 1;//TODO: Signal timeout
		} else if ( (task->wait_timeout_s != 0) && (task->wait_timeout_s <= system_time) ) {
			task->state = PROCESS_TIMED_OUT;
			task->waiting_on = NULL;
			return 1;//TODO: Signal timeout
		} else if ( (task->waiting_on == NULL) || (!semaphore_try_down(task->waiting_on)) )
			return 0;
		task->state = PROCESS_READY;
		return 1;		
	} else
		return 0;
		
}

void schedule()
{
	//TODO: Increment ticks only on preemptive scheduler calls
#ifdef CONFIG_SERIAL_DEBUGGER_TRIG	
	if (debugcon_have_data())
		dbgapi_invoke_kdbg(0);
#endif
	scheduler_current_task->cpu_ticks++;
	scheduler_task_t *next_task = (scheduler_task_t *) llist_iterate_select(scheduler_task_list, &scheduler_next_task_iterator, NULL);
	if (scheduler_current_task->state == PROCESS_RUNNING)
		scheduler_current_task->state = PROCESS_READY;
	if (next_task != 0) {
	//	earlycon_printf("scheduler switch from %i to %i\n", scheduler_current_task->pid, next_task->pid);
		scheduler_switch_task(next_task);
		if (scheduler_current_task->state == PROCESS_READY) 
			scheduler_current_task->state = PROCESS_RUNNING;
		return;
	}
	//TODO: Figure out why this was here
	//if ((scheduler_current_task->state != PROCESS_KILLED ) &&(scheduler_current_task->signal_pending != 0))
	//	scheduler_switch_task(scheduler_current_task);
	if (scheduler_idle_task != 0)
		scheduler_switch_task(scheduler_idle_task);	
	else 
		return;
}

void scheduler_set_as_idle()
{
	scheduler_idle_task = scheduler_current_task;
	scheduler_idle_task->state = PROCESS_NO_SCHED;
}

int scheduler_wait_micros(ktime_t microtime)
{
	scheduler_current_task->waiting_on = NULL;
	scheduler_current_task->wait_timeout_u = microtime;
	scheduler_current_task->wait_timeout_s = 0;
	scheduler_current_task->state = PROCESS_WAITING;
	schedule();
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
