/**
 * kernel/streams.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 20-04-2014 - Created
 */
#include "kernel/vfs.h"
#include "kernel/pipe.h"
#include "util/llist.h"
#include <sys/types.h>

#ifndef __KERNEL_STREAMS_H__
#define __KERNEL_STREAMS_H__

#define STREAM_TYPE_FILE	(0)
#define STREAM_TYPE_PIPE	(1)

typedef struct stream_info stream_info_t;

typedef struct stream_ptr stream_ptr_t;

struct stream_ptr {
	llist_t  	node;
	int 		id;
	int		fd_flags;//NAME IS NOT FLAGS TO PREVENT CONFUSION WITH
				 //INFO FLAGS
	stream_info_t  *info;
};

struct stream_info {
	int		 type;
	int		 ref_count;
	int	 	 flags;
	aoff_t		 offset;
	inode_t		*inode;
	dir_cache_t	*dirc;
	semaphore_t	*lock;
	pipe_info_t	*pipe;
};

stream_ptr_t *stream_get_ptr (int fd);

int stream_copy_fd_table (llist_t *target);

void stream_do_close_on_exec ();

void stream_do_close_all (process_info_t *process);

ssize_t _sys_read(int fd, void * buffer, size_t count);

ssize_t _sys_write(int fd, void * buffer, size_t count);

off_t _sys_lseek(int fd, off_t offset, int whence);

int _sys_fchdir(int fd);

int _sys_fchmod(int fd, mode_t mode);

int _sys_fchown(int fd, uid_t owner, gid_t group);

int _sys_fcntl(int fd, int cmd, int arg);

int _sys_ioctl(int fd, int cmd, int arg);

int _sys_dup2(int oldfd, int newfd);

int _sys_dup(int oldfd);

int _sys_pipe2(int pipefd[2], int flags);

int _sys_open(char *path, int flags, mode_t mode);

int _sys_close(int fd);

int _sys_fstat(int fd, struct stat* buf);

ssize_t _sys_getdents(int fd, void * buffer, size_t count);

int _sys_close_int(process_info_t *process, int fd);

int _sys_truncate(int fd, off_t size);

#endif
