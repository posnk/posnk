/**
 * @file kernel/streams.c
 *
 * @brief Implements the UNIX file API ( fd streams )
 *
 * Slightly different terminology is used here:
 * 
 * UNIX - POSNK
 * @li File description - stream info
 * @li File descriptor - stream pointer
 * @li File (opened) - stream
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:\n
 * @li 20-04-2014 - Created
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

/**
 * Iterator to test if stream pointer matches the fd
 */
int stream_ptr_search_iterator (llist_t *node, void *param)
{
	int id = (int) param;
	stream_ptr_t *ptr = (stream_ptr_t *) node;
	return ptr->id == id;
}

/** 
 * @brief Get the stream pointer object for a file number
 * @param fd The open file number to look up
 * @return The stream pointer or NULL if fd is invalid
 */
stream_ptr_t *stream_get_ptr (int fd)
{
	/* Look it up using stream_ptr_search_iterator */
	return (stream_ptr_t *) llist_iterate_select(scheduler_current_task->fd_table, &stream_ptr_search_iterator, (void *) fd);
}

/** 
 * @brief Get the stream pointer object for a file number on a different 
 * process
 * @param process The process context to use when looking up the stream pointer
 * @param fd The open file number to look up
 * @return The stream pointer or NULL if fd is invalid
 */

stream_ptr_t *stream_get_ptr_o (process_info_t *process, int fd)
{
	/* Look it up using stream_ptr_search_iterator */
	return (stream_ptr_t *) llist_iterate_select(process->fd_table, &stream_ptr_search_iterator, (void *) fd);
}

/**
 * @brief Linked list iterator that copies a process's open fds 
 *
 * This iterator will create a new stream pointer for each fd it is called on
 * that refers to the same stream info
 * @param node The linked list node we are called on (must be a stream_ptr_t *)
 * @param param The parameter for this iterator (must be a llist_t *head)
 * @return Whether to select this item, will always be 0 incase of success
 */

int stream_ptr_copy_iterator (llist_t *node, void *param)
{
	llist_t *table = (llist_t *) param;
	stream_ptr_t *ptr = (stream_ptr_t *) node;
	stream_ptr_t *newptr;

	/* Allocate the new stream pointer */
	newptr = heapmm_alloc(sizeof(stream_ptr_t));

	/* Check whether the allocation succeeded */
	if (!newptr) {
		return 1;
	}

	/* Copy information */
	newptr->id = ptr->id;
	newptr->info = ptr->info;

	/* Stream pointer flags are NOT copied */
	newptr->fd_flags = 0;
	
	/* Up the reference counter for the stream info */
	newptr->info->ref_count++;
	
	/* Add the copied stream pointer to the table */
	llist_add_end(table, (llist_t *) newptr);	

	return 0;
}

/**
 * @brief Copy the current process's stream ptr table to target
 *
 * The copied stream ptrs will point to the same stream info and the pointer
 * flags will not be copied.
 * @param target The head of the linked list to store the copies in
 * @return 0 if successful
 */

int stream_copy_fd_table (llist_t *target)
{
	/* Run stream_ptr_copy_iterator over the current process's ptr table */
	return llist_iterate_select(scheduler_current_task->fd_table, &stream_ptr_copy_iterator, (void *) target) == NULL;
}

/**
 * @brief Linked list iterator that finds stream ptrs that are to be closed on
 * execution of a new image
 *
 * Whether or not this is the case is determined by the pointer flag FD_CLOEXEC
 *
 * @param node The pointer to test
 * @param param UNUSED
 * @return Whether this ptr is to be closed on exec
 */

int stream_exec_iterator (llist_t *node, __attribute__((__unused__)) void *param)
{
	stream_ptr_t *ptr = (stream_ptr_t *) node;

	/* Test the FD_CLOEXEC flag */
	return ptr->fd_flags & FD_CLOEXEC;
}

/**
 * @brief Close the stream ptrs that are to be closed on execute
 * @see stream_exec_iterator
 */
void stream_do_close_on_exec ()
{
	stream_ptr_t *ptr;
	/* Loop until there are no more ptrs left that are to be closed */
	for (ptr = (stream_ptr_t *) llist_iterate_select(scheduler_current_task->fd_table, &stream_exec_iterator, NULL); ptr != NULL; 
	     ptr = (stream_ptr_t *) llist_iterate_select(scheduler_current_task->fd_table, &stream_exec_iterator, NULL))
		/* Close the pointer that was found */
		_sys_close(ptr->id);
}

/**
 * @brief Iterator that will select all closeable stream pointers 
 * @return Whether the stream is closeable
 */
int stream_closeall_iterator ( __attribute__((__unused__)) llist_t *node, __attribute__((__unused__)) void *param)
{
	return 1;
}

/**
 * @brief Close all stream pointers for a process
 * @param process The process to close the pointers for
 */
void stream_do_close_all (process_info_t *process)
{
	stream_ptr_t *ptr;
	/* Loop until there are no more ptrs left that are to be closed */
	for (ptr = (stream_ptr_t *) llist_iterate_select(process->fd_table, &stream_closeall_iterator, NULL); ptr != NULL; 
	     ptr = (stream_ptr_t *) llist_iterate_select(process->fd_table, &stream_closeall_iterator, NULL))
		/* Close the pointer that was found */
		_sys_close_int(process, ptr->id);
}

/**
 * @brief Allocate a new fd handle
 * @return A unique new handle
 */
int stream_alloc_fd()
{
	int fd = scheduler_current_task->fd_ctr++;
	if(scheduler_current_task->fd_ctr == 0) {
		scheduler_current_task->fd_ctr--;
		syscall_errno = EMFILE;
		return -1;
	}

	return fd;
}

/**
 * @brief Claim a fd handle
 * @param fd The handle to claim
 */
