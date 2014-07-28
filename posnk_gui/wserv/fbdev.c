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
#include "graphics.h"
char *		 fb_pixels_real;
int		 fb_fd;
long int	 fb_length = 0;

int open_fb()
{
	char *path = "/dev/fb0";
	fb_device_info_t fb_dev_info;
	fb_mode_info_t fb_mode_info;
	/* open the file for reading and writing */
	fb_fd = open(path, O_RDWR);
	if (fb_fd == -1) {
		printf("fatal: cannot open framebuffer device: %s\n", strerror(errno));
		return 0;
	} else if (ioctl(fb_fd, IOCTL_FBGDINFO, &fb_dev_info)) {
		printf("fatal: cannot get framebuffer device info: %s\n", strerror(errno));
		return 0;
	} else if (ioctl(fb_fd, IOCTL_FBGMINFO, &fb_mode_info)) {
		printf("fatal: cannot get framebuffer mode info: %s\n", strerror(errno));
		return 0;
	}
	printf("info: fbdev %s size:%i mode:%ix%ix%i stride:%i fd:%i\n",
		path,
		fb_dev_info.fb_size, 
		fb_mode_info.fb_width, 
		fb_mode_info.fb_height,
		fb_mode_info.fb_bpp,
		fb_mode_info.fb_stride, fb_fd);
	fb_length = (long int) fb_dev_info.fb_size;
	fb_width = (int) fb_mode_info.fb_width;
	fb_height = (int) fb_mode_info.fb_height;
	fb_stride = (int) fb_mode_info.fb_stride;
	fb_pix_len = ((int) fb_mode_info.fb_bpp / 8) + 3;
	fb_pix_len &= ~3;
	fb_pixels = (char *) malloc(fb_length);
	if ((int)fb_pixels == 0) {
		printf ("fatal: failed to allocate backbuffer memory: %s\n", strerror(errno));
		return 0;
	}
	fb_pixels_real = (char *) mmap(0, (int)fb_length, 0, PROT_READ | PROT_WRITE, fb_fd, 0);
	if ((int)fb_pixels_real == 0) {
		printf ("fatal: failed to map framebuffer device to memory: %s\n", strerror(errno));
		return 0;
	}
	return 1;
}

void flip_fb(){
	memcpy(fb_pixels_real, fb_pixels, fb_length);
}

void close_fb()
{
	//munmap(fb_pixels, fb_finfo.smem_len);
	close(fb_fd);
}
