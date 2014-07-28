#include "osession.h"
#include "odecor.h"
#include "oinput.h"
#include <clara/clara.h>
#include <stdlib.h>
#include <bitmap.h>
#include <clara/cllist.h>
#include <clara/cwindow.h>

extern cllist_t oswin_session_list;

bitmap_image_t *oswin_bmp_title_active;
bitmap_image_t *oswin_bmp_title_inactive;
bitmap_image_t *oswin_bmp_close_active_d;
bitmap_image_t *oswin_bmp_close_active_m;
bitmap_image_t *oswin_bmp_cursor;

void oswin_decorator_init()
{
	oswin_bmp_title_active   = load_bmp("/share/oswin/title_active.bmp");
	oswin_bmp_title_inactive = load_bmp("/share/oswin/title_inactive.bmp");
	oswin_bmp_close_active_d = load_bmp("/share/oswin/close_active_d.bmp");
	oswin_bmp_close_active_m = load_bmp("/share/oswin/close_active_m.bmp");
	oswin_bmp_cursor = load_bmp("/share/oswin/cursor.bmp");
}

void oswin_decorator_do_init(osession_node_t *session, clara_window_t *window)
{
	int tb_h = oswin_bmp_title_active->height;
	window->frame_dims.x = window->dimensions.x;
	window->frame_dims.w = window->dimensions.w + 4;
	window->frame_dims.y = window->dimensions.y - tb_h;
	window->frame_dims.h = window->dimensions.h + 4 + tb_h;
}
int  oswin_decorator_handle_input(osession_node_t *session, clara_window_t *window, clara_event_msg_t *event)
{
	return 1;
}

void oswin_decorations_render(osession_node_t *session, clara_surface_t *target, clara_window_t *window)
{
	int tb_y = window->dimensions.y - oswin_bmp_title_active->height;
	int tb_x = window->dimensions.x;
	int cb_x = tb_x + window->dimensions.w - oswin_bmp_close_active_d->width;
	clara_draw_1d_image(target, tb_x, tb_y, window->dimensions.w, oswin_bmp_title_active);
	clara_draw_image(target, cb_x, tb_y, oswin_bmp_close_active_d);
}

void oswin_cursor_render(clara_surface_t *target)
{
	clara_draw_image(target, oswin_input_pointer_position.x, oswin_input_pointer_position.y, oswin_bmp_cursor);
}
