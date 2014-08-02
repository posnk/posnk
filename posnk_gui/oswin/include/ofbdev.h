#include <clara/ctypes.h>
#ifndef __OSWIN_OFBDEV_H__
#define __OSWIN_OFBDEV_H__

typedef struct {
	int		fd;
	clara_surface_t draw_surface;
} oswin_fbdev_t;

int oswin_fbdev_init(const char *path);

#endif
