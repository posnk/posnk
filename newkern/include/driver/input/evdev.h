/**
 * driver/evdev.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-07-2014 - Created
 */

#ifndef __DRIVER_EVDEV_H__
#define __DRIVER_EVDEV_H__

#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include "util/llist.h"
#include "kernel/synch.h"

#define EVDEV_MAJOR		0x04

typedef struct {
	llist_t			 link;
	struct input_event	 event;
} evdev_event_t;

typedef struct {
	dev_t			 device;
} evdev_device_info_t;

typedef struct {
	evdev_device_info_t	*info;
	llist_t			 queue;
	int			 queue_count;
	semaphore_t		 event_wait;
} evdev_device_t;

int evdev_register_device(evdev_device_info_t *info);

void evdev_init();

#endif
