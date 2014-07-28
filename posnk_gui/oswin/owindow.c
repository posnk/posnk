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
#include "omsg.h"

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

int oswin_window_cmd(osession_node_t *session, clara_window_t *window, clara_msg_buffer_t *cmd)
{
	assert(session != NULL);
	assert(window != NULL);
	assert(cmd != NULL);
	switch (cmd->msg.type) {
		case CLARA_MSG_INIT_WIN:
			oswin_window_init(session, window, (clara_initwin_msg_t *) cmd);
			break;
		case CLARA_MSG_SWAP_WIN:
			oswin_window_swap(session, window, (clara_swapwin_msg_t *) cmd);
			break;
		default:
			fprintf(stderr, "warn: invalid command received for window %i.%li: %i\n", session->session.cmd_queue, cmd->msg.target, cmd->msg.type);
	}
	return 1;
}

void oswin_window_init(osession_node_t *session, clara_window_t *window, clara_initwin_msg_t *cmd)
{
	int s;

	assert(session != NULL);
	assert(window != NULL);
	assert(cmd != NULL);

	fprintf(stderr, "info: session %i initializing window %li \n", session->session.cmd_queue, cmd->msg.target);
	
	s = clara_window_do_init(window, cmd);

	if (s == -1)
		s = -2;

	s = oswin_send_sync_ack(session, cmd->msg.seq, s);

	if (s == -1) {
		fprintf(stderr, "error: session %i synchronous command acknowledge failed to send: %s \n", session->session.cmd_queue, strerror(errno));
	}
}

void oswin_window_swap(osession_node_t *session, clara_window_t *window, clara_swapwin_msg_t *cmd)
{
	int s = 0;

	assert(session != NULL);
	assert(window != NULL);
	assert(cmd != NULL);

	window->display_surface = cmd->draw_surface;	
	window->current_surface = cmd->back_surface;

	s = oswin_send_sync_ack(session, cmd->msg.seq, s);

	if (s == -1) {
		fprintf(stderr, "error: session %i synchronous command acknowledge failed to send: %s \n", session->session.cmd_queue, strerror(errno));
	}
}
