#include <stdint.h>
#include <sys/types.h>
#ifndef __CLARA_CTYPES_H__
#define __CLARA_CTYPES_H__

typedef struct {
	int16_t	x;
	int16_t	y;
} clara_point_t;

typedef struct {
	int16_t	x;
	int16_t	y;
	int16_t	w;
	int16_t	h;
} clara_rect_t;

typedef struct {
	uint16_t	 w;
	uint16_t	 h;
	size_t		 size;
	uint32_t	*pixels;	
} clara_surface_t;

#endif




