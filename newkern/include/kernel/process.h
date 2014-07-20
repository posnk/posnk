/**
 * kernel/process.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-04-2014 - Created
 */

#ifndef __KERNEL_PROCESS_H__
#define __KERNEL_PROCESS_H__

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include "kernel/paging.h"
#include "kernel/synch.h"
#include "kernel/time.h"
#include "kernel/vfs.h"
#include "util/llist.h"

#define PROCESS_RUNNING 	0
#define PROCESS_WAITING 	1
#define PROCESS_READY		2
#define PROCESS_NO_SCHED	3
#define PROCESS_KILLED		4
#define PROCESS_INTERRUPTED	5

#define SIGNAL_ACTION_ABORT	0
#define SIGNAL_ACTION_TERM	1
#define SIGNAL_ACTION_CONT	2
#define SIGNAL_ACTION_IGNORE	3
#define SIGNAL_ACTION_STOP	4

#define PROCESS_TERM_EXIT	0
#define PROCESS_TERM_SIGNAL	1

#define PROCESS_CHILD_KILLED	0
#define PROCESS_CHILD_STOPPED	1
#define PROCESS_CHILD_CONTD	2

#define PROCESS_MMAP_FLAG_WRITE		(1<<1)
#define PROCESS_MMAP_FLAG_FILE		(1<<2)
#define PROCESS_MMAP_FLAG_PUBLIC	(1<<3)
#define PROCESS_MMAP_FLAG_STACK		(1<<4)
#define PROCESS_MMAP_FLAG_HEAP		(1<<5)

struct process_mmap {
	llist_t		 node;
	char		*name;
	void		*start;
	size_t		 size;
	int		 flags;
	inode_t		*file;
	off_t		 offset;
	off_t		 file_sz;
};

struct process_child_event {
	llist_t		 node;
	pid_t		 child_pid;
	pid_t		 child_pgid;
	int		 event;
};

struct process_info {
	llist_t		 node;

	/* Process info */
	pid_t		 parent_pid;
	pid_t		 pid;
	uid_t		 uid;
	uid_t		 gid;
	uid_t		 effective_uid;
	uid_t		 effective_gid;
	pid_t		 pgid;
	pid_t		 sid;
	dev_t		 ctty;
	char 		*name;
	void		*entry_point;

	/* VFS */
	dir_cache_t	*root_directory;
	dir_cache_t	*current_directory;
	umode_t		 umask;
	inode_t		*image_inode;

	/* Streams */
	llist_t		*fd_table;
	int		 fd_ctr;

	/* Signal handling */
	void 		*signal_handler_exit_func;
	uint32_t	 waiting_signal_bitmap;
	uint32_t	 signal_mask_bitmap;
	void 		*signal_handler_table[32];
	int		 last_signal;

	/* Proces status */
	int		 sc_errno;
	int		 term_cause;
	int		 exit_status;

	/* Process memory */
	llist_t		*memory_map;
	void		*image_start;
	void		*image_end;
	void		*heap_start;
	void		*heap_end;
	void		*heap_max;
	void		*stack_bottom;// Actually a higher address than top
	void		*stack_top;

	/* Process statistics */
	ticks_t		 cpu_ticks;

	/* Process state */
	page_dir_t	*page_directory;
	void		*arch_state;
	int		 state;
	semaphore_t	*waiting_on;
	ktime_t		 wait_timeout_u;//In microseconds
	ktime_t		 wait_timeout_s;//In microseconds
	semaphore_t	*child_sema;
	llist_t		*child_events;
	uint32_t	 in_syscall;

	void		*arch_state_pre_signal;
	void		*isr_stack_pre_signal;
	size_t		 isr_stack_pre_signal_size;
};

typedef struct process_info process_info_t;

typedef struct process_mmap process_mmap_t;

typedef struct process_child_event process_child_event_t;

process_info_t *process_get(pid_t pid);	

int process_push_user_data(void *data, size_t size);

void process_send_signal(process_info_t *process, int signal);

void process_set_signal_mask(process_info_t *process, int mask);

int process_was_interrupted(process_info_t *process);

void process_handle_signals();

void process_reap(process_info_t *);

process_child_event_t *process_get_event(pid_t pid);

process_child_event_t *process_get_event_pg(pid_t pid);

void process_absorb_event(process_child_event_t *ev_info);

void process_child_event(process_info_t *process, int event);

int process_signal_pgroup(pid_t pid, int signal);

process_mmap_t *procvmm_get_memory_region(void *address);

int procvmm_copy_memory_map (llist_t *target);

int procvmm_resize_map(void *start, size_t newsize);

int procvmm_mmap_anon(void *start, size_t size, int flags, char *name);

int procvmm_mmap_file(void *start, size_t size, inode_t* file, off_t offset, off_t file_sz, int flags, char *name);

void procvmm_unmmap(process_mmap_t *region);

int procvmm_handle_fault(void *address);

int procvmm_do_exec_mmaps();

void procvmm_clear_mmaps();

int procvmm_check(void *dest, size_t size);

void process_user_call(void *entry, void *stack);

int process_exec(char *path, char **args, char **envs);

#endif
