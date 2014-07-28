#include <stdint.h>
#include <sys/types.h>
#include <clara/ctypes.h>
#include <bitmap.h>

#ifndef __CLARA_CDRAW_H__
#define __CLARA_CDRAW_H__

void clara_draw_v_line(clara_surface_t *s, int x, int y, int h, int rgb);

void clara_draw_h_line(clara_surface_t *s, int x, int y, int w, int rgb);

void clara_draw_rect(clara_surface_t *s, int x, int y, int w, int h, int rgb);

void clara_fill_rect(clara_surface_t *s, int x, int y, int w, int h, int rgb);

void clara_draw_image(clara_surface_t *s, int x, int y, bitmap_image_t *image);

void clara_copy_surface(clara_surface_t *s, int x, int y, clara_surface_t *source);

void clara_clear_surface(clara_surface_t *s, int bg);

void clara_draw_1d_image(clara_surface_t *s, int x, int y, int w, bitmap_image_t *image);

#endif
