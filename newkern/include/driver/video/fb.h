/**
 * driver/video/fb.h
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 21-07-2014 - Created
 */

#ifndef __DRIVER_VIDEO_FB_H__
#define __DRIVER_VIDEO_FB_H__

#include <sys/ioctl.h>
 
extern uint32_t 	*fb_primary_lfb;
extern fb_device_info_t *fb_primary_device_info;
extern fb_mode_info_t	*fb_primary_mode_info;

#endif

