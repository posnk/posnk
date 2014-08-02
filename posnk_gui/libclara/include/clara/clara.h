#include <stdint.h>
#include <clara/ctypes.h>
#include <clara/cdraw.h>
#ifndef __CLARA_H__
#define __CLARA_H__

int clara_init_client(const char *disp_path);

int clara_create_window();

int clara_init_window(uint32_t handle, const char *title, clara_rect_t dimensions, uint32_t flags, int surface_w, int surface_h);

clara_surface_t *clara_window_get_surface(uint32_t handle);

void clara_exit_client();

int clara_rect_test(clara_rect_t rect, clara_point_t point);

int clara_window_add_damage(uint32_t handle, clara_rect_t rect);

#endif

