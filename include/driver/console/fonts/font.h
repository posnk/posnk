/**
 * driver/console/fonts/font.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 21-07-2014 - Created
 */

#ifndef __DRIVER_CONSOLE_FONTS_FONT_H__
#define __DRIVER_CONSOLE_FONTS_FONT_H__

#include <stdint.h>

typedef struct {
	int ascii_offset;
	int count;
	int height;
	uint8_t *data;
} font_def_t;



#endif

