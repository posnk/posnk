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
bitmap_image_t *load_bmp(char *path)
{
	FILE *file;
	bmp_info_header_t info_header;
	bmp_file_header_t file_header;
	uint8_t *image_data;

	bitmap_image_t *image;
	uint32_t *pixels;

	int _x, _y, _abgr, _idx,s;

	file = fopen(path,"r");

	if (file == NULL)
		return NULL;

	fread(&file_header, sizeof(bmp_file_header_t)	, 1, file);

	if (file_header.magic != BMP_MAGIC)
	{
		fclose(file);
		return NULL;
	}

	fread(&info_header, sizeof(bmp_info_header_t), 1, file);

	fseek(file, file_header.data_offset, SEEK_SET);

	image_data = (uint8_t *) malloc(info_header.image_size);

	if (!image_data)
	{
		fclose(file);
		return NULL;
	}

	pixels = (uint32_t *) malloc(info_header.width * info_header.height * sizeof(uint32_t));

	if (!pixels)
	{
		free(image_data);
		fclose(file);
		return NULL;
	}
	fread(image_data, info_header.image_size, 1, file);
	s = (info_header.width * 3 + 3) & ~3;
	for (_y = 0; _y < info_header.height; _y++)
		for (_x = 0; _x < info_header.width; _x++) {
			_idx = _x * 3 + _y * s;
			_abgr  = (image_data[_idx] & 0xFF);
			_abgr |= (image_data[_idx + 1] & 0xFF) << 8;
			_abgr |= (image_data[_idx + 2] & 0xFF) << 16;
			pixels[_x + (info_header.height - _y - 1) * info_header.width] = _abgr;
		}

	fclose(file);

	image = (bitmap_image_t *) malloc(sizeof(bitmap_image_t));

	if (!image)
	{
		free(image_data);
		free(pixels);
		return NULL;
	}
	
	image->width = info_header.width;
	image->height = info_header.height;
	image->pixels = pixels;

	free(image_data);

	return image;
}
void draw_image(int x, int y, bitmap_image_t *image)
{
	int ptr = x + y * w_open->winfo->width;
	int _x, _y;
	int scn = w_open->winfo->width - image->width;
	for (_y = 0; _y < image->height; _y++) {
		for (_x = 0; _x < image->width; _x++)
			w_open->pixels[ptr++] = image->pixels[_x + _y * image->width];
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
	strcpy(win->title, argv[1]);
	win->id = 0;
	w_open = window_open(win);
	win = w_open->winfo;
	draw_image(0,0,img);
	return 0;
}

