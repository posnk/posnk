#include <clara/ctypes.h>

#ifndef __OSWIN_VIDEO_H__
#define __OSWIN_VIDEO_H__

void oswin_flip_buffers();

clara_surface_t *oswin_get_surface();

void oswin_free_surface(clara_surface_t *surf);

#endif
