#include <stdint.h>
#include <clara/cllist.h>
#include <clara/csession.h>
#include <clara/cwindow.h>
#include <clara/ctypes.h>

#ifndef __OSWIN_OSESSION_H__
#define __OSWIN_OSESSION_H__

typedef struct {
	cllist_t		link;
	clara_session_t 	session;
} osession_node_t;

void oswin_session_init();

void oswin_session_accept(uint32_t cmdq_id);

void oswin_session_process();

void oswin_session_render(clara_surface_t *target);

int oswin_session_cmd(osession_node_t *session, clara_msg_buffer_t *cmd);

void oswin_session_connect(osession_node_t *session, clara_connect_msg_t *cmd);

void oswin_session_create_win(osession_node_t *session, clara_createwin_msg_t *cmd);

#endif
