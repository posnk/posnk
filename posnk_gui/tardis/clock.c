#include <time.h>
#include <stdlib.h>
#include <cairo.h>
#include <wtk/widget.h>
#include <wtk/textbox.h>
#include <wtk/resources.h>
#include "tardis.h"

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

void clock_process(panel_widget_t *w)
{
	if (difftime(time(NULL), (time_t) w->impl) >= 1) {
		wtk_widget_redraw(w->widget);
		time((time_t *) &w->impl);
	}
}

panel_widget_t *clock_create()
{
	clara_rect_t rect = {0, 0, 200, 0};

	panel_widget_t *w = malloc(sizeof(panel_widget_t));

	w->w_mode = PANEL_W_MODE_FIXED;

	w->widget = wtk_create_widget(rect);

	w->widget->callbacks.paint = &clock_paint;
	w->process = &clock_process;

	w->impl = 0;
	return w;
}
