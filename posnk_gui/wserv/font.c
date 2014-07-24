#include "font.h"
#include "rnd8x13.h"
#include "graphics.h"

font_def_t *fbcon_font = &rnd8x13;
void render_char(int x, int y, uint32_t fg, uint32_t bg, char c) {
	int _x , _y, lp, bp;
	uint8_t line;

	if (c < fbcon_font->ascii_offset)
		c = ' ';

	bp = (c - fbcon_font->ascii_offset) * fbcon_font->height + fbcon_font->height - 1;

	for (_y = 0; _y < fbcon_font->height; _y++) {

		lp = (y + _y) * fb_width + x;
		line = fbcon_font->data[bp - _y];

		for (_x = 7; _x >= 0; _x--) 
			((uint32_t *)fb_pixels)[lp++] = (line & (1 << _x)) ? fg : bg;
	}	
}

void render_string(int x, int y, uint32_t fg, uint32_t bg, char *str) 
{
	int n, l;
	l = strlen(str);
	for (n = 0; n < l; n++) {
		render_char(x, y, fg, bg, str[n]);
		x += 8;
	}
}
