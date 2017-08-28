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
#include <string.h>
#include "kernel/heapmm.h"
#include "kernel/physmm.h"
#include "kernel/paging.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "kernel/earlycon.h"
#include "kernel/permissions.h"
#include "kernel/synch.h"
#include "kernel/syscall.h"
#include "kernel/streams.h"

/**
 * @brief Syscall implementation: fork
 * Create a copy of a 
 */

SYSCALL_DEF0(fork)
{


	/* Call the schedulur to actually do the fork */
	return (uint32_t) posix_fork();
	
}

/**
 * @brief Syscall implementation: kill
 * Sends a signal to a process.
 */

SYSCALL_DEF2(kill)
{
	struct siginfo info;
	process_info_t *process;
	
	memset( &info, 0, sizeof( struct siginfo ) );

	/* The process to signal */
	int pid = (int) a;
	
	/* The signal to send */
	int sig = (int) b;

	/* Do a range check on sid */	
	if (sig < 0 || sig > 31) {
		syscall_errno = EINVAL;
		return (uint32_t)-1;		
	} 

	/* Fill info */
	info.si_code = SI_USER;
	info.si_pid  = current_process->pid;
	info.si_uid  = current_process->uid;
	
	/* Handle various cases for pid */
	if (pid == 0) {		   
	
		/* pid = 0  -> Send signal to current process group */
		syscall_errno = ESRCH;
		if (process_signal_pgroup(
			current_process->pgid, sig, info ) == 0) {
			return (uint32_t) -1;
		} 
		syscall_errno = 0;
		return 0;
		
	} else if (pid == -1) {
		/* pid = -1 -> Send signal to all processes except init and things we
		   aren't allowed to signal. */
		//TODO: Implement kill all system processes
		syscall_errno = EINVAL;
		return (uint32_t)-1;
		
	} else if (pid < -1) {
		/* pid < -1 -> Send signal to process group -pgid */
	
		syscall_errno = ESRCH;
		if (process_signal_pgroup((pid_t) -pid, sig, info) == 0) {
			return (uint32_t) -1;
		} 
		syscall_errno = 0;
		return 0;
		
	} else {
		/* Signal process with id pid */
		
		process = process_get((pid_t) pid);
		if (process == NULL) {
			syscall_errno = ESRCH;
			return (uint32_t) -1;			
		}
		if (get_perm_class(process->uid, process->gid) != PERM_CLASS_OWNER){
			syscall_errno = EPERM;
			return (uint32_t) -1;
		}	
		process_send_signal(process, sig, info);
		return 0;
	}		
}

SYSCALL_DEF0(getpid)
{
	return (pid_t) current_process->pid;
}

SYSCALL_DEF0(getppid)
{
	return (pid_t) current_process->parent_pid;
}

SYSCALL_DEF0(getpgrp)
{
	return (pid_t) current_process->pgid;
}

SYSCALL_DEF0(getsid) //TODO: Not compliant, fix this
{
	return (pid_t) current_process->sid;
}

SYSCALL_DEF0(setsid)
{
	if (current_process->pgid == current_process->pid) {
		syscall_errno = EPERM;
		return (uint32_t) -1;	
	}
	current_process->pgid = current_process->pid;
	current_process->sid = current_process->pid;
	current_process->ctty = 0;
	return (pid_t) current_process->sid;
}

SYSCALL_DEF0(setpgrp)
{
	if (current_process->pgid == current_process->pid) {
		syscall_errno = EPERM;
		return (uint32_t) -1;	
	}
	current_process->pgid = current_process->pid;
	return 0;
}

SYSCALL_DEF2(setpgid)
{
	process_info_t *task;
	pid_t pid = (pid_t) a;//TODO: FIX PERMS
	pid_t pgid = (pid_t) b;
	if (pid == 0)
		pid = current_process->pid;
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


SYSCALL_DEF1(exit)
{
	current_process->exit_status = a;
	current_process->state = PROCESS_KILLED;
	process_child_event(current_process, PROCESS_CHILD_KILLED);
	stream_do_close_all (current_process);
	procvmm_clear_mmaps();
	schedule();
	return 0; // NEVER REACHED
}


SYSCALL_DEF0(yield)
{
	schedule();
	return 0;
}

#define WNOHANG 1
#define WUNTRACED 2

//pid_t pid, int *status, int options

SYSCALL_DEF3(waitpid)
{
	int status, options;
	pid_t pid;
	process_child_event_t *ev_info;
	process_info_t *chld;
	if (!copy_user_to_kern((void *)b, &status, sizeof(int))) {
		syscall_errno = EFAULT;
		return -1;
	}
	pid = (pid_t) a;
	options = (int) c;

	if (pid == 0)
		pid = -(current_process->pid);
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

				if (semaphore_idown(current_process->child_sema)) {
					syscall_errno = EINTR;
					return -1;
				}
			}
		}
	} else if (pid == -1) {
		if (options & WNOHANG) {
			while (  
				(!(ev_info = (process_child_event_t *) 
					llist_get_last(current_process->child_events))) ||
				 ((ev_info->event == PROCESS_CHILD_STOPPED) && !(options & WUNTRACED)) || 
				 (ev_info->event == PROCESS_CHILD_CONTD)) {

				if (ev_info == NULL)	
					return 0;	

				process_absorb_event(ev_info);
			}
		} else {
			while (  (!(ev_info = (process_child_event_t *) llist_get_last(current_process->child_events)))  ||
				 ((ev_info->event == PROCESS_CHILD_STOPPED) && !(options & WUNTRACED)) || 
				 (ev_info->event == PROCESS_CHILD_CONTD)) {

				if (ev_info != NULL)	
					process_absorb_event(ev_info);

				if (semaphore_idown(current_process->child_sema)) {
					syscall_errno = EINTR;
					return -1;
				}
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

				if (semaphore_idown(current_process->child_sema)) {
					syscall_errno = EINTR;
					return -1;
				}
			}
		}
	}
	chld = process_get(ev_info->child_pid);
	pid = ev_info->child_pid;
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
	if (!copy_kern_to_user(&status, (void *)b, sizeof(int))) {
		syscall_errno = EFAULT;
		return -1;
	}
	return pid;	
}

