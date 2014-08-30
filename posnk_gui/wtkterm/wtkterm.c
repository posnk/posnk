/**
 * wtktern.c
 *
 * Part of P-OS
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 21-07-2014 - Created
 */

#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <clara/clara.h>
#include <cairo.h>
#include <cairo-ft.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wtk/window.h>
#include <wtk/widget.h>
#include <wtk/button.h>
#include <wtk/textbox.h>
#include <wtk/menubar.h>
#include <assert.h>
#include <ft2build.h>
#include <glib.h>
#include "vterm/vterm.h"
#include "vterm/vterm_private.h"

cairo_font_face_t *font;
wtk_widget_t	*term_widget;
vterm_t *term;
cairo_font_extents_t font_extents;
int	term_w = 80;
int	term_h = 25;

uint32_t fbcon_colour_map[16] = {
	0x000000, 0xCD0000, 0x00CD00, 0xCDCD00, 0x0000EE, 0xCD00CD, 0x00CDCD, 0xE5E5E5, 
	0x7F7F7F, 0xFF0000, 0x00FF00, 0xFFFF00, 0x5C5CFF, 0xFF00FF, 0x00FFFF, 0xFFFFFF
};

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

void set_colour(cairo_t *ctx, uint32_t colour)
{
	cairo_set_source_rgb(ctx, ((double) ((colour >> 16) & 0xFF)) / 255.0, ((double) ((colour >> 8) & 0xFF)) / 255.0, ((double) ((colour) & 0xFF)) / 255.0);
}

void vterm_draw_cursor(cairo_t *ctx, vterm_t *vt)
{
	/*int x = col * 8;
	int y = row * (fbcon_font->height + 3) + fbcon_font->height;
	int c = ((row == vt->crow) && (col == vt->ccol)) ? 0xFF00CD00 : 0;
	fbcon_draw_hline(x, y    , 8, c);
	fbcon_draw_hline(x, y + 1, 8, c);*/
	int c_w = font_extents.max_x_advance;
	int c_h = font_extents.height;
	int c_x = c_w * vt->ccol;
	int c_y = c_h * vt->crow;
	cairo_rectangle(ctx,c_x, c_y + c_h - 2, c_w, 2);
	cairo_set_source_rgb(ctx,0,1,0);
	cairo_fill(ctx);
}

inline void render_term(cairo_t *ctx, vterm_t *vt, int row, int col)
{
	char buf[2];
	int c_w = font_extents.max_x_advance;
	int c_h = font_extents.height;
	int c_x = c_w * col;
	int c_y = c_h * row;

	set_colour(ctx, fbcon_get_bg(vt->cells[row][col].attr));
	cairo_rectangle(ctx, c_x, c_y, c_w, c_h);
	cairo_fill(ctx);

	buf[0] = (char) vt->cells[row][col].ch;
	buf[1] = 0;

	set_colour(ctx, fbcon_get_fg(vt->cells[row][col].attr));
	cairo_move_to (ctx, c_x,  c_y + font_extents.ascent);
	cairo_show_text (ctx, buf);
	vt->cells[row][col].dirty = 0;
}

void vterm_invalidate_screen(vterm_t *vt)
{
	int row, col;
	for (row = 0; row < term->rows; row++)
		for (col = 0; col < term->cols; col++)
			vt->cells[row][col].dirty = 1;
	wtk_widget_redraw(term_widget);
}

void term_paint(wtk_widget_t *w, cairo_t *ctx, int focused) 
{
	int row, col;
	cairo_set_font_face(ctx, font);
	cairo_set_font_size(ctx, 16);
	cairo_font_extents (ctx, &font_extents);
	for (row = 0; row < term->rows; row++)
		for (col = 0; col < term->cols; col++)
			if (term->cells[row][col].dirty)
				render_term(ctx, term, row, col);
	vterm_draw_cursor(ctx, term);
}

void term_do_clip(wtk_widget_t *w, cairo_t *ctx) 
{
	int row, col, c_w, c_h, c_x, c_y;
	cairo_set_font_face(ctx, font);
	cairo_set_font_size(ctx, 16);
	cairo_font_extents (ctx, &font_extents);
	for (row = 0; row < term->rows; row++)
		for (col = 0; col < term->cols; col++)
			if (term->cells[row][col].dirty){
				c_w = font_extents.max_x_advance;
				c_h = font_extents.height;
				c_x = c_w * col;
				c_y = c_h * row;
				cairo_rectangle(ctx, c_x, c_y, c_w, c_h);
			}
}

