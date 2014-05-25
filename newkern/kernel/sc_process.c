/**
 * kernel/sc_process.c
 *
 * Part of P-OS kernel.
 *
 * Contains process related syscalls
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-04-2014 - Created
 */
#include <sys/errno.h>
#include "kernel/heapmm.h"
#include "kernel/physmm.h"
#include "kernel/paging.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "kernel/permissions.h"
#include "kernel/synch.h"
#include "kernel/syscall.h"

uint32_t sys_fork(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) scheduler_fork();
}

uint32_t sys_kill(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	process_info_t *process;
	int pid = (int) param[0];
	int sig = (int) param[1];
	if (sig < 0 || sig > 31) {
		syscall_errno = EINVAL;
		return (uint32_t)-1;		
	} else if (pid == 0) {
		syscall_errno = ESRCH;
		if (process_signal_pgroup(
			scheduler_current_task->pgid, sig) == 0) {
			return (uint32_t) -1;
		} 
		syscall_errno = 0;
		return 0;
	} else if (pid == -1) {
		//TODO: Implement kill all system processes
		syscall_errno = EINVAL;
		return (uint32_t)-1;
	} else if (pid < -1) {
		syscall_errno = ESRCH;
		if (process_signal_pgroup((pid_t) -pid, sig) == 0) {
			return (uint32_t) -1;
		} 
		syscall_errno = 0;
		return 0;
	} else {
		process = process_get((pid_t) pid);
		if (process == NULL) {
			syscall_errno = ESRCH;
			return (uint32_t) -1;			
		}
		if (get_perm_class(process->uid, process->gid) != PERM_CLASS_OWNER){
			syscall_errno = EPERM;
			return (uint32_t) -1;
		}	
		process_send_signal(process, sig);
		return 0;
	}		
}

uint32_t sys_getpid(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (pid_t) scheduler_current_task->pid;
}

uint32_t sys_getppid(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (pid_t) scheduler_current_task->parent_pid;
}

uint32_t sys_getpgrp(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (pid_t) scheduler_current_task->pgid;
}

uint32_t sys_getsid(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (pid_t) scheduler_current_task->sid;
}

uint32_t sys_setsid(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	if (scheduler_current_task->pgid == scheduler_current_task->pid) {
		syscall_errno = EPERM;
		return (uint32_t) -1;	
	}
	scheduler_current_task->pgid = scheduler_current_task->pid;
	scheduler_current_task->sid = scheduler_current_task->pid;
	scheduler_current_task->ctty = 0;
	return (pid_t) scheduler_current_task->sid;
}

uint32_t sys_setpgrp(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	if (scheduler_current_task->pgid == scheduler_current_task->pid) {
		syscall_errno = EPERM;
		return (uint32_t) -1;	
	}
	scheduler_current_task->pgid = scheduler_current_task->pid;
	return 0;
}

uint32_t sys_setpgid(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	process_info_t *task;
	pid_t pid = (pid_t) param[0];//TODO: FIX PERMS
	pid_t pgid = (pid_t) param[1];
	if (pid == 0)
		pid = scheduler_current_task->pid;
	if (pgid == 0)
		pgid = pid;
	if (pgid < 0) {
		syscall_errno = EINVAL;
		return (uint32_t) -1;
	}
	task = process_get(pid);
	if (!task) {
		syscall_errno = ESRCH;
		return (uint32_t) -1;	
	}
	task->pgid = pgid;
	return 0;
}

uint32_t sys_exit(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	scheduler_current_task->state = PROCESS_KILLED;
	process_child_event(scheduler_current_task, PROCESS_CHILD_KILLED);
	procvmm_clear_mmaps();
	schedule();
	return 0; // NEVER REACHED
}

uint32_t sys_yield(__attribute__((__unused__)) uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	schedule();
	return 0;
}

#define WNOHANG 1
#define WUNTRACED 2

