/**
 * kernel/pipe.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 21-04-2014 - Created
 */

#include <sys/errno.h>
#include <sys/types.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <poll.h>
#include "config.h"
#include "kernel/heapmm.h"
#include "kernel/synch.h"
#include "kernel/pipe.h"

pipe_info_t *pipe_create()
{
	pipe_info_t *pipe = heapmm_alloc(sizeof(pipe_info_t));
	if (!pipe)
		return NULL;
	pipe->buffer = heapmm_alloc(CONFIG_PIPE_BUFFER_SIZE);
	pipe->read_usage_count  = 0;
	pipe->write_usage_count = 0;
	pipe->write_ptr = 0;
	pipe->read_ptr  = 0;
	pipe->write_lock = semaphore_alloc();
	if (!(pipe->write_lock)) {
		heapmm_free(pipe->buffer, CONFIG_PIPE_BUFFER_SIZE);
		heapmm_free(pipe, sizeof(pipe_info_t));
		return NULL;
	}
	pipe->read_lock  = semaphore_alloc();
	if (!(pipe->read_lock)) {
		semaphore_free(pipe->write_lock);
		heapmm_free(pipe->buffer, CONFIG_PIPE_BUFFER_SIZE);
		heapmm_free(pipe, sizeof(pipe_info_t));
		return NULL;
	}
	return pipe;
}

int pipe_free(pipe_info_t *pipe)
{
	if ((pipe->read_usage_count + pipe->write_usage_count) != 0)
		return EBUSY;
	semaphore_free(pipe->write_lock);
	semaphore_free(pipe->read_lock);
	heapmm_free(pipe->buffer, CONFIG_PIPE_BUFFER_SIZE);
	heapmm_free(pipe, sizeof(pipe_info_t));
	return 0;
}

void pipe_open_read(pipe_info_t *pipe)
{
	pipe->read_usage_count++;
}

void pipe_open_write(pipe_info_t *pipe)
{
	pipe->write_usage_count++;
}

void pipe_close_read(pipe_info_t *pipe)
{
	pipe->read_usage_count--;
	if (pipe->read_usage_count == 0) {
		semaphore_up(pipe->write_lock);
	}
}

void pipe_close_write(pipe_info_t *pipe)
{
	pipe->write_usage_count--;
	if (pipe->write_usage_count == 0) {
		semaphore_up(pipe->read_lock);
	}
}

short int pipe_poll( pipe_info_t *pipe, short int events )
{
	short int revents = 0;
	aoff_t ts;
	if ( events & POLLIN ) {
		if ( pipe->write_usage_count == 0 )
			revents |= POLLHUP;
		ts = pipe->write_ptr - pipe->read_ptr;
		if ( ts > 0 )
			revents |= POLLIN;
	}
	if ( events & POLLOUT ) {
		if ( pipe->read_usage_count == 0 )
			revents |= POLLHUP;//TODO: Is this correct?
		if ( pipe->write_ptr < CONFIG_PIPE_BUFFER_SIZE )
			revents |= POLLIN;
	}
	return revents;
}

int pipe_write(pipe_info_t *pipe, const void * buffer, aoff_t count, aoff_t *write_count, int non_block)
{
	uintptr_t wbuf = (uintptr_t) buffer;
	uintptr_t pbuf = (uintptr_t) pipe->buffer;
	aoff_t turn_size, current_pos;
	current_pos = 0;
	for (;;){
		turn_size = CONFIG_PIPE_BUFFER_SIZE - pipe->write_ptr;
		if (turn_size > count)
			turn_size = count;
		if (turn_size == 0 || ((turn_size < count) && count <= CONFIG_PIPE_BUFFER_SIZE)) {
			(*write_count) = current_pos;
			if (non_block) {
				if (pipe->read_usage_count != 0)
					return EAGAIN;
				else
					return EPIPE;
			} else {
				if (semaphore_idown(pipe->write_lock))
					return EINTR;
				if (pipe->read_usage_count != 0)
					continue;
				else
					return EPIPE;
			}
		}
		memcpy((void *)(pbuf + (uintptr_t) pipe->write_ptr), (void *)(wbuf + (uintptr_t) current_pos), turn_size);
		pipe->write_ptr += turn_size;
		current_pos += turn_size;
		semaphore_up(pipe->read_lock);
		(*write_count) = count;
		if (current_pos == count)
			return 0;
	}
}

int pipe_read(pipe_info_t *pipe, void * buffer, aoff_t count, aoff_t *read_count, int non_block)
{
	uintptr_t rbuf = (uintptr_t) buffer;
	uintptr_t pbuf = (uintptr_t) pipe->buffer;
	aoff_t turn_size, current_pos;
	current_pos = 0;
	void *ov_buf;
	for (;;){
		turn_size = pipe->write_ptr - pipe->read_ptr;
		if (turn_size > count)
			turn_size = count;
		if (turn_size == 0) {
			pipe->write_ptr = 0;
			pipe->read_ptr = 0;
			semaphore_up(pipe->write_lock);
			(*read_count) = current_pos;
			if (current_pos != 0) {
				return 0;
			} else if (non_block) {
				if (pipe->write_usage_count != 0)
					return EAGAIN;
				else
					return 0;
			} else {
				if (semaphore_idown(pipe->read_lock))
					return EINTR;
				turn_size = pipe->write_ptr - pipe->read_ptr;
				if ((turn_size != 0) || (pipe->write_usage_count != 0))
					continue;
				else
					return 0;
			}
		}
		memcpy((void *)(rbuf + (uintptr_t) current_pos), (void *)(pbuf + (uintptr_t) pipe->read_ptr), turn_size);
		pipe->read_ptr += turn_size;
		current_pos += turn_size;
		if (current_pos == count) {
			(*read_count) = count;
			turn_size = pipe->write_ptr - pipe->read_ptr;
			if (turn_size == 0) {
				pipe->write_ptr = 0;
				pipe->read_ptr = 0;
				semaphore_up(pipe->write_lock);
			} else {
				ov_buf = heapmm_alloc(CONFIG_PIPE_BUFFER_SIZE);
				if (ov_buf == NULL)
					return 0; //we might be able to recover from this
				memcpy_o(pipe->buffer, (void *)(pbuf + (uintptr_t) pipe->read_ptr), turn_size, ov_buf);
				heapmm_free(ov_buf, CONFIG_PIPE_BUFFER_SIZE);
				pipe->write_ptr = turn_size;
				pipe->read_ptr = 0;
				semaphore_up(pipe->write_lock);
			}
			return 0;
		}
	}
}
