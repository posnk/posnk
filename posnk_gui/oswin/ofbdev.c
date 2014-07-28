#include <fcntl.h>        /* for fcntl */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>        /* for mmap */
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include "ofbdev.h"
#include <clara/clara.h> 
//TODO: Support more framebuffer layouts

oswin_fbdev_t	oswin_fbdev;

int oswin_fbdev_init(const char *path)
{
	int fb_pix_len;
	oswin_fbdev_t *fbdev = &oswin_fbdev;
	fb_device_info_t fb_dev_info;
	fb_mode_info_t fb_mode_info;

	/* open the file for reading and writing */
	fbdev->fd = open(path, O_RDWR);
	if (fbdev->fd == -1) {
		fprintf(stderr, "fatal: cannot open framebuffer device: %s\n", strerror(errno));
		return -1;
	} else if (ioctl(fbdev->fd, IOCTL_FBGDINFO, &fb_dev_info)) {
		fprintf(stderr, "fatal: cannot get framebuffer device info: %s\n", strerror(errno));
		return -1;
	} else if (ioctl(fbdev->fd, IOCTL_FBGMINFO, &fb_mode_info)) {
		fprintf(stderr, "fatal: cannot get framebuffer mode info: %s\n", strerror(errno));
		return -1;
	}
	fprintf(stderr, "info: fbdev %s size:%i mode:%ix%ix%i stride:%i fd:%i\n",
		path,
		(int)fb_dev_info.fb_size, 
		fb_mode_info.fb_width, 
		fb_mode_info.fb_height,
		fb_mode_info.fb_bpp,
		fb_mode_info.fb_stride, fbdev->fd);
	fbdev->draw_surface.size = (size_t) fb_dev_info.fb_size;
	fbdev->draw_surface.w = (uint16_t) fb_mode_info.fb_width;
	fbdev->draw_surface.h = (uint16_t) fb_mode_info.fb_height;
	fbdev->back_surface.size = (size_t) fb_dev_info.fb_size;
	fbdev->back_surface.w = (uint16_t) fb_mode_info.fb_width;
	fbdev->back_surface.h = (uint16_t) fb_mode_info.fb_height;
	fb_pix_len = ((int) fb_mode_info.fb_bpp / 8) + 3;
	fb_pix_len &= ~3;
	if ((fb_mode_info.fb_width * 4) != fb_mode_info.fb_stride) {
		fprintf(stderr, "fatal: framebuffer mode not linear\n");
		return -1;
	}
	if (fb_pix_len != 4) {
		fprintf(stderr, "fatal: framebuffer mode not 32-bit aligned\n");
		return -1;
	}
	fbdev->back_surface.pixels = (uint32_t *) malloc(fbdev->back_surface.size);
	if (fbdev->back_surface.pixels == 0) {
		fprintf (stderr, "fatal: failed to allocate backbuffer memory: %s\n", strerror(errno));
		return -1;
	}
	fbdev->draw_surface.pixels = (uint32_t *) mmap(0, fbdev->draw_surface.size, 0, PROT_READ | PROT_WRITE, fbdev->fd, 0);
	if (fbdev->draw_surface.pixels == 0) {
		fprintf (stderr, "fatal: failed to map framebuffer device to memory: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

void oswin_flip_buffers()
{
	clara_copy_surface(&(oswin_fbdev.draw_surface), 0, 0, &(oswin_fbdev.back_surface));
}

clara_surface_t *oswin_get_surface()
{
	return &(oswin_fbdev.back_surface);
}

void oswin_free_surface(clara_surface_t *surf){};
