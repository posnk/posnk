#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "graphics.h"
#include "window.h"
#include "llist.h"

llist_t window_list;
int x = 0;
void window_init()
{
	llist_create(&window_list);
}

void window_register(int ctlbuf)
{
	wserv_window_t *win = malloc(sizeof(wserv_window_t));
	win->ctlbuf = ctlbuf;
	win->winfo = shmat(ctlbuf, (void *) 0, 0);
	if (-1 == (int) win->winfo) {
		printf("warn: couldn't attach window %i: %s\n", ctlbuf, strerror(errno));
		return;
	}
	if (win->winfo->magic != WINDOW_MAGIC) {
		printf("warn: invalid window magic (%i) for window %i\n", win->winfo->magic, ctlbuf);
		return;
	}
	printf("info: new window %i %ix%i %s\n", ctlbuf, win->winfo->width, win->winfo->height, win->winfo->title);
	win->pixels = shmat(win->winfo->pixbuf, (void *) 0, 0);
	if (-1 == (int) win->pixels) {
		printf("warn: couldn't attach pixbuf %i: %s\n", win->winfo->pixbuf, strerror(errno));
		return;
	}
	win->winfo->x = x;
	x+= win->winfo->width;
	llist_add_end(&window_list,(llist_t *)win);
}

void window_render(wserv_window_t *w)
{
	int x,y,_x, _y, dw ,dh, bstart, pos;
	x = w->winfo->x+3;
	y = w->winfo->y+20;
	dw = w->winfo->width+6;
	dh = w->winfo->height+24;
	if ((w->winfo->x + dw) > fb_width)
		dw = fb_width - w->winfo->x;
	if ((w->winfo->y + dh) > fb_height)
		dh = fb_height - w->winfo->y;
	if ((w->winfo->x > fb_width) || (w->winfo->y > fb_height))
		return;
	fill_rect(w->winfo->x, w->winfo->y, dw, 18, 0x00FF00);
	render_string(w->winfo->x+5, w->winfo->y, 0xFFFFFF, 0x00FF00, w->winfo->title);
	draw_rect(x - 1,y - 1,dw-2,dh-22,0xFFFFFF);
	draw_rect(x - 2,y - 1,dw-1,dh-21,0xFFFFFF);
	draw_rect(x - 3,y - 1,dw,dh-20,0xFFFFFF);
	dw -= 6;
	dh -= 24;
	if (dw < 0)
		dw = 0;
	if (dh < 0)
		dh = 0;
	bstart = w->winfo->display_buf * w->winfo->width * w->winfo->height;
	for (_y = 0; _y < dh; _y++) {
		pos = bstart + _y * w->winfo->width;
		for (_x = 0; _x < dw; _x++) {
			*FB_PIX_AT(x + _x, y + _y) = w->pixels[pos++];
		}
	}
}

void window_render_all()
{
	wserv_window_t *w;
	llist_t *_w;
	for (_w = window_list.next; _w != &window_list; _w = _w->next) {
		w = (wserv_window_t *) _w;
		window_render(w);
	}
}
