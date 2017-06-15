/**
 * kernel/tty.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 08-05-2014 - Created
 */

#ifndef __KERNEL_TTY_H__
#define __KERNEL_TTY_H__

#include <sys/types.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include "kernel/pipe.h"
#include "util/llist.h"
#include "kernel/process.h"
#include "kernel/streams.h"

typedef struct tty_info tty_info_t;
typedef int (*tty_write_out_t)(dev_t, char);
typedef struct winsize winsize_t;

typedef struct {
	llist_t			 node;
	process_info_t	*proc;
	stream_ptr_t	*ptr;
} tty_fd_t;

struct tty_info {
	dev_t		 device;
	pid_t		 fg_pgid;
	pid_t		 ct_pid;
	termios_t	 termios;
	winsize_t	 win_size;
	int		 	 ref_count;

	char		*line_buffer;
	size_t		 line_buffer_pos;
	size_t		 line_buffer_size;
	
	llist_t		 fds;

	pipe_info_t	*pipe_in;
	tty_write_out_t	 write_out;
	
};

void tty_register_driver(char *name, dev_t major, int minor_count, tty_write_out_t);

void tty_input_char(dev_t device, char c);
void tty_input_str(dev_t device, const char *c);


void tty_init();

tty_info_t *tty_get(dev_t device);
#endif
