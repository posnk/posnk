/**
 * driver/evdev.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 30-07-2014 - Created
 */

#include "driver/input/evdev.h"
#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/time.h"
#include <sys/errno.h>
#include <assert.h>
#include "config.h"

evdev_device_t **evdev_list;
dev_t		 evdev_minor_counter = 0;

int evdev_register_device(evdev_device_info_t *info)
{
	dev_t minor;
	evdev_device_t *dev = heapmm_alloc(sizeof(evdev_device_t));
	if (!dev)
		return ENOMEM;
	minor = evdev_minor_counter++;
	dev->info = info;
	dev->queue_count = 0;
	dev->event_wait = 0;
	llist_create(&(dev->queue));
	evdev_list[minor] = dev;
	info->device = MAKEDEV(EVDEV_MAJOR, minor);
	return 0;
}

static inline evdev_device_t *evdev_get(dev_t device)
{
	dev_t minor = MINOR(device);
	return evdev_list[minor];
}

void evdev_post_event(dev_t device, struct input_event event)
{
	evdev_event_t *ev;
	evdev_device_t *dev = evdev_get(device);
	assert(dev != NULL);
	ev = heapmm_alloc(sizeof(evdev_event_t));
	ev->event = event;
	ev->event.time.tv_sec = (time_t) system_time;
	ev->event.time.tv_usec = (time_t) system_time_micros;
	llist_add_end(&(dev->queue), (llist_t *) ev);
	dev->queue_count++;
	if (dev->queue_count == CONFIG_EVDEV_QUEUE_SIZE) {
		ev = (evdev_event_t *)llist_remove_first(&(dev->queue));
		heapmm_free(ev, sizeof(evdev_event_t));
		dev->queue_count--;
	}
	semaphore_up(&(dev->event_wait));
}

int evdev_open(dev_t device, __attribute__((__unused__)) int fd, __attribute__((__unused__)) int options)			//device, fd, options
{
	evdev_device_t *dev = evdev_get(device);
	if (!dev)
		return ENXIO;
	return 0;

}

int evdev_close(dev_t device, __attribute__((__unused__)) int fd)
{
	evdev_device_t *dev = evdev_get(device);
	if (!dev)
		return ENXIO;
	return 0;
}

int evdev_write(dev_t device, __attribute__((__unused__)) const void *buf, __attribute__((__unused__)) aoff_t count, __attribute__((__unused__)) aoff_t *write_size, __attribute__((__unused__)) int non_block) //device, buf, count, wr_size, non_block
{
	evdev_device_t *dev = evdev_get(device);
	if (!dev)
		return ENXIO;
	return EROFS;
}

int evdev_read(dev_t device, void *buf, aoff_t count, aoff_t *read_size, int non_block)	//device, buf, count, rd_size, non_block
{
	int ev_count, n, s;
	evdev_event_t *ev;
	struct input_event *ev_buf;
	evdev_device_t *dev = evdev_get(device);
	if (!dev)
		return ENXIO;
	assert (buf != NULL);
	ev_buf = (struct input_event *) buf;
	ev_count = count / sizeof(struct input_event);
	while (dev->queue_count == 0) {
		if (non_block) {
			*read_size = 0;
			return EAGAIN;
		} else {
			s = semaphore_ndown(
				/* semaphore */ &dev->event_wait,
				/* timeout   */ 0,
				/* flags     */ SCHED_WAITF_INTR );
			if ( s != SCHED_WAIT_OK ) {
				*read_size = 0;
				return EINTR;
			}
		}
	}
	for (n = 0; n < ev_count; n++){
		ev = (evdev_event_t *) llist_remove_first(&(dev->queue));
		if (!ev)
			break;
		dev->queue_count--;
		ev_buf[n] = ev->event;
		heapmm_free(ev, sizeof(evdev_event_t));
	}
	*read_size = n * sizeof(struct input_event);
	return 0;
}

int evdev_ioctl(dev_t device, __attribute__((__unused__)) int fd,
								int func,
								__attribute((__unused__)) int arg)			//device, fd, func, arg
{
	evdev_device_t *dev = evdev_get(device);
	if (!dev)
		return ENXIO;
	switch (func) {

		default:
			return 0;
	}
	return 0;
}

tty_ops_t evdev_ops = {
	.open = &evdev_open,
	.close = &evdev_close,
	.read = &evdev_read,
	.write = &evdev_write,
	.ioctl = &evdev_ioctl,
};

char_dev_t evdev_driver = {
	.major = EVDEV_MAJOR,
	.name = "event device interface",
	.ops = &evdev_ops
};

void evdev_init(){
	evdev_list = heapmm_alloc(sizeof(evdev_device_t *) * 256);
	device_char_register(&evdev_driver);
}