void vterm_invalidate_cell(vterm_t *vt, int row, int col)
{
	vt->cells[row][col].dirty = 1;
	wtk_widget_redraw(term_widget);
}

void vterm_invalidate_cursor(vterm_t *vt)
{
	/*//fbcon_vc_t *vc = &fbcon_all_vcs[MINOR(vt->device_id)];	
	if (MINOR(vt->device_id) != fbcon_current_vc){
	/	vc->cursor_x = vt->ccol;
		vc->cursor_y = vt->crow;
		return;
	}*/
	vt->cells[vt->cursor_y][vt->cursor_x].dirty = 1;
	vt->cursor_x = vt->ccol;
	vt->cursor_y = vt->crow;
	vt->cells[vt->cursor_y][vt->cursor_x].dirty = 1;
	wtk_widget_redraw(term_widget);
}

void vterm_handle_bell(__attribute__((__unused__)) vterm_t *vt)
{
}

void term_key_typed(wtk_widget_t *w, uint32_t keycode, char keychar)
{
	vterm_write_pipe(term, keychar);
}
int kbdus_nt[128] =
{
        0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
  '(', ')', '_', '+', KEY_BACKSPACE,     /* Backspace */
  '\t',                 /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\r', /* Enter key */
    0,                  /* 29   - Control */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',       /* 39 */
 '\"', '~',   0,                /* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',                     /* 49 */
  'M', '<', '>', '?',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    KEY_F(1),   KEY_F(2),   KEY_F(3),   KEY_F(4),   KEY_F(5),   KEY_F(6),   KEY_F(7),   KEY_F(8),   KEY_F(9),
    KEY_F(10),  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    KEY_HOME,  /* Home key */
    KEY_UP,  /* Up Arrow */
    KEY_PPAGE,  /* Page Up */
  '-',
    KEY_LEFT,  /* Left Arrow */
    0,
    KEY_RIGHT,  /* Right Arrow */
  '+',
    KEY_END,  /* 79 - End key*/
    KEY_DOWN,  /* Down Arrow */
    KEY_NPAGE,  /* Page Down */
    KEY_IC,  /* Insert Key */
    KEY_DC,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};   
void term_key_down(wtk_widget_t *w, uint32_t keycode, char keychar)
{
	int cc = kbdus_nt[keycode];
	if (cc < 256)
		return;
	vterm_write_pipe(term, cc);
}

int main(int argc, char **argv)
{
	int st;
	FT_Library	lib;
	FT_Face 	ft;
	clara_rect_t dims1 = {0,0,80*8, 25*16};
	wtk_window_t *window;

	clara_init_client("/oswdisp");	
	
	window = wtk_window_create(80*8, 25*16, 0, "Terminal");

	term_widget = wtk_create_widget(dims1);
	wtk_widget_add(window->widget, term_widget);		

	term_widget->callbacks.paint = &term_paint;
	term_widget->callbacks.do_clip = &term_do_clip;
	term_widget->callbacks.key_typed = term_key_typed;
	term_widget->callbacks.key_down = term_key_down;

	term = malloc(sizeof(vterm_t));
	
	vterm_init(term, 0, 25, 80);

	FT_Init_FreeType( &lib );
	st = FT_New_Face( lib, "/usr/share/fonts/FreeMono.ttf", 0, &ft );
	assert(st == 0);
	font =  cairo_ft_font_face_create_for_ft_face(ft,0);
	assert(font != 0);
	//font = cairo_ft_font_face_create_for_pattern(pattern);

	term->fd = open("/dev/ptym0", O_RDWR | O_NOCTTY| O_NONBLOCK);
	pid_t p = fork();
	if (p == 0) {
		execlp("/sbin/getty", "getty", "/dev/ptys0", NULL);
	}
	while (1) {
		vterm_process_tty(term);
		wtk_window_process(window);
		usleep(999);
	}

	clara_exit_client();
	return 0;
}