void stream_claim_fd(int fd)
{
	/* Keep track of the open fds */
	if (fd >= scheduler_current_task->fd_ctr)
		scheduler_current_task->fd_ctr = fd + 1;
}

/**
 * @brief Free a fd handles
 * @param fd The handle to free
 */
void stream_free_fd( __attribute__(( __unused__ )) int fd)
{
	//TODO: Implement fd reuse
}

/**
 * @brief Get directory entries
 *  
 * This function implements the getdents(2) system call
 * It will read as many *whole* dirent's as will fit in count, no partial 
 * dirents will be read.
 * @param fd The directory stream to read from
 * @param buffer The buffer to store the dirents in
 * @param count The number of bytes to read
 * @return The number of bytes actually read or -1 in case of error
 * @exception EBADF fd does not refer to an open file stream
 * @exception EINVAL fd was not opened with READ access mode
 * @exception ENOTDIR fd does not refer to a directory stream
 * @exception EFAULT At least one of the parameters was a null pointer 
 * @see vfs_getdents for other errors that might occur
 */

//int getdents(unsigned int fd, struct linux_dirent *dirp,
//                    unsigned int count);

ssize_t _sys_getdents(int fd, void * buffer, size_t count)
{
	aoff_t read_count;
	int st;
	stream_ptr_t *ptr;
	/* Check for null pointers */
	if (!buffer)
		return EFAULT;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}

	/* Acquire a lock on the stream */
	semaphore_down(ptr->info->lock);

	/* Check if we are allowed to read from this stream */
	if ((ptr->info->flags & O_ACCMODE) == O_WRONLY) {
		/* We are not */

		/* Release the lock on the stream */
		semaphore_up(ptr->info->lock);
	
		/* Signal the error, EINVAL because directories can not
                 * be opened write only */
		syscall_errno = EINVAL;
		return -1;		
	}

	/* Check stream type */
	switch (ptr->info->type) {
		case STREAM_TYPE_FILE:
			/* Stream is a file or directory stream */
			
			/* Call VFS function */
			st = vfs_getdents(ptr->info->inode, ptr->info->offset, buffer, (aoff_t) count, &read_count);

			/* Increase the file offset */
			ptr->info->offset += read_count;

			/* Check for errors */
			if (st) {
				/* Pass error code to userland */
				syscall_errno = st;		

				/* Release the lock on the stream */
				semaphore_up(ptr->info->lock);

				/* Return -1 to signal insuccessful read */
				return -1;
			}

			/* Release the lock on the stream */			
			semaphore_up(ptr->info->lock);

			/* Return the amount of bytes read */
			return (ssize_t) read_count;
		case STREAM_TYPE_PIPE:
			/* Stream is a pipe endpoint */

			/* Release the lock on the stream */
			semaphore_up(ptr->info->lock);

			/* We can't get dirents from a pipe */
			syscall_errno = ENOTDIR;
			return -1;
		default:
			/* Invalid stream type */

			/* Release the lock on the stream */
			semaphore_up(ptr->info->lock);

			/* Tell the application about the error */
			syscall_errno = EINVAL;
			return -1;
	}
}


/**
 * @brief Read from a stream
 *  
 * This function implements the read(2) system call, 
 * It attempts to read _count_ number of bytes from the stream
 *
 * @param fd The stream to read from
 * @param buffer The buffer to store the data in
 * @param count The number of bytes to read
 * @return The number of bytes actually read or -1 in case of error
 * @exception EBADF fd does not refer to an open file stream
 * @exception EBADF fd was not opened with READ access mode
 * @exception EISDIR fd refers to a directory stream
 * @exception EFAULT At least one of the parameters was a null pointer 
 * @exception EPIPE The remote end of the pipe has closed and no data remains
 * @see vfs_read for other errors that might occur
 */

ssize_t _sys_read(int fd, void * buffer, size_t count)
{
	aoff_t read_count;
	int st;
	stream_ptr_t *ptr;

	/* Check for null pointers */
	if (!buffer)
		return EFAULT;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}

	/* Acquire a lock on the stream */
	semaphore_down(ptr->info->lock);

	/* Check if we are allowed to read from this stream */
	if ((ptr->info->flags & O_ACCMODE) == O_WRONLY) {
		/* We are not */

		/* Release the lock on the stream */
		semaphore_up(ptr->info->lock);
	
		/* Signal the error */
		syscall_errno = EBADF;
		return -1;		
	}

	/* Handle the various stream types */
	switch (ptr->info->type) {
		case STREAM_TYPE_FILE:
			/* Stream is a file or directory stream */

			/* Call the VFS */
			st = vfs_read(ptr->info->inode, ptr->info->offset, buffer, (aoff_t) count, &read_count, ptr->info->flags & O_NONBLOCK);

			/* Increase the file offset */
			ptr->info->offset += read_count;

			/* Check for errors */
			if (st) {
				/* Pass error code to userland */
				syscall_errno = st;		

				/* Release the lock on the stream */
				semaphore_up(ptr->info->lock);

				/* Return -1 to signal insuccessful read */
				return -1;
			}

			/* Release the lock on the stream */			
			semaphore_up(ptr->info->lock);

			/* Return the amount of bytes read */
			return (ssize_t) read_count;

		case STREAM_TYPE_PIPE:
			/* Stream is a pipe endpoint */

			/* Call the pipe driver */
			st = pipe_read(ptr->info->pipe, buffer, count, &read_count, ptr->info->flags & O_NONBLOCK);

			/* Release the lock on the stream  */
			semaphore_up(ptr->info->lock);

			/* Check for errors */
			if (st) {
				/* Pass error code to userland */
				syscall_errno = st;

				/* Return error */
				return -1; 
			}

			/* Return the amount of bytes read */
			return (ssize_t) read_count;
		default:
			/* Invalid stream type */

			/* Release the lock on the stream */
			semaphore_up(ptr->info->lock);

			/* Tell the application about the error */
			syscall_errno = EINVAL;
			return -1;
	}
}


