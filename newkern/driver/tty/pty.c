/**
 * driver/video/mbfb.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 21-07-2014 - Created
 */

#include <string.h>
#include <sys/errno.h>
#include <stdint.h>
#include <sys/types.h>
#include "kernel/pipe.h"
#include "kernel/heapmm.h"
#include "kernel/earlycon.h"
#include "kernel/physmm.h"
#include "kernel/device.h"
#include "kernel/syscall.h"
#include <string.h>
#include <stdint.h> 
#include "kernel/tty.h"

pipe_info_t *ptys[256];

int pty_open(  __attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int options)
{
	return 0;	
}

int pty_close( __attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd)
{
	return 0;
}

int pty_write(	dev_t device, 
		void *buf, 
		aoff_t count,
		aoff_t *write_size,
		__attribute__((__unused__)) int non_block)
{
	dev_t other_end = MAKEDEV(0x78, MINOR(device));
	aoff_t ptr;
	char *_buf = buf;
	for (ptr = 0; ptr < count; ptr++)
		tty_input_char(other_end, _buf[ptr]);
	*write_size = count;
	return 0;
}

int pty_read(	dev_t device, 
		void *buf, 
		aoff_t count,
		aoff_t *read_size,
		int non_block)
{
	return pipe_read(ptys[MINOR(device)], buf, count, read_size, non_block);
}

int pty_putc(dev_t device, char a)
{
	aoff_t w;
	pipe_write(ptys[MINOR(device)], &a, 1, &w, 1);
	return 0;
}

int pty_ioctl(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, int func, __attribute__((__unused__)) int arg)
{
	switch(func) {	
		
		default:
			return 0;
				
	}
}

tty_ops_t pty_ops = {
	.open = &pty_open,
	.close = &pty_close,
	.write = &pty_write,
	.read = &pty_read,
	.ioctl = &pty_ioctl
};

char_dev_t pty_desc = {
	"pty master",
	0x79,
	&pty_ops
};

void pty_init()
{	
	int m;
	for (m = 0; m < 255; m++){
		ptys[m] = pipe_create();
		pipe_open_read(ptys[m]);
		pipe_open_write(ptys[m]);
	}
	device_char_register(&pty_desc);
	tty_register_driver("pty slave", 0x78, 255, &pty_putc);
}
