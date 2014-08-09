#include <cairo.h>
#include <wtk/widget.h>
#include <wtk/button.h>
#include <wtk/resources.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <murrine.h>

void wtk_button_paint(wtk_widget_t *w, cairo_t *context, int focused)
{
	wtk_button_t *button = (wtk_button_t *) w->impl;
	cairo_font_face_t *face = wtk_get_normal_font();
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	int t_x, t_y, t_h, t_w;
	int s = button->state;
	if (focused && (s == WTK_STATE_NORMAL))
		s = WTK_STATE_SELECTED;
	t_w = w->rect.w;
	t_h = w->rect.h;
	murrine_draw_btn(context, focused, 0, button->state, t_w, t_h);

	cairo_set_font_face(context, face);
	cairo_set_font_size(context, 16);

	cairo_font_extents (context, &fe);
	cairo_text_extents (context, button->text, &te);

	t_x = (t_w / 2) - (te.width / 2);
	t_y = (t_h / 2) - (te.height / 2);

	cairo_move_to (context, te.x_bearing + t_x + 1,  t_y - te.y_bearing + 1);

	cairo_set_source_rgb (context, 1, 1, 1);
	cairo_show_text (context, button->text);

	cairo_move_to (context, te.x_bearing + t_x,  t_y - te.y_bearing);

	cairo_set_source_rgb (context, 0, 0, 0);
	cairo_show_text (context, button->text);
}

void wtk_button_mouse_down(wtk_widget_t *w, clara_point_t position, int mbutton)
{
	wtk_button_t *button = (wtk_button_t *) w->impl;
	button->state = WTK_STATE_ACTIVE;
	printf("btn down!\n");
	wtk_widget_redraw(w);
}

void wtk_button_mouse_up(wtk_widget_t *w, clara_point_t position, int mbutton)
{
	wtk_button_t *button = (wtk_button_t *) w->impl;
	button->state = 0;
	printf("btn up!\n");
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
