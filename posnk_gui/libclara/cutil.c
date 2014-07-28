#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <clara/ctypes.h>

int clara_rect_test(clara_rect_t rect, clara_point_t point)
{
	return	(point.x >= rect.x) && (point.x < (rect.x + rect.w)) &&
		(point.y >= rect.y) && (point.y < (rect.y + rect.h));
}
