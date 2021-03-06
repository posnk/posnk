/**
 * kernel/process.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-04-2014 - Created
 */
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <sys/errno.h>
#include "kernel/process.h"
#include "kernel/synch.h"
#include "kernel/scheduler.h"
#include "kernel/syscall.h"
#include "kernel/paging.h"
#include "kernel/permissions.h"
#include "kernel/earlycon.h"
#include "kernel/streams.h"
#include "kernel/elfloader.h"
#include "kernel/signals.h"
#include "kernel/heapmm.h"
#include "config.h"

process_info_t  kernel_process;
llist_t        *process_list;

pid_t           pid_counter = 1;

void create_kprocess()
{

	/* Initialize process info */
	kernel_process.uid  = 0;
	kernel_process.gid  = 0;
	kernel_process.pgid = 0;
	kernel_process.sid  = 0;
	kernel_process.pid  = 0;
	kernel_process.parent_pid = 0;
	kernel_process.name = CONFIG_SYSTEM_PROCESS_NAME;

	kernel_process.fd_table = heapmm_alloc(sizeof(llist_t));
	llist_create(kernel_process.fd_table);

	kernel_process.memory_map = heapmm_alloc(sizeof(llist_t));
	llist_create(kernel_process.memory_map);

	/* Initialize process memory info */
	kernel_process.heap_start	 = (void *) 0xE0000000;
	kernel_process.heap_end  	 = (void *) 0x12345678;
	// TOTALLY NOT RELEVANT ON PROCESS ZERO
	kernel_process.stack_bottom = (void *) 0x12345678;
	kernel_process.stack_top	 = (void *) 0x12345678;

	signal_init_process( &kernel_process );

	/* Initialize process state */
	kernel_process.page_directory = paging_active_dir;

	semaphore_init( &kernel_process.child_sema );

	llist_create( &kernel_process.child_events );
	llist_create( &kernel_process.tasks );

	kernel_process.state = PROCESS_READY;
	llist_add_end( process_list, (llist_t *) &kernel_process );

	scheduler_reown_task( scheduler_current_task, &kernel_process );
}

int posix_fork()
{

	int status;
	process_info_t *proc;

	proc = fork_process();

	status = scheduler_spawn( scheduler_fork_main, proc, NULL );

	if ( status ) {

		//TODO: Cleanup process

		return -1;

	}

	return proc->pid;

}


process_info_t *fork_process( void )
{
	process_info_t *child = ( process_info_t *)
		heapmm_alloc(sizeof( process_info_t ));

	if ( !child )
		return NULL;

	memset(child, 0, sizeof( process_info_t));

	/* Initialize process info */
	child->pid            = pid_counter++;
	child->uid            = current_process->uid;
	child->gid            = current_process->gid;
	child->effective_uid  = current_process->effective_uid;
	child->effective_gid  = current_process->effective_gid;
	child->pgid           = current_process->pgid;
	child->sid            = current_process->sid;
	child->parent_pid     = current_process->pid;

	child->name = heapmm_alloc(CONFIG_PROCESS_MAX_NAME_LENGTH);//XXX: Why is this not part of process struct
	//TODO: Handle out of memory condition here
	strcpy(child->name, current_process->name);

	child->fd_table = heapmm_alloc(sizeof(llist_t));//XXX: Why is this not part of process struct
	llist_create(child->fd_table);
	stream_copy_fd_table (child->fd_table);
	child->fd_ctr = current_process->fd_ctr;

	child->memory_map = heapmm_alloc(sizeof(llist_t));//XXX: Why is this not part of process struct
	llist_create(child->memory_map);
	procvmm_copy_memory_map (child->memory_map);

	child->current_directory = vfs_dir_cache_ref(current_process->current_directory);
	child->root_directory = vfs_dir_cache_ref(current_process->root_directory);
	child->root_directory->usage_count++;
	child->current_directory->usage_count++;

	/* Initialize proces signal handling */
	memcpy( child->signal_actions,
			current_process->signal_actions,
			sizeof(struct sigaction[32]));

	/* Initialize process memory info */
	child->heap_start	= current_process->heap_start;
	child->heap_end	    = current_process->heap_end;
	child->heap_max	    = current_process->heap_max;
	child->stack_bottom	= current_process->stack_bottom;
	child->stack_top	= current_process->stack_top;
	semaphore_init(&child->child_sema);

	llist_create(&child->child_events);
	llist_create(&child->tasks);

	/* fork the user pages */
	child->page_directory = paging_create_dir(); //TODO: Check for errors

	/* Initialize process state */
	child->state = PROCESS_READY;
	llist_add_end( process_list, (llist_t *) child );

	return child;
}


void process_init()
{
	process_list = (llist_t *) heapmm_alloc(sizeof(llist_t));
	llist_create( process_list );
	create_kprocess();
}

int curpid()
{
	if ( current_process )
		return current_process->pid;
	return -1;
}

/**
 * Iterator function that looks up eventwith the given child pid
 */
int process_find_event_iterator (llist_t *node, void *param)
{
	process_child_event_t *ev_info = (process_child_event_t *) node;
	return (ev_info->child_pid == (pid_t) param);
}