/**
 * @brief Write to a stream
 *  
 * This function implements the write(2) system call, 
 * It attempts to write _count_ number of bytes from to stream
 *
 * @param fd The stream to write to
 * @param buffer The buffer to get the data from
 * @param count The number of bytes to write
 * @return The number of bytes actually written or -1 in case of error
 * @exception EBADF fd does not refer to an open file stream
 * @exception EBADF fd was not opened with WRITE access mode
 * @exception EISDIR fd refers to a directory stream
 * @exception EFAULT At least one of the parameters was a null pointer 
 * @exception EPIPE The remote end of the pipe has been closed and there is no room
 *        left in the pipe buffer
 * @see vfs_write for other errors that might occur
 */

ssize_t _sys_write(int fd, void * buffer, size_t count)
{
	aoff_t read_count;
	int st;
	stream_ptr_t *ptr;

	/* Check for null pointers */
	if (!buffer)
		return EFAULT;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}

	/* Acquire a lock on the stream */
	semaphore_down(ptr->info->lock);

	/* Check if we are allowed to write to this stream */
	if ((ptr->info->flags & O_ACCMODE) == O_RDONLY) {
		/* We are not */

		/* Release the lock on the stream */
		semaphore_up(ptr->info->lock);
	
		/* Signal the error */
		syscall_errno = EBADF;
		return -1;		
	}

	/* Handle the various stream types */
	switch (ptr->info->type) {
		case STREAM_TYPE_FILE:
			/* Stream is a file or directory stream */

			/* Check for O_APPEND */
			if (ptr->info->flags & O_APPEND) {
				/* It is set, seek to EOF */
				ptr->info->offset = ptr->inode->size;
	
			}

			/* Call the VFS */
			st = vfs_write(ptr->info->inode, ptr->info->offset, buffer, (aoff_t) count, &read_count, ptr->info->flags & O_NONBLOCK);

			/* Increase the file offset */
			ptr->info->offset += read_count;

			/* Check for errors */
			if (st) {
				/* Pass error code to userland */
				syscall_errno = st;		

				/* Release the lock on the stream */
				semaphore_up(ptr->info->lock);

				/* Return -1 to signal insuccessful read */
				return -1;
			}

			/* Release the lock on the stream */	
			semaphore_up(ptr->info->lock);

			/* Return the amount of bytes written */
			return (ssize_t) read_count;
		case STREAM_TYPE_PIPE:
			/* Stream is a pipe endpoint */

			/* Call the pipe driver */
			st = pipe_write(ptr->info->pipe, buffer, count, &read_count, ptr->info->flags & O_NONBLOCK);

			/* Release the lock on the stream  */
			semaphore_up(ptr->info->lock);

			/* Check for errors */
			if (st) {
	
				/* In case of EPIPE the process should also be
				 * sent SIGPIPE */
				if (st == EPIPE)
					process_send_signal(scheduler_current_task, SIGPIPE);

				/* Pass error code to userland */
				syscall_errno = st;

				/* Return error */
				return -1; 
			}
			return (ssize_t) read_count;
		default:
			/* Invalid stream type */

			/* Release the lock on the stream */
			semaphore_up(ptr->info->lock);

			/* Tell the application about the error */
			syscall_errno = EINVAL;
			return -1;
	}
}

/**
 * @brief Resize a file
 * 
 * If the new size is larger than the file currently is the new room will be
 * filled with zero bytes, if the new size is smaller the data in the truncated
 * part of the file is deleted
 * 
 * @param fd          The file stream to operate on
 * @param size        The new size of the file
 * @return In case of error: -1, Otherwise 0
 *
 * @exception EBADF fd does not refer to an open stream
 * @exception EBADF fd was not opened with WRITE access mode
 * @exception EINVAL fd does not refer to a file
 * @see vfs_truncate for more errors
 */

int _sys_ftruncate(int fd, off_t size)
{
	int st;
	stream_ptr_t *ptr;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}

	/* Acquire a lock on the stream */
	semaphore_down(ptr->info->lock);

	/* Check if we are allowed to write to this stream */
	if ((ptr->info->flags & O_ACCMODE) == O_RDONLY) {
		/* We are not */

		/* Release the lock on the stream */
		semaphore_up(ptr->info->lock);
	
		/* Signal the error */
		syscall_errno = EBADF;
		return -1;		
	}

	/* Handle the various stream types */
	switch (ptr->info->type) {
		case STREAM_TYPE_FILE:
			/* Stream is a file or directory stream */

			/* Call the VFS */
			st = vfs_truncate(ptr->info->inode, (aoff_t) size);

			if (st) {

				/* Pass error code to userland */
				syscall_errno = st;

				/* Release the lock on the stream */
				semaphore_up(ptr->info->lock);
				return -1;
			}

			/* Release the lock on the stream */
			semaphore_up(ptr->info->lock);
			return 0;

		case STREAM_TYPE_PIPE:
		default:
			/* Stream is a pipe or is otherwisely invalid */

			/* Release the lock on the stream */
			semaphore_up(ptr->info->lock);

			/* We can't ftruncate a pipe */
			syscall_errno = EINVAL;
			return -1;
	}
}

