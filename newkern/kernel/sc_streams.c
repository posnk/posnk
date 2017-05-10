/*
 * kernel/sc_vfs.c
 *
 * Part of P-OS kernel.
 *
 * Contains process related syscalls
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 22-04-2014 - Created
 */

#include <string.h>
#include <stdint.h>
#include "kernel/syscall.h"
#include "kernel/streams.h"
#include "kernel/heapmm.h"
#include <sys/errno.h>
#include <sys/stat.h>

//int fchdir(int fd);
uint32_t sys_fchdir(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_fchdir((int) param[0]);
}

//int fchmod(int fd, mode_t mode);

uint32_t sys_fchmod(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_fchmod((int) param[0], (mode_t) param[1]);
}

//int fchown(char *path, uid_t owner, gid_t group);
uint32_t sys_fchown(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_fchown((int) param[0], (uid_t) param[1], (uid_t) param[2]);
}

//int _sys_fcntl(int fd, int cmd, int arg)
uint32_t sys_fcntl(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_fcntl((int) param[0], (int) param[1], (int) param[2]);
}

//int _sys_ioctl(int fd, int cmd, int arg)
uint32_t sys_ioctl(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_ioctl((int) param[0], (int) param[1], (int) param[2]);
}

//int fstat(int id, struct stat* buf);
uint32_t sys_fstat(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	struct stat* buf;
	int status;
	buf = heapmm_alloc(sizeof(struct stat));
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	status = _sys_fstat((int) param[0], buf);
	if (!copy_kern_to_user(buf, (void *)param[1], sizeof(struct stat))) {
		syscall_errno = EFAULT;
		heapmm_free(buf, sizeof(struct stat));
		return (uint32_t) -1;
	}
	heapmm_free(buf, sizeof(struct stat));
	return (uint32_t) status;
}

//int dup2(int oldfd, int newfd);
uint32_t sys_dup2(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_dup2((int) param[0], (int) param[1]);
}

//int dup2(int fd);
uint32_t sys_dup(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_dup((int) param[0]);
}

//int open(char *path, int flags, mode_t mode);
uint32_t sys_open(uint32_t param[4], uint32_t param_size[4])
{
	char *path;
	int status;
	if ((param_size[0] > CONFIG_FILE_MAX_NAME_LENGTH)) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(param_size[0]);
	if (!copy_user_to_kern((void *)param[0], path, param_size[0])) {
		syscall_errno = EFAULT;
		heapmm_free(path, param_size[0]);
		return (uint32_t) -1;
	}
	status = _sys_open(path, (int) param[1], (mode_t) param[2]);
	heapmm_free(path, param_size[0]);
	return (uint32_t) status;
}

//int close(int fd);
uint32_t sys_close(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_close((int) param[0]);
}

//int pipe2(int pipefd[2], int flags);
uint32_t sys_pipe2(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	int pipefd[2];
	int status;
	status = _sys_pipe2(pipefd, (int) param[1]);
	if (!copy_kern_to_user(pipefd, (void *)param[0], sizeof(int[2]))) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	return (uint32_t) status;
}

//int pipe(int pipefd[2]);
uint32_t sys_pipe(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	int pipefd[2];
	int status;
	status = _sys_pipe2(pipefd, 0);
	if (!copy_kern_to_user(pipefd, (void *)param[0], sizeof(int[2]))) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	return (uint32_t) status;
}

//ssize_t _sys_getdents(int fd, void * buffer, size_t count)
uint32_t sys_getdents(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	void* buf;
	ssize_t status;
	buf = heapmm_alloc(param[2]);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	memset(buf, 0, param[2]);
	status = _sys_getdents((int) param[0], buf, (size_t) param[2]);
	if (!copy_kern_to_user(buf, (void *)param[1], param[2])) {
		syscall_errno = EFAULT;
		heapmm_free(buf, param[2]);
		return (uint32_t) -1;
	}
	heapmm_free(buf, param[2]);
	return (uint32_t) status;
}

//ssize_t read(int fd, void * buffer, size_t count);
uint32_t sys_read(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	void* buf;
	ssize_t status;
	buf = heapmm_alloc(param[2]);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	memset(buf, 0, param[2]);
	status = _sys_read((int) param[0], buf, (size_t) param[2]);
	if (!copy_kern_to_user(buf, (void *)param[1], param[2])) {
		syscall_errno = EFAULT;
		heapmm_free(buf, param[2]);
		return (uint32_t) -1;
	}
	heapmm_free(buf, param[2]);
	return (uint32_t) status;
}

//ssize_t readdir(int fd, void * buffer, size_t buflen);
uint32_t sys_readdir(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	void* buf;
	ssize_t status;
	buf = heapmm_alloc(param[2]);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	memset(buf, 0, param[2]);
	status = _sys_readdir((int) param[0], buf, (size_t) param[2]);
	if (!copy_kern_to_user(buf, (void *)param[1], param[2])) {
		syscall_errno = EFAULT;
		heapmm_free(buf, param[2]);
		return (uint32_t) -1;
	}
	heapmm_free(buf, param[2]);
	return (uint32_t) status;
}

//ssize_t write(int fd, void * buffer, size_t count);
uint32_t sys_write(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	void* buf;
	ssize_t status;
	buf = heapmm_alloc(param[2]);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *)param[1], buf, param[2])) {
		syscall_errno = EFAULT;
		heapmm_free(buf, param[2]);
		return (uint32_t) -1;
	}
	status = _sys_write((int) param[0], buf, (size_t) param[2]);
	heapmm_free(buf, param[2]);
	return (uint32_t) status;
}

//int poll(struct pollfd[], nfds_t, int);
uint32_t sys_poll(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	void* buf;
	int no;
	size_t bs = param[1] * sizeof(struct pollfd);
	buf = heapmm_alloc(bs);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *)param[0], buf, bs)) {
		syscall_errno = EFAULT;
		heapmm_free(buf, bs);
		return (uint32_t) -1;
	}
	no = _sys_poll(buf, (nfds_t) param[1], (int) param[2]);
	if (!copy_kern_to_user(buf, (void *)param[0], bs)) {
		syscall_errno = EFAULT;
		heapmm_free(buf, bs);
		return (uint32_t) -1;
	}
	heapmm_free(buf, bs);
	return (uint32_t) no;
}

//off_t lseek(int fd, off_t offset, int whence);
uint32_t sys_lseek(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_lseek((int) param[0], (off_t) param[1], (int) param[2]);
}

//int ftruncate(int fd, off_t length);
uint32_t sys_ftruncate(uint32_t param[4], __attribute__((__unused__)) uint32_t param_size[4])
{
	return (uint32_t) _sys_ftruncate((int) param[0], (off_t) param[1]);
}
