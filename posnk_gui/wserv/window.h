#include <stdint.h>
#include "llist.h"
#ifndef WINDOW_H
#define WINDOW_H

#define WINDOW_MAGIC	0xCAFEC007

typedef struct {
	uint32_t	 magic;
	uint32_t	 id;
	uint16_t	 width;
	uint16_t	 height;
	uint16_t	 x;
	uint16_t	 y;
	int		 pixbuf;
	char		 title[128];
	int		 buffer_count;
	int		 current_buf;
	int		 display_buf;
} posgui_window_t;

typedef struct {
	llist_t		 link;
	int		 ctlbuf;
	posgui_window_t *winfo;	
	uint32_t	*pixels;
} wserv_window_t;

void window_register(int ctlbuf);

void window_init();

void window_render_all();

#endif

