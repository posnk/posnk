/**
 * Part of P-OS wserv
 * Written by Peter Bosch <peterbosc@gmail.com>
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <bitmap.h>

extern char		*fb_pixels;
extern int		 fb_stride;
extern int		 fb_pix_len;
extern int		 fb_width;
extern int		 fb_height;

#define FB_PIX_AT(fbp_x, fbp_y)  ((uint32_t *) &(fb_pixels[(fbp_x) * fb_pix_len + (fbp_y) * fb_stride]))

void draw_v_line(int x, int y, int h, int rgb);

void draw_h_line(int x, int y, int w, int rgb);

void draw_rect(int x, int y, int w, int h, int rgb);

void fill_rect(int x, int y, int w, int h, int rgb);

void draw_image(int x, int y, bitmap_image_t *image);

void clear_gfx(int bg);

int open_fb();

void close_fb();
void flip_fb();

void render_string(int x, int y, uint32_t fg, uint32_t bg, char *str) ;

#endif
