/**
 * Part of libbmp
 * Written by Peter Bosch <peterbosc@gmail.com>
 */

#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>

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
