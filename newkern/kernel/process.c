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

int curpid()
{
	if (scheduler_current_task)
		return scheduler_current_task->pid;
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
	return (process_child_event_t *) llist_iterate_select(scheduler_current_task->child_events, &process_find_event_iterator, (void *) pid);
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
	return (process_child_event_t *) llist_iterate_select(scheduler_current_task->child_events, &process_find_event_pg_iterator, (void *) pid);
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
		debugcon_printf("SERIOUS ERROR!!!! Could not allocate ev_info!\n");
		//TODO: Handle this shit...
	}
	ev_info->child_pid = process->pid;
	ev_info->child_pgid = process->pgid;
	ev_info->event = event;
	llist_add_end(parent->child_events, (llist_t *) ev_info);
	semaphore_up(parent->child_sema);
	//TODO: Generate SIGCHLD

}
void process_reap(process_info_t *process)
{
	assert( process != scheduler_current_task );
	//TODO: Implement
	debugcon_printf("Unlink process! %i\n",process->pid);
	llist_unlink ((llist_t *) process);
	heapmm_free(process->name, CONFIG_PROCESS_MAX_NAME_LENGTH);
	debugcon_printf("Free arch task!\n");
	scheduler_free_task( process );
	debugcon_printf("Clear mmaps!\n");
	procvmm_clear_mmaps_other( process );
	debugcon_printf("Clear pagedir!\n");
	paging_free_dir( process->page_directory );
	debugcon_printf("Release directory caches!\n");
	vfs_dir_cache_release(process->root_directory);
	vfs_dir_cache_release(process->current_directory);
	debugcon_printf("Release inode!\n");

	if (process->image_inode) {
		process->image_inode->open_count--;
		vfs_inode_release(process->image_inode);
	}
	debugcon_printf("Close streams!\n");

	stream_do_close_all (process);
	debugcon_printf("Free semaphore!\n");
	semaphore_free(process->child_sema);
	
}

extern llist_t		 *scheduler_task_list;

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
	return (process_info_t *) llist_iterate_select(scheduler_task_list, &process_find_iterator, (void *) pid);
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
	llist_iterate_select(scheduler_task_list, &process_sig_pgrp_iterator, (void *) &param);
	debugcon_printf(" sent %i to %i tasks in group %i\n", signal, process_signal_pgroup_numdone, pid);
	return process_signal_pgroup_numdone;
}


