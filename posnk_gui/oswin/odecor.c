#include "osession.h"
#include "owindow.h"
#include "orender.h"
#include "odecor.h"
#include "oinput.h"
#include "ovideo.h"
#include <clara/clara.h>
#include <stdlib.h>
#include <bitmap.h>
#include <clara/cllist.h>
#include <clara/cwindow.h>
#include <stdio.h>
#include <ft2build.h>
#include <cairo-ft.h>
#include <assert.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

extern cllist_t oswin_session_list;

FT_Library 	 oswin_title_font_lib;
FT_Face 	 oswin_title_font_ft;
cairo_font_face_t *oswin_title_font;
cairo_surface_t *oswin_bmp_cursor;
cairo_surface_t *oswin_bmp_background;

cairo_surface_t *oswin_bmp_title[2];
cairo_surface_t *oswin_bmp_close_d[2];
cairo_surface_t *oswin_bmp_close_m[2];
cairo_surface_t *oswin_bmp_max_d[2];
cairo_surface_t *oswin_bmp_min_d[2];
cairo_pattern_t *oswin_ptn_title[2];
double		 oswin_clr_title[2][3] = {{52.0, 51.0, 48.0}, {60.0, 59.0, 55.0}};

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
	int dd = 0;
	int st;
	int fd;
	struct stat f_b;
	uint8_t	*f_m;
	oswin_bmp_title[1]   = oswin_bmp_surface(load_bmp("/share/oswin/title_active.bmp"));
	oswin_bmp_title[0] = oswin_bmp_surface(load_bmp("/share/oswin/title_inactive.bmp"));
	oswin_bmp_close_d[1] = oswin_bmp_surface(load_bmp("/share/oswin/close_active_d.bmp"));
	oswin_bmp_close_m[1] = oswin_bmp_surface(load_bmp("/share/oswin/close_active_m.bmp"));
	oswin_bmp_close_d[0] = oswin_bmp_surface(load_bmp("/share/oswin/close_inactive_d.bmp"));
	oswin_bmp_close_m[0] = oswin_bmp_surface(load_bmp("/share/oswin/close_inactive_m.bmp"));
	oswin_bmp_max_d[1] = oswin_bmp_surface(load_bmp("/share/oswin/max_active_d.bmp"));
	oswin_bmp_max_d[0] = oswin_bmp_surface(load_bmp("/share/oswin/max_inactive_d.bmp"));
	oswin_bmp_min_d[1] = oswin_bmp_surface(load_bmp("/share/oswin/min_active_d.bmp"));
	oswin_bmp_min_d[0] = oswin_bmp_surface(load_bmp("/share/oswin/min_inactive_d.bmp"));
	oswin_bmp_cursor 	 = cairo_image_surface_create_from_png ("/share/oswin/cursor.png");
	oswin_bmp_background	 = oswin_bmp_surface(load_bmp("/root/dsotm.bmp"));
	oswin_ptn_title[1]	 = cairo_pattern_create_for_surface(oswin_bmp_title[1]);
	oswin_ptn_title[0]	 = cairo_pattern_create_for_surface(oswin_bmp_title[0]);
	cairo_pattern_set_extend(oswin_ptn_title[0], CAIRO_EXTEND_REPEAT);
	cairo_pattern_set_extend(oswin_ptn_title[1], CAIRO_EXTEND_REPEAT);
	FT_Init_FreeType( &oswin_title_font_lib );
	st = FT_New_Face( oswin_title_font_lib, "/share/oswin/title.ttf", 0, &oswin_title_font_ft );
	assert(st == 0);
	oswin_title_font =  cairo_ft_font_face_create_for_ft_face(oswin_title_font_ft,0);
	assert(oswin_title_font != 0);
	
}

void oswin_decorator_update_frame_dims(clara_window_t *window)
{
	int tb_h = cairo_image_surface_get_height(oswin_bmp_title[0]);
	window->frame_dims.x = window->dimensions.x - 2;
	window->frame_dims.w = window->dimensions.w + 4;
	window->frame_dims.y = window->dimensions.y - tb_h;
	window->frame_dims.h = window->dimensions.h + 2 + tb_h;
}