/**
 * @brief Reposition read/write file offset
 * 
 * The way the new file offset is calculated is determined by _whence_:@n@n
 * Whence:@n
 * @li *SEEK_SET*\n The offset is set to _offset_ bytes
 * @li *SEEK_CUR*\n The offset is set to its current location plus _offset_ bytes
 * @li *SEEK_END*\n The offset is set to the size of the file plus _offset_ bytes
 *
 * Setting an offset past the end of the file will not grow the file until the
 * next write() to the file
 *
 * @param fd The file stream to operate on
 * @param offset The offset to use when repositioning the file offset
 * @param whence The reference offset to use
 * @return The new file offset or -1 in case of an error
 * @exception EBADF fd does not refer to an open stream
 * @exception ESPIPE fd does not refer to a file
 * @exception EINVAL New offset is negative
 * @exception EINVAL Invalid value for _whence_
 */

off_t _sys_lseek(int fd, off_t offset, int whence)
{
	int _before_begin = 0;
	aoff_t old_offset;
	stream_ptr_t *ptr;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}

	/* Check whether the stream refers to a file */
	if ((ptr->info->type != STREAM_TYPE_FILE) || (S_ISFIFO(ptr->info->inode->mode))) {
		syscall_errno = ESPIPE;
		return (off_t) -1;
	}

	/* Acquire a lock on the stream */
	semaphore_down(ptr->info->lock);

	/* Store the old offset so we can recover from errors */
	old_offset = ptr->info->offset;

	/* Handle the different whence modes */
	switch (whence) {
		case SEEK_SET:
			/* Just set the offset */
			ptr->info->offset = (aoff_t) offset;
			break;

		case SEEK_CUR:
			/* Offset relative to current offset */

			/* Check whether offset is positive */
			if (offset >= 0) {
				/* It is, just add it to the offset */	
				ptr->info->offset += (aoff_t) offset;
			} else {
				/* This hack is necessary because file offset
                                 * is unsigned and 2^32-1 is a valid offset */
	
				/* It is negative, check for negative result */
				if (((aoff_t) -offset) > ptr->info->offset)
					_before_begin = 1;
				
				/* Subtract it from the offset */
				ptr->info->offset -= (aoff_t) -offset;
			}			

			break;
		case SEEK_END:
			/* Offset relative to EOF */

			/* Check whether offset is positive */
			if (offset >= 0) {
				/* It is, just add it to the size */	
				ptr->info->offset = ptr->info->inode->size + (aoff_t) offset;
			} else {
				/* This hack is necessary because file offset
                                 * is unsigned and 2^32-1 is a valid offset */
	
				/* It is negative, check for negative result */
				if (((aoff_t) -offset) > ptr->info->inode->size)
					_before_begin = 1;
				
				/* Subtract it from the size */
				ptr->info->offset = ptr->info->inode->size - (aoff_t) -offset;
			}
			break;

		default:
			/* Invalid whence value */
			/* Set before_begin to trigger an undo */
			_before_begin = 1;
			break;			
	}

	/* Check for errors */
	if (_before_begin) {
		/* Undo seek */
		ptr->info->offset = old_offset;

		/* Release the lock on the stream */
		semaphore_up(ptr->info->lock);
		
		/* Report error */
		syscall_errno = EINVAL;
		return (off_t) -1;
	}

	/* Release the lock on the stream */
	semaphore_up(ptr->info->lock);

	/* Return the new offset */
	return (off_t) ptr->info->offset;
}

/**
 * @brief Changes the working directory for this process to the dir pointed
 * to by _fd_
 *
 * @param fd The fd referring to the new working directory
 * @return In case of an error: -1, Otherwise 0
 * @exception EBADF fd does not refer to an open stream
 * @exception ENOTDIR fd does not refer to a directory stream
 * @see vfs_chdir for other errors that might occur
 */

int _sys_fchdir(int fd)
{
	int status;
	stream_ptr_t *ptr;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}

	/* Check whether the stream refers to a directory */
	if ((ptr->info->type != STREAM_TYPE_FILE) || (!S_ISDIR(ptr->info->inode->mode))) {
		syscall_errno = ENOTDIR;
		return -1;
	}
	
	/* Call the VFS */
	status = vfs_chdir(ptr->info->dirc);

	/* Report any errors to the application */
	if (status)
		syscall_errno = status;
	
	/* Return -1 on error, 0 on success */
	return status ? -1 : 0;
}

/**
 * @brief Get file status
 * @see struct stat
 * @param fd The fd to get the file status of
 * @param buf The buffer to put the status info in
 * @return In case of an error: -1, Otherwise 0
 * @exception EBADF fd does not refer to an open stream
 * @exception EBADF fd does not refer to a file stream
 */

int _sys_fstat(int fd, struct stat* buf)
{
	stream_ptr_t *ptr;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}

	/* Check whether this is a file stream */
	if ((ptr->info->type != STREAM_TYPE_FILE)) {
		syscall_errno = EBADF;
		return -1;
	}

	/* Copy inode fields to the status struct */
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
	buf->st_atime = (time_t) ptr->info->inode->atime; 
	buf->st_mtime = (time_t) ptr->info->inode->mtime;
	buf->st_ctime = (time_t) ptr->info->inode->ctime;

	/* Return success */
	return 0;
}

/**
 * @brief Change file mode
 * @param fd The fd to operate on
 * @param mode The new mode to set on the file (is masked with 07777 to prevent
          type changes)
 * @return In case of an error: -1, Otherwise 0
 * @exception EBADF fd does not refer to an open stream
 * @exception EBADF fd does not refer to a file stream
 * @exception EPERM User does not have permission to change mode on this file
 */

int _sys_fchmod(int fd, mode_t mode)
{
	stream_ptr_t *ptr;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	} 

	/* Check whether this is a file stream */
	if ((ptr->info->type != STREAM_TYPE_FILE)) {
		syscall_errno = EBADF;
		return -1;
	}

	/* Check whether we have permission to do this */
	if (get_perm_class(ptr->info->inode->uid, ptr->info->inode->gid) != PERM_CLASS_OWNER) {
		syscall_errno = EPERM;
		return -1;
	}

	/* Only root may set the SETUID/SETGID/STICKY flags */
	if (get_effective_uid() != 0)
		mode &= 0777;

	/* Actually set mode */
	ptr->info->inode->mode &= ~07777;
	ptr->info->inode->mode |= mode & 07777;

	/* Return success */
	return 0;
}

