#ifndef __WTK_WINDOW_H__
#define __WTK_WINDOW_H__

#include <cairo.h>
#include <clara/clara.h>
#include <wtk/widget.h>
#include <stdint.h>

typedef struct wtk_window wtk_window_t;

struct wtk_window {
	uint32_t	 handle;
	wtk_widget_t	*widget;
	cairo_t		*context;
	cairo_surface_t	*surface;
	cairo_t		*b_context;
	cairo_surface_t	*b_surface;
	clara_surface_t *c_surf;
};

void wtk_window_process(wtk_window_t *wnd);

wtk_window_t *wtk_window_create(int width, int height, int flags, char *name);

#endif
