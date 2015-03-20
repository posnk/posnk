/**
 * driver/console/fbcon_vterm.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 21-07-2014 - Created
 */

#include <string.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include "driver/console/fonts/font.h"
#include "driver/console/fonts/rnd8x13.h"
#include "kernel/tty.h"
#include "driver/console/vterm/vterm.h"
#include "driver/console/vterm/vterm_private.h"
#include "driver/video/fb.h"

int fbcon_bppix;
int fbcon_bpline;

extern uint32_t *fb;
font_def_t *fbcon_font = &rnd8x13;
int fbcon_width;
int fbcon_height;

typedef struct fbcon_vc {
	short cursor_x;
	short cursor_y;
	char page_id;
} fbcon_vc_t;

void vterm_tty_setup(char *name, dev_t major, int minor_count, int rows, int cols);

fbcon_vc_t fbcon_all_vcs[9];

#define FBCON_PIXEL_PTR(pOff) ((void *) ( \
	((uintptr_t) fb_primary_lfb) + \
	fbcon_bpix * (pOff) ))

#define FBCON_RGBA8888_TO_RGBA8565(cOlI) ( (cOlI & 0xFF000000) | \
					  ((cOlI & 0x00F80000) >> 8) | \
					  ((cOlI & 0x0000FC00) >> 5) | \
					  ((cOlI & 0x000000F8) >> 3) )


int fbcon_current_vc = 0;

uint32_t fbcon_colour_map[16] = {
0x000000, 0xCD0000, 0x00CD00, 0xCDCD00, 0x0000EE, 0xCD00CD, 0x00CDCD, 0xE5E5E5, 
0x7F7F7F, 0xFF0000, 0x00FF00, 0xFFFF00, 0x5C5CFF, 0xFF00FF, 0x00FFFF, 0xFFFFFF
};

void fbcon_render_char(int x, int y, uint32_t fg, uint32_t bg, char c) {
	int _x , _y, lp, bp;
	uint8_t line;
	uint32_t *_fb32 = fb_primary_lfb;
	uint16_t *_fb16 = fb_primary_lfb;	

	if (c < fbcon_font->ascii_offset)
		c = ' ';

	bp = (c - fbcon_font->ascii_offset) * fbcon_font->height + fbcon_font->height - 1;

	if (fbcon_bppix == 2) {
		fg = FBCON_RGBA8888_TO_RGBA8565(fg);
		bg = FBCON_RGBA8888_TO_RGBA8565(bg);
		for (_y = 0; _y < fbcon_font->height; _y++) {

			lp = (y + _y) * fbcon_width + x;
			line = fbcon_font->data[bp - _y];

			for (_x = 7; _x >= 0; _x--) {
				_fb16[lp] = 
					(line & (1 << _x)) ? ((uint16_t)fg) : ((bg & 0xff000000) ? 
						((uint16_t)bg): 
						(_fb16[lp]));
				lp++;
			}
		}
	} else {

		for (_y = 0; _y < fbcon_font->height; _y++) {

			lp = (y + _y) * fbcon_width + x;
			line = fbcon_font->data[bp - _y];

			for (_x = 7; _x >= 0; _x--) {
				_fb32[lp] = 
					(line & (1 << _x)) ? fg : ((bg & 0xff000000) ? 
						bg : 
						(_fb32[lp]));
				lp++;
			}
		}
	}	
}

void fbcon_draw_hline(int x, int y, int w, int c) {
	int _x, lp;
	lp = y * fbcon_width + x;
	uint32_t *_fb32 = fb_primary_lfb;
	uint16_t *_fb16 = fb_primary_lfb;	
	if (fbcon_bppix == 2) {
		c = FBCON_RGBA8888_TO_RGBA8565(c);
		for (_x = w; _x >= 0; _x--) 
			_fb16[lp++] = (uint16_t)c;
	} else {
		for (_x = w; _x >= 0; _x--) 
			_fb32[lp++] = c;
	}
}

int fbcon_line_len(char *str)
{
	int i = 0;
	while ( (*str != '\0') && (*(str++) != '\n') ) i++;
	return i;
}