int  oswin_decorator_handle_input(osession_node_t *session, oswin_window_t *_window, clara_event_msg_t *event)
{
	clara_window_t *window = _window->window;
	int tb_w = window->dimensions.w + 4;
	int tb_h = cairo_image_surface_get_height(oswin_bmp_title[0]);
	int cb_w = cairo_image_surface_get_width(oswin_bmp_close_d[0]);
	int cb_h = cairo_image_surface_get_height(oswin_bmp_close_d[0]);
	int cb_x = tb_w - cb_w;
	clara_rect_t tb_r = {0, 0, tb_w, tb_h};
	clara_rect_t cb_r = {cb_x, 0, cb_w, cb_h};
	switch (event->event_type) {
		case CLARA_EVENT_TYPE_MOUSE_BTN_DOWN:
			if (clara_rect_test(cb_r, event->ptr)) {
				return OSWIN_INPUT_ACTION_WIN_CLOSE;
			} else if (clara_rect_test(tb_r, event->ptr)) {
				return OSWIN_INPUT_ACTION_WIN_DRAG;
			} else {
				return OSWIN_INPUT_ACTION_WIN_RESIZE;
			}
			break;
		default:
			break;			
	}
	return OSWIN_INPUT_ACTION_NONE;
}

void oswin_decorations_render(osession_node_t *session, cairo_t *cr, clara_window_t *window)
{
	cairo_matrix_t m;
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	int f = 0;
	int tb_w = window->dimensions.w + 4;
	int tb_h = cairo_image_surface_get_height(oswin_bmp_title[f]);
	int cb_w = cairo_image_surface_get_width(oswin_bmp_close_d[f]);
	int mb_w = cairo_image_surface_get_width(oswin_bmp_max_d[f]);
	int ib_w = cairo_image_surface_get_width(oswin_bmp_min_d[f]);
	int cb_x = tb_w - cb_w;
	int mb_x = cb_x - mb_w;
	int ib_x = mb_x - ib_w;
	int t_y, t_x;

	f = ((session == oswin_focused_session) && (window == oswin_focused_window->window)) ? 1 : 0;

	cairo_get_matrix(cr, &m);
	cairo_translate(cr, -2, -tb_h);

	cairo_set_source_rgb(cr, oswin_clr_title[f][0]/255.0, oswin_clr_title[f][1]/255.0, oswin_clr_title[f][2]/255.0);
	cairo_rectangle(cr, 0, 0, window->frame_dims.w, window->frame_dims.h);
	cairo_fill(cr);

	cairo_set_source(cr, oswin_ptn_title[f]);
	cairo_rectangle(cr, 0, 0, tb_w, tb_h);
	cairo_fill(cr);

	cairo_set_source_surface(cr, oswin_bmp_close_d[f], cb_x, 0);
	cairo_rectangle(cr, cb_x, 0, cb_w, tb_h);
	cairo_fill(cr);

	cairo_set_source_surface(cr, oswin_bmp_max_d[f], mb_x, 0);
	cairo_rectangle(cr, mb_x, 0, mb_w, tb_h);
	cairo_fill(cr);

	cairo_set_source_surface(cr, oswin_bmp_min_d[f], ib_x, 0);
	cairo_rectangle(cr, ib_x, 0, ib_w, tb_h);
	cairo_fill(cr);

	cairo_set_font_face(cr, oswin_title_font);
	cairo_set_font_size (cr, 16);

	cairo_font_extents (cr, &fe);
	cairo_text_extents (cr, window->title, &te);

	t_x = (tb_w / 2) - (te.width / 2);
	t_y = (tb_h / 2) - (te.height / 2);
	cairo_move_to (cr, te.x_bearing + t_x+1,  t_y - te.y_bearing+1);

	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_show_text (cr, window->title);

	cairo_move_to (cr, te.x_bearing + t_x,  t_y - te.y_bearing);

	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_show_text (cr, window->title);

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
