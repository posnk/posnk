#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <clara/ctypes.h>
#include <bitmap.h>

void clara_draw_v_line(clara_surface_t *s, int x, int y, int h, int rgb)
{
	int _c;
	int off;
	int scan;
	assert(s != NULL);
	if ((x < 0) || (x >= s->w))
		return;
	if (((y + h) < 0) || (y >= s->h))
		return;
	if (y < 0) {
		h -= -y;
		y = 0;
	}
	if ((y + h) >= s->h) {
		h = s->h - y;
	}

	scan = s->w;		
	off = x + y * scan;
	for (_c = 0; _c < h; _c++) {
		s->pixels[off] = (uint32_t) rgb;
		off += scan;
	}
}

void clara_draw_h_line(clara_surface_t *s, int x, int y, int w, int rgb)
{
	int _c;
	int off;
	assert(s != NULL);
	if ((y < 0) || (y >= s->h))
		return;
	if (((x + w) < 0) || (x >= s->w))
		return;
	if (x < 0) {
		w -= -x;
		x = 0;
	}
	if ((x + w) >= s->w) {
		w = s->w - x;
	}
		
	off = x + y * s->w;
	for (_c = 0; _c < w; _c++)
		s->pixels[off++] = (uint32_t) rgb;
}

void clara_draw_rect(clara_surface_t *s, int x, int y, int w, int h, int rgb)
{
	clara_draw_v_line	(s, x		, y	, h, rgb);
	clara_draw_v_line	(s, x+w-1	, y	, h, rgb);
	clara_draw_h_line	(s, x		, y	, w, rgb);
	clara_draw_h_line	(s, x		, y+h-1	, w, rgb);
}

void clara_fill_rect(clara_surface_t *s, int x, int y, int w, int h, int rgb)
{
	int _off, off, scan, end_off;
	if (((x + w) < 0) || ((y + h) < 0))
		return;
	if ((x >= s->w) || (y >= s->h))
		return;
	if (x < 0) {
		w -= -x;
		x = 0;
	}
	if (y < 0) {
		h -= -y;
		y = 0;
	}
	if ((x + w) >= s->w) {
		w = s->w - x;
	}
	if ((y + h) >= s->h) {
		h = s->h - y;
	}

	scan = s->w;
	off = x + y * s->w;
	end_off = off + h * s->w;
	for (; off < end_off; off += scan)
		for (_off = off; _off < (off+w); _off++)
			s->pixels[_off] = rgb;
}

void clara_draw_image(clara_surface_t *s, int x, int y, bitmap_image_t *image)
{
	int _x, _y, ox, oy, _w, _h;
	int off, scan, end_off, soff, sscan, _off;

	_w = image->width;
	_h = image->height;
	ox = x;
	oy = y;

	if ((x == 0) && (y == 0) && (s->w == _w) && (s->h == _h)) {
		memcpy(s->pixels, image->pixels, sizeof(uint32_t) * s->w * s->h);
		return;
	}

	if (((x + _w) < 0) || ((y + _h) < 0))
		return;
	if ((x >= s->w) || (y >= s->h))
		return;
	if (x < 0) {
		_w -= -x;
		x = 0;
	}
	if (y < 0) {
		_h -= -y;
		y = 0;
	}
	if ((x + _w) >= s->w) {
		_w = s->w - x;
	}
	if ((y + _h) >= s->h) {
		_h = s->h - y;
	}
	_x = x - ox;
	_y = y - oy;

	sscan = image->width;
	scan = s->w;
	off = x + y * s->w;
	soff = _x + _y * image->width;
	end_off = off  + _h * s->w;
	for (; off < end_off; off += scan) {
		for (_off = 0; _off < _w; _off++)
			if (image->pixels[soff + _off] != 0xff00ff)
				s->pixels[off + _off] = image->pixels[soff + _off];
		soff += sscan;
	}
}

void clara_draw_1d_image(clara_surface_t *s, int x, int y, int w, bitmap_image_t *image)
{
	int _y,  oy, _w, _h;
	int _c,_d, off, end_off, scan;

	_w = w;
	_h = image->height;
	oy = y;

	if (((x + _w) < 0) || ((y + _h) < 0))
		return;
	if ((x >= s->w) || (y >= s->h))
		return;
	if (x < 0) {
		_w -= -x;
		x = 0;
	}
	if (y < 0) {
		_h -= -y;
		y = 0;
	}
	if ((x + _w) >= s->w) {
		_w = s->w - x;
	}
	if ((y + _h) >= s->h) {
		_h = s->h - y;
	}
	_y = y - oy;

	scan = s->w;
	off = x + y * scan;
	end_off = off + _w;

	for (_d = 0; _d < _h; _d++) {
		for (_c = off; _c < end_off;)
			s->pixels[_c++] = image->pixels[_d + _y];
		off += scan;
		end_off += scan;
	}
}

void clara_copy_surface(clara_surface_t *s, int x, int y, clara_surface_t *source)
{
	int _x, _y, ox, oy, _w, _h;
	int off, scan, end_off, soff, sscan;

	_w = source->w;
	_h = source->h;
	ox = x;
	oy = y;

	if ((x == 0) && (y == 0) && (s->w == _w) && (s->h == _h)) {
		memcpy(s->pixels, source->pixels, sizeof(uint32_t) * s->w * s->h);
		return;
	}

	if (((x + _w) < 0) || ((y + _h) < 0))
		return;
	if ((x >= s->w) || (y >= s->h))
		return;
	if (x < 0) {
		_w -= -x;
		x = 0;
	}
	if (y < 0) {
		_h -= -y;
		y = 0;
	}
	if ((x + _w) >= s->w) {
		_w = s->w - x;
	}
	if ((y + _h) >= s->h) {
		_h = s->h - y;
	}
	_x = x - ox;
	_y = y - oy;

	sscan = source->w;
	scan = s->w;
	off = x + y * s->w;
	soff = _x + _y * source->w;
	end_off = off  + _h * s->w;
	for (; off < end_off; off += scan) {
		memcpy(&(s->pixels[off]), &(source->pixels[soff]), _w * sizeof(uint32_t));
		soff += sscan;
	}
}

void clara_clear_surface(clara_surface_t *s, int bg)
{
	clara_fill_rect(s,0,0,s->w,s->h,bg);
}
