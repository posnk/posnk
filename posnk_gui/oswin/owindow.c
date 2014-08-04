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
#include "osession.h"
#include "owindow.h"
#include "odecor.h"
#include "omsg.h"
#include "orender.h"

cllist_t	oswin_window_list;

void oswin_window_ginit()
{
	cllist_create(&oswin_window_list);
}

void oswin_window_order()
{
	if (!oswin_focused_window)
		return;
	cllist_unlink((cllist_t *)oswin_focused_window);
	cllist_add_end(&oswin_window_list, (cllist_t *)oswin_focused_window);
}

void oswin_window_process(osession_node_t *session)
{
	ssize_t nr;
	clara_msg_buffer_t in_buf;
	cllist_t *_s;
	cllist_t *_n;
	clara_window_t *window;
	for (_s = session->session.window_list.next; _s != &(session->session.window_list); _s = _s->next) {
		window = (clara_window_t *) _s;
		do {
			nr = oswin_recv_cmd(session, window->handle, &in_buf, CLARA_MSG_SIZE(clara_msg_buffer_t));
			_n = _s->next;
			if (nr == -1) {
				fprintf(stderr, "error: could not receive commands (%s) for session %i\n", strerror(errno), session->session.cmd_queue);
				return;
			} else if (nr) {
				nr = oswin_window_cmd(session, window, &in_buf);				
			}
			_s = _n->prev;			
		} while (nr);

	}
}

void oswin_window_close(osession_node_t *session, oswin_window_t *window)
{
	clara_event_msg_t msg;

	assert(session != NULL);
	assert(window != NULL);
	
	/*Remove from window list*/
	cllist_unlink((cllist_t *) window);
	
	/*Remove from session->window list*/
	if (window == oswin_focused_window)
		oswin_focused_window = NULL;
	
	cllist_unlink((cllist_t *) window->window);

	msg.event_type = CLARA_EVENT_TYPE_CLOSE_WINDOW;

	oswin_send_event(session, window->window->handle, CLARA_MSG_EVENT, &msg, CLARA_MSG_SIZE(clara_event_msg_t));
}

int oswin_window_cmd(osession_node_t *session, clara_window_t *window, clara_msg_buffer_t *cmd)
{
	assert(session != NULL);
	assert(window != NULL);
	assert(cmd != NULL);
	switch (cmd->msg.type) {
		case CLARA_MSG_INIT_WIN:
			oswin_window_init(session, window, (clara_initwin_msg_t *) cmd);
			break;
		case CLARA_MSG_DAMAGE_WIN:
			oswin_window_damage(session, window, (clara_dmgwin_msg_t *) cmd);
			break;
		default:
			fprintf(stderr, "warn: invalid command received for window %i.%li: %i\n", session->session.cmd_queue, cmd->msg.target, cmd->msg.type);
	}
	return 1;
}

void oswin_window_init(osession_node_t *session, clara_window_t *window, clara_initwin_msg_t *cmd)
{
	int s;
	oswin_window_t	*ownd;	
	clara_surface_t *sf;

	assert(session != NULL);
	assert(window != NULL);
	assert(cmd != NULL);

	fprintf(stderr, "info: session %i initializing window %li \n", session->session.cmd_queue, cmd->msg.target);
	
	s = clara_window_do_init(window, cmd);

	oswin_decorator_update_frame_dims(window);

	ownd = malloc(sizeof(oswin_window_t));
	if (!ownd) {
		fprintf(stderr, "error: out of memory\n");
		s = -1;
	}

	sf = &(window->surface);
	
	ownd->window = window;
	ownd->session = session;
	ownd->surface = cairo_image_surface_create_for_data ((unsigned char *)sf->pixels, CAIRO_FORMAT_RGB24, sf->w, sf->h, sf->w * 4);
	if (!ownd->surface) {
		fprintf(stderr, "error: could not make cairo surface for window\n");
		s = -1;
	}

	cllist_add_end(&oswin_window_list, (cllist_t *) ownd);

	if (s == -1)
		s = -2;

	s = oswin_send_sync_ack(session, cmd->msg.seq, s);

	if (s == -1) {
		fprintf(stderr, "error: session %i synchronous command acknowledge failed to send: %s \n", session->session.cmd_queue, strerror(errno));
	}
	
	oswin_add_damage(window->frame_dims);
}

void oswin_window_damage(osession_node_t *session, clara_window_t *window, clara_dmgwin_msg_t *cmd)
{
	int s = 0;

	assert(session != NULL);
	assert(window != NULL);
	assert(cmd != NULL);

	cmd->rect.x += window->dimensions.x;
	cmd->rect.y += window->dimensions.y;

	oswin_add_damage(cmd->rect);

	s = oswin_send_sync_ack(session, cmd->msg.seq, s);

	if (s == -1) {
		fprintf(stderr, "error: session %i synchronous command acknowledge failed to send: %s \n", session->session.cmd_queue, strerror(errno));
	}
}
