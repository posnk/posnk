#include <cairo.h>
#include <wtk/widget.h>
#include <wtk/button.h>
#include <wtk/resources.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void wtk_button_paint(wtk_widget_t *w, cairo_t *context)
{
	wtk_button_t *button = (wtk_button_t *) w->impl;
	cairo_font_face_t *face = wtk_get_normal_font();
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	cairo_pattern_t *left_ptn, *mid_ptn, *right_ptn;
	cairo_matrix_t left_mtx, mid_mtx, right_mtx;
	int t_x, t_y, b_h, s_w, s, t_h, t_w, m_w, mi_w;
	double y_scale;
	double x_scale;

	s = button->state;
	t_w = w->rect.w;
	t_h = w->rect.h;
	mi_w = cairo_image_surface_get_width(wtk_button_images[s][1]);
	b_h = cairo_image_surface_get_height(wtk_button_images[s][1]);
	s_w = cairo_image_surface_get_width(wtk_button_images[s][0]);
	m_w = t_w - 2 * s_w;

	x_scale = ((double)mi_w) / ((double)m_w);
	y_scale = ((double)b_h) / ((double)t_h);
	
	left_ptn  = cairo_pattern_create_for_surface(wtk_button_images[s][0]);
	mid_ptn   = cairo_pattern_create_for_surface(wtk_button_images[s][1]);
	right_ptn = cairo_pattern_create_for_surface(wtk_button_images[s][2]);

	cairo_matrix_init(&left_mtx, 1, 0, 0, y_scale, 0, 0);
	cairo_matrix_init(&mid_mtx, x_scale, 0, 0, y_scale, -s_w, 0);
	cairo_matrix_init(&right_mtx, 1, 0, 0, y_scale, -(t_w - s_w), 0);

	cairo_pattern_set_matrix(left_ptn, &left_mtx);
	cairo_pattern_set_matrix(mid_ptn, &mid_mtx);
	cairo_pattern_set_matrix(right_ptn, &right_mtx);

	cairo_rectangle(context, 0, 0, s_w, t_h);
	cairo_set_source(context, left_ptn);
	cairo_fill(context);

	cairo_rectangle(context, s_w, 0, t_w - 2 * s_w, t_h);
	cairo_set_source(context, mid_ptn);
	cairo_fill(context);

	cairo_rectangle(context, t_w - s_w, 0, s_w, t_h);
	cairo_set_source(context, right_ptn);
	cairo_fill(context);
	
	cairo_set_font_face(context, face);
	cairo_set_font_size(context, 16);

	cairo_font_extents (context, &fe);
	cairo_text_extents (context, button->text, &te);

	t_x = (t_w / 2) - (te.width / 2);
	t_y = (t_h / 2) - (te.height / 2);

	cairo_move_to (context, te.x_bearing + t_x,  t_y - te.y_bearing);

	cairo_set_source_rgb (context, 1, 1, 1);
	cairo_show_text (context, button->text);

	cairo_pattern_destroy(left_ptn);
	cairo_pattern_destroy(mid_ptn);
	cairo_pattern_destroy(right_ptn);
}

void wtk_button_mouse_down(wtk_widget_t *w, clara_point_t position, int mbutton)
{
	wtk_button_t *button = (wtk_button_t *) w->impl;
	button->state = 1;
	wtk_widget_redraw(w);
}

void wtk_button_mouse_up(wtk_widget_t *w, clara_point_t position, int mbutton)
{
	wtk_button_t *button = (wtk_button_t *) w->impl;
	button->state = 0;
	if (button->callbacks.clicked)
		button->callbacks.clicked(w);
	wtk_widget_redraw(w);
}

wtk_widget_t *wtk_create_button(clara_rect_t rect, char *text, void (*clicked)(wtk_widget_t *))
{
	wtk_widget_t *w = wtk_create_widget(rect);
	wtk_button_t *button = malloc(sizeof(wtk_button_t));
	w->callbacks.paint = &wtk_button_paint;
	w->callbacks.mouse_down = &wtk_button_mouse_down;
	w->callbacks.mouse_up = &wtk_button_mouse_up;
	w->impl = button;
	button->state = 0;
	button->callbacks.clicked = clicked;
	button->text = malloc(strlen(text) + 1);
	strcpy(button->text, text);
	return w;
}
