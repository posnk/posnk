#include <stdint.h>
#include "graphics.h"

//((fbp_x + fb_vinfo.xoffset) * fb_pix_len) + ((fbp_y + fb_vinfo.yoffset) * fb_finfo.line_length)

char		*fb_pixels;
int		 fb_stride = 4096;
int		 fb_pix_len = 4;
int		 fb_width = 1024;
int		 fb_height = 768;

void draw_v_line(int x, int y, int h, int rgb)
{
	int _c;
	for (_c = 0; _c < h; _c++)
		*FB_PIX_AT(x,y + _c) = (uint32_t) rgb;
}

void draw_h_line(int x, int y, int w, int rgb)
{
	int _c;
	for (_c = 0; _c < w; _c++)
		*FB_PIX_AT(x + _c,y) = (uint32_t) rgb;
}

void draw_rect(int x, int y, int w, int h, int rgb)
{
	draw_v_line	(x	, y	, h, rgb);
	draw_v_line	(x+w-1	, y	, h, rgb);
	draw_h_line	(x	, y	, w, rgb);
	draw_h_line	(x	, y+h-1	, w, rgb);
}

void fill_rect(int x, int y, int w, int h, int rgb)
{
	int _c;
	for (_c = 0; _c < h; _c++)
		draw_h_line(x, y+_c, w, rgb);
}

void draw_image(int x, int y, bitmap_image_t *image)
{
	int _x, _y;
	for (_x = 0; _x < image->width; _x++)
		for (_y = 0; _y < image->height; _y++) {
			*FB_PIX_AT(x + _x, y + _y) = image->pixels[_x + _y * image->width];
		}
}

void clear_fb(int bg)
{
	fill_rect(0,0,fb_width,fb_height,bg);
}