void fbcon_render_string(int x, int y, uint32_t fg, uint32_t bg, char *str) 
{
	int n, l, nl;
	int sx = x;
	l = strlen(str);
	x = x - (fbcon_line_len(str) / 2) * 8;
	nl = 1;
	for (n = 0; n < l; n++) 
		if (str[n] == '\n')
			 nl++;
	y = y - (nl * (fbcon_font->height + 2)) / 2; 
	for (n = 0; n < l; n++) {
		if ((str[n] == '\n') && (n != (l - 1))) {
			x = sx - fbcon_line_len(&(str[n + 1])) * 4;
			y += fbcon_font->height + 2;
		} else {
			fbcon_render_char(x, y, fg, bg, str[n]);
			x += 9;;
		}
	}
}

uint32_t fbcon_get_fg(int attr)
{
	int fg;
	fg = (int) attr & 0xF;
	if (attr & A_BOLD)
		fg |= 8;
	return fbcon_colour_map[fg] | 0xFF000000;
}

uint32_t fbcon_get_bg(int attr)
{
	int bg;
	bg = (int) (attr & 0xF0) >> 4;
	//if (attr & A_BOLD)
//bg |= 8;
	return fbcon_colour_map[bg] | 0xFF000000;
}

void vterm_draw_cursor(vterm_t *vt, int row, int col)
{
	int x = col * 9;
	int y = row * (fbcon_font->height + 3) + fbcon_font->height;
	int c = ((row == vt->crow) && (col == vt->ccol)) ? 0xFF00CD00 : 0;
	fbcon_draw_hline(x, y    , 8, c);
	fbcon_draw_hline(x, y + 1, 8, c);
	
}

void vterm_invalidate_screen(vterm_t *vt)
{
	int row, col;
	//fbcon_vc_t *vc = &fbcon_all_vcs[MINOR(vt->device_id)];
	if (MINOR(vt->device_id) != fbcon_current_vc)
		return;
	for (row = 0; row < vt->rows; row++)
		for (col = 0; col < vt->cols; col++) {
			fbcon_render_char(col * 9, 
						row * (fbcon_font->height + 3), 
						fbcon_get_fg(vt->cells[row][col].attr), 
						fbcon_get_bg(vt->cells[row][col].attr),
						(char) vt->cells[row][col].ch);
			vterm_draw_cursor(vt, row, col);
		}
}

void vterm_invalidate_cell(vterm_t *vt, int row, int col)
{
	//fbcon_vc_t *vc = &fbcon_all_vcs[MINOR(vt->device_id)];
	if (MINOR(vt->device_id) != fbcon_current_vc)
		return;
	fbcon_render_char(col * 9, 
				row * (fbcon_font->height + 3), 
				fbcon_get_fg(vt->cells[row][col].attr), 
				fbcon_get_bg(vt->cells[row][col].attr),
				(char) vt->cells[row][col].ch);
	vterm_draw_cursor(vt, row, col);
}

void vterm_invalidate_cursor(vterm_t *vt)
{
	fbcon_vc_t *vc = &fbcon_all_vcs[MINOR(vt->device_id)];	
	if (MINOR(vt->device_id) != fbcon_current_vc){
		vc->cursor_x = vt->ccol;
		vc->cursor_y = vt->crow;
		return;
	}
	vterm_draw_cursor(vt, vc->cursor_y, vc->cursor_x);
	vc->cursor_x = vt->ccol;
	vc->cursor_y = vt->crow;
	vterm_draw_cursor(vt, vt->crow, vt->ccol);
}

void vterm_handle_bell(__attribute__((__unused__)) vterm_t *vt)
{
}

void vterm_tty_invalidate_screen(dev_t dev);

void fbcon_switch_vc(int vc)
{
	fbcon_current_vc = vc;
	vterm_tty_invalidate_screen(MAKEDEV(2, vc));
}

void vterm_post_key_tty(dev_t dev, int keycode);

void con_handle_key(int keycode)
{
	if (keycode > KEY_VT0){
		fbcon_switch_vc(keycode - KEY_VT(1));
	} else
		vterm_post_key_tty(MAKEDEV(2,fbcon_current_vc), keycode);
}

