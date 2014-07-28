#include <stdint.h>
#include <clara/ctypes.h>
#include <clara/cdraw.h>
#ifndef __CLARA_H__
#define __CLARA_H__

int clara_init_client(const char *disp_path);

int clara_create_window();

int clara_init_window(uint32_t handle, const char *title, clara_rect_t dimensions, uint32_t flags, int surface_w, int surface_h, int surface_count);

int clara_window_swap_buffers(uint32_t handle);

clara_surface_t *clara_window_get_surface(uint32_t handle);

void clara_exit_client();


#endif

