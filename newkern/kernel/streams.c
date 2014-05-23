/**
 * kernel/streams.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 20-04-2014 - Created
 */
#include "kernel/syscall.h"
#include "kernel/streams.h"
#include "kernel/scheduler.h"
#include "kernel/vfs.h"
#include "kernel/heapmm.h"
#include "kernel/pipe.h"
#include "kernel/permissions.h"
#include "kernel/device.h"
#include "kernel/process.h"
#include "kernel/signals.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int stream_ptr_search_iterator (llist_t *node, void *param)
{
	int id = (int) param;
	stream_ptr_t *ptr = (stream_ptr_t *) node;
	return ptr->id == id;
}

stream_ptr_t *stream_get_ptr (int fd)
{
	return (stream_ptr_t *) llist_iterate_select(scheduler_current_task->fd_table, &stream_ptr_search_iterator, (void *) fd);
}

stream_ptr_t *stream_get_ptr_o (process_info_t *process, int fd)
{
	return (stream_ptr_t *) llist_iterate_select(process->fd_table, &stream_ptr_search_iterator, (void *) fd);
}

int stream_ptr_copy_iterator (llist_t *node, void *param)
{
	llist_t *table = (llist_t *) param;
	stream_ptr_t *ptr = (stream_ptr_t *) node;
	stream_ptr_t *newptr;
	newptr = heapmm_alloc(sizeof(stream_ptr_t));
	if (!newptr) {
		return 1;
	}
	newptr->id = ptr->id;
	newptr->info = ptr->info;
	newptr->fd_flags = 0;
	newptr->info->ref_count++;
	llist_add_end(table, (llist_t *) newptr);	
	return 0;
}

int stream_copy_fd_table (llist_t *target)
{
	return llist_iterate_select(scheduler_current_task->fd_table, &stream_ptr_copy_iterator, (void *) target) == NULL;
}

int stream_exec_iterator (llist_t *node, __attribute__((__unused__)) void *param)
{
	stream_ptr_t *ptr = (stream_ptr_t *) node;
	return ptr->fd_flags & FD_CLOEXEC;
}

void stream_do_close_on_exec ()
{
	stream_ptr_t *ptr;
	for (ptr = (stream_ptr_t *) llist_iterate_select(scheduler_current_task->fd_table, &stream_exec_iterator, NULL); ptr != NULL; 
	     ptr = (stream_ptr_t *) llist_iterate_select(scheduler_current_task->fd_table, &stream_exec_iterator, NULL))
		_sys_close(ptr->id);
}

int stream_closeall_iterator ( __attribute__((__unused__)) llist_t *node, __attribute__((__unused__)) void *param)
{
	return 1;
}

void stream_do_close_all (process_info_t *process)
{
	stream_ptr_t *ptr;
	for (ptr = (stream_ptr_t *) llist_iterate_select(process->fd_table, &stream_closeall_iterator, NULL); ptr != NULL; 
	     ptr = (stream_ptr_t *) llist_iterate_select(process->fd_table, &stream_closeall_iterator, NULL))
		_sys_close_int(process, ptr->id);
}

//int getdents(unsigned int fd, struct linux_dirent *dirp,
//                    unsigned int count);


