/**
 * kernel/pipe.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 21-04-2014 - Created
 */

#ifndef __KERNEL_PIPE_H__
#define __KERNEL_PIPE_H__

#include "kernel/synch.h"

#define	PIPE_STATUS_READ_OPEN	(1)
#define	PIPE_STATUS_WRITE_OPEN	(2)


typedef struct pipe_info pipe_info_t;

struct pipe_info {
	int		 read_usage_count;
	int		 write_usage_count;
	void		*buffer;
	aoff_t		 write_ptr;
	aoff_t		 read_ptr;
	semaphore_t	*write_lock;
	semaphore_t	*read_lock;
};

pipe_info_t *pipe_create();

int pipe_free(pipe_info_t *pipe);

void pipe_open_read(pipe_info_t *pipe);

void pipe_open_write(pipe_info_t *pipe);

void pipe_close_read(pipe_info_t *pipe);

void pipe_close_write(pipe_info_t *pipe);

short int pipe_poll(pipe_info_t *pipe, short int events);

int pipe_write(pipe_info_t *pipe, const void * buffer, aoff_t count, aoff_t *write_count, int non_block);

int pipe_read(pipe_info_t *pipe, void * buffer, aoff_t count, aoff_t *read_count, int non_block);

#endif