/**
 * @brief Change file owner and group
 * @param fd The fd to operate on
 * @param owner The new owner of the file
 * @param group The new group of the file
 *
 * If either group or owner is -1 then that ID will not be changed.
 * To change the group of a file, the user must both own the file and be
 * in the file's group
 * @return In case of an error: -1, Otherwise 0
 * @exception EBADF fd does not refer to an open stream
 * @exception EBADF fd does not refer to a file stream
 * @exception EPERM User is not the owner of the file
 */

int _sys_fchown(int fd, uid_t owner, gid_t group)
{
	stream_ptr_t *ptr;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	} 

	/* Check whether this is a file stream */
	if ((ptr->info->type != STREAM_TYPE_FILE)) {
		syscall_errno = EBADF;
		return -1;
	}

	/* Check whether we have permission to do this */
	if (get_perm_class(ptr->info->inode->uid, ptr->info->inode->gid) != PERM_CLASS_OWNER) {
		syscall_errno = EPERM;
		return -1;
	}
	
	/* Check whether we may change the group for this file */
	if (get_perm_class(-1, group) > PERM_CLASS_GROUP)
		group = 65535;

	/* If the ID is not -1, change the group */
	if (group != 65535)
		ptr->info->inode->gid = group;

	/* If the ID is not -1, change the owner */
	if (owner != 65535)
		ptr->info->inode->uid = owner;

	/* Return success */
	return 0;
}

/**
 * @brief Manipulate stream pointer
 * @param fd The fd to operate on
 * @param cmd The function to execute
 * @param arg An argument to the function
 *
 * Supported functions are:
 * @li *F_DUPFD* Duplicate stream pointer\n \n This differs from dup2 in that it 
 * will use the lowest available fd when arg does not refer to an existant fd.
 * @param arg The fd to assign to the duplicate stream pointer
 * @return The duplicate fd or -1 in case of an error
 * @see _sys_dup For other errors that may occur
 *
 * @li *F_GETFD* Read the stream pointer flags\n \n
 * @param arg IGNORED
 * @return The stream pointer flags or -1 in case of an error
 *
 * @li *F_SETFD* Set the stream pointer flags\n \n
 * @param arg The new stream pointer flags
 * @return In case of an error: -1, Otherwise 0
 *
 * @li *F_GETFL* Read the stream flags\n \n
 * @param arg IGNORED
 * @return The stream flags or -1 in case of an error
 *
 * @li *F_SETFL* Set the stream flags\n \n 
 * The stream creation and access flags are ignored
 * @param arg The new stream pointer flags
 * @return In case of an error: -1, Otherwise 0
 *
 * @exception EBADF fd does not refer to an open stream
 */

int _sys_fcntl(int fd, int cmd, int arg)
{
	stream_ptr_t *ptr;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	} 

	/* Handle the various commands */
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

/**
 * @brief Control device
 * @param fd The fd to operate on
 * @param cmd The function to execute
 * @param arg An argument to the function
 * 
 * @return Usually 0 on success, and -1 on error.
 * @exception EBADF fd does not refer to an open stream
 * @exception ENOTTY fd does not refer to a special file
 *
 * @see tty_ioctls ioctl functions supported by teletypes
 * @see ioctl_list List of all ioctl functionsq
 */

int _sys_ioctl(int fd, int cmd, int arg)
{
	stream_ptr_t *ptr;

	/* Get the stream ptr for fd */
 	ptr = stream_get_ptr(fd);
	
	/* Check whether the fd exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	} 

	/* Check whether fd refers to a file */
	if ((ptr->info->type != STREAM_TYPE_FILE)) {
		syscall_errno = ENOTTY;
		return -1;
	}

	/* Check whether fd refers to a character special file or block special
         * file */
	if (S_ISCHR(ptr->info->inode->mode)) {
		/* Call the character device driver */
		return device_char_ioctl(ptr->info->inode->if_dev, fd, cmd, arg);
	} else if (S_ISBLK(ptr->info->inode->mode)) {
		/* Call the block device driver */
		return device_block_ioctl(ptr->info->inode->if_dev, fd, cmd, arg);
	} else {
		/* File is not a special file */
		syscall_errno = ENOTTY;
		return -1;
	}
}

/**
 * @brief Duplicate stream pointer
 *
 * If newfd already refers to an open pointer that pointer will be closed
 * @param oldfd The stream pointer to duplicate
 * @param newfd The fd to use for the duplicate
 * @return The duplicate fd if successful, if an error occurred: -1
 * @exception EBADF oldfd does not refer to an open stream
 * @exception EBADF newfd is not a valid fd
 * @exception EMFILE Maximum number of open stream pointers reached
 */
int _sys_dup2(int oldfd, int newfd)
{
	stream_ptr_t *oldptr;
	stream_ptr_t *newptr;

	/* Get the stream ptr for oldfd */
 	oldptr = stream_get_ptr(oldfd);
	
	/* Check whether the fd exists */
	if (!oldptr) {
		syscall_errno = EBADF;
		return -1;
	}
	
	/* Check if newfd is valid */	
	if (newfd < 0) {
		syscall_errno = EBADF;
		return -1;
	} 
	
	/* Get the stream ptr for newfd */
	newptr = stream_get_ptr(newfd);

	/* If it exists, close it */
	if (newptr) {
		_sys_close(newfd);
	}

	/* Keep track of used fds */
	stream_claim_fd(newfd);

	/* Allocate memory for the new pointer */
	newptr = heapmm_alloc(sizeof(stream_ptr_t));

	/* Check if allocation succeded */
	if (!newptr) {
		syscall_errno = EMFILE;
		return -1;
	}

	/* Fill out stream pointer fields */
	newptr->info = oldptr->info;
	newptr->id   = newfd;
	newptr->fd_flags = 0;

	/* Bump stream info reference count */
	newptr->info->ref_count++;
	
	
	/* Add duplicate to the table */
	llist_add_end(scheduler_current_task->fd_table, (llist_t *) newptr);
	return newfd;	
}