ssize_t _sys_getdents(int fd, void * buffer, size_t count)
{
	size_t read_count;
	int st;
	stream_ptr_t *ptr = stream_get_ptr(fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	semaphore_down(ptr->info->lock);
	if ((ptr->info->flags & O_ACCMODE) == O_WRONLY) {
		semaphore_up(ptr->info->lock);
		syscall_errno = EINVAL;
		return -1;		
	}
	switch (ptr->info->type) {
		case STREAM_TYPE_FILE:
			if (ptr->info->offset > ptr->info->inode->size) {
				memset(buffer, 0, count);
				read_count = count;
			} else {
				st = vfs_getdents(ptr->info->inode, ptr->info->offset, buffer, count, &read_count);
				if (st != 0) {
					if (read_count != 0)
						ptr->info->offset += (off_t) read_count;
					syscall_errno = st;		
					semaphore_up(ptr->info->lock);
					return -1;
				}
			}
			ptr->info->offset += (off_t) read_count;			
			semaphore_up(ptr->info->lock);
			return (ssize_t) read_count;
		case STREAM_TYPE_PIPE:
			semaphore_up(ptr->info->lock);
			syscall_errno = ENOTDIR;
			return -1;
		default:
			semaphore_up(ptr->info->lock);
			syscall_errno = EINVAL;
			return -1;
	}
}

ssize_t _sys_read(int fd, void * buffer, size_t count)
{
	size_t read_count;
	int st;
	stream_ptr_t *ptr = stream_get_ptr(fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	semaphore_down(ptr->info->lock);
	if ((ptr->info->flags & O_ACCMODE) == O_WRONLY) {
		semaphore_up(ptr->info->lock);
		syscall_errno = EINVAL;
		return -1;		
	}
	switch (ptr->info->type) {
		case STREAM_TYPE_FILE:
			if ((ptr->info->offset > ptr->info->inode->size) && S_ISREG(ptr->info->inode->mode)) {
				memset(buffer, 0, count);
				read_count = count;
			} else {
				st = vfs_read(ptr->info->inode, ptr->info->offset, buffer, count, &read_count, ptr->info->flags & O_NONBLOCK);
				if (st != 0) {
					if (read_count != 0)
						ptr->info->offset += (off_t) read_count;
					syscall_errno = st;
					semaphore_up(ptr->info->lock);
					return -1;
				}
			}
			ptr->info->offset += (off_t) read_count;			
			semaphore_up(ptr->info->lock);
			return (ssize_t) read_count;
		case STREAM_TYPE_PIPE:
			semaphore_up(ptr->info->lock);
			st = pipe_read(ptr->info->pipe, buffer, count, &read_count, ptr->info->flags & O_NONBLOCK);
			if (st != 0) {
				syscall_errno = st;
				if (st == EPIPE)
					process_send_signal(scheduler_current_task, SIGPIPE);
				return (read_count == 0) ? ((ssize_t)-1) : (ssize_t) read_count; 
			}
			return (ssize_t) read_count;
		default:
			semaphore_up(ptr->info->lock);
			syscall_errno = EINVAL;
			return -1;
	}
}

ssize_t _sys_write(int fd, void * buffer, size_t count)
{
	size_t read_count;
	int st;
	stream_ptr_t *ptr = stream_get_ptr(fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	semaphore_down(ptr->info->lock);
	if ((ptr->info->flags & O_ACCMODE) == O_RDONLY) {
		semaphore_up(ptr->info->lock);
		syscall_errno = EINVAL;
		return -1;		
	}
	switch (ptr->info->type) {
		case STREAM_TYPE_FILE:
			st = vfs_write(ptr->info->inode, ptr->info->offset, buffer, count, &read_count, ptr->info->flags & O_NONBLOCK);
			if (st != 0) {
				if (read_count != 0)
					ptr->info->offset += (off_t) read_count;
				syscall_errno = st;
				semaphore_up(ptr->info->lock);
				return -1;
			}
			ptr->info->offset += (off_t) read_count;
			semaphore_up(ptr->info->lock);
			return (ssize_t) read_count;
		case STREAM_TYPE_PIPE:
			semaphore_up(ptr->info->lock);
			st = pipe_write(ptr->info->pipe, buffer, count, &read_count, ptr->info->flags & O_NONBLOCK);
			if (st != 0) {
				if (st == EPIPE)
					process_send_signal(scheduler_current_task, SIGPIPE);
				syscall_errno = st;
				return (read_count == 0) ? -1 : (ssize_t) read_count; 
			}
			return (ssize_t) read_count;
		default:
			semaphore_up(ptr->info->lock);
			syscall_errno = EINVAL;
			return -1;
	}
}

off_t _sys_lseek(int fd, off_t offset, int whence)
{
	off_t old_offset;
	stream_ptr_t *ptr = stream_get_ptr(fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return (off_t) -1;
	}
	if ((ptr->info->type != STREAM_TYPE_FILE) || (S_ISFIFO(ptr->info->inode->mode))) {
		syscall_errno = ESPIPE;
		return (off_t) -1;
	}
	semaphore_down(ptr->info->lock);
	old_offset = ptr->info->offset;
	switch (whence) {
		case SEEK_SET:
			ptr->info->offset = offset;
			break;
		case SEEK_CUR:
			ptr->info->offset += offset;
			break;
		case SEEK_END:
			ptr->info->offset = ptr->info->inode->size + offset;
			break;
		default:
			ptr->info->offset = (off_t) -1;
			break;			
	}
	if (ptr->info->offset < 0) {
		ptr->info->offset = old_offset;
		syscall_errno = EINVAL;
		semaphore_up(ptr->info->lock);
		return (off_t) -1;
	}
	semaphore_up(ptr->info->lock);
	return ptr->info->offset;
}

int _sys_fchdir(int fd)
{
	stream_ptr_t *ptr = stream_get_ptr(fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	if ((ptr->info->type != STREAM_TYPE_FILE) || (!S_ISDIR(ptr->info->inode->mode))) {
		syscall_errno = ENOTDIR;
		return -1;
	}
	scheduler_current_task->current_directory->usage_count--;
	ptr->info->dirc->usage_count++;
	scheduler_current_task->current_directory = ptr->info->dirc;
	return 0;
}

int _sys_fstat(int fd, struct stat* buf)
{
	stream_ptr_t *ptr = stream_get_ptr(fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	if ((ptr->info->type != STREAM_TYPE_FILE)) {
		syscall_errno = EBADF;
		return -1;
	}
	buf->st_dev  = (dev_t) ptr->info->inode->device_id;//TODO: FIX
	buf->st_ino  = ptr->info->inode->id;
	buf->st_rdev = ptr->info->inode->if_dev;
	buf->st_size = ptr->info->inode->size;
	buf->st_mode = ptr->info->inode->mode;
	//buf->st_blocks = buf->st_size * 512;//TODO: Implement sparse files
	//buf->st_blksize = 512;//TODO: Ask FS about block size
	buf->st_nlink = ptr->info->inode->hard_link_count;
	buf->st_uid = ptr->info->inode->uid;
	buf->st_gid = ptr->info->inode->gid;
	buf->st_atime = 0; //TODO: Monitor file time
	buf->st_mtime = 0; //TODO: Monitor file time
	buf->st_ctime = 0; //TODO: Monitor file time
	return 0;
}

int _sys_fchmod(int fd, mode_t mode)
{
	stream_ptr_t *ptr = stream_get_ptr(fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	if ((ptr->info->type != STREAM_TYPE_FILE)) {
		syscall_errno = EBADF;
		return -1;
	}
	if (get_perm_class(ptr->info->inode->uid, ptr->info->inode->gid) != PERM_CLASS_OWNER) {
		syscall_errno = EPERM;
		return -1;
	}
	if (get_effective_uid() != 0)
		mode &= 0777;
	ptr->info->inode->mode &= ~07777;
	ptr->info->inode->mode |= mode & 07777;
	return 0;
}

int _sys_fchown(int fd, uid_t owner, gid_t group)
{
	stream_ptr_t *ptr = stream_get_ptr(fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	if ((ptr->info->type != STREAM_TYPE_FILE)) {
		syscall_errno = EBADF;
		return -1;
	}
	if (get_perm_class(ptr->info->inode->uid, ptr->info->inode->gid) != PERM_CLASS_OWNER) {
		syscall_errno = EPERM;
		return -1;
	}
	if (get_perm_class(-1, group) != PERM_CLASS_GROUP)
		group = 65535;
	if (group != 65535)
		ptr->info->inode->gid = group;
	if (owner != 65535)
		ptr->info->inode->uid = owner;
	return 0;
}

int _sys_fcntl(int fd, int cmd, int arg)
{
	stream_ptr_t *ptr = stream_get_ptr(fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	switch (cmd) {
		case F_DUPFD:
			if (!stream_get_ptr(arg))
				return _sys_dup2(fd, arg);
			else
				return _sys_dup(fd);
		case F_GETFD:
			return ptr->fd_flags; 
		case F_SETFD:
			ptr->fd_flags = arg;
			return 0;
		case F_GETFL:
			return ptr->info->flags;
		case F_SETFL:
			ptr->info->flags &= ~(O_APPEND | FASYNC | O_NONBLOCK);
			ptr->info->flags |= arg & (O_APPEND | FASYNC | O_NONBLOCK);
			return 0;
		default:
			return 0;
	}
}

int _sys_ioctl(int fd, int cmd, int arg)
{
	stream_ptr_t *ptr = stream_get_ptr(fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	if ((ptr->info->type != STREAM_TYPE_FILE)) {
		syscall_errno = ENOTTY;
		return -1;
	}
	if (!S_ISCHR(ptr->info->inode->mode)) {
		syscall_errno = ENOTTY;
		return -1;
	}
	return device_char_ioctl(ptr->info->inode->if_dev, fd, cmd, arg);
}

int _sys_dup2(int oldfd, int newfd)
{
	stream_ptr_t *oldptr = stream_get_ptr(oldfd);
	stream_ptr_t *newptr = stream_get_ptr(newfd);
	if (!oldptr) {
		syscall_errno = EBADF;
		return -1;
	}
	if (newptr) {
		_sys_close(newfd);
	}
	if (newfd < 0) {
		syscall_errno = EBADF;
		return -1;
	}
	newptr = heapmm_alloc(sizeof(stream_ptr_t));
	if (!newptr) {
		syscall_errno = EMFILE;
		return -1;
	}
	if (newfd >= scheduler_current_task->fd_ctr)
		scheduler_current_task->fd_ctr = newfd + 1;
	newptr->info = oldptr->info;
	newptr->id   = newfd;
	newptr->fd_flags = 0;
	newptr->info->ref_count++;
	llist_add_end(scheduler_current_task->fd_table, (llist_t *) newptr);
	return newfd;	
}

int _sys_dup(int oldfd)
{
	return _sys_dup2(oldfd, scheduler_current_task->fd_ctr++);
}

int _sys_pipe2(int pipefd[2], int flags)
{
	pipe_info_t	*pipe;
	stream_info_t	*info_read;
	stream_info_t	*info_write;
	stream_ptr_t	*ptr_read;
	stream_ptr_t	*ptr_write;	
	pipefd[0] = scheduler_current_task->fd_ctr++;//READ
	pipefd[1] = scheduler_current_task->fd_ctr++;//WRITE
	pipe = pipe_create();
	if (!pipe) {
		syscall_errno = ENOMEM;
		return -1;
	}
	info_read = heapmm_alloc(sizeof(stream_info_t));
	if (!info_read) {
		pipe_free(pipe);
		syscall_errno = ENOMEM;
		return -1;
	}
	ptr_read = heapmm_alloc(sizeof(stream_ptr_t));
	if (!ptr_read) {
		heapmm_free(info_read, sizeof(stream_info_t));
		pipe_free(pipe);
		syscall_errno = ENOMEM;
		return -1;
	}
	info_write = heapmm_alloc(sizeof(stream_info_t));
	if (!info_write) {
		heapmm_free(info_read, sizeof(stream_info_t));
		heapmm_free(ptr_read, sizeof(stream_ptr_t));
		pipe_free(pipe);
		syscall_errno = ENOMEM;
		return -1;
	}
	ptr_write = heapmm_alloc(sizeof(stream_ptr_t));
	if (!ptr_write) {
		heapmm_free(info_read, sizeof(stream_info_t));
		heapmm_free(info_write, sizeof(stream_info_t));
		heapmm_free(ptr_read, sizeof(stream_ptr_t));
		pipe_free(pipe);
		syscall_errno = ENOMEM;
		return -1;
	}
	ptr_read->id = pipefd[0];
	ptr_read->info = info_read;
	ptr_read->fd_flags = 0;
	ptr_write->id = pipefd[1];
	ptr_write->info = info_write;
	ptr_write->fd_flags = 0;
	info_read->pipe = pipe;
	info_read->flags = flags | O_RDONLY;
	info_read->type = STREAM_TYPE_PIPE;
	info_read->offset = 0;
	info_read->ref_count = 1;
	info_read->lock = semaphore_alloc();
	if (!(info_read->lock)) {
		heapmm_free(info_read, sizeof(stream_info_t));
		heapmm_free(info_write, sizeof(stream_info_t));
		heapmm_free(ptr_read, sizeof(stream_ptr_t));
		heapmm_free(ptr_write, sizeof(stream_ptr_t));
		pipe_free(pipe);
		syscall_errno = ENOMEM;
		return -1;
	}
	info_write->pipe = pipe;
	info_write->flags = flags | O_WRONLY;
	info_write->type = STREAM_TYPE_PIPE;
	info_write->offset = 0;
	info_write->ref_count = 1;
	info_write->lock = semaphore_alloc();
	if (!(info_write->lock)) {
		semaphore_free(info_read->lock);
		heapmm_free(info_read, sizeof(stream_info_t));
		heapmm_free(info_write, sizeof(stream_info_t));
		heapmm_free(ptr_read, sizeof(stream_ptr_t));
		heapmm_free(ptr_write, sizeof(stream_ptr_t));
		pipe_free(pipe);
		syscall_errno = ENOMEM;
		return -1;
	}
	info_write->pipe = pipe;
	pipe_open_read(pipe);
	pipe_open_write(pipe);
	semaphore_up(info_read->lock);	
	semaphore_up(info_write->lock);	
	llist_add_end(scheduler_current_task->fd_table, (llist_t *) ptr_read);
	llist_add_end(scheduler_current_task->fd_table, (llist_t *) ptr_write);		
	return 0;
}

int _sys_open(char *path, int flags, mode_t mode)
{
	int st;
	int fd = scheduler_current_task->fd_ctr++;
	stream_info_t *info;
	stream_ptr_t *ptr;	
	dir_cache_t *dirc = vfs_find_dirc(path);
	inode_t *inode;
	if (!dirc) {
		if (flags & O_CREAT) {
			st = vfs_mknod(path, (mode & 0777) | S_IFREG, 0);
			if (st != 0) {
				syscall_errno = st;
				return -1;
			}
			dirc = vfs_find_dirc(path);
			if (!dirc) {
				syscall_errno = ENOENT;
				return -1;
			}
		} else {
			syscall_errno = ENOENT;
			return -1;
		}
	} else if ((flags & O_CREAT) && (flags & O_EXCL)) {
		syscall_errno = EBUSY;
		return -1;
	}
	inode = dirc->inode;
	info = heapmm_alloc(sizeof(stream_info_t));
	if (!info) {
		syscall_errno = ENOMEM;
		return -1;
	}
	ptr = heapmm_alloc(sizeof(stream_ptr_t));
	if (!ptr) {
		heapmm_free(info, sizeof(stream_info_t));
		syscall_errno = ENOMEM;
		return -1;
	}	
	ptr->id = fd;
	ptr->info = info;
	ptr->fd_flags = 0;
	info->ref_count = 1;
	info->dirc = dirc;
	info->inode = inode;
	info->flags = flags;
	info->type = STREAM_TYPE_FILE;
	info->offset = 0;
	info->lock = semaphore_alloc();
	semaphore_up(info->lock);
	if (!(info->lock)) {
		heapmm_free(ptr, sizeof(stream_ptr_t));
		heapmm_free(info, sizeof(stream_info_t));
		syscall_errno = ENOMEM;
		return -1;
	}
	if (flags & O_CLOEXEC) {
		ptr->fd_flags = FD_CLOEXEC;
	}
	if (flags & O_TRUNC) {
		//TODO: Call on VFS to actually truncate file
		inode->size = 0;
	}
	if (flags & O_APPEND) {
		info->offset = inode->size;
	}
	if (S_ISFIFO(inode->mode)) {
		if ((flags & O_ACCMODE) == O_RDONLY)
			pipe_open_read(inode->fifo);
		else if ((flags & O_ACCMODE) == O_WRONLY)
			pipe_open_write(inode->fifo);
		else {
			pipe_open_read(inode->fifo);
			pipe_open_write(inode->fifo);
		}
	}
	if (flags & (O_NDELAY | O_NONBLOCK)) {
		//TODO: Implement non-blocking IO
	}
	inode->usage_count++;
	llist_add_end(scheduler_current_task->fd_table, (llist_t *) ptr);
	if (S_ISCHR(inode->mode))
		device_char_open(inode->if_dev, fd, flags);//TODO: Handle result
	return fd;
}

int _sys_close(int fd)
{
	return _sys_close_int(scheduler_current_task, fd);
}

int _sys_close_int(process_info_t *process, int fd)
{	
	stream_ptr_t *ptr = stream_get_ptr_o(process, fd);
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	if ((ptr->info->type == STREAM_TYPE_FILE) && S_ISCHR(ptr->info->inode->mode) && (process = scheduler_current_task))
		device_char_close(ptr->info->inode->if_dev, fd);//TODO: Handle result
	llist_unlink((llist_t *) ptr);
	semaphore_down(ptr->info->lock);
	ptr->info->ref_count--;
	if (ptr->info->ref_count == 0) {
		switch (ptr->info->type) {
			case STREAM_TYPE_FILE:
				ptr->info->inode->usage_count--;
				if (S_ISFIFO(ptr->info->inode->mode)) {
					if ((ptr->info->flags & O_ACCMODE) == O_RDONLY)
						pipe_close_read(ptr->info->inode->fifo);
					else if ((ptr->info->flags & O_ACCMODE) == O_WRONLY)
						pipe_close_write(ptr->info->inode->fifo);
					else {
						pipe_close_read(ptr->info->inode->fifo);
						pipe_close_write(ptr->info->inode->fifo);
					}
				}
				break;
			case STREAM_TYPE_PIPE:
				if ((ptr->info->flags & O_ACCMODE) == O_RDONLY)
					pipe_close_read(ptr->info->pipe);
				else if ((ptr->info->flags & O_ACCMODE) == O_WRONLY)
					pipe_close_write(ptr->info->pipe);
				pipe_free(ptr->info->pipe);//If there are still endpoints open this will fail, which is not an error condition here.
				break;
		}
		heapmm_free(ptr->info, sizeof(stream_info_t));
	} else
		semaphore_up(ptr->info->lock);
	heapmm_free(ptr, sizeof(stream_ptr_t));
	return 0;
}
