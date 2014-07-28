#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <clara/cmsg.h>
#include <clara/cllist.h>
#include <clara/csession.h>
#include <clara/cwindow.h>
#include <clara/cinput.h>
#include <clara/clara.h>
#include "osession.h"
#include "owindow.h"
#include "omsg.h"
#include "odecor.h"
#include "oinput.h"

int		oswin_input_queue;
clara_point_t	oswin_input_pointer_position = {0,0};

int oswin_input_init()
{
	oswin_input_queue = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
	if (oswin_input_queue == -1) {
		fprintf(stderr, "fatal: could not create input message queue: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

void oswin_input_close()
{
	msgctl(oswin_input_queue, IPC_RMID, NULL);
}

int oswin_input_recv(clara_event_msg_t *msg)
{
	ssize_t sz;
	assert(msg != NULL);

	sz = msgrcv(oswin_input_queue, msg, CLARA_MSG_SIZE(clara_event_msg_t), 0, IPC_NOWAIT);
	if (sz == -1) {
		if ((errno == ENOMSG) || (errno == EAGAIN))
			return 0;
		else
			return -1;
	}

	if (sz < CLARA_MSG_SIZE(clara_event_msg_t)) {
		errno = EINVAL;
		return -1;
	}

	if (msg->msg.magic != CLARA_MSG_MAGIC) {
		errno = EINVAL;
		return -1;
	}
	
	return 1;

}

int oswin_input_send(clara_event_msg_t *msg)
{
	clara_message_t *m = (clara_message_t *) msg;
	assert(m != NULL);

	m->target = 1;
	m->type = CLARA_MSG_EVENT;
	m->magic = CLARA_MSG_MAGIC;

	return msgsnd(oswin_input_queue, msg, CLARA_MSG_SIZE(clara_event_msg_t), 0);	
}

int oswin_input_process()
{
	int s;
	clara_event_msg_t in_buf;
	do {
		s = oswin_input_recv(&in_buf);
		if (s == -1) {
			fprintf(stderr, "error: could not receive input events (%s)\n", strerror(errno));
			s = 0;
			return -1;
		} else if (s) {
			s = oswin_input_handle(&in_buf);				
		}	
	} while (s);
	return 0;
}

int oswin_input_handle(clara_event_msg_t *event)
{
	uint32_t target;
	cllist_t *_s, *_w;
	clara_window_t *window;
	osession_node_t *session;
	assert(event != NULL);

	if (event->flags & CLARA_EVENT_FLAG_PNT_ABS) {
		oswin_input_pointer_position = event->ptr;
	} else if (event->flags & CLARA_EVENT_FLAG_PNT_DELTA) {
		oswin_input_pointer_position.x += event->ptr.x;
		oswin_input_pointer_position.y += event->ptr.y;
	}

	if (event->flags & CLARA_EVENT_FLAG_WM) {
		//Handle oswin input
	} else if (event->flags & CLARA_EVENT_FLAG_FOCUS) {
		if (oswin_focused_session != NULL) {
			window = clara_window_get(&(oswin_focused_session->session), oswin_focused_handle);
			if (window)
				target = oswin_focused_handle;
			else
				target = CLARA_MSG_TARGET_SESSION;
			oswin_send_event(oswin_focused_session, target, CLARA_MSG_EVENT, event, CLARA_MSG_SIZE(clara_event_msg_t));
		} else
			return 0;
	} else {
		for (_s = oswin_session_list.next; _s != &oswin_session_list; _s = _s->next) {
			session = (osession_node_t *) _s;
			for (_w = session->session.window_list.next; _w != &(session->session.window_list); _w = _w->next) {
				window = (clara_window_t *) _w;
				if (clara_rect_test(window->dimensions, oswin_input_pointer_position)) {
					if (event->flags & CLARA_EVENT_FLAG_SETFOCUS) {
						oswin_focused_session = session;
						oswin_focused_handle = window->handle;
					}
					oswin_send_event(session, window->handle, CLARA_MSG_EVENT, event, CLARA_MSG_SIZE(clara_event_msg_t));
					return 1;
				} else if (clara_rect_test(window->frame_dims, oswin_input_pointer_position)) {
					if (event->flags & CLARA_EVENT_FLAG_SETFOCUS) {
						oswin_focused_session = session;
						oswin_focused_handle = window->handle;
					}
					oswin_decorator_handle_input(session, window, event);
					return 1;
				}
			}
		}
	}
	return 0;
}
