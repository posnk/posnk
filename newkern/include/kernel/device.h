/**
 * kernel/device.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 17-04-2014 - Created
 */

#ifndef __KERNEL_DEVICE_H__
#define __KERNEL_DEVICE_H__
#include <sys/types.h>
#include "kernel/blkcache.h"
#include "kernel/synch.h"

typedef struct char_dev	char_dev_t;
typedef struct blk_dev	blk_dev_t;
typedef struct tty_ops	tty_ops_t;
typedef struct blk_ops	blk_ops_t;

struct char_dev {
	char			 *name;
	dev_t			  major;
	tty_ops_t		 *ops;	
};

struct blk_dev {
	char			 *name;
	dev_t			  major;
	int			  minor_count;
	aoff_t			  block_size;
	int			  cache_size;
	semaphore_t		**locks;
	blkcache_cache_t	**caches;
	blk_ops_t		 *ops;	
};

struct tty_ops {
	int	(*open)		  (dev_t, int, int);			//device, fd, options
	int	(*close)	  (dev_t, int);				//device, fd
	int	(*write)	  (dev_t, void *, aoff_t, aoff_t *, int); //device, buf, count, wr_size, non_block
	int	(*read)		  (dev_t, void *, aoff_t, aoff_t *, int);	//device, buf, count, rd_size, non_block
	int	(*ioctl)	  (dev_t, int, int, int);			//device, fd, func, arg
};

struct blk_ops {
	int	(*open)		  (dev_t, int, int);			//device, fd, options
	int	(*close)	  (dev_t, int);				//device, fd
	int	(*write)	  (dev_t, aoff_t, void *);		//device, offset, buf
	int	(*read)		  (dev_t, aoff_t, void *); 		//device, offset, buf
	int	(*ioctl)	  (dev_t, int, int, int);		//device, fd, func, arg
};

void device_char_init();

void device_block_init();

int device_char_register(char_dev_t *driver);

int device_block_register(blk_dev_t *driver);

int device_block_ioctl(dev_t device, int fd, int func, int arg);

int device_block_open(dev_t device, int fd, int options);

int device_block_close(dev_t device, int fd);

int device_block_write(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *write_size);

int device_block_read(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size);

int device_char_ioctl(dev_t device, int fd, int func, int arg);

int device_char_open(dev_t device, int fd, int options);

int device_char_close(dev_t device, int fd);

int device_char_write(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *write_size, int non_block);

int device_char_read(dev_t device, aoff_t file_offset, void * buffer, aoff_t count, aoff_t *read_size, int non_block);

#endif
