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
SYSCALL_DEF1(fchdir)
{
	return (uint32_t) _sys_fchdir((int) a);
}

//int fchmod(int fd, mode_t mode);
SYSCALL_DEF2(fchmod)
{
	return (uint32_t) _sys_fchmod((int) a, (mode_t) b);
}

//int fchown(char *path, uid_t owner, gid_t group);
SYSCALL_DEF3(fchown)
{
	return (uint32_t) _sys_fchown((int) a, (uid_t) b, (uid_t) c);
}

//int _sys_fcntl(int fd, int cmd, int arg)
SYSCALL_DEF3(fcntl)
{
	return (uint32_t) _sys_fcntl((int) a, (int) b, (int) c);
}

//int _sys_ioctl(int fd, int cmd, int arg)
SYSCALL_DEF3(ioctl)
{
	return (uint32_t) _sys_ioctl((int) a, (int) b, (int) c);
}

//int fstat(int id, struct stat* buf);
SYSCALL_DEF2(fstat)
{
	struct stat* buf;
	int status;
	buf = heapmm_alloc(sizeof(struct stat));
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	status = _sys_fstat((int) a, buf);
	if (!copy_kern_to_user(buf, (void *)b, sizeof(struct stat))) {
		syscall_errno = EFAULT;
		heapmm_free(buf, sizeof(struct stat));
		return (uint32_t) -1;
	}
	heapmm_free(buf, sizeof(struct stat));
	return (uint32_t) status;
}

//int dup2(int oldfd, int newfd);
SYSCALL_DEF2(dup2)
{
	return (uint32_t) _sys_dup2((int) a, (int) b);
}

//int dup(int fd);
SYSCALL_DEF1(dup)
{
	return (uint32_t) _sys_dup((int) a);
}

//int open(char *path, int flags, mode_t mode);
SYSCALL_DEF3(open)
{
	char *path;
	int sz;
	int status;
	sz = procvmm_check_string( (char *) a, CONFIG_FILE_MAX_NAME_LENGTH );
	if ( sz < 0 ) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	path = heapmm_alloc(sz);
	if (!copy_user_to_kern((void *)a, path, sz)) {
		syscall_errno = EFAULT;
		heapmm_free(path, sz);
		return (uint32_t) -1;
	}
	status = _sys_open(path, (int) b, (mode_t) c);
	heapmm_free(path, sz);
	return (uint32_t) status;
}

//int close(int fd);
SYSCALL_DEF1(close)
{
	return (uint32_t) _sys_close((int) a);
}

//int pipe2(int pipefd[2], int flags);
SYSCALL_DEF2(pipe2)
{
	int pipefd[2];
	int status;
	status = _sys_pipe2(pipefd, (int) b);
	if (!copy_kern_to_user(pipefd, (void *)a, sizeof(int[2]))) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	return (uint32_t) status;
}

//int pipe(int pipefd[2]);
SYSCALL_DEF1(pipe)
{
	int pipefd[2];
	int status;
	status = _sys_pipe2(pipefd, 0);
	if (!copy_kern_to_user(pipefd, (void *)a, sizeof(int[2]))) {
		syscall_errno = EFAULT;
		return (uint32_t) -1;
	}
	return (uint32_t) status;
}

//ssize_t _sys_getdents(int fd, void * buffer, size_t count)
SYSCALL_DEF3(getdents)
{
	void* buf;
	ssize_t status;
	buf = heapmm_alloc(c);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	memset(buf, 0, c);
	status = _sys_getdents((int) a, buf, (size_t) c);
	if (!copy_kern_to_user(buf, (void *)b, c)) {
		syscall_errno = EFAULT;
		heapmm_free(buf, c);
		return (uint32_t) -1;
	}
	heapmm_free(buf, c);
	return (uint32_t) status;
}

//ssize_t read(int fd, void * buffer, size_t count);
SYSCALL_DEF3(read)
{
	void* buf;
	ssize_t status;
	buf = heapmm_alloc(c);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	memset(buf, 0, c);
	status = _sys_read((int) a, buf, (size_t) c);
	if (!copy_kern_to_user(buf, (void *)b, c)) {
		syscall_errno = EFAULT;
		heapmm_free(buf, c);
		return (uint32_t) -1;
	}
	heapmm_free(buf, c);
	return (uint32_t) status;
}

//ssize_t readdir(int fd, void * buffer, size_t buflen);
SYSCALL_DEF3(readdir)
{
	void* buf;
	ssize_t status;
	buf = heapmm_alloc(c);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	memset(buf, 0, c);
	status = _sys_readdir((int) a, buf, (size_t) c);
	if (!copy_kern_to_user(buf, (void *)b, c)) {
		syscall_errno = EFAULT;
		heapmm_free(buf, c);
		return (uint32_t) -1;
	}
	heapmm_free(buf, c);
	return (uint32_t) status;
}

//ssize_t write(int fd, void * buffer, size_t count);
SYSCALL_DEF3(write)
{
	void* buf;
	ssize_t status;
	buf = heapmm_alloc(c);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *)b, buf, c)) {
		syscall_errno = EFAULT;
		heapmm_free(buf, c);
		return (uint32_t) -1;
	}
	status = _sys_write((int) a, buf, (size_t) c);
	heapmm_free(buf, c);
	return (uint32_t) status;
}

//int poll(struct pollfd[], nfds_t, int);
SYSCALL_DEF3(poll)
{
	void* buf;
	int no;
	size_t bs = b * sizeof(struct pollfd);
	if ( b > 4096 )
		return EFAULT;
	buf = heapmm_alloc(bs);
	if (!buf) {
		syscall_errno = ENOMEM;
		return (uint32_t) -1;
	}
	if (!copy_user_to_kern((void *)a, buf, bs)) {
		syscall_errno = EFAULT;
		heapmm_free(buf, bs);
		return (uint32_t) -1;
	}
	no = _sys_poll(buf, (nfds_t) b, (int) c);
	if (!copy_kern_to_user(buf, (void *)a, bs)) {
		syscall_errno = EFAULT;
		heapmm_free(buf, bs);
		return (uint32_t) -1;
	}
	heapmm_free(buf, bs);
	return (uint32_t) no;
}

//off_t lseek(int fd, off_t offset, int whence);
SYSCALL_DEF3(lseek)
{
	return (uint32_t) _sys_lseek((int) a, (off_t) b, (int) c);
}

//int ftruncate(int fd, off_t length);
SYSCALL_DEF2(ftruncate)
{
	return (uint32_t) _sys_ftruncate((int) a, (off_t) b);
}
