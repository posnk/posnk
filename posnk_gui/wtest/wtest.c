/**
 * getty.c
 *
 * Part of P-OS getty
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 25-05-2014 - Created
 */

#include "window.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <bitmap.h>

wlib_window_t *w_open;

void usage(){
}

void draw_image(int x, int y, bitmap_image_t *image)
{
	uint32_t *buf = window_get_buffer(w_open);
	int ptr = x + y * w_open->winfo->width;
	int _x, _y;
	int scn = w_open->winfo->width - image->width;
	for (_y = 0; _y < image->height; _y++) {
		for (_x = 0; _x < image->width; _x++)
			buf[ptr++] = image->pixels[_x + _y * image->width];
		ptr += scn;
	}
}

int main(int argc, char *argv[], char *envp[]){
	bitmap_image_t *img = load_bmp(argv[1]);
	posgui_window_t *win = malloc(sizeof(posgui_window_t));
	printf("wtest 0.01 by Peter Bosch\n");
	win->width = img->width;
	win->height = img->height;
	win->x = 10;
	win->y = 40;
	win->buffer_count = 2;
	strcpy(win->title, argv[1]);
	win->id = 0;
	w_open = window_open(win);
	win = w_open->winfo;
	while (1) {
		draw_image(0,0,img);
		window_swap_buffers(w_open);
	}
	return 0;
}