process_child_event_t *process_get_event(pid_t pid)
{
	return (process_child_event_t *) llist_iterate_select(
		&current_process->child_events,
		&process_find_event_iterator,
		(void *) pid);
}

/**
 * Iterator function that looks up eventwith the given child pid
 */
int process_find_event_pg_iterator (llist_t *node, void *param)
{
	process_child_event_t *ev_info = (process_child_event_t *) node;
	return (ev_info->child_pgid == (pid_t) param);
}

process_child_event_t *process_get_event_pg(pid_t pid)
{
	return (process_child_event_t *) llist_iterate_select(
		&current_process->child_events,
		&process_find_event_pg_iterator,
		(void *) pid);
}

void process_absorb_event(process_child_event_t *ev_info)
{
	llist_unlink((llist_t *) ev_info);
	heapmm_free(ev_info, sizeof(process_child_event_t));
}

void process_child_event(process_info_t *process, int event)
{
	process_child_event_t *ev_info;
	process_info_t *parent = process_get(process->parent_pid);
	if (parent == NULL)
		parent = process_get(1);//Orphaned processes get reaped by init
	ev_info = heapmm_alloc(sizeof(process_child_event_t));
	if (!ev_info) {
		printf( CON_ERROR, "SERIOUS ERROR!!!! Could not allocate ev_info!\n");
		//TODO: Handle this shit...
	}
	ev_info->child_pid = process->pid;
	ev_info->child_pgid = process->pgid;
	ev_info->event = event;
	llist_add_end(&parent->child_events, (llist_t *) ev_info);
	semaphore_up(&parent->child_sema);
	//TODO: Generate SIGCHLD

}

/**
 * Interrupts all threads in the process
 */
void process_interrupt_all( process_info_t *process ) {
	llist_t *c, *n, *h;

	h = &process->tasks;

	for ( c = h->next, n = c->next; c != h; c = n, n = c->next ) {
		scheduler_interrupt_task( ( scheduler_task_t * ) c );
	}
}

/**
 * Stops all threads in the process and stops the process itself
 */
void process_stop( process_info_t *process ) {
	llist_t *c, *n, *h;

	h = &process->tasks;

	process->old_state = process->state;
	process->state = PROCESS_STOPPED;

	for ( c = h->next, n = c->next; c != h; c = n, n = c->next ) {
		scheduler_stop_task( ( scheduler_task_t * ) c );
	}
}

/**
 * Stops all threads in the process
 */
void process_deschedule( process_info_t *process ) {
	llist_t *c, *n, *h;

	h = &process->tasks;
	for ( c = h->next, n = c->next; c != h; c = n, n = c->next ) {
		scheduler_stop_task( ( scheduler_task_t * ) c );
	}

}

/**
 * Restarts all threads in the process
 */
void process_debug_start( process_info_t *process ) {
    llist_t *c, *n, *h;

    h = &process->tasks;
    for ( c = h->next, n = c->next; c != h; c = n, n = c->next ) {
        scheduler_debug_start_task( ( scheduler_task_t * ) c );
    }

}

/**
 * Stops all threads in the process
 */
void process_debug_stop( process_info_t *process ) {
    llist_t *c, *n, *h;

    h = &process->tasks;
    for ( c = h->next, n = c->next; c != h; c = n, n = c->next ) {
        scheduler_debug_stop_task( ( scheduler_task_t * ) c );
    }

}

/**
 * Continues all threads in the process
 */
void process_continue( process_info_t *process ) {
	llist_t *c, *n, *h;

	h = &process->tasks;

	assert( process->state == PROCESS_STOPPED );

	process->state = process->old_state;
	for ( c = h->next, n = c->next; c != h; c = n, n = c->next ) {
		scheduler_continue_task( ( scheduler_task_t * ) c );
	}

}

void process_reap(process_info_t *process)
{

	llist_t *c, *n, *h;
	assert( process != current_process );

	//TODO: Implement
	llist_unlink ((llist_t *) process);
	heapmm_free(process->name, CONFIG_PROCESS_MAX_NAME_LENGTH);
	h = &process->tasks;

	for ( c = h->next, n = c->next; c != h; c = n, n = c->next ) {
		scheduler_reap ( ( scheduler_task_t * ) c );//TODO: Check for errors
	}

	procvmm_clear_mmaps_other( process );
	paging_free_dir( process->page_directory );
	vfs_dir_cache_release(process->root_directory);
	vfs_dir_cache_release(process->current_directory);

	if (process->image_inode) {
		process->image_inode->open_count--;
		vfs_inode_release(process->image_inode);
	}

	stream_do_close_all (process);

}

/**
 * Iterator function that looks up process with the given pid
 */
int process_find_iterator (llist_t *node, void *param)
{
	process_info_t *task = (process_info_t *) node;
	return (task->pid == (pid_t) param);
}

process_info_t *process_get(pid_t pid)
{
	return (process_info_t *)
		llist_iterate_select( process_list, &process_find_iterator, (void *) pid);
}



