/**
 * Part of libbmp
 * Written by Peter Bosch <peterbosc@gmail.com>
 */

#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>

#define BMP_MAGIC	0x4D42

struct bmp_file_header 
{
	uint16_t magic;  //specifies the file type
	uint32_t file_size;  //specifies the size in bytes of the bitmap file
	uint32_t reserved;  //reserved; must be 0
	uint32_t data_offset;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
} __attribute__((__packed__ ));

struct bmp_info_header
{
	uint32_t info_size;
	 int32_t width;
	 int32_t height;
	uint16_t plane_count; //specifies the number of color planes, must be 1
	uint16_t bpp; //specifies the number of bit per pixel
	uint32_t compression;//spcifies the type of compression
	uint32_t image_size;  //size of image in bytes
	 int32_t h_dpm;  //number of pixels per meter in x axis
	 int32_t v_dpm;  //number of pixels per meter in y axis
	uint32_t colour_count;  //number of colors used by th ebitmap
	uint32_t imp_colour_count;  //number of colors that are important
} __attribute__((__packed__ ));

typedef struct bmp_info_header bmp_info_header_t;
typedef struct bmp_file_header bmp_file_header_t;

typedef struct bitmap_image
{
	int		 width;
	int		 height;
	uint32_t	*pixels;
} bitmap_image_t;

bitmap_image_t *load_bmp(char *path);

#endif
