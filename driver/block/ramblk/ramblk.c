/**
 * @file driver/block/ramblk/ramblk.c
 *
 * Implements a RAM backed block device
 *
 * Part of P-OS driver library.
 *
 * @author Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * \li 18-07-2016 - Created
 */

#include "config.h"
#include <sys/errno.h>
#include <string.h>

#include "kernel/earlycon.h"
#include "kernel/time.h"
#include "kernel/scheduler.h"
#include "kernel/paging.h"
#include "kernel/heapmm.h"
#include "kernel/device.h"

extern blk_ops_t ramblk_driver_ops;

typedef struct {
	char	*data;
	aoff_t	 size;
} ramblk_device_t;

ramblk_device_t ramblk_devs[256];


int ramblk_open(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int options) {
	return 0;
}

int ramblk_close(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd) {
	return 0;
}

int ramblk_write(dev_t device, aoff_t file_offset, void * buffer )
{
	dev_t major = MAJOR(device);
	ramblk_device_t *_dev = &ramblk_devs[major-0x30];

	if (_dev->data == NULL)
		return ENODEV;
		
	if ((file_offset > _dev->size) || ((file_offset + 512) > _dev->size)) {
		return ENOSPC;
	}
	memcpy( &_dev->data[file_offset], buffer, 512);
	return 0;
}

int ramblk_read(dev_t device, aoff_t file_offset, void * buffer )
{
	dev_t major = MAJOR(device);
	ramblk_device_t *_dev = &ramblk_devs[major-0x30];

	if (_dev->data == NULL)
		return ENODEV;
		
	if ((file_offset > _dev->size) || ((file_offset + 512) > _dev->size)) {
		return ENOSPC;	
	}
	memcpy( buffer, &_dev->data[file_offset], 512);
	
	return 0;
}

int ramblk_ioctl(__attribute__((__unused__)) dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int func, __attribute__((__unused__)) int arg)
{
	return 0;
}

void ramblk_init() {}

void ramblk_register( int num, aoff_t size, void * data )
{
	blk_dev_t *drv;
	
	ramblk_devs[num].data = data;
	ramblk_devs[num].size = size;
	
	drv = heapmm_alloc(sizeof(blk_dev_t));
	drv->name = "RAM block driver";
	drv->major = 0x30 + num;
	drv->minor_count = 1;
	drv->block_size = 512;
	drv->cache_size = 16;
	drv->ops = &ramblk_driver_ops;
	device_block_register(drv);
}

blk_ops_t ramblk_driver_ops = {
		&ramblk_open,
		&ramblk_close,
		&ramblk_write,
		&ramblk_read,
		&ramblk_ioctl
};