//pid_t pid, int *status, int options
uint32_t sys_waitpid(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	int status, options;
	pid_t pid;
	process_child_event_t *ev_info;
	process_info_t *chld;
	if (!copy_user_to_kern((void *)param[1], &status, sizeof(int))) {
		syscall_errno = EFAULT;
		return -1;
	}
	pid = (pid_t) param[0];
	options = (int) param[2];

	if (pid == 0)
		pid = -(scheduler_current_task->pid);
	if (pid < -1) {
		//TODO: Determine whether pgroup exists
		if (options & WNOHANG) {
			while (  (!(ev_info = process_get_event_pg(-pid))) || 
				 ((ev_info->event == PROCESS_CHILD_STOPPED) && !(options & WUNTRACED)) || 
				 (ev_info->event == PROCESS_CHILD_CONTD)) {

				if (!ev_info)	
					return 0;	

				process_absorb_event(ev_info);
			}
			
		} else {
			while (  (!(ev_info = process_get_event_pg(-pid))) || 
				 ((ev_info->event == PROCESS_CHILD_STOPPED) && !(options & WUNTRACED)) || 
				 (ev_info->event == PROCESS_CHILD_CONTD)) {

				if (ev_info)		
					process_absorb_event(ev_info);

				semaphore_down(scheduler_current_task->child_sema);
			}
		}
	} else if (pid == -1) {
		if (options & WNOHANG) {
			while (  (!(ev_info = (process_child_event_t *) llist_get_last(scheduler_current_task->child_events))) ||
				 (ev_info == (process_child_event_t *) scheduler_current_task->child_events) ||
				 ((ev_info->event == PROCESS_CHILD_STOPPED) && !(options & WUNTRACED)) || 
				 (ev_info->event == PROCESS_CHILD_CONTD)) {

				if (ev_info == (process_child_event_t *) scheduler_current_task->child_events)	
					return 0;	

				process_absorb_event(ev_info);
			}
		} else {
			while (  (!(ev_info = (process_child_event_t *) llist_get_last(scheduler_current_task->child_events))) || 
				 (ev_info == (process_child_event_t *) scheduler_current_task->child_events) ||
				 ((ev_info->event == PROCESS_CHILD_STOPPED) && !(options & WUNTRACED)) || 
				 (ev_info->event == PROCESS_CHILD_CONTD)) {

				if (ev_info != (process_child_event_t *) scheduler_current_task->child_events)	
					process_absorb_event(ev_info);

				semaphore_down(scheduler_current_task->child_sema);
			}
		}
	} else if (pid > 0) {
		chld = process_get(pid);
		if (!chld) {
			syscall_errno = ECHILD;
			return -1;
		}
		if (options & WNOHANG) {
			while (  (!(ev_info = process_get_event(pid))) || 
				 ((ev_info->event == PROCESS_CHILD_STOPPED) && !(options & WUNTRACED)) || 
				 (ev_info->event == PROCESS_CHILD_CONTD)) {
				if (!ev_info)	
					return 0;	
				process_absorb_event(ev_info);
			}
			
		} else {
			while (  (!(ev_info = process_get_event(pid))) || 
				 ((ev_info->event == PROCESS_CHILD_STOPPED) && !(options & WUNTRACED)) || 
				 (ev_info->event == PROCESS_CHILD_CONTD)) {
				if (ev_info)		
					process_absorb_event(ev_info);
				semaphore_down(scheduler_current_task->child_sema);
			}
		}
	}
	chld = process_get(ev_info->child_pid);
	pid = ev_info->child_pid;
	debugcon_aprintf("received msg: %i\n", pid);
	switch (ev_info->event) {
		case PROCESS_CHILD_KILLED:
			switch (chld->term_cause) {
				case PROCESS_TERM_SIGNAL:
					status = chld->last_signal;
					break;
				case PROCESS_TERM_EXIT:
					status = ((chld->exit_status) << 8) & 0xFF00;
					break;
			}
			process_reap(chld);
			break;
		case PROCESS_CHILD_STOPPED:
			if(options & WUNTRACED) 
				status = (((chld->last_signal) << 8) & 0xFF00) | 0x7f;
			else
				return 0;
			break;
		case PROCESS_CHILD_CONTD://TODO: Implement this in libc, linux only hehehe
			//if(options & WCONTINUED) 
			//	status = (((chld->last_signal) << 8) & 0xFF00) | 0x7f;
			//else
				return 0;
			//break;
	}
	process_absorb_event(ev_info);
	if (!copy_kern_to_user(&status, (void *)param[1], sizeof(int))) {
		syscall_errno = EFAULT;
		return -1;
	}
	return pid;	
}

