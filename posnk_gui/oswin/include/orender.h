#include <clara/ctypes.h>
#include <clara/cllist.h>
#ifndef __OSWIN_ORENDER_H__
#define __OSWIN_ORENDER_H__

typedef struct {
	cllist_t	link;
	clara_rect_t	rect;
} oswin_damage_t;

void oswin_render_init();

void oswin_add_damage(clara_rect_t rect);

#endif
