#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <cairo.h>
#include <clara/clara.h> 
#include "ovideo.h"

oswin_video_t	oswin_video;
clara_rect_t	oswin_screen_dims;

int oswin_video_init(clara_surface_t *s)
{
	cairo_status_t status;

	oswin_video.fb_surface = cairo_image_surface_create_for_data ((unsigned char *)s->pixels, CAIRO_FORMAT_RGB24, s->w, s->h, s->w * 4);
	if (!oswin_video.fb_surface) {
		fprintf(stderr, "fatal: could not create cairo surface for framebuffer\n");
		return -1;
	}

	oswin_video.back_surface = cairo_surface_create_similar_image (oswin_video.fb_surface, CAIRO_FORMAT_RGB24, s->w, s->h);
	if (!oswin_video.back_surface) {
		fprintf(stderr, "fatal: could not create cairo surface for back buffer\n");
		return -1;
	}

	oswin_video.fb_context = cairo_create (oswin_video.fb_surface);

	status = cairo_status(oswin_video.fb_context);
	if (status != CAIRO_STATUS_SUCCESS) {
		fprintf(stderr, "fatal: could not create cairo context for framebuffer: %s\n", cairo_status_to_string(status));
		return -1;
	}

	oswin_video.back_context = cairo_create (oswin_video.back_surface);

	status = cairo_status(oswin_video.back_context);
	if (status != CAIRO_STATUS_SUCCESS) {
		fprintf(stderr, "fatal: could not create cairo context for back buffer: %s\n", cairo_status_to_string(status));
		return -1;
	}

	oswin_screen_dims.x = 0;
	oswin_screen_dims.y = 0;
	oswin_screen_dims.w = s->w;
	oswin_screen_dims.h = s->h;

	return 0;
}

void oswin_flip_buffers()
{
	cairo_set_source_surface (oswin_video.fb_context, oswin_video.back_surface, 0, 0);
	cairo_paint (oswin_video.fb_context);
}

cairo_surface_t *oswin_get_surface()
{
	return oswin_video.back_surface;
}

cairo_t *oswin_get_context()
{
	return oswin_video.back_context;
}

void oswin_start_clip()
{
	cairo_new_path(oswin_video.back_context);
	cairo_new_path(oswin_video.fb_context);
	cairo_reset_clip(oswin_video.back_context);
	cairo_reset_clip(oswin_video.fb_context);

}

void oswin_set_clip(int x, int y, int w, int h)
{
	cairo_rectangle (oswin_video.back_context, x, y, w, h);
	cairo_rectangle (oswin_video.fb_context, x, y, w, h);
}

void oswin_finish_clip()
{
	cairo_clip (oswin_video.back_context);
	cairo_clip (oswin_video.fb_context);
}

clara_rect_t oswin_get_screen_dims()
{
	return oswin_screen_dims;
}
