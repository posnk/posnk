#include "osession.h"
#include "odecor.h"
#include "oinput.h"
#include "ovideo.h"
#include <clara/clara.h>
#include <stdlib.h>
#include <bitmap.h>
#include <clara/cllist.h>
#include <clara/cwindow.h>
#include <stdio.h>

extern cllist_t oswin_session_list;

cairo_surface_t *oswin_bmp_title_active;
cairo_surface_t *oswin_bmp_title_inactive;
cairo_surface_t *oswin_bmp_close_active_d;
cairo_surface_t *oswin_bmp_close_active_m;
cairo_surface_t *oswin_bmp_cursor;
cairo_surface_t *oswin_bmp_background;
cairo_pattern_t *oswin_ptn_title_active;
cairo_pattern_t *oswin_ptn_title_inactive;

cairo_surface_t *oswin_bmp_surface(bitmap_image_t *s)
{
	cairo_surface_t *sf = cairo_image_surface_create_for_data ((unsigned char *)s->pixels, CAIRO_FORMAT_RGB24, s->width, s->height, s->width * 4);
	if (!sf) {
		fprintf(stderr, "error: could not create cairo surface for image\n");
	}
	return sf;
}

void oswin_decorator_init()
{
	oswin_bmp_title_active   = oswin_bmp_surface(load_bmp("/share/oswin/title_active.bmp"));
	oswin_bmp_title_inactive = oswin_bmp_surface(load_bmp("/share/oswin/title_inactive.bmp"));
	oswin_bmp_close_active_d = oswin_bmp_surface(load_bmp("/share/oswin/close_active_d.bmp"));
	oswin_bmp_close_active_m = oswin_bmp_surface(load_bmp("/share/oswin/close_active_m.bmp"));
	oswin_bmp_cursor 	 = oswin_bmp_surface(load_bmp("/share/oswin/cursor.bmp"));
	oswin_bmp_background	 = oswin_bmp_surface(load_bmp("/root/dsotm.bmp"));
	oswin_ptn_title_active	 = cairo_pattern_create_for_surface(oswin_bmp_title_active);
	oswin_ptn_title_inactive = cairo_pattern_create_for_surface(oswin_bmp_title_inactive);
	cairo_pattern_set_extend(oswin_ptn_title_active, CAIRO_EXTEND_REPEAT);
	cairo_pattern_set_extend(oswin_ptn_title_inactive, CAIRO_EXTEND_REPEAT);
}

void oswin_decorator_do_init(osession_node_t *session, clara_window_t *window)
{
	int tb_h = cairo_image_surface_get_height(oswin_bmp_title_active);
	window->frame_dims.x = window->dimensions.x - 2;
	window->frame_dims.w = window->dimensions.w + 4;
	window->frame_dims.y = window->dimensions.y - tb_h;
	window->frame_dims.h = window->dimensions.h + 2 + tb_h;
}
int  oswin_decorator_handle_input(osession_node_t *session, clara_window_t *window, clara_event_msg_t *event)
{
	return 1;
}

void oswin_decorations_render(clara_session_t *session, cairo_t *cr, clara_window_t *window)
{
	cairo_matrix_t m;
	int tb_h = cairo_image_surface_get_height(oswin_bmp_title_active);
	int cb_w = cairo_image_surface_get_width(oswin_bmp_close_active_d);
	int cb_x = window->dimensions.w + 2 - cb_w;

	cairo_get_matrix(cr, &m);
	cairo_translate(cr, -2, -tb_h);

	cairo_set_source_rgb(cr, 60.0/255.0, 59.0/255.0, 55.0/255.0);
	cairo_rectangle(cr, 0, 0, window->frame_dims.w, window->frame_dims.h);
	cairo_fill(cr);

	cairo_set_source(cr, oswin_ptn_title_active);
	cairo_rectangle(cr, 0, 0, window->dimensions.w + 4, tb_h);
	cairo_fill(cr);

	cairo_set_source_surface(cr, oswin_bmp_close_active_d, cb_x, 0);
	cairo_rectangle(cr, cb_x, 0, cb_w, tb_h);
	cairo_fill(cr);

	cairo_set_matrix(cr, &m);
}

void oswin_cursor_render(cairo_t *cr, int x, int y)
{
	int w = cairo_image_surface_get_width(oswin_bmp_cursor);
	int h = cairo_image_surface_get_height(oswin_bmp_cursor);
	cairo_rectangle (cr, x, y, w, h);
	cairo_set_source_surface (cr, oswin_bmp_cursor, x, y);
	cairo_fill(cr);
}

void oswin_cursor_clip(int x, int y)
{
	int w = cairo_image_surface_get_width(oswin_bmp_cursor);
	int h = cairo_image_surface_get_height(oswin_bmp_cursor);
	oswin_set_clip(x,y,w,h);
}

void oswin_background_render(cairo_t *cr)
{
	cairo_set_source_surface (cr, oswin_bmp_background, 0, 0);
	cairo_paint(cr);
}
