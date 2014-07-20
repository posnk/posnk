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

int signal_default_actions[] = 
{
	SIGNAL_ACTION_IGNORE,	//UNDEFINED
	SIGNAL_ACTION_TERM,	//SIGHUP
	SIGNAL_ACTION_TERM,	//SIGINT
	SIGNAL_ACTION_ABORT,	//SIGQUIT
	SIGNAL_ACTION_ABORT,	//SIGILL
	SIGNAL_ACTION_ABORT,	//SIGTRAP
	SIGNAL_ACTION_ABORT,	//SIGABRT
	SIGNAL_ACTION_IGNORE,	//UNDEFINED
	SIGNAL_ACTION_ABORT,	//SIGFPE
	SIGNAL_ACTION_TERM,	//SIGKILL
	SIGNAL_ACTION_ABORT,	//SIGBUS
	SIGNAL_ACTION_ABORT,	//SIGSEGV
	SIGNAL_ACTION_ABORT,	//SIGSYS
	SIGNAL_ACTION_TERM,	//SIGPIPE
	SIGNAL_ACTION_TERM,	//SIGALRM
	SIGNAL_ACTION_TERM,	//SIGTERM
	SIGNAL_ACTION_TERM,	//SIGUSR1
	SIGNAL_ACTION_TERM,	//SIGUSR2
	SIGNAL_ACTION_IGNORE,	//SIGCHLD 18
	SIGNAL_ACTION_IGNORE,	//UNDEFINED 19
	SIGNAL_ACTION_IGNORE,	//UNDEFINED 20
	SIGNAL_ACTION_IGNORE,	//SIGURG 21
	SIGNAL_ACTION_TERM,	//SIGPOLL
	SIGNAL_ACTION_STOP,	//SIGSTOP
	SIGNAL_ACTION_STOP,	//SIGTSTP
	SIGNAL_ACTION_CONT,	//SIGCONT
	SIGNAL_ACTION_STOP,	//SIGTTIN
	SIGNAL_ACTION_STOP,	//SIGTTOU
	SIGNAL_ACTION_TERM,	//SIGVTALRM
	SIGNAL_ACTION_TERM,	//SIGPROF
	SIGNAL_ACTION_ABORT,	//SIGXCPU
	SIGNAL_ACTION_ABORT,	//SIGXFSZ
};

int curpid()
{
	if (scheduler_current_task)
		return scheduler_current_task->pid;
	return -1;
}

void process_send_signal(process_info_t *process, int signal)
{
	debugcon_printf("sending signal to pid %i : %i\n",process->pid, signal);
	if (signal == 0)
		return;
	process->waiting_signal_bitmap |= (1 << signal);
}

void process_set_signal_mask(process_info_t *process, int mask)
{
	process->signal_mask_bitmap = mask & CONFIG_MASKABLE_SIGNAL_BITMAP;
}

void process_signal_handler_default(int signal)
{
	//debugcon_printf("sigrecv: %i\n", scheduler_current_task->pid);
	scheduler_current_task->last_signal = signal;
	switch (signal_default_actions[signal]) {
		case SIGNAL_ACTION_ABORT:
		case SIGNAL_ACTION_TERM:
			scheduler_current_task->term_cause = PROCESS_TERM_SIGNAL;	
			scheduler_current_task->state = PROCESS_KILLED;
			//earlycon_printf("killed: %i\n", scheduler_current_task->pid);
			break;
		case SIGNAL_ACTION_CONT:
			scheduler_current_task->state = PROCESS_RUNNING;
			break;
		case SIGNAL_ACTION_STOP:
			if (scheduler_current_task->state != PROCESS_WAITING)
				scheduler_current_task->waiting_on = NULL;
			scheduler_current_task->state = PROCESS_WAITING;
			break;
		case SIGNAL_ACTION_IGNORE:
		default:
			return;			
	}
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
}

int process_was_interrupted(process_info_t *process)
{
	uint32_t sig_active = process->waiting_signal_bitmap & ~process->signal_mask_bitmap;
	return sig_active;
}

void process_handle_signals()
{
	
	uint32_t bit_counter;
	uint32_t sig_active = scheduler_current_task->waiting_signal_bitmap & ~scheduler_current_task->signal_mask_bitmap;
	//debugcon_printf("sigactive: %i\n", sig_active);
	if (sig_active != 0) {
		debugcon_printf("sighandle: %i\n", scheduler_current_task->pid);
		if (scheduler_current_task->state != PROCESS_INTERRUPTED) {
			for (bit_counter = 0; bit_counter < 32; bit_counter++) {
				if (sig_active & (1 << bit_counter)) {
					scheduler_current_task->waiting_signal_bitmap &= ~ (1 << bit_counter);
					debugcon_printf("handling signal: %i\n", sig_active);
					if (scheduler_current_task->signal_handler_table[bit_counter] == NULL)
						process_signal_handler_default(bit_counter);
					else if (scheduler_current_task->signal_handler_table[bit_counter] == (void *) 1)
						continue;
					else
						scheduler_invoke_signal_handler(bit_counter);
					if (scheduler_current_task->state == PROCESS_KILLED) {
						debugcon_printf("killedby: %i\n", sig_active);
						process_child_event(scheduler_current_task, PROCESS_CHILD_KILLED);

						stream_do_close_all (scheduler_current_task);
						procvmm_clear_mmaps();
						schedule();
						return;
					}
				}
			}
		}
	}
}

