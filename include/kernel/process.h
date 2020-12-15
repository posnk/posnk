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
#include <signal.h>
#include <sys/types.h>

typedef struct process_info process_info_t;

typedef struct process_mmap process_mmap_t;

typedef struct process_child_event process_child_event_t;

#include "kernel/paging.h"
#include "kernel/scheduler.h"
#include "kernel/synch.h"
#include "kernel/time.h"
#include "kernel/sem.h"
#include "kernel/shm.h"
#include "kernel/vfs.h"
#include "util/llist.h"

#define PROCESS_RUNNING 	0
#define PROCESS_WAITING 	1
#define PROCESS_READY		2
#define PROCESS_NO_SCHED	3
#define PROCESS_KILLED		4
#define PROCESS_INTERRUPTED	5
#define PROCESS_TIMED_OUT	6
#define PROCESS_STOPPED		7

#define PROCESS_TERM_EXIT	0
#define PROCESS_TERM_SIGNAL	1

#define PROCESS_CHILD_KILLED	0
#define PROCESS_CHILD_STOPPED	1
#define PROCESS_CHILD_CONTD		2

#define PROCESS_MMAP_FLAG_WRITE		(1<<1)
#define PROCESS_MMAP_FLAG_FILE		(1<<2)
#define PROCESS_MMAP_FLAG_PUBLIC	(1<<3)
#define PROCESS_MMAP_FLAG_STACK		(1<<4)
#define PROCESS_MMAP_FLAG_HEAP		(1<<5)
#define PROCESS_MMAP_FLAG_DEVICE	(1<<6)
#define PROCESS_MMAP_FLAG_STREAM	(1<<7)
#define PROCESS_MMAP_FLAG_SHM		(1<<8)

#define PROCVMM_TOO_LARGE   (-2)
#define PROCVMM_INV_MAPPING (-1)

#define current_process (scheduler_current_task->process)
extern llist_t *process_list;
struct process_mmap {
	llist_t		 node;
	char		*name;
	void		*start;
	size_t		 size;
	int		 flags;
	inode_t		*file;
	int		 fd;
	aoff_t		 offset;
	aoff_t		 file_sz;
	shm_info_t	*shm;
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
	int			 fd_ctr;

	/* Signal handling */
	sigset_t	 signal_pending;
	struct sigaction	signal_actions[32];
	struct siginfo		signal_info[32];
	stack_t		 signal_altstack;
	void 		*signal_handler_exit_func;
	int			 last_signal;
	int			 old_state;
	int			 state;

	/* Proces status */
	int			 sc_errno;
	int			 term_cause;
	int			 exit_status;

	/* Process memory */
	llist_t		*memory_map;
	void		*image_start;
	void		*image_end;
	void		*heap_start;
	void		*heap_end;
	void		*heap_max;
	void		*stack_bottom;// Actually a higher address than top
	void		*stack_top;
	void		*kernel_stack;

	/* Process statistics */
	ticks_t		 cpu_ticks;
	llist_t		 tasks;
	/* Process state */
	page_dir_t	*page_directory;

	semaphore_t	 child_sema;
	llist_t		 child_events;
};


int curpid();

void procvmm_clear_mmaps_other( process_info_t *info );

process_info_t *process_get(pid_t pid);

int process_push_user_data(const void *data, size_t size);

int process_was_interrupted( scheduler_task_t *task );

int process_was_continued(process_info_t *process);

void process_reap(process_info_t *);

process_child_event_t *process_get_event(pid_t pid);

process_child_event_t *process_get_event_pg(pid_t pid);

void process_send_signal(	process_info_t *process,
							int signal,
							struct siginfo info );

void process_absorb_event(process_child_event_t *ev_info);

void process_child_event(process_info_t *process, int event);

int process_signal_pgroup(pid_t pid, int signal, struct siginfo info);

process_mmap_t *procvmm_get_memory_region(const void *address);

int procvmm_copy_memory_map (llist_t *target);

int procvmm_resize_map(void *start, size_t newsize);

int procvmm_mmap_anon(void *start, size_t size, int flags, const char *name);

int procvmm_mmap_file(void *start, size_t size, inode_t* file, off_t offset, off_t file_sz, int flags, const char *name);

void procvmm_unmmap_other(process_info_t *task, process_mmap_t *region);

void procvmm_unmmap(process_mmap_t *region);

int procvmm_handle_fault(void *address);

int procvmm_do_exec_mmaps(void);

void procvmm_clear_mmaps(void);

void process_init(void);

process_info_t *fork_process( void );

int procvmm_check( const void *dest, size_t size);
int procvmm_check_string( const char *dest, size_t size_max );
int procvmm_check_stringlist(	const char **dest,
				size_t len_max );

void process_load_exec_state( void *entry, void *stack );

int posix_fork(void);
int process_exec(const char *path, char * const args[], char * const envs[] );

void *procvmm_attach_shm(void *addr, shm_info_t *shm, int flags);

int procvmm_detach_shm(void *shmaddr);

void *_sys_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

void process_interrupt_all( process_info_t *process );

void process_stop( process_info_t *process );

void process_continue( process_info_t *process );

void process_deschedule( process_info_t *process );

#endif
