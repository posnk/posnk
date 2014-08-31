#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <cairo.h>

#include <clara/csession.h>
#include <clara/cwindow.h>

#include <wtk/widget.h>
#include <wtk/resources.h>

#include "tardis.h"

clara_wndlist_t *tasklist_window_list;

int tasklist_paint_task(wtk_widget_t *w, cairo_t *context, clara_wndlist_e_t *wnd, int x)
{
	cairo_font_face_t *face = wtk_get_normal_font();
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	int t_x, t_y, t_h, t_w;

	t_w = w->rect.w;
	t_h = w->rect.h;
	
	cairo_set_font_face(context, face);
	cairo_set_font_size(context, 16);

	cairo_font_extents (context, &fe);
	cairo_text_extents (context, wnd->title, &te);

	t_x = x + 5;
	t_y = (t_h / 2) - (te.height / 2);

	if (wnd->focused) {
		cairo_rectangle(context, x, 2, te.width + 10, t_h - 4);	
		cairo_set_source_rgb(context, 0.3, 0.3, 1);
		cairo_fill(context);
	}

	cairo_move_to (context, te.x_bearing + t_x + 1,  t_y - te.y_bearing + 1);

	cairo_set_source_rgb (context, 0, 0, 0);
	cairo_show_text (context, wnd->title);

	cairo_move_to (context, te.x_bearing + t_x,  t_y - te.y_bearing);

	cairo_set_source_rgb (context, 1, 1, 1);
	cairo_show_text (context, wnd->title);
	return te.width + 10;
}

void tasklist_paint(wtk_widget_t *w, cairo_t *context, int focused)
{
	int p, x = 0;
	if (tasklist_window_list)
		free(tasklist_window_list);
	tasklist_window_list = clara_list_windows();
	assert (tasklist_window_list != NULL);
	for (p = 0; p < tasklist_window_list->entry_count; p++) {
		if (tasklist_window_list->entries[p].flags & CLARA_WIN_FLAG_NOLIST)
			continue;
		x += tasklist_paint_task(w, context, &(tasklist_window_list->entries[p]), x);
	}
}

void tasklist_process(panel_widget_t *widg)
{
	wtk_widget_redraw(widg->widget);
}

panel_widget_t *tasklist_create()
{
	clara_rect_t rect = {0, 0, 0, 0};

	panel_widget_t *w = malloc(sizeof(panel_widget_t));

	w->w_mode = PANEL_W_MODE_FILL;

	w->widget = wtk_create_widget(rect);

	w->widget->callbacks.paint = &tasklist_paint;

	w->process = &tasklist_process;

	return w;
}