/**
 * @brief Duplicate stream pointer
 *
 * @param oldfd The stream pointer to duplicate
 * @return The duplicate fd if successful, if an error occurred: -1
 * @exception EBADF oldfd does not refer to an open stream
 * @exception EMFILE Maximum number of open stream pointers reached
 */

int _sys_dup(int oldfd)
{
	int newfd, status;

	/* Allocate a new fd */
	newfd = stream_alloc_fd();
	
	/* Check for errors */
	if (newfd == -1){
		syscall_errno = EMFILE;
		return -1;
	}	
	
	/** Call _sys_dup2 with a new fd for newfd */
	status = _sys_dup2(oldfd, newfd);

	if (status == -1) {
		stream_free_fd(newfd);
		return -1;
	}

	return status;
}

/** 
 * @brief Create a pipe
 * @param pipefd The created fd's will be put in this array, the 
 * read endpoint will be in pipefd[0], the write endpoint in pipefd[1]
 * @param flags Flags to set on the streams (O_CLOEXEC and/or O_NONBLOCK)
 * @return On success, zero is returned. On error, -1 is returned.
 * @exception ENOMEM There was not enough memory to create the pipe
 * @exception EMFILE Maximum number of open stream pointers reached
 * @exception EINVAL Invalid value in flags
 */
int _sys_pipe2(int pipefd[2], int flags)
{
	pipe_info_t	*pipe;
	stream_info_t	*info_read;
	stream_info_t	*info_write;
	stream_ptr_t	*ptr_read;
	stream_ptr_t	*ptr_write;	

	/* Allocate new fd's for the pipe endpoints */
	if ((pipefd[0] = stream_alloc_fd()) == -1) {
		syscall_errno = EMFILE;
		goto _bailout_A;
	}

	if ((pipefd[1] = stream_alloc_fd()) == -1) {
		syscall_errno = EMFILE;
		goto _bailout_B;
	}

	/* Create the pipe itself */
	pipe = pipe_create();

	/* Check for errors */
	if (!pipe) {
		syscall_errno = ENOMEM;
		goto _bailout_1;
	}

	/* Allocate the read endpoint stream info */
	info_read = heapmm_alloc(sizeof(stream_info_t));

	/* Check for errors */
	if (!info_read) {
		syscall_errno = ENOMEM;
		goto _bailout_2;
	}

	/* Allocate the read endpoint stream pointer */
	ptr_read = heapmm_alloc(sizeof(stream_ptr_t));

	/* Check for errors */
	if (!ptr_read) {
		syscall_errno = ENOMEM;
		goto _bailout_3;
	}

	/* Allocate the write endpoint stream info */
	info_write = heapmm_alloc(sizeof(stream_info_t));

	/* Check for errors */
	if (!info_write) {
		syscall_errno = ENOMEM;
		goto _bailout_4;
	}

	/* Allocate the write endpoint stream pointer */
	ptr_write = heapmm_alloc(sizeof(stream_ptr_t));

	/* Check for errors */
	if (!ptr_write) {
		syscall_errno = ENOMEM;
		goto _bailout_5;
	}
	
	/* Fill out the read endpoint pointer fields */
	ptr_read->id = pipefd[0];
	ptr_read->info = info_read;
	ptr_read->fd_flags = 0;

	/* Fill out the write endpoint pointer fields */
	ptr_write->id = pipefd[1];
	ptr_write->info = info_write;
	ptr_write->fd_flags = 0;

	/* Check for FD_CLOEXEC flag */
	if (flags & O_CLOEXEC) {
		flags &= ~O_CLOEXEC;
		ptr_read->fd_flags |= FD_CLOEXEC;
		ptr_write->fd_flags |= FD_CLOEXEC;
	}

	if (flags & ~O_NONBLOCK) {
		syscall_errno = EINVAL;
		goto _bailout_6;
	}

	/* Fill out the read endpoint info fields */
	info_read->pipe = pipe;
	info_read->flags = flags | O_RDONLY;
	info_read->type = STREAM_TYPE_PIPE;
	info_read->offset = 0;
	info_read->ref_count = 1;

	/* Allocate the write endpoint lock */
	info_read->lock = semaphore_alloc();

	/* Check for errors */
	if (!(info_read->lock)) {
		syscall_errno = ENOMEM;
		goto _bailout_6;
	}

	/* Fill out the write endpoint info fields */
	info_write->pipe = pipe;
	info_write->flags = flags | O_WRONLY;
	info_write->type = STREAM_TYPE_PIPE;
	info_write->offset = 0;
	info_write->ref_count = 1;

	/* Allocate the read endpoint lock */
	info_write->lock = semaphore_alloc();

	/* Check for errors */
	if (!(info_write->lock)) {
		syscall_errno = ENOMEM;
		goto _bailout_7;
	}

	/* Signal the opening of the endpoints to the pipe driver */
	pipe_open_read(pipe);
	pipe_open_write(pipe);

	/* Release the lock on the endpoints */
	semaphore_up(info_read->lock);	
	semaphore_up(info_write->lock);	

	/* Add the endpoints to the fd table */
	llist_add_end(scheduler_current_task->fd_table, (llist_t *) ptr_read);
	llist_add_end(scheduler_current_task->fd_table, (llist_t *) ptr_write);		

	/* Return success */
	return 0;

	/* Clean up on error */
_bailout_7:
	semaphore_free(info_read->lock);
_bailout_6:
	heapmm_free(ptr_write, sizeof(stream_ptr_t));
_bailout_5:
	heapmm_free(info_write, sizeof(stream_info_t));
_bailout_4:
	heapmm_free(ptr_read, sizeof(stream_ptr_t));
_bailout_3:
	heapmm_free(info_read, sizeof(stream_info_t));
_bailout_2:
	pipe_free(pipe);
_bailout_1:
	stream_free_fd(pipefd[1]);
_bailout_B:
	stream_free_fd(pipefd[0]);
_bailout_A:
	return -1;
	
}