void process_reap(process_info_t *process)
{
	//TODO: Implement
	llist_unlink ((llist_t *) process);
	heapmm_free(process->name, CONFIG_PROCESS_MAX_NAME_LENGTH);
	//TODO: Free arch-state
	//TODO: Free page-dir
	vfs_dir_cache_release(process->root_directory);
	vfs_dir_cache_release(process->current_directory);

	if (process->image_inode) {
		process->image_inode->open_count--;
		vfs_inode_release(process->image_inode);
	}

	stream_do_close_all (process);
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
} sig_pgrp_param_t;

uint32_t  process_signal_pgroup_numdone = 0;

/** 
 * Iterator function that signals up processes with the given pgid
 */
int process_sig_pgrp_iterator (llist_t *node, void *param)
{
	sig_pgrp_param_t *p = (sig_pgrp_param_t *) param;
	process_info_t *process = (process_info_t *) node;
	if ((process->pid > 1) && (process->pid != 2) && (process->pgid == p->group)) {
		if (get_perm_class(process->uid, process->gid) != PERM_CLASS_OWNER){
			syscall_errno = EPERM;
			return 0;
		}
		process_send_signal(process, p->signal);
		process_signal_pgroup_numdone++;	
	}
	return 0;
}

/* TODO: Add some checks for existence to signal to caller */
int process_signal_pgroup(pid_t pid, int signal)
{
	sig_pgrp_param_t param;
	param.group = pid;
	param.signal = signal;
	process_signal_pgroup_numdone = 0;
	llist_iterate_select(scheduler_task_list, &process_sig_pgrp_iterator, (void *) &param);
	debugcon_printf(" sent %i to %i tasks in group %i\n", signal, process_signal_pgroup_numdone, pid);
	return process_signal_pgroup_numdone;
}

int strlistlen(char **list)
{
	int len = 0;
	while (list[len] != 0)
		len++;
	return len;
}

int process_exec(char *path, char **args, char **envs)
{
	int status;
	size_t arg_size, env_size, argl_size, envl_size, args_size, envs_size, c;
	uintptr_t ptr;
	char **n_args;
	char **n_envs;

	if (scheduler_current_task->image_inode) {
		scheduler_current_task->image_inode->open_count--;
		vfs_inode_release(scheduler_current_task->image_inode);
	}

	procvmm_clear_mmaps();

	scheduler_current_task->signal_mask_bitmap = 0;
	memset(scheduler_current_task->signal_handler_table,0 , 32*sizeof(void *));
	/* Load image */
	status = elf_load(path);
	if (status) {
		debugcon_printf("error loading elf\n");
		process_send_signal(scheduler_current_task, SIGSYS);
		return status;	//NEVER REACHED
	}

	/* Pre exec bookkeeping */
	stream_do_close_on_exec();

	/* Prepare environment */
	status = procvmm_do_exec_mmaps();
	if (status) {
		debugcon_printf("error mmapping stuff\n");
		process_send_signal(scheduler_current_task, SIGSYS);
		return status;	//NEVER REACHED
	}
	//TODO: Verify arg and env list sizes

	argl_size = strlistlen(args);
	
	args_size = 0;

	for (c = 0; c < argl_size; c++)
		args_size += strlen(args[c]) + 1;

	arg_size = (argl_size + 1) * sizeof(char *) + args_size;

	procvmm_mmap_anon((void *) 0x4000000, arg_size, PROCESS_MMAP_FLAG_WRITE, "(args)");

	ptr = 0x4000000 + sizeof(char *) * (argl_size + 1);
	n_args = (char **) 0x4000000;

	for (c = 0; c < argl_size; c++) {
		strcpy((char *)ptr, args[c]);
		n_args[c] = (char *) ptr;		
		ptr += strlen(args[c]) + 1;
	}
	n_args[argl_size] = (char *) 0;

	envl_size = strlistlen(envs);
	
	envs_size = 0;

	for (c = 0; c < envl_size; c++)
		envs_size += strlen(envs[c]) + 1;

	env_size = sizeof(char *) * (envl_size + 1) + envs_size;

	procvmm_mmap_anon((void *) 0x4020000, env_size, PROCESS_MMAP_FLAG_WRITE, "(envs)");

	ptr = 0x4020000 + sizeof(char *) * (envl_size + 1);
	n_envs = (char **) 0x4020000;

	for (c = 0; c < envl_size; c++) {
		strcpy((char *)ptr, envs[c]);
		n_envs[c] = (char *) ptr;		
		ptr += strlen(envs[c]) + 1;
	}
	n_envs[envl_size] = (char *) 0;
	//debugcon_printf("calling program\n");

	process_user_call(scheduler_current_task->entry_point, scheduler_current_task->stack_bottom);
	return 0;//NEVER REACHED
}

