#include <clara/cwindow.h>
#include <clara/cmsg.h>
#include <clara/ctypes.h>
#include "osession.h"

#ifndef __OSWIN_OWINDOW_H__
#define __OSWIN_OWINDOW_H__

void oswin_window_process(osession_node_t *session);

int oswin_window_cmd(osession_node_t *session, clara_window_t *window, clara_msg_buffer_t *cmd);

void oswin_window_init(osession_node_t *session, clara_window_t *window, clara_initwin_msg_t *cmd);

void oswin_window_swap(osession_node_t *session, clara_window_t *window, clara_swapwin_msg_t *cmd);
#endif