SYSCALL_DEF1(sbrk)
{
	uintptr_t old_brk   = (uintptr_t) current_process->heap_end;
	uintptr_t heap_max = (uintptr_t) current_process->heap_max;
	uintptr_t base = (uintptr_t) current_process->heap_start;
	uintptr_t old_size = old_brk - base;
	int incr = a;
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
		current_process->heap_end = (void *) (old_brk + incr);
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
SYSCALL_DEF3(execve)
{
	int pl,argvc, envpc,sc,ct;
	const char  *patho;
	const char **argvo;
	const char **envpo;
	char  *path;
	char **argv;
	char **envp;
	char  *user_str;
	ssize_t status;
	patho = ( const char * ) a;
	argvo = ( const char ** ) b;
	envpo = ( const char ** ) c;
	
	pl = procvmm_check_string( patho, CONFIG_FILE_MAX_NAME_LENGTH );
	if ( pl < 0 ) {
		syscall_errno = EFAULT;
		goto fault_a;
	}
	
	argvc = procvmm_check_stringlist( argvo, CONFIG_MAX_ARG_COUNT );
	if ( argvc < 0 ) {
		syscall_errno = EFAULT;
		goto fault_a;
	}
	
	envpc = procvmm_check_stringlist( envpo, CONFIG_MAX_ENV_COUNT );
	if ( envpc < 0 ) {
		syscall_errno = EFAULT;
		goto fault_a;
	}
	
	path = heapmm_alloc( pl * sizeof( char ) );
	if (!path) {
		syscall_errno = ENOMEM;
		goto fault_a;
	}
	argv = heapmm_alloc( argvc * sizeof ( char * ) );
	if (!argv) {
		syscall_errno = ENOMEM;
		goto fault_b;
	}
	envp = heapmm_alloc( envpc * sizeof ( char * ) );
	if (!envp) {
		syscall_errno = ENOMEM;
		goto fault_c;
	}
	
	if (!copy_user_to_kern(patho, path, pl)) {
		syscall_errno = EFAULT;
		goto fault_d;
	}
	if (!copy_user_to_kern(argvo, argv, argvc * sizeof(char *))) {
		syscall_errno = EFAULT;
		goto fault_d;
	}
	if (!copy_user_to_kern(envpo, envp, envpc * sizeof ( char * ))) {
		syscall_errno = EFAULT;
		goto fault_d;
	}
	
	for (ct = 0; ct < argvc - 1; ct++) {
		user_str = argv[ct];
		sc = procvmm_check_string( user_str, CONFIG_MAX_ARG_LENGTH );
		if (( sc < 0 ) || !(argv[ct] = heapmm_alloc(sc))) {
			debugcon_printf("alloc %x %i\n",user_str,sc);
			argvc = ct;
			syscall_errno = EFAULT;
			goto fault_e;		
		}
		if (!copy_user_to_kern(user_str, argv[ct], sc)) {
			debugcon_printf("copy\n");
			argvc = ct + 1;
			memset ( argv[ct], ' ', sc );
			argv[ct][sc - 1] = 0;
			syscall_errno = EFAULT;
			goto fault_e;
		}		
	}
	
	for (ct = 0; ct < envpc - 1; ct++) {
		user_str = envp[ct];
		sc = procvmm_check_string( user_str, CONFIG_MAX_ENV_LENGTH );
		if (( sc < 0 ) || !(envp[ct] = heapmm_alloc(sc))) {
			envpc = ct;
			syscall_errno = EFAULT;
			goto fault_f;		
		}
		if (!copy_user_to_kern(user_str, envp[ct], sc)) {
			envpc = ct + 1;
			memset ( envp[ct], ' ', sc );
			envp[ct][sc - 1] = 0;
			syscall_errno = EFAULT;
			goto fault_f;
		}		
	}
	
	status = process_exec( ( char * ) path, ( char ** ) argv, ( char ** ) envp);
	if ( status != 0 ) {
		syscall_errno = status;
		status = -1;
	}
	
	return (uint32_t) status;
fault_f:
	debugcon_printf("Fault f\n");
	for (ct = 0; ct < envpc; ct++) 
		heapmm_free(envp[ct], strlen(envp[ct])+1);
fault_e:
	debugcon_printf("Fault e\n");
	for (ct = 0; ct < argvc; ct++) 
		heapmm_free(argv[ct], strlen(argv[ct])+1);
fault_d:
	debugcon_printf("Fault d\n");
	heapmm_free(envp , envpc * sizeof ( char * ));
fault_c:
	debugcon_printf("Fault c\n");
	heapmm_free(argv , argvc * sizeof ( char * ));
fault_b:
	debugcon_printf("Fault b\n");
	heapmm_free(path , pl * sizeof( char ));
fault_a:
	debugcon_printf("Fault a\n");
	return (uint32_t) -1;
}

//void *_sys_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

SYSCALL_DEF6(mmap)
{
	return (uint32_t) _sys_mmap(	(void *) a, 
					(size_t) b, 
					(int)    c, 
					(int)    d,
					(int)    e,
					(int)    f);
}


