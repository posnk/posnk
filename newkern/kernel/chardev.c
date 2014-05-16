/**
 * kernel/chardev.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 17-04-2014 - Created
 */
#include "kernel/syscall.h"
#include "kernel/device.h"
#include "kernel/heapmm.h"
#include <sys/types.h>
#include <sys/errno.h>
#include <string.h>

char_dev_t **char_dev_table;

void device_char_init()
{
	char_dev_table = heapmm_alloc(sizeof(char_dev_t *) * 256);
	memset(char_dev_table, 0, sizeof(char_dev_t *) * 256);
}

int device_char_register(char_dev_t *driver)
{
	if (!driver)
		return 0;
	if (char_dev_table[driver->major])
		return 0;
	char_dev_table[driver->major] = driver;
	return 1;
}

int device_char_ioctl(dev_t device, int fd, int func, int arg)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	if (!drv) {
		syscall_errno = ENODEV;
		return -1;
	}
	return drv->ops->ioctl(device, fd, func, arg);
}

int device_char_open(dev_t device, int fd, int options)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	if (!drv) {
		syscall_errno = ENODEV;
		return -1;
	}
	return drv->ops->open(device, fd, options);
}

int device_char_close(dev_t device, int fd)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	if (!drv) {
		syscall_errno = ENODEV;
		return -1;
	}
	return drv->ops->close(device, fd);
}

int device_char_write(dev_t device, __attribute__((__unused__)) off_t file_offset, void * buffer, size_t count, size_t *write_size, int non_block)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	if (!drv) {
		syscall_errno = ENODEV;
		return -1;
	}
	return drv->ops->write(device, buffer, count, write_size, non_block);
	/*size_t c;//HACKHACKHACKHACK
	char *b = (char *)buffer;
	if (device == 0x1337){
		for (c = 0; c < count; c++)
			earlycon_putc(b[c]);
		(*write_size) = count;
		return 0;
	} else
		return ENODEV;*/
}

int device_char_read(dev_t device, __attribute__((__unused__)) off_t file_offset, void * buffer, size_t count, size_t *read_size, int non_block)
{
	dev_t major = MAJOR(device);
	char_dev_t *drv = char_dev_table[major];
	if (!drv) {
		syscall_errno = ENODEV;
		return -1;
	}
	return drv->ops->read(device, buffer, count, read_size, non_block);
	/*size_t c;//HACKHACKHACKHACK
	char *b = (char *)buffer;
			//debugcon_aputs("reading//\n");
	if (device == 0x1337){
		for (c = 0; c < count; c++){
		//	debugcon_aputs("reading//\n");
			b[c] = debugcon_getc();
			earlycon_putc(b[c]);
			if ((b[c] == '\r')) {
				b[c] = '\n';
				(*read_size) = c + 1;//count;
				earlycon_putc('\n');
				return 0;				
			}
			if (b[c] == '\n')
				c--;	
		}
		(*read_size) = count;
		return 0;
	} else
		return ENODEV;*/
}