uint32_t sys_sbrk(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	uintptr_t old_brk   = (uintptr_t) scheduler_current_task->heap_end;
	uintptr_t heap_max = (uintptr_t) scheduler_current_task->heap_max;
	uintptr_t base = (uintptr_t) scheduler_current_task->heap_start;
	uintptr_t old_size = old_brk - base;
	int incr = param[0];
	if (incr > 0) {	
		if (incr & PHYSMM_PAGE_ADDRESS_MASK)
			incr = (incr & ~PHYSMM_PAGE_ADDRESS_MASK) + PHYSMM_PAGE_SIZE;
		if ((old_brk + incr) >= heap_max) {
			syscall_errno = ENOMEM;
			return (uint32_t) -1;
		}
		if (old_size != 0) {
			if (procvmm_resize_map((void *)base, old_size + incr)) {
				syscall_errno = ENOMEM;
				return (uint32_t) -1;
			}
		} else {
			if (procvmm_mmap_anon((void *)base, incr, PROCESS_MMAP_FLAG_HEAP | PROCESS_MMAP_FLAG_WRITE, "(heap)")) {
				syscall_errno = ENOMEM;
				return (uint32_t) -1;
			}
	
		}
		scheduler_current_task->heap_end = (void *) (old_brk + incr);
		return (uint32_t) old_brk;
	} else if (incr == 0) {
		return (uint32_t) old_brk;
	} else {
		//TODO: Handle decreasing heap size
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
}

int strlistlen(char **list);

//int execve(char *path, char **argv, char **envp);
uint32_t sys_execve(uint32_t param[4], uint32_t param_size[4])
{
	int c,argvc, envpc;
	char* path;
	char **argv;
	char **envp;
	char *user_str;
	uintptr_t *argvs; // These are uintptr_t because they have to have the same size as the pointer lists
	uintptr_t *envps;
	ssize_t status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!path) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	argv = heapmm_alloc(param_size[1]);
	if (!argv) {
		syscall_errno = ENOMEM;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	envp = heapmm_alloc(param_size[2]);
	if (!envp) {
		syscall_errno = ENOMEM;
		heapmm_free(path, param_size[0]);
		heapmm_free(argv, param_size[1]);
		return (uint32_t) -1;
	}
	argvs = heapmm_alloc(param_size[1]);
	if (!argv) {
		syscall_errno = ENOMEM;
		heapmm_free(path, param_size[0]);
		heapmm_free(argv, param_size[1]);
		heapmm_free(envp, param_size[2]);
		return (uint32_t) -1;
	}
	envps = heapmm_alloc(param_size[2]);
	if (!envp) {
		syscall_errno = ENOMEM;
		heapmm_free(path , param_size[0]);
		heapmm_free(argv , param_size[1]);
		heapmm_free(argvs, param_size[1]);
		heapmm_free(envp , param_size[2]);
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path , param_size[0]);
		heapmm_free(argv , param_size[1]);
		heapmm_free(argvs, param_size[1]);
		heapmm_free(envp , param_size[2]);
		heapmm_free(envps, param_size[2]);
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *)param[1], argv, param_size[1])) {
		syscall_errno = EFAULT;
		heapmm_free(path , param_size[0]);
		heapmm_free(argv , param_size[1]);
		heapmm_free(argvs, param_size[1]);
		heapmm_free(envp , param_size[2]);
		heapmm_free(envps, param_size[2]);
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *)param[2], envp, param_size[2])) {
		syscall_errno = EFAULT;
		heapmm_free(path , param_size[0]);
		heapmm_free(argv , param_size[1]);
		heapmm_free(argvs, param_size[1]);
		heapmm_free(envp , param_size[2]);
		heapmm_free(envps, param_size[2]);
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *)param[3], argvs, param_size[1])) {
		syscall_errno = EFAULT;
		heapmm_free(path , param_size[0]);
		heapmm_free(argv , param_size[1]);
		heapmm_free(argvs, param_size[1]);
		heapmm_free(envp , param_size[2]);
		heapmm_free(envps, param_size[2]);
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *)param_size[3], envps, param_size[2])) {
		syscall_errno = EFAULT;
		heapmm_free(path , param_size[0]);
		heapmm_free(argv , param_size[1]);
		heapmm_free(argvs, param_size[1]);
		heapmm_free(envp , param_size[2]);
		heapmm_free(envps, param_size[2]);
		return (uint32_t) -1;
	}
	if (param_size[0] != 0)
		path[param_size[0] - 1] = 0;
	if (param_size[1] >= sizeof(char *))
		argv[(param_size[1] / sizeof(char *)) - 1] = 0;
	if (param_size[2] >= sizeof(char *))
		envp[(param_size[2] / sizeof(char *)) - 1] = 0;
	argvc = strlistlen(argv);
	for (c = 0; c < argvc; c++) {
		user_str = argv[c];
		if ((argvs[c] > CONFIG_MAX_ARGV_LEN) || !(argv[c] = heapmm_alloc(argvs[c]))) {
			argvc = c;
			for (c = 0; c < argvc; c++) 
				heapmm_free(argv[c], argvs[c]);
			syscall_errno = EFAULT;
			heapmm_free(path , param_size[0]);
			heapmm_free(argv , param_size[1]);
			heapmm_free(argvs, param_size[1]);
			heapmm_free(envp , param_size[2]);
			heapmm_free(envps, param_size[2]);
			return (uint32_t) -1;			
		}
		if (!copy_user_to_kern(user_str, argv[c], argvs[c])) {
			argvc = c + 1;
			for (c = 0; c < argvc; c++) 
				heapmm_free(argv[c], argvs[c]);
			syscall_errno = EFAULT;
			heapmm_free(path , param_size[0]);
			heapmm_free(argv , param_size[1]);
			heapmm_free(argvs, param_size[1]);
			heapmm_free(envp , param_size[2]);
			heapmm_free(envps, param_size[2]);
			return (uint32_t) -1;
		}		
	}
	envpc = strlistlen(envp);
	for (c = 0; c < envpc; c++) {
		user_str = envp[c];
		if ((envps[c] > CONFIG_MAX_ENV_LEN) || !(envp[c] = heapmm_alloc(envps[c]))) {
			envpc = c;
			for (c = 0; c < envpc; c++) 
				heapmm_free(envp[c], envps[c]);
			syscall_errno = EFAULT;
			heapmm_free(path , param_size[0]);
			heapmm_free(argv , param_size[1]);
			heapmm_free(argvs, param_size[1]);
			heapmm_free(envp , param_size[2]);
			heapmm_free(envps, param_size[2]);
			return (uint32_t) -1;			
		}
		if (!copy_user_to_kern(user_str, envp[c], envps[c])) {
			envpc = c + 1;
			for (c = 0; c < envpc; c++) 
				heapmm_free(envp[c], envps[c]);
			syscall_errno = EFAULT;
			heapmm_free(path , param_size[0]);
			heapmm_free(argv , param_size[1]);
			heapmm_free(argvs, param_size[1]);
			heapmm_free(envp , param_size[2]);
			heapmm_free(envps, param_size[2]);
			return (uint32_t) -1;
		}		
	}
	status = process_exec(path, argv, envp);
	heapmm_free(path , param_size[0]);
	heapmm_free(argv , param_size[1]);
	heapmm_free(argvs, param_size[1]);
	heapmm_free(envp , param_size[2]);
	heapmm_free(envps, param_size[2]);
	return (uint32_t) status;
}

