/**
 * driver/video/fb.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 21-07-2014 - Created
 */

#include <string.h>
#include <sys/errno.h>
#include <stdint.h>
#include "driver/video/fb.h"

fb_device_info_t *fb_primary_device_info;
fb_mode_info_t *fb_primary_mode_info;
void *fb_primary_lfb;

void fb_init()
{	
}
