/**
* Part of oswin
* Written by Peter Bosch <peterbosc@gmail.com>
*/

#include <stdint.h>
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

int evdev_init(const char *dev)
{
	int evdev_fd = open(dev, O_RDONLY | O_NONBLOCK);
	if (evdev_fd == -1) {
		fprintf(stderr, "fatal: cannot open event device %s : %s\n",dev,  strerror(errno));
		return -1;
	}
	return evdev_fd;
}

int evdev_poll(int evdev_fd)
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
				msg.event_type = CLARA_EVENT_TYPE_MOUSE_MOVE;
				if (ev.code == REL_X)
					msg.ptr.x = ev.value;
				else if (ev.code == REL_Y)
					msg.ptr.y = -ev.value;
				oswin_input_handle(&msg);
			} else if (ev.type == EV_KEY) {
				switch (ev.code) {
					case BTN_LEFT:	
						msg.flags = ev.value ? CLARA_EVENT_FLAG_SETFOCUS : CLARA_EVENT_FLAG_FOCUS;
						msg.event_type = ev.value | CLARA_EVENT_TYPE_MOUSE_BTN_UP;
						msg.param[0] = 0;
						oswin_input_handle(&msg);
						break;
					case BTN_RIGHT:	
						msg.flags = ev.value ? CLARA_EVENT_FLAG_SETFOCUS : CLARA_EVENT_FLAG_FOCUS;
						msg.event_type = ev.value | CLARA_EVENT_TYPE_MOUSE_BTN_UP;
						msg.param[0] = 1;
						oswin_input_handle(&msg);
						break;
					case BTN_MIDDLE:	
						msg.flags = ev.value ? CLARA_EVENT_FLAG_SETFOCUS : CLARA_EVENT_FLAG_FOCUS;
						msg.event_type = ev.value | CLARA_EVENT_TYPE_MOUSE_BTN_UP;
						msg.param[0] = 2;
						oswin_input_handle(&msg);
						break;
					default:
						if (ev.code < KEY_UNKNOWN)
							oswin_handle_key(ev.value, ev.code);
						break;						
				}
			} else
				fprintf(stderr, "debug: unhandled event: %i", ev.type);
		}
	}
	return 1;

}
