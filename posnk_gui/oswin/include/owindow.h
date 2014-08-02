#include <clara/cwindow.h>
#include <clara/cmsg.h>
#include <clara/ctypes.h>
#include <clara/cllist.h>
#include <cairo.h>
#include "osession.h"

#ifndef __OSWIN_OWINDOW_H__
#define __OSWIN_OWINDOW_H__

typedef struct {
	cllist_t		 link;
	clara_session_t		*session;
	clara_window_t		*window;
	cairo_surface_t		*surface;
} oswin_window_t;

extern cllist_t	oswin_window_list;

void oswin_window_process(osession_node_t *session);

int oswin_window_cmd(osession_node_t *session, clara_window_t *window, clara_msg_buffer_t *cmd);

void oswin_window_init(osession_node_t *session, clara_window_t *window, clara_initwin_msg_t *cmd);

void oswin_window_damage(osession_node_t *session, clara_window_t *window, clara_dmgwin_msg_t *cmd);

void oswin_window_ginit();
#endif
