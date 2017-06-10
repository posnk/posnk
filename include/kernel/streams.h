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
#include <poll.h>
#include <sys/types.h>
#include <sys/dirent.h>

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
#define STREAM_TYPE_FILE		(0)

/** This stream is a pipe endpoint */
#define STREAM_TYPE_PIPE		(1)

/** This stream is based on callbacks to an external module */
#define STREAM_TYPE_EXTERNAL	(2)

#define STREAM_IMPL_FILE_FSTAT	(1)
#define STREAM_IMPL_FILE_CHDIR	(2)
#define STREAM_IMPL_FILE_CHOWN	(4)
#define STREAM_IMPL_FILE_CHMOD	(8)
#define STREAM_IMPL_FILE_LSEEK	(16)
#define STREAM_IMPL_FILE_TRUNC	(32)

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
 * @brief Callback functions for externally implemented streams
 * @see stream_ops
 */

typedef struct stream_ops stream_ops_t;

/**
 * @brief Describes a poll call waiting on this descriptor
 * @see stream_poll
 */

typedef struct stream_poll stream_poll_t;

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
	int			fd_flags;

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
	int		 	 type;

	/** The amount of pointers referring to this stream */
	int			 ref_count;

	/** The flags for this stream @see _sys_open */
	int	 	 	 flags;

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
	
	/** Implementation flags */
	int			 impl_flags;
	
	/** Implementation specific information */
	void		*impl;
	
	/** Operations for external streams */
	stream_ops_t *ops;

	/** List of polls waiting on this stream */
	llist_t		 poll;
};

/** 
 * Callback functions for externally implemented streams
 */
struct stream_ops {
	
	/**
	 * @brief Performs on-close cleanup for the stream
	 * @param stream	The pointer to the stream being closed
	 */
	SVFUNCPTR( close, stream_info_t * );
	
	/**
	 * @brief Read information from the stream
	 * @param stream	The stream to read from
	 * @param buffer	The buffer to perform IO on
	 * @param count		The number of bytes to transfer
	 * @return			The number of bytes actually transferred
	 */
	SFUNCPTR( aoff_t, read, stream_info_t *, void *, aoff_t );
	
	/**
	 * @brief Write information to the stream
	 * @param stream	The stream to write to
	 * @param buffer	The buffer to perform IO on
	 * @param count		The number of bytes to transfer
	 * @return			The number of bytes actually transferred
	 */
	SFUNCPTR( aoff_t, write, stream_info_t *, void *, aoff_t );
	
	/**
	 * @brief Controls a device
	 * @param stream	The stream to operate on
	 * @param cmd		The function to execute
	 * @param arg		An argument to the function.
	 * @return 			Usually 0 on success, and -1 on error.
	 * @exception ENOTTY fd does not refer to a special file
	 */
	SFUNCPTR( int, ioctl, stream_info_t *, int, void * );
	
	/**
	 * @brief Seeks to a new position
	 * @param stream	The stream to operate on
	 * @param pos		The new position to set
	 */
	SVFUNCPTR( seek, stream_info_t *, aoff_t );
	
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
	SFUNCPTR(off_t, lseek, stream_info_t *, off_t, int );
	
	/**
	 * @brief Reads a directory entry from the stream
	 * @param stream	The stream to read from
	 * @param buffer	The buffer to read to
	 * @param buflen	The length of the buffer
	 * @return			The number of bytes written to the buffer
	 * @exception E2BIG	The dirent did not fit in the buffer ( return val valid)
	 * @exception ENMFILE The end of the directory was reached
	 */
	 SFUNCPTR( aoff_t, readdir, stream_info_t *, sys_dirent_t *, aoff_t );
	 
	SVFUNCPTR( truncate, stream_info_t *, aoff_t );
	 
	SVFUNCPTR( chdir, stream_info_t * );
	SVFUNCPTR( chmod, stream_info_t *, mode_t );
	SVFUNCPTR( chown, stream_info_t *, uid_t, gid_t );
	SVFUNCPTR( stat, stream_info_t *, struct stat* );
	SFUNCPTR( short int, poll, stream_info_t *, short int );
	
	
};

/** Describes a poll request on a stream */
struct stream_poll {
	llist_t			 node;
	/** the file being polled */
	stream_ptr_t	*stream;
	/** the input event flags */
	short int		 events;
	/** the output event flags */
	short int		 revents;
	/** the semaphore this poll is locked on 
	 * @note Owned by _sys_poll(), not this struct */
	semaphore_t		*notify;
};

void stream_notify_poll( stream_info_t *info );

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

ssize_t _sys_readdir(int fd, void * buffer, size_t buflen);

ssize_t _sys_getdents(int fd, void * buffer, size_t count);

int _sys_close_int(process_info_t *process, int fd);

int _sys_ftruncate(int fd, off_t size);

int _sys_poll( struct pollfd fds[], nfds_t nfds, int timeout );
///@}
#endif
