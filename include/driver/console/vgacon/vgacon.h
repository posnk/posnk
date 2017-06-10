#ifndef __text_video_h__
#define __text_video_h__
#include "arch/i386/x86.h"

typedef struct vgacon_screen_character {
	char character;
	char attribute;
} vgacon_screen_character_t;

typedef struct vgacon_vc_info {
	short cursor_x;
	short cursor_y;
	char page_id;
	volatile vgacon_screen_character_t *video_buffer;
	char attrib;
	char escape;
	int no_scroll;
} vgacon_vc_info_t;

void vgacon_write_crtc_register(char id,char val);
#endif
