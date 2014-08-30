#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <clara/clara.h>
#include <clara/cwindow.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <wtk/window.h>
#include <wtk/widget.h>
#include "tardis.h"

int		 desktop_dirty = 1;

wtk_window_t	*desktop_window;
wtk_widget_t	*desktop_widget;
wtk_widget_t	*desktop_clock_widget;


cairo_surface_t *desktop_background;

time_t		 desktop_update_ts;

void desktop_paint(wtk_widget_t *w, cairo_t *ctx, int focused) 
{
	cairo_set_source_surface(ctx, desktop_background, 0, 0);
	cairo_paint(ctx);
}

void desktop_do_clip(wtk_widget_t *w, cairo_t *ctx) 
{
	if (desktop_dirty) {
		desktop_dirty = 0;
		cairo_rectangle(ctx, 0, 0, w->rect.w, w->rect.h);
	}
}

void desktop_initialize(const char * bg_path)
{
	int16_t cw = 256;
	clara_rect_t d = clara_get_screen_dims();
	clara_rect_t clock_rect = {d.w - cw, 0, cw, 32};

	desktop_background = cairo_image_surface_create_from_png (bg_path);
	assert (desktop_background != NULL);

	desktop_window = wtk_window_create(d.w, d.h, CLARA_WIN_FLAG_NOFRONT | CLARA_WIN_FLAG_UNDECORATED, "TARDIS (Clara Desktop)");
	assert (desktop_window != NULL);

	desktop_widget = wtk_create_widget(d);
	assert (desktop_widget != NULL);

	desktop_widget->callbacks.paint = &desktop_paint;
	desktop_widget->callbacks.do_clip = &desktop_do_clip;

	wtk_widget_add(desktop_window->widget, desktop_widget);
	
	desktop_clock_widget = clock_create(clock_rect);

	wtk_widget_add(desktop_widget, desktop_clock_widget);
}

void desktop_process()
{
	if (difftime(time(NULL), desktop_update_ts) >= 1) {
		wtk_widget_redraw(desktop_clock_widget);
		time(&desktop_update_ts);
	}
	wtk_window_process(desktop_window);
}
