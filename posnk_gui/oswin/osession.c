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
#include "orender.h"

cllist_t	 oswin_session_list;

oswin_window_t	*oswin_focused_window = NULL;
osession_node_t *oswin_focused_session = NULL;

void oswin_session_init()
{
	cllist_create(&oswin_session_list);
}

void oswin_session_focus(osession_node_t *ses, oswin_window_t *wnd)
{
	
	if (oswin_focused_window)
		oswin_add_damage(oswin_focused_window->window->frame_dims);
	oswin_focused_session = ses;
	oswin_focused_window = wnd;
	if (oswin_focused_window)
		oswin_add_damage(oswin_focused_window->window->frame_dims);
}

void oswin_session_accept(uint32_t cmdq_id)
{
	osession_node_t *session;
	fprintf(stderr, "info: accepting new session %i\n", cmdq_id);
	session = malloc(sizeof(osession_node_t));
	session->session.cmd_queue = cmdq_id;
	session->session.server_pid = getpid();
	cllist_create(&(session->session.window_list));
	cllist_add_end(&oswin_session_list, (cllist_t *) session);
}

void oswin_session_close(osession_node_t *session)
{
	cllist_unlink((cllist_t *) session);	
	if (session == oswin_focused_session)
		oswin_focused_session = NULL;
	msgctl(session->session.cmd_queue, IPC_RMID, NULL);
	msgctl(session->session.event_queue, IPC_RMID, NULL);
	free(session);
}

void oswin_session_process()
{
	ssize_t nr;
	clara_msg_buffer_t in_buf;
	cllist_t *_s;
	cllist_t *_n;
	osession_node_t *session;
	for (_s = oswin_session_list.next; _s != &oswin_session_list; _s = _s->next) {
		session = (osession_node_t *) _s;
		oswin_window_process(session);
		do {
			nr = oswin_recv_cmd(session, CLARA_MSG_TARGET_SESSION, &in_buf, CLARA_MSG_SIZE(clara_msg_buffer_t));
			_n = _s->next;
			if (nr == -1) {
				fprintf(stderr, "error: could not receive commands (%s), dropping session %i\n", strerror(errno), session->session.cmd_queue);
				oswin_session_close(session);
				nr = 0;
			} else if (nr) {
				nr = oswin_session_cmd(session, &in_buf);				
			}
			_s = _n->prev;			
		} while (nr);

	}
}

int oswin_session_cmd(osession_node_t *session, clara_msg_buffer_t *cmd)
{
	assert(session != NULL);
	assert(cmd != NULL);
	switch (cmd->msg.type) {
		case CLARA_MSG_CONNECT:
			//TODO: Verify size
			oswin_session_connect(session, (clara_connect_msg_t *) cmd);
			break;
		case CLARA_MSG_DISCONNECT:
			//TODO: Verify size
			fprintf(stderr, "info: session %i disconnected\n", session->session.cmd_queue);
			oswin_session_close(session);
			return 0;
		case CLARA_MSG_CREATE_WIN:
			oswin_session_create_win(session, (clara_createwin_msg_t *) cmd);
			break;
		default:
			fprintf(stderr, "warn: invalid command received for session %i: %i\n", session->session.cmd_queue, cmd->msg.type);
	}
	return 1;
}

void oswin_session_connect(osession_node_t *session, clara_connect_msg_t *cmd)
{
	int s;
	clara_conn_acc_msg_t reply;

	assert(session != NULL);
	assert(cmd != NULL);

	fprintf(stderr, "info: session %i connected (client_pid: %i, event_queue:%i) \n", session->session.cmd_queue, cmd->client_pid, cmd->event_queue);
	session->session.event_queue = cmd->event_queue;
	session->session.client_pid = cmd->client_pid;

	reply.protocol = CLARA_PROTOCOL_VER;
	reply.server_pid = session->session.server_pid;

	s = oswin_send_event_reliable(session, CLARA_MSG_TARGET_SESSION, CLARA_MSG_CONN_ACC, &reply, CLARA_MSG_SIZE(clara_conn_acc_msg_t));

	if (s == -1) {
		fprintf(stderr, "error: session %i connection acknowledge failed to send: %s \n", session->session.cmd_queue, strerror(errno));
	}
}

void oswin_session_create_win(osession_node_t *session, clara_createwin_msg_t *cmd)
{
	int s;

	assert(session != NULL);
	assert(cmd != NULL);

	fprintf(stderr, "info: session %i creating window %i \n", session->session.cmd_queue, cmd->handle);
	
	clara_window_add(&(session->session), cmd->handle);

	s = oswin_send_sync_ack(session, cmd->msg.seq, 0);

	if (s == -1) {
		fprintf(stderr, "error: session %i synchronous command acknowledge failed to send: %s \n", session->session.cmd_queue, strerror(errno));
	}
}
