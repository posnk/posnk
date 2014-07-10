/**
 * kernel/syscall.c
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
#include "kernel/heapmm.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "kernel/signals.h"
#include "kernel/paging.h"
#include "kernel/syscall.h"
#include "kernel/earlycon.h"
#include "kdbg/dbgapi.h"
#include "config.h"

char *syscall_names[] =
{
	"dbgputs",
	"fork",
	"kill",
	"getpid",
	"getpgid",
	"setsid",
	"exit",
	"yield",
	"sbrk",
	"getuid",
	"geteuid",
	"getgid",
	"getegid",
	"link",
	"unlink",
	"chdir",
	"chown",
	"chmod",
	"mknod",
	"mkdir",
	"stat",
	"fchdir",
	"fchown",
	"fchmod",
	"fstat",
	"dup2",
	"dup",
	"open",
	"close",
	"pipe2",
	"pipe",
	"read",
	"write",
	"lseek",
	"rmdir",
	"execve",
	"waitpid",
	"getppid",
	"fcntl",
	"umask",
	"readlink",
	"lstat",
	"symlink",
	"getdents",
	"ioctl",
	"setpgrp",
	"getsid",
	"setpgid",
	"time",
	"usleep",
	"sleep",
	"stime",
	"setuid",
	"setgid",
	"signal",
	"exitsig",
	"sigprocmask",
	"ssigex",
	"dbgdrop",
	"mount"
};

syscall_func_t syscall_table[CONFIG_MAX_SYSCALL_COUNT];

//TODO: Find way to free syscall buffers after signal kill

void syscall_register(int call_id, syscall_func_t func)
{
	syscall_table[call_id] = func;
}

int copy_user_to_kern(void *src, void *dest, size_t size)
{
	if (!procvmm_check(dest,size))
		return 0;
	memcpy(dest,src,size);	
	return 1;
}

int copy_kern_to_user(void *src, void *dest, size_t size)
{
	if (!procvmm_check(dest,size))
		return 0;
	memcpy(dest,src,size);	
	return 1;
}

uint32_t debug_uputs(uint32_t param[4], uint32_t param_size[4])
{
	char *buffer;
	if (param[1] > 256) {
		syscall_errno = 1; //TODO: ENOMEM
		return 0;
	}
	buffer = heapmm_alloc(param_size[0]);
	copy_user_to_kern((void *)param[0], buffer, param_size[0]);
	buffer[param_size[0] - 1] = '\0';
	earlycon_puts(buffer);
	heapmm_free(buffer, param_size[0]);
	return 1;	
}

uint32_t sys_dbgdrop(uint32_t param[4], uint32_t param_size[4])
{
	dbgapi_invoke_kdbg(0);
	return 1;	
}

int curpid();
void syscall_dispatch(void *user_param_block, void *instr_ptr)
{
	int result,call;

	syscall_params_t params;
	if (!copy_user_to_kern(user_param_block, &params, sizeof(syscall_params_t))) {	
		debugcon_printf("Error copying data for syscall in process <%s>[%i] at 0x%x, data: 0x%x\n",scheduler_current_task->name,curpid(), instr_ptr, user_param_block);
		process_send_signal(scheduler_current_task, SIGSEGV);
	}
	if ((params.magic != SYSCALL_MAGIC) || (params.call_id > CONFIG_MAX_SYSCALL_COUNT) || syscall_table[params.call_id] == NULL)
		process_send_signal(scheduler_current_task, SIGSYS);
	syscall_errno = 0;
	call = params.call_id;
#ifdef CONFIG_SYSCALL_DEBUG
	if ((call == SYS_OPEN) || (call == SYS_STAT))
		debugcon_printf("[%s:%i] %s(\"%s\", %x, %x, %x) = ", scheduler_current_task->name, curpid(), syscall_names[call], params.param[0], params.param[1], params.param[2], params.param[3]);
	else
		debugcon_printf("[%s:%i] %s(%x, %x, %x, %x) = ", scheduler_current_task->name, curpid(), syscall_names[call], params.param[0], params.param[1], params.param[2], params.param[3]);
#endif
	result = syscall_table[params.call_id]((uint32_t*)params.param, (uint32_t*)params.param_size);
#ifdef CONFIG_SYSCALL_DEBUG
	debugcon_printf("%x (Errno: %i)\n", result,syscall_errno);
#endif
	params.return_val = result;
	params.sc_errno = syscall_errno;
	copy_kern_to_user(&params, user_param_block, sizeof(syscall_params_t));
	process_handle_signals();
	//debugcon_printf("im still alive, yeeaahhh\n");
	//heapmm_free(params,sizeof(syscall_params_t));
}

void syscall_init()
{
	syscall_register(0,&debug_uputs);
	syscall_register(SYS_FORK, &sys_fork);
	syscall_register(SYS_KILL, &sys_kill);
	syscall_register(SYS_GETPID, &sys_getpid);
	syscall_register(SYS_GETPGRP, &sys_getpgrp);
	syscall_register(SYS_SETSID, &sys_setsid);
	syscall_register(SYS_EXIT, &sys_exit);
	syscall_register(SYS_YIELD, &sys_yield);
	syscall_register(SYS_SBRK, &sys_sbrk);
	syscall_register(SYS_GETUID, &sys_getuid);
	syscall_register(SYS_GETEUID, &sys_geteuid);
	syscall_register(SYS_GETGID, &sys_getgid);
	syscall_register(SYS_GETEGID, &sys_getegid);
	syscall_register(SYS_LINK, &sys_link);
	syscall_register(SYS_UNLINK, &sys_unlink);
	syscall_register(SYS_CHDIR, &sys_chdir);
	syscall_register(SYS_CHOWN, &sys_chown);
	syscall_register(SYS_CHMOD, &sys_chmod);
	syscall_register(SYS_MKDIR, &sys_mkdir);
	syscall_register(SYS_MKNOD, &sys_mknod);
	syscall_register(SYS_STAT, &sys_stat);
	syscall_register(SYS_FCHDIR, &sys_fchdir);
	syscall_register(SYS_FCHOWN, &sys_fchown);
	syscall_register(SYS_FCHMOD, &sys_fchmod);
	syscall_register(SYS_FSTAT, &sys_fstat);
	syscall_register(SYS_DUP2, &sys_dup2);
	syscall_register(SYS_DUP, &sys_dup);
	syscall_register(SYS_OPEN, &sys_open);
	syscall_register(SYS_CLOSE, &sys_close);
	syscall_register(SYS_PIPE2, &sys_pipe2);
	syscall_register(SYS_PIPE, &sys_pipe);
	syscall_register(SYS_READ, &sys_read);
	syscall_register(SYS_WRITE, &sys_write);
	syscall_register(SYS_LSEEK, &sys_lseek);
	syscall_register(SYS_RMDIR, &sys_rmdir);
	syscall_register(SYS_EXECVE, &sys_execve);
	syscall_register(SYS_WAITPID, &sys_waitpid);
	syscall_register(SYS_GETPPID, &sys_getppid);
	syscall_register(SYS_FCNTL, &sys_fcntl);
	syscall_register(SYS_UMASK, &sys_umask);	
	syscall_register(SYS_READLINK, &sys_readlink);
	syscall_register(SYS_LSTAT, &sys_lstat);
	syscall_register(SYS_SYMLINK, &sys_symlink);
	syscall_register(SYS_GETDENTS, &sys_getdents);
	syscall_register(SYS_IOCTL, &sys_ioctl);
	syscall_register(SYS_GETSID, &sys_getsid);
	syscall_register(SYS_SETPGRP, &sys_setpgrp);
	syscall_register(SYS_SETPGID, &sys_setpgid);
	syscall_register(SYS_TIME, &sys_time);
	syscall_register(SYS_USLEEP, &sys_usleep);
	syscall_register(SYS_SLEEP, &sys_sleep);
	syscall_register(SYS_STIME, &sys_stime);
	syscall_register(SYS_SETUID, &sys_setuid);
	syscall_register(SYS_SETGID, &sys_setgid);
	syscall_register(SYS_SIGNAL, &sys_signal);
	syscall_register(SYS_EXITSIG, &sys_exitsig);
	syscall_register(SYS_SIGPROCMASK, &sys_sigprocmask);
	syscall_register(SYS_SSIGEX, &sys_ssigex);
	syscall_register(SYS_DBGDROP, &sys_dbgdrop);
	syscall_register(SYS_MOUNT, &sys_mount);
}