/**
 * @brief Open a file stream
 * 
 * The behaviour of this function is determined by _flags_\n
 * It is required to specify one of the access mode flags:\n
 * @li *O_RDONLY* Open read-only
 * @li *O_WRONLY* Open write-only
 * @li *O_RDWR* Open read-write\n
 *
 * Valid optional flags:\n \n
 * @li *O_APPEND* Open the file for appending \n 
 * \n
 * Before each _sys_write the offset is set to the end of the file\n
 * \n
 * @li *O_CLOEXEC* Set this stream pointer to close on execve(2) \n 
 * \n
 * When this flag is set this stream pointer will be closed when executing a 
 * new program.\n
 * \n
 * @li *O_CREAT* Create file \n 
 * \n
 * If this flag is set and the file does not exist the file will be created
 * using the mode specified in _mode_ \n
 * \n
 * @li *O_NONBLOCK* Use non-blocking IO \n 
 * \n
 * When this flag is set _sys_read and _sys_write will not block if no data is
 * available\n
 * \n
 * @li *O_TRUNC* Truncate file \n 
 * \n
 * If this file already exists and is a regular file and the open mode allows 
 * writing the file will be truncated to length 0\n
 * \n
 * @li *O_EXCL* Ensure the file is created \n 
 * \n
 * If this flag is set and O_CREAT is set and the file already exists _sys_open
 * \n
 * will fail with EEXIST\n
 *
 * @param path The path of the file to open
 * @param flags The options for opening the file
 * @param mode The mode to create the file with
 * @return -1 if an error occurred, if successful the fd of the opened stream 
 * is returned
 * @exception ENOENT The file does not exist and *O_CREAT* is not set
 * @exception EEXIST The file exists and *O_EXCL* is set
 * @exception ENOMEM Insufficient kernel memory was available
 * @exception ENOSPC Insufficient room was available to create the file
 * @exception ENOENT An element in path does not exist
 * @exception ENXIO Tried to open a FIFO for writing that is not open for
 * reading and O_NONBLOCK | O_WRONLY is set
 * @exception ENXIO Tried to open a special file that points to an non-existant
 * device
 * @exception EMFILE Too many files are open
 * @exception EACCES The requested access is not allowed
 * @exception EFAULT Path is a null pointer
 */

int _sys_open(char *path, int flags, mode_t mode)
{
	int st;
	int fd;
	stream_info_t *info;
	stream_ptr_t *ptr;	
	dir_cache_t *dirc;
	inode_t *inode;
	
	/* Check for null pointers */
	if (!path) {
		syscall_errno = EFAULT;
		return -1;
	}		

	/* Look up the file */
	dirc = vfs_find_dirc(path);
	
	/* Check if the file exists */
	if (!dirc) {
		/* If not, check if we are to create it */
		if (flags & O_CREAT) {
	
			/* Create the file */
			st = vfs_mknod(path, (mode & 0777) | S_IFREG, 0);
	
			/* Check for errors */
			if (st != 0) {
				/* Pass the error to the application */
				syscall_errno = st;
				return -1;
			}

			/* Look up the newly created file */
			dirc = vfs_find_dirc(path);

			/* Check if it exists */
			//TODO: Make this an assertion
			if (!dirc) {
				syscall_errno = ENOENT;
				return -1;
			}

		} else {
			/* O_CREAT not set: return error */
			syscall_errno = ENOENT;
			return -1;
		}

	} else if ((flags & O_CREAT) && (flags & O_EXCL)) {
		/* File exists and O_EXCL is set, signal error */
		syscall_errno = EEXIST;
		return -1;
	}
	
	/* Grab the files inode */
	inode = vfs_inode_ref(dirc->inode);

	/* Check whether we have permission to open with this mode */
	if ((flags & O_ACCMODE) == O_RDONLY)
		st = vfs_have_permissions(inode, MODE_READ);
	else if ((flags & O_ACCMODE) == O_WRONLY)
		st = vfs_have_permissions(inode, MODE_WRITE);
	else if ((flags & O_ACCMODE) == O_RDWR)
		st = vfs_have_permissions(inode, MODE_READ | MODE_WRITE);

	if (!st) {
		vfs_inode_release(inode);
		syscall_errno = EACCES;
		return -1;
	}
	
	/* Allocate memory for the stream info */
	info = heapmm_alloc(sizeof(stream_info_t));

	/* Check for errors */
	if (!info) {
		vfs_inode_release(inode);
		syscall_errno = ENOMEM;
		return -1;
	}

	/* Allocate memory for the stream pointer */
	ptr = heapmm_alloc(sizeof(stream_ptr_t));

	/* Check for errors */
	if (!ptr) {
		heapmm_free(info, sizeof(stream_info_t));
		vfs_inode_release(inode);
		syscall_errno = ENOMEM;
		return -1;
	}

	/* Allocate fd handle */
	fd = stream_alloc_fd();

	if (fd == -1) {
		heapmm_free(info, sizeof(stream_info_t));
		heapmm_free(ptr, sizeof(stream_ptr_t));
		vfs_inode_release(inode);
		syscall_errno = EMFILE;
		return -1;
	}
			
	/* Fill stream pointer fields */
	ptr->id = fd;
	ptr->info = info;
	ptr->fd_flags = 0;

	/* Fill stream info fields */
	info->ref_count = 1;
	info->dirc = dirc;
	info->inode = inode;
	info->flags = flags;
	info->type = STREAM_TYPE_FILE;
	info->offset = 0;

	/* Allocate stream lock */
	info->lock = semaphore_alloc();

	/* Check for errors */	
	if (!(info->lock)) {
		heapmm_free(ptr, sizeof(stream_ptr_t));
		heapmm_free(info, sizeof(stream_info_t));
		vfs_inode_release(inode);
		stream_free_fd(fd);
		syscall_errno = ENOMEM;
		return -1;
	}

	/* Is the close on exec flag set? */
	if (flags & O_CLOEXEC) {
		/* Set it on the pointer */
		ptr->fd_flags = FD_CLOEXEC;
	}

	/* Is the truncate flag set? */
	if (flags & O_TRUNC) {
		//TODO: Call on VFS to actually truncate file
		inode->size = 0;
	}
	
	/* Is the append flag set? */
	if (flags & O_APPEND) {
	
	}
	
	/* Check if file is a FIFO */
	if (S_ISFIFO(inode->mode)) {

		/* If so, signal open to the pipe driver */
		if ((flags & O_ACCMODE) == O_RDONLY)
			pipe_open_read(inode->fifo);
		else if ((flags & O_ACCMODE) == O_WRONLY)
			pipe_open_write(inode->fifo);
		else {
			pipe_open_read(inode->fifo);
			pipe_open_write(inode->fifo);
		}
	}

	/* Check if non-blocking IO flags are set */
	if (flags & (O_NDELAY | O_NONBLOCK)) {
		//TODO: Implement non-blocking IO
	}

	/* Bump inode open count */
	//TODO: Fix this
	inode->usage_count++;

	/* Release lock on the stream info */
	semaphore_up(info->lock);

	/* Add the pointer to the fd table */
	llist_add_end(scheduler_current_task->fd_table, (llist_t *) ptr);

	/* If the file opened is a special file, signal open to the driers */
	if (S_ISCHR(inode->mode))
		device_char_open(inode->if_dev, fd, flags);//TODO: Handle result
	else if (S_ISBLK(inode->mode))
		device_block_open(inode->if_dev, fd, flags);//TODO: Handle result

	/* Return new fd */
	return fd;
}

