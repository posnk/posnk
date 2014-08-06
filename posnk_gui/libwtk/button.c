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
	int t_x, t_y;

	cairo_rectangle(context, 0, 0, w->rect.w, w->rect.h);
	if (button->state)
		cairo_set_source_rgb(context, 0.3, 0.3, 0.3);
	else
		cairo_set_source_rgb(context, 0.6, 0.6, 0.6);
	cairo_fill(context);

	cairo_rectangle(context, 0, 0, w->rect.w, w->rect.h);
	cairo_set_source_rgb(context, 0, 0, 0);
	cairo_stroke(context);
	
	cairo_set_font_face(context, face);
	cairo_set_font_size(context, 16);

	cairo_font_extents (context, &fe);
	cairo_text_extents (context, button->text, &te);

	t_x = (w->rect.w / 2) - (te.width / 2);
	t_y = (w->rect.h / 2) - (te.height / 2);

	cairo_move_to (context, te.x_bearing + t_x,  t_y - te.y_bearing);

	cairo_set_source_rgb (context, 1, 1, 1);
	cairo_show_text (context, button->text);
}

void wtk_button_mouse_down(wtk_widget_t *w, clara_point_t position, int mbutton)
{
	wtk_button_t *button = (wtk_button_t *) w->impl;
	button->state = 1;
	printf("button down\n");
	wtk_widget_redraw(w);
}

void wtk_button_mouse_up(wtk_widget_t *w, clara_point_t position, int mbutton)
{
	wtk_button_t *button = (wtk_button_t *) w->impl;
	button->state = 0;
	printf("button up\n");
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
