#include <stdint.h>
#include <sys/types.h>
#ifndef __CLARA_CTYPES_H__
#define __CLARA_CTYPES_H__

typedef struct {
	uint16_t	x;
	uint16_t	y;
	uint16_t	w;
	uint16_t	h;
} clara_rect_t;

typedef struct {
	uint16_t	 w;
	uint16_t	 h;
	size_t		 size;
	uint32_t	*pixels;	
} clara_surface_t;

#endif




