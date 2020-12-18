/******************************************************************************\
Copyright (C) 2017 Peter Bosch

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
\******************************************************************************/

/**
 * @file sys/syscall.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __SYS_SYSCALL_H__
#define __SYS_SYSCALL_H__

#include <stdint.h>

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
#define SYS_DBGDROP	58
#define SYS_MOUNT	59
#define SYS_FTRUNCATE	60
#define SYS_TRUNCATE	61
#define SYS_MMAP	62
#define SYS_SHMAT	63
#define SYS_SHMDT	64
#define SYS_SHMCTL	65
#define SYS_SHMGET	66
#define SYS_SEMOP	67
#define SYS_SEMCTL	68
#define SYS_SEMGET	69
#define SYS_MSGSND	70
#define SYS_MSGRCV	71
#define SYS_MSGCTL	72
#define SYS_MSGGET	73
#define SYS_MUNMAP	74
#define SYS_CHROOT	75
#define SYS_SYNC	76
#define SYS_READDIR	77
#define SYS_POLL	78
#define SYS_SIGACTION	79
#define SYS_SIGALTSTACK	80
#define SYS_SIGPENDING	81
#define SYS_SIGSUSPEND	82
#define SYS_UNAME	83
#define SYS_ACCESS  84

uint32_t syscall( int,	uint32_t a,
			uint32_t b,
			uint32_t c,
			uint32_t d,
			uint32_t e,
			uint32_t f);

#endif
