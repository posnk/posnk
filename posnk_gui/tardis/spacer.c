#include <time.h>
#include <stdlib.h>

#include <cairo.h>

#include <wtk/widget.h>

#include "tardis.h"

void spacer_paint(wtk_widget_t *w, cairo_t *context, int focused)
{
}

panel_widget_t *spacer_create()
{
	clara_rect_t rect = {0, 0, 0, 0};

	panel_widget_t *w = malloc(sizeof(panel_widget_t));

	w->w_mode = PANEL_W_MODE_FILL;

	w->widget = wtk_create_widget(rect);

	w->widget->callbacks.paint = &spacer_paint;

	w->process = NULL;

	return w;
}
