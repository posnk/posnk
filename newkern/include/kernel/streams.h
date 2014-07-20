/**
 * @file kernel/streams.h
 *
 * @brief Implements the UNIX file API ( fd streams )
 *
 * Part of P-OS kernel.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:\n
 * @li 20-04-2014 - Created
 */

#include "kernel/vfs.h"
#include "kernel/pipe.h"
#include "util/llist.h"
#include <sys/types.h>

#ifndef __KERNEL_STREAMS_H__
#define __KERNEL_STREAMS_H__

/**
 * @defgroup stream Stream API
 * @brief Implements the UNIX file API ( fd streams )
 *
 * The streams programming interface provides an unified way of transferring 
 * data. It wraps the VFS API and implements pipes. For files it keeps track
 * of the current offset.
 *
 * Slightly different terminology is used here:
 * 
 * UNIX - POSNK
 * @li File description - stream info
 * @li File descriptor - stream pointer
 * @li File (opened) - stream
 * @{
 */

/** This stream refers to a file */
#define STREAM_TYPE_FILE	(0)

/** This stream is a pipe endpoint */
#define STREAM_TYPE_PIPE	(1)

/**
 * @brief Describes a stream
 * @see stream_info
 */

typedef struct stream_info stream_info_t;

/**
 * @brief Describes a pointer to a stream
 * @see stream_ptr
 */

typedef struct stream_ptr stream_ptr_t;

/**
 * @brief Describes a pointer to a stream
 *
 * A stream pointer is what is handled by most programs, it is referred to by
 * a unique handle (fd) and refers to a stream. The actual state of a stream 
 * is stored in the stream info object, the stream pointer contains only 
 * flags affecting its behaviour on process events
 */

struct stream_ptr {

	/** The linked list node */
	llist_t  	node;

	/** The handle for this stream pointer */
	int 		id;

	/**
         * @brief The flags for this stream pointer 
         * @warning These are distinct from the stream flags
         */
	int		fd_flags;

	/** A pointer to the stream info object this stream_ptr refers to*/
	stream_info_t  *info;
};

/**
 * @brief Describes a stream
 * A stream object is shared between all pointers to it and thus all processes
 * having such a pointer. This means that the state of the stream is shared 
 * too. The stream object is only destroyed when the last pointer to it is
 * closed
 */

struct stream_info {

	/** The type of stream */
	int		 type;

	/** The amount of pointers referring to this stream */
	int		 ref_count;

	/** The flags for this stream @see _sys_open */
	int	 	 flags;

	/** The current file offset for this stream */
	aoff_t		 offset;

	/** The inode referred to by this stream */
	inode_t		*inode;

	/** The dir_cache entry referred to by this stream */
	dir_cache_t	*dirc;

	/** This stream's mutex lock */
	semaphore_t	*lock;

	/** The pipe referred to by this stream */
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

int _sys_ftruncate(int fd, off_t size);
///@}
#endif
