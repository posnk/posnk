#ifndef __OSWIN_VIDEO_H__
#define __OSWIN_VIDEO_H__

#include <clara/ctypes.h>
#include <cairo.h>

typedef struct {
	cairo_surface_t		*back_surface;
	cairo_surface_t		*fb_surface;
	cairo_t			*back_context;
	cairo_t			*fb_context;
} oswin_video_t;

int oswin_video_init(clara_surface_t *csurf);

void oswin_flip_buffers();

cairo_surface_t *oswin_get_surface();

cairo_t *oswin_get_context();

void oswin_start_clip();

void oswin_set_clip(int x, int y, int w, int h);

void oswin_finish_clip();
#endif
