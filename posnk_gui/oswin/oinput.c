#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
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

clara_point_t	oswin_input_pointer_position = {0,0};

clara_window_t  *oswin_dragging_window = NULL;
clara_point_t	 oswin_dragging_start;
int		 oswin_dragging_resize = 0;

int oswin_input_init()
{
	return 0;
}

void oswin_input_close()
{
}

void oswin_frame_handle_click(osession_node_t *session, oswin_window_t *_window, clara_event_msg_t *event)
{
	int action;
	event->ptr.x -= _window->window->frame_dims.x;
	event->ptr.y -= _window->window->frame_dims.y;
	action = oswin_decorator_handle_input(session, _window, event);
	switch (action) {
		case OSWIN_INPUT_ACTION_WIN_CLOSE:
			printf("close\n");
			break;
		case OSWIN_INPUT_ACTION_WIN_MAXIMIZE:
			printf("maximize\n");
			break;
		case OSWIN_INPUT_ACTION_WIN_ICONIFY:
			printf("iconify\n");
			break;
		case OSWIN_INPUT_ACTION_WIN_DRAG:
			oswin_dragging_window = _window->window;
			oswin_dragging_resize = 0;
			oswin_dragging_start = event->ptr;
			break;
		case OSWIN_INPUT_ACTION_WIN_RESIZE:
			oswin_dragging_window = _window->window;
			oswin_dragging_resize = 1;
			oswin_dragging_start = event->ptr;
			break;
		case OSWIN_INPUT_ACTION_NONE:
			break;
		default:
			printf("unh wnd act: %i\n", action);
			break;
	}
}

void oswin_input_handle_drag(clara_event_msg_t *event)
{
	assert(oswin_dragging_window != NULL);

	event->ptr.x -= oswin_dragging_window->frame_dims.x;
	event->ptr.y -= oswin_dragging_window->frame_dims.y;

	if (oswin_dragging_resize) {

		oswin_dragging_window->dimensions.w += event->ptr.x - oswin_dragging_start.x;
		oswin_dragging_window->dimensions.h += event->ptr.y - oswin_dragging_start.y;

		if (oswin_dragging_window->dimensions.w < 100)
			oswin_dragging_window->dimensions.w = 100;
		if (oswin_dragging_window->dimensions.h < 100)
			oswin_dragging_window->dimensions.h = 100;

		if (event->event_type == CLARA_EVENT_TYPE_MOUSE_BTN_UP)
			oswin_dragging_window = NULL;

		oswin_add_damage(oswin_dragging_window->frame_dims);

		oswin_decorator_update_frame_dims(oswin_dragging_window);

		oswin_add_damage(oswin_dragging_window->frame_dims);

		oswin_dragging_start = event->ptr;

	} else {

		oswin_dragging_window->dimensions.x += event->ptr.x - oswin_dragging_start.x;
		oswin_dragging_window->dimensions.y += event->ptr.y - oswin_dragging_start.y;

		oswin_add_damage(oswin_dragging_window->frame_dims);

		oswin_decorator_update_frame_dims(oswin_dragging_window);

		oswin_add_damage(oswin_dragging_window->frame_dims);

		if (event->event_type == CLARA_EVENT_TYPE_MOUSE_BTN_UP)
			oswin_dragging_window = NULL;

	}
}

int oswin_input_handle(clara_event_msg_t *event)
{
	uint32_t target;
	cllist_t *_w;
	clara_window_t *window;
	oswin_window_t *wnd;
	assert(event != NULL);

	if (event->flags & CLARA_EVENT_FLAG_PNT_ABS) {
		oswin_input_pointer_position = event->ptr;
	} else if (event->flags & CLARA_EVENT_FLAG_PNT_DELTA) {
		oswin_input_pointer_position.x += event->ptr.x;
		oswin_input_pointer_position.y += event->ptr.y;
	}
	event->ptr = oswin_input_pointer_position;

	if (oswin_dragging_window != NULL){
		oswin_input_handle_drag(event);
		return 1;
	}

	if (event->flags & CLARA_EVENT_FLAG_WM) {
		//Handle oswin input
	} else if (event->flags & CLARA_EVENT_FLAG_FOCUS) {
		if (oswin_focused_session != NULL) {
			if (oswin_focused_window) {
				event->ptr.x -= oswin_focused_window->window->dimensions.x;
				event->ptr.y -= oswin_focused_window->window->dimensions.y;
			} else
				target = CLARA_MSG_TARGET_SESSION;
			oswin_send_event(oswin_focused_session, target, CLARA_MSG_EVENT, event, CLARA_MSG_SIZE(clara_event_msg_t));
			return 1;
		} else
			return 0;
	} else {
		for (_w = oswin_window_list.prev; _w != &oswin_window_list; _w = _w->prev) {
			wnd = (oswin_window_t *) _w;
			window = wnd->window;
			if (clara_rect_test(window->dimensions, oswin_input_pointer_position)) {
				if (event->flags & CLARA_EVENT_FLAG_SETFOCUS) {
					oswin_session_focus(wnd->session, wnd);
				}
				event->ptr.x -= window->dimensions.x;
				event->ptr.y -= window->dimensions.y;
				oswin_send_event(wnd->session, window->handle, CLARA_MSG_EVENT, event, CLARA_MSG_SIZE(clara_event_msg_t));
				return 1;
			} else if (clara_rect_test(window->frame_dims, oswin_input_pointer_position)) {
				if (event->flags & CLARA_EVENT_FLAG_SETFOCUS) {
					oswin_session_focus(wnd->session, wnd);
				}
				oswin_frame_handle_click(wnd->session, wnd, event);
				return 1;
			}
		}
	}
	return 0;
}
