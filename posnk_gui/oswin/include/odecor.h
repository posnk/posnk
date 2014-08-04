#include <clara/cwindow.h>
#include <clara/cmsg.h>
#include <clara/ctypes.h>
#include <clara/cinput.h>
#include "osession.h"
#include <cairo.h>

#ifndef __OSWIN_ODECOR_H__
#define __OSWIN_ODECOR_H__

void oswin_decorator_init();
void oswin_decorator_update_frame_dims(clara_window_t *window);
int  oswin_decorator_handle_input(osession_node_t *session, oswin_window_t *window, clara_event_msg_t *event);
void oswin_decorations_render(osession_node_t *session, cairo_t *cr, clara_window_t *window);
void oswin_cursor_clip(int x, int y);
void oswin_cursor_render(cairo_t *cr, int x, int y);
void oswin_background_render(cairo_t *cr);
#endif
