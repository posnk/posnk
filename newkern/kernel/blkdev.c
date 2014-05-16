/**
 * kernel/blkdev.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 17-04-2014 - Created
 */

#include "kernel/device.h"
#include <sys/types.h>
#include <sys/errno.h>

//TODO: TODO: TODO: IMPLEMENT BLOCK DEVICES

int device_block_write(dev_t device, off_t file_offset, void * buffer, size_t count, size_t *write_size)
{
	(*write_size) = 0;
	return EIO;
}

int device_block_read(dev_t device, off_t file_offset, void * buffer, size_t count, size_t *read_size)
{
	(*read_size) = 0;
	return EIO;
}
