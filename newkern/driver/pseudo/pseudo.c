/**
 * driver/pseudo.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 07-05-2017 - Created
 */

#include "driver/input/evdev.h"
#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/time.h"
#include <sys/errno.h>
#include <assert.h>
#include "config.h"

#define PSEUDO_MAJOR (42)
#define MINOR_ZERO (0)
#define MINOR_NULL (1)


int pseudo_open(dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int options)			//device, fd, options
{
	dev_t minor = MINOR(device);	
	if ( minor > 1 )
		return ENXIO;
	return 0;
	
}

int pseudo_close(dev_t device, __attribute__((__unused__)) int fd)
{
	dev_t minor = MINOR(device);	
	if ( minor > 1 )
		return ENXIO;
	return 0;
}

int pseudo_write(dev_t device, __attribute__((__unused__)) void *buf, __attribute__((__unused__)) aoff_t count, __attribute__((__unused__)) aoff_t *write_size, __attribute__((__unused__)) int non_block) //device, buf, count, wr_size, non_block
{
	dev_t minor = MINOR(device);	
	if ( minor > 1 )
		return ENXIO;
	*write_size = count;
	return 0;
}

int pseudo_read(dev_t device, void *buf, aoff_t count, aoff_t *read_size, int non_block)	//device, buf, count, rd_size, non_block
{
	dev_t minor = MINOR(device);	
	switch ( minor ) {
		case MINOR_ZERO:
			memset( buf, 0, count );
			*read_size = count;
			return 0;
		case MINOR_NULL:
			*read_size = 0;
			return 0;
		default:
			return ENXIO;
	}
}

int pseudo_ioctl(dev_t device, __attribute__((__unused__)) int fd, int func, int arg)			//device, fd, func, arg
{
	switch (func) {
		
		default:
			return 0;
	}
	return 0;
}

tty_ops_t pseudo_ops = {
	.open = &pseudo_open,
	.close = &pseudo_close,
	.read = &pseudo_read,
	.write = &pseudo_write,
	.ioctl = &pseudo_ioctl,
};

char_dev_t pseudo_driver = {
	.major = PSEUDO_MAJOR,
	.name = "character pseudo-devices",
	.ops = &pseudo_ops
};

void pseudo_init(){
	device_char_register(&pseudo_driver);
}
