#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <clara/clara.h>
#include <clara/cwindow.h>
#include <clara/cllist.h>
#include <wtk/window.h>
#include <wtk/widget.h>

#include "tardis.h"

wtk_widget_t	*panel_widget;
cllist_t	 panel_widgets;

void panel_paint(wtk_widget_t *w, cairo_t *ctx, int focused) 
{
	cairo_set_source_rgb (ctx, 0, 0, 0);
	cairo_rectangle(ctx, 0, 0, w->rect.w, w->rect.h);
	cairo_fill(ctx);
}

void panel_layout()
{
	cllist_t *_w;
	panel_widget_t *w;
	int sw, nvw, x, fw, pw, ph;

	/* Initialize vars */
	sw = 0;
	nvw = 0;
	x = 0;
	pw = panel_widget->rect.w;
	ph = panel_widget->rect.h;

	/* Pass 1 - Calculate fixed width sigma-w and Nvarwidth */
	for (_w = panel_widgets.next; _w != &panel_widgets; _w = _w->next) {
		w = (panel_widget_t *) _w;
		switch (w->w_mode) {
			case PANEL_W_MODE_FILL:
				nvw++;
				break;
			case PANEL_W_MODE_FIXED:
				sw += w->widget->rect.w;
				break;
			default:
				fprintf(stderr, "[warning] tardis: panel: widget with invalid w_mode %i found.\n", w->w_mode);
				break;
		}
	}

	/* For now, variable width widgets will all be equally wide */
	if (nvw)
		fw = (pw-sw) / nvw;
	else
		fw = 0;

	/* Pass 2 - Update positions and sizes */
	for (_w = panel_widgets.next; _w != &panel_widgets; _w = _w->next) {
		w = (panel_widget_t *) _w;
		/* Handle width mode specifics */
		switch (w->w_mode) {
			case PANEL_W_MODE_FILL:
				w->widget->rect.w = fw;
				break;
			case PANEL_W_MODE_FIXED:
				break;
			default:
				break;
		}

		/* Update widget bounds */
		w->widget->rect.x = x;
		w->widget->rect.y = 0;
		w->widget->rect.h = ph;

		/* Increment x counter */
		x += w->widget->rect.w;

		/* Request widget repaint */
		wtk_widget_redraw(w->widget);
	}
	
	wtk_widget_redraw(panel_widget);
}

void panel_add_widget(panel_widget_t *widget)
{
	cllist_add_end(&panel_widgets, (cllist_t *) widget);
	panel_layout();
	wtk_widget_add(panel_widget, widget->widget);
}

wtk_widget_t *panel_create()
{
	clara_rect_t d = clara_get_screen_dims();
	clara_rect_t panel_rect = {0, 0, d.w, 32};

	panel_widget = wtk_create_widget(panel_rect);
	assert (panel_widget != NULL);

	panel_widget->callbacks.paint = &panel_paint;
	
	cllist_create(&panel_widgets);

	return panel_widget;
}

void panel_process()
{
	cllist_t *_w;
	panel_widget_t *w;

	for (_w = panel_widgets.next; _w != &panel_widgets; _w = _w->next) {
		w = (panel_widget_t *) _w;
		if (w->process)
			w->process(w);
	}
}
