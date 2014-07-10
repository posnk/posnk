/**
 * kernel/syscall_ids.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 09-04-2014 - Created
 */

#ifndef __KERNEL_SYSCALL_IDS_H__
#define __KERNEL_SYSCALL_IDS_H__

#include <stdint.h>

#define SYSCALL_MAGIC	(0xCAFECA11)

#define syscall_errno	(scheduler_current_task->sc_errno)

struct syscall_params {
	volatile uint32_t	magic;
	volatile uint32_t	call_id;
	volatile uint32_t	param[4];
	volatile uint32_t	param_size[4];
	volatile uint32_t	return_val;
	volatile uint32_t	sc_errno;
};

typedef struct syscall_params syscall_params_t;

#define SYS_FORK	1
#define SYS_KILL	2
#define SYS_GETPID	3
#define SYS_GETPGRP	4
#define SYS_SETSID	5
#define SYS_EXIT	6
#define SYS_YIELD	7
#define SYS_SBRK	8
#define SYS_GETUID	9
#define SYS_GETEUID	10
#define SYS_GETGID	11
#define SYS_GETEGID	12
#define SYS_LINK	13
#define SYS_UNLINK	14
#define SYS_CHDIR	15
#define SYS_CHOWN	16
#define SYS_CHMOD	17
#define SYS_MKNOD	18
#define SYS_MKDIR	19
#define SYS_STAT	20
#define SYS_FCHDIR	21
#define SYS_FCHOWN	22
#define SYS_FCHMOD	23
#define SYS_FSTAT	24
#define SYS_DUP2	25
#define SYS_DUP		26
#define SYS_OPEN	27
#define SYS_CLOSE	28
#define SYS_PIPE2	29
#define SYS_PIPE	30
#define SYS_READ	31
#define SYS_WRITE	32
#define SYS_LSEEK	33
#define SYS_RMDIR	34
#define SYS_EXECVE	35
#define SYS_WAITPID	36
#define SYS_GETPPID	37
#define SYS_FCNTL	38
#define SYS_UMASK	39
#define SYS_READLINK	40
#define SYS_LSTAT	41
#define SYS_SYMLINK	42
#define SYS_GETDENTS	43
#define SYS_IOCTL	44
#define SYS_SETPGRP	45
#define SYS_GETSID	46
#define SYS_SETPGID	47
#define SYS_TIME	48
#define SYS_USLEEP	49
#define SYS_SLEEP	50
#define SYS_STIME	51
#define SYS_SETUID	52
#define SYS_SETGID	53
#define SYS_SIGNAL	54
#define SYS_EXITSIG	55
#define SYS_SIGPROCMASK	56
#define SYS_SSIGEX	57

uint32_t nk_do_syscall(uint32_t no, uint32_t param[4], uint32_t param_size[4]);
#endif
