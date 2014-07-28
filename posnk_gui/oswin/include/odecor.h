#include <clara/cwindow.h>
#include <clara/cmsg.h>
#include <clara/ctypes.h>
#include <clara/cinput.h>
#include "osession.h"

#ifndef __OSWIN_ODECOR_H__
#define __OSWIN_ODECOR_H__

void oswin_decorator_init();
void oswin_decorator_do_init(osession_node_t *session, clara_window_t *window);
int  oswin_decorator_handle_input(osession_node_t *session, clara_window_t *window, clara_event_msg_t *event);
void oswin_decorations_render(osession_node_t *session, clara_surface_t *target, clara_window_t *window);
#endif