void fbcon_vterm_init()
{
	int vc_id;
	fbcon_width = fb_primary_mode_info->fb_width;
	fbcon_height = fb_primary_mode_info->fb_height;
	fbcon_bppix = fb_primary_mode_info->fb_bpp / 8;
	if (fbcon_bppix == 3)
		fbcon_bppix = 4;
	fbcon_bpline = fbcon_bppix * fbcon_width;
        for (vc_id = 0;vc_id < 9;vc_id++){
                fbcon_all_vcs[vc_id].cursor_x = 0;
                fbcon_all_vcs[vc_id].cursor_y = 0;  
                fbcon_all_vcs[vc_id].page_id = vc_id;
        }
	vterm_tty_setup("fbcon", 2, 9, fbcon_height / (fbcon_font->height + 3), fbcon_width / 9);
	fbcon_switch_vc(0);
	earlycon_switchover();
}

inline uint32_t fbcon_mkrgb(uint32_t r, uint32_t g, uint32_t b)
{
	return 0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF) ;
}

inline uint32_t fbcon_mix(uint32_t a, uint32_t b, uint32_t m)
{
	uint32_t s = m & 0xFF;
	uint32_t o = 255 - s;	
	return (((a & 0xFF) * s) + ((b & 0xFF) * o))/255;
}

inline uint32_t fbcon_mixpix(uint32_t p1, uint32_t p2, uint32_t m)
{
	uint32_t r, g ,b,r1, g1 ,b1;
	r = (p2 >> 16) & 0xff;
	g = (p2 >> 8) & 0xff;
	b = p2 & 0xff;
	r1 = (p1 >> 16) & 0xff;
	g1 = (p1 >> 8) & 0xff;
	b1 = p1 & 0xff;
	r = fbcon_mix(r, r1, m);
	g = fbcon_mix(g, g1, m);
	b = fbcon_mix(b, b1, m);
	return fbcon_mkrgb(r,g,b);
}

inline uint32_t fbcon_blurpix(int x, int y)
{
	int rgb;
	int s = fbcon_width * y + x;
	uint32_t *_fb = fb_primary_lfb;
	if (x == 0)
		rgb = _fb[s];
	else
		rgb = fbcon_mixpix(_fb[s], _fb[s - 1], 170);
	if (x < (fbcon_width - 1))
		return fbcon_mixpix(rgb, _fb[s + 1], 150);
	else
		return rgb;
}

void panicscreen(const char *text){
	uint32_t m, r, g ,b, rgb,i;
	int x, y, lp, ml, cl, cx, cy;
	uint32_t *_fb = fb_primary_lfb;
	lp = 0;
	cx = fbcon_width / 2;
	cy = fbcon_height / 2;
	ml = cx * cx + cy * cy;
	if (fbcon_bppix >= 3) {
		for (y = 0; y < fbcon_height; y++) {
			for (x = 0; x < fbcon_width; x++) {
				r = _fb[lp];
				if (y != 0)
					r = fbcon_mixpix(r, fbcon_blurpix(x, y - 1), 170);
				if (y != fbcon_height - 1)
					r = fbcon_mixpix(r, fbcon_blurpix(x, y + 1), 150);
				_fb[lp++] = r;
			}
		}
		lp = 0;
		for (y = 0; y < fbcon_height; y++) {
			for (x = 0; x < fbcon_width; x++) {
				cl = (x - cx) * (x - cx) + (y - cy) * (y - cy);
				m = (uint32_t) ((cl * 255) / ml);
				rgb =_fb[lp];
				r = (rgb >> 16) & 0xff;
					g = (rgb >> 8) & 0xff;
				b = rgb & 0xff;
				i = ((r * 76) + (g * 150) + (b * 29)) / 255;
				r = fbcon_mix(r, i, m);
				g = fbcon_mix(g, i, m);
				b = fbcon_mix(b, i, m);
				_fb[lp++] = fbcon_mkrgb(r,g,b);
			}
		}
	}
	fbcon_render_string(cx-1,cy-1,0xFF000000, 0x00000000, text);
	fbcon_render_string(cx,cy,0xFFFFFFFF, 0x00000000, text);
}
