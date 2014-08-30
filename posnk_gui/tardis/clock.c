#include <time.h>
#include <cairo.h>
#include <wtk/widget.h>
#include <wtk/textbox.h>
#include <wtk/resources.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <murrine.h>
#include <linux/input.h>


void clock_paint(wtk_widget_t *w, cairo_t *context, int focused)
{
	time_t rawtime;
	char *text;
	cairo_font_face_t *face = wtk_get_normal_font();
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	int t_x, t_y, t_h, t_w;

	t_w = w->rect.w;
	t_h = w->rect.h;
	
	cairo_set_font_face(context, face);
	cairo_set_font_size(context, 16);

	time (&rawtime);
	text = ctime(&rawtime);

	cairo_font_extents (context, &fe);
	cairo_text_extents (context, text, &te);

	t_x = (t_w / 2) - (te.width / 2);
	t_y = (t_h / 2) - (te.height / 2);

	cairo_move_to (context, te.x_bearing + t_x + 1,  t_y - te.y_bearing + 1);

	cairo_set_source_rgb (context, 0, 0, 0);
	cairo_show_text (context, text);

	cairo_move_to (context, te.x_bearing + t_x,  t_y - te.y_bearing);

	cairo_set_source_rgb (context, 1, 1, 1);
	cairo_show_text (context, text);
}

wtk_widget_t *clock_create(clara_rect_t rect)
{
	wtk_widget_t *w = wtk_create_widget(rect);
	w->callbacks.paint = &clock_paint;
	return w;
}