/**
 * @brief Close a stream pointer
 *
 * If the stream pointer was the last pointer for the stream the stream itself
 * will be closed.
 *
 * @param fd The fd for the stream pointer to close
 * @return If an error occurred -1 is returned, otherwise 0 is returned
 * @exception EBADF fd does not refer to an open stream pointer
 * @exception EIO An io error occurred
 */
int _sys_close(int fd)
{
	return _sys_close_int(scheduler_current_task, fd);
}

/**
 * @brief Close a stream pointer on a different process
 *
 * @see _sys_close
 *
 * @param process The process context to use
 * @param fd The fd for the stream pointer to close
 * @return If an error occurred -1 is returned, otherwise 0 is returned
 * @exception EBADF fd does not refer to an open stream pointer
 * @exception EIO An io error occurred
 */

int _sys_close_int(process_info_t *process, int fd)
{	
	stream_ptr_t *ptr;
	/* Get the stream pointer */
	ptr = stream_get_ptr_o(process, fd);

	/* Check if it exists */
	if (!ptr) {
		syscall_errno = EBADF;
		return -1;
	}
	
	/* If the pointer refers to a device stream, signal the device driver */
	//TODO: Move to stream close
	if ((ptr->info->type == STREAM_TYPE_FILE) && S_ISCHR(ptr->info->inode->mode) && (process == scheduler_current_task))
		device_char_close(ptr->info->inode->if_dev, fd);//TODO: Handle result
	else if ((ptr->info->type == STREAM_TYPE_FILE) && S_ISBLK(ptr->info->inode->mode) && (process == scheduler_current_task))
		device_block_close(ptr->info->inode->if_dev, fd);//TODO: Handle result

	/* Remove the pointer from the fd table */
	llist_unlink((llist_t *) ptr);

	/* Free the fd handle */
	stream_free_fd(fd);
	
	/* Acquire a lock on the stream */
	semaphore_down(ptr->info->lock);

	/* Decrease the stream reference count */
	ptr->info->ref_count--;

	/* Check if this was the last pointer */
	if (ptr->info->ref_count == 0) {
		/* Close the stream */

		/* Handle the various stream types */
		switch (ptr->info->type) {
			case STREAM_TYPE_FILE:
				/* Stream is a file or directory stream */

				/* If the stream referred to a FIFO signal 
				 * the pipe driver */
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

				/* Release the inode */
				vfs_inode_release(ptr->info->inode);

				break;
			case STREAM_TYPE_PIPE:
				/* Stream is a pipe */
		
				/* Signal the pipe driver */
				if ((ptr->info->flags & O_ACCMODE) == O_RDONLY)
					pipe_close_read(ptr->info->pipe);
				else if ((ptr->info->flags & O_ACCMODE) == O_WRONLY)
					pipe_close_write(ptr->info->pipe);

				/* Free the pipe, if there are still endpoints
				 * open this will fail, which is not an error 
                                 * condition here */
				pipe_free(ptr->info->pipe);
				break;
		}

		/* Free the memory for the stream info */
		heapmm_free(ptr->info, sizeof(stream_info_t));

	} else /* It was not, release the lock on the stream */
		semaphore_up(ptr->info->lock);

	/* Free the memory for the pointer */
	heapmm_free(ptr, sizeof(stream_ptr_t));

	/* Return success */
	return 0;
}
