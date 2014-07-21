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

int fbcon_current_vc = 0;

uint32_t fbcon_colour_map[16] = {
0x000000, 0xCD0000, 0x00CD00, 0xCDCD00, 0x0000EE, 0xCD00CD, 0x00CDCD, 0xE5E5E5, 
0x7F7F7F, 0xFF0000, 0x00FF00, 0xFFFF00, 0x5C5CFF, 0xFF00FF, 0x00FFFF, 0xFFFFFF
};

void fbcon_render_char(int x, int y, uint32_t fg, uint32_t bg, char c) {
	int _x , _y, lp, bp;
	uint8_t line;

	if (c < fbcon_font->ascii_offset)
		c = ' ';

	bp = (c - fbcon_font->ascii_offset) * fbcon_font->height + fbcon_font->height - 1;

	for (_y = 0; _y < fbcon_font->height; _y++) {

		lp = (y + _y) * fbcon_width + x;
		line = fbcon_font->data[bp - _y];

		for (_x = 7; _x >= 0; _x--) 
			fb_primary_lfb[lp++] = (line & (1 << _x)) ? fg : bg;
	}	
}

void fbcon_draw_hline(int x, int y, int w, int c) {
	int _x, lp;
	lp = y * fbcon_width + x;
	for (_x = w; _x >= 0; _x--) 
		fb_primary_lfb[lp++] = c;
}

void fbcon_render_string(int x, int y, uint32_t fg, uint32_t bg, char *str) 
{
	int n, l;
	l = strlen(str);
	for (n = 0; n < l; n++) {
		fbcon_render_char(x, y, fg, bg, str[n]);
		x += 8;
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
	int x = col * 8;
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
			fbcon_render_char(col * 8, 
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
	fbcon_render_char(col * 8, 
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
        for (vc_id = 0;vc_id < 9;vc_id++){
                fbcon_all_vcs[vc_id].cursor_x = 0;
                fbcon_all_vcs[vc_id].cursor_y = 0;  
                fbcon_all_vcs[vc_id].page_id = vc_id;
        }
	vterm_tty_setup("fbcon", 2, 9, fbcon_height / (fbcon_font->height + 3), fbcon_width / 8);
	fbcon_switch_vc(0);
}
