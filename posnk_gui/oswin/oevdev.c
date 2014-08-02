/**
* Part of oswin
* Written by Peter Bosch <peterbosc@gmail.com>
*/

#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <clara/clara.h>
#include <clara/cinput.h>
#include <errno.h>
#include <string.h>
#include "oinput.h"

int evdev_fd;

int evdev_init(const char *dev)
{
	evdev_fd = open(dev, O_RDONLY | O_NONBLOCK);
	if (evdev_fd == -1) {
		fprintf(stderr, "fatal: cannot open event device %s : %s\n",dev,  strerror(errno));
		return -1;
	}
	return 0;
}

int evdev_poll()
{
	int nr = 1;
	clara_event_msg_t msg;
	struct input_event ev;
	while (nr) {
		nr = read(evdev_fd, &ev, sizeof(struct input_event));
		if ((nr == -1) && (errno == EAGAIN))
			return 0;
		if (nr)	{
			msg.flags = CLARA_EVENT_FLAG_FOCUS;
			if (ev.type == EV_REL) {
				msg.flags |= CLARA_EVENT_FLAG_PNT_DELTA;
				msg.ptr.x = 0;
				msg.ptr.y = 0;
				if (ev.code == REL_X)
					msg.ptr.x = ev.value;
				else if (ev.code == REL_Y)
					msg.ptr.y = -ev.value;
				oswin_input_handle(&msg);
			} else
				fprintf(stderr, "debug: unhandled event: %i", ev.type);
		}
	}
	return 1;

}
