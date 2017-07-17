/**
 * kernel/syscall.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-04-2014 - Created
 */

#ifndef __KERNEL_SYSCALL_H__
#define __KERNEL_SYSCALL_H__
#include <stddef.h>
#include <stdint.h>
#include "kernel/scheduler.h"
#include "kernel/syscall_ids.h"

#define SYSCALL_DEF0(Name)	uint32_t sys_ ## Name ( \
	__attribute__(( unused )) uint32_t a,	\
	__attribute__(( unused )) uint32_t b,	\
	__attribute__(( unused )) uint32_t c,	\
	__attribute__(( unused )) uint32_t d,	\
	__attribute__(( unused )) uint32_t e,	\
	__attribute__(( unused )) uint32_t f )
	
#define SYSCALL_DEF1(Name)	uint32_t sys_ ## Name ( \
				  uint32_t a,	\
	__attribute__(( unused )) uint32_t b,	\
	__attribute__(( unused )) uint32_t c,	\
	__attribute__(( unused )) uint32_t d,	\
	__attribute__(( unused )) uint32_t e,	\
	__attribute__(( unused )) uint32_t f )

#define SYSCALL_DEF2(Name)	uint32_t sys_ ## Name ( \
				  uint32_t a,	\
				  uint32_t b,	\
	__attribute__(( unused )) uint32_t c,	\
	__attribute__(( unused )) uint32_t d,	\
	__attribute__(( unused )) uint32_t e,	\
	__attribute__(( unused )) uint32_t f )
	
#define SYSCALL_DEF3(Name)	uint32_t sys_ ## Name ( \
				  uint32_t a,	\
				  uint32_t b,	\
				  uint32_t c,	\
	__attribute__(( unused )) uint32_t d,	\
	__attribute__(( unused )) uint32_t e,	\
	__attribute__(( unused )) uint32_t f )
	
#define SYSCALL_DEF4(Name)	uint32_t sys_ ## Name ( \
				  uint32_t a,	\
				  uint32_t b,	\
				  uint32_t c,	\
				  uint32_t d,	\
	__attribute__(( unused )) uint32_t e,	\
	__attribute__(( unused )) uint32_t f )
	
#define SYSCALL_DEF5(Name)	uint32_t sys_ ## Name ( \
				  uint32_t a,	\
				  uint32_t b,	\
				  uint32_t c,	\
				  uint32_t d,	\
	                          uint32_t e,	\
	__attribute__(( unused )) uint32_t f )
	
#define SYSCALL_DEF6(Name)	uint32_t sys_ ## Name ( \
				  uint32_t a,	\
				  uint32_t b,	\
				  uint32_t c,	\
				  uint32_t d,	\
	                          uint32_t e,	\
	                          uint32_t f )

typedef uint32_t (*syscall_func_t)(	uint32_t a,
					uint32_t b,
					uint32_t c,
					uint32_t d,
					uint32_t e,
					uint32_t f);

void syscall_init();
void syscall_register(int call_id, syscall_func_t func);
void syscall_dispatch(void *user_param_block, void *instr_ptr);
uint32_t syscall_dispatch_new( int call,
				uint32_t a, 
				uint32_t b,
				uint32_t c,
				uint32_t d,
				uint32_t e,
				uint32_t f );
int copy_user_to_kern(const void *src, void *dest, size_t size);
int copy_kern_to_user(const void *src, void *dest, size_t size);


uint32_t sys_fork(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_kill(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_getpid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_getppid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_getpgrp(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_setsid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_exit(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_yield(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_sbrk(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_getuid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_geteuid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_getgid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_getegid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_link(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_unlink(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_chdir(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_chmod(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_chown(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_mknod(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_mkdir(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_stat(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_fchdir(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_fchmod(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_fchown(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_fstat(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_dup2(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_dup(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_open(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_close(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_pipe2(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_pipe(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_read(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_write(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_lseek(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_rmdir(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_execve(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_waitpid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_fcntl(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_umask(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_readlink(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_lstat(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_symlink(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_getdents(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_ioctl(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_getsid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_setpgrp(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_setpgid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_time(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_usleep(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_sleep(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_stime(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_setuid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_setgid(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_signal(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_exitsig(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_sigprocmask(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_ssigex(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_dbgdrop(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_mount(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_ftruncate(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_truncate(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_mmap(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_shmat(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_shmdt(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_shmctl(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_shmget(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_semop(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_semctl(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_semget(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_msgrcv(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_msgsnd(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_msgctl(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_msgget(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_chroot(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_sync(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_readdir(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_poll(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_sigaction(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_sigaltstack(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_sigsuspend(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_sigpending(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

uint32_t sys_uname( uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, uint32_t f);

#endif
